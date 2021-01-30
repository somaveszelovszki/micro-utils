#include <micro/math/linalg.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/LinePattern.hpp>
#include <micro/utils/trajectory.hpp>

#include <iterator>

namespace micro {

static constexpr meter_t TRAJECTORY_RESOLUTION = centimeter_t(1);

radian_t Trajectory::lastSpeedAngle() const {
    return this->configs_.size() >= 2 ?
        (this->configs_.back()->pose.pos - std::prev(this->configs_.back())->pose.pos).getAngle() :
        radian_t(0);
}

void Trajectory::setStartConfig(const config_t& start, meter_t currentDist) {
    this->configs_.clear();
    this->configs_.push_back(start);
    this->carDistanceAtLastConfig_ = currentDist;
    this->coveredDistanceUntilLastConfig_ = meter_t(0);
    this->carDistanceSinceLastConfig_ = meter_t(0);
}

void Trajectory::appendLine(const config_t& dest) {
    const meter_t diff = dest.pose.pos.distance(this->configs_.back()->pose.pos);
    if (diff >= TRAJECTORY_RESOLUTION) {
        this->length_ += diff;
        this->configs_.push_back(dest);
    }
}

void Trajectory::appendCircle(const point2m& center, radian_t angle, m_per_sec_t destSpeed) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const vec2m relativeVec = lastCfg->pose.pos - center;

    const uint32_t numSections = (relativeVec.length() * abs(angle)) / TRAJECTORY_RESOLUTION;

    for (uint32_t i = 1; i <= numSections; ++i) {
        const m_per_sec_t currentSpeed = map<uint32_t, m_per_sec_t>(i, 0, numSections, lastCfg->speed, destSpeed);
        const vec2m currentRelativeVec = relativeVec.rotate(map<uint32_t, radian_t>(i, 0, numSections, radian_t(0), angle));
        const radian_t currentFwdAngle = currentRelativeVec.getAngle() + sgn(angle) * PI_2;

        this->appendLine(config_t{ { center + currentRelativeVec, toCarOrientation(currentFwdAngle, currentSpeed) }, currentSpeed });
    }
}

void Trajectory::appendSineArc(const config_t& dest, radian_t fwdAngle, orientationUpdate_t orientationUpdate, radian_t sineStart, radian_t sineEnd) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const point2m c1_ = lastCfg->pose.pos.rotate(-fwdAngle);
    const point2m c2_ = dest.pose.pos.rotate(-fwdAngle);

    const meter_t dx = c2_.X - c1_.X;
    const meter_t dy = c2_.Y - c1_.Y;

    const uint32_t numSections = (abs(dx) + abs(dy)) / TRAJECTORY_RESOLUTION;

    configs_t::const_iterator prevCfg = lastCfg;

    for (uint32_t i = 1; i <= numSections; ++i) {
        const meter_t x_ = c1_.X + static_cast<float>(i) / numSections * dx;
        const meter_t y_ = c1_.Y + dy * (1 - cos(map<float, radian_t>(i, 0, numSections, sineStart, sineEnd))) / 2;
        const point2m currentPoint     = point2m{ x_, y_ }.rotate(fwdAngle);
        const m_per_sec_t currentSpeed = map<uint32_t, m_per_sec_t>(i, 0, numSections, lastCfg->speed, dest.speed);
        const radian_t currentFwdAngle = orientationUpdate == orientationUpdate_t::FIX_ORIENTATION ? lastCfg->pose.angle : (currentPoint - prevCfg->pose.pos).getAngle();

        this->appendLine(config_t{ { currentPoint, toCarOrientation(currentFwdAngle, currentSpeed) }, currentSpeed });
        prevCfg = this->configs_.back();
    }
}

ControlData Trajectory::update(const CarProps& car) {

    const std::pair<configs_t::const_iterator, configs_t::const_iterator> sectionBoundaries = getSectionBoundaries(car);

    if (sectionBoundaries.first > this->sectionStartConfig_) {
        for (configs_t::const_iterator it = this->sectionStartConfig_; it != sectionBoundaries.first; ++it) {
            this->coveredDistanceUntilLastConfig_ += std::next(it)->pose.pos.distance(it->pose.pos);
        }
        this->sectionStartConfig_ = sectionBoundaries.first;
        this->carDistanceAtLastConfig_ = car.distance;
    }

    this->carDistanceSinceLastConfig_ = car.distance - this->carDistanceAtLastConfig_;

    const line2m carPerpendicularLine(car.pose.pos, car.pose.pos + vec2m{ meter_t(0), meter_t(1) }.rotate(car.pose.angle));

    const line2m sectionLine(sectionBoundaries.first->pose.pos, sectionBoundaries.second->pose.pos);
    const radian_t sectionLineAngle = sectionLine.getAngle();

    const point2m linePoint       = lineLine_intersection(sectionLine, carPerpendicularLine);
    const radian_t linePointAngle = (linePoint - car.pose.pos).getAngle();
    const Sign lineSign           = eqWithOverflow360(linePointAngle, car.pose.angle + PI_2, PI_2) ? Sign::NEGATIVE : Sign::POSITIVE;

    const meter_t sectionStartDist = sectionBoundaries.first->pose.pos.distance(car.pose.pos);
    const meter_t sectionEndDist   = sectionBoundaries.second->pose.pos.distance(car.pose.pos);
    const meter_t sectionLength    = sectionEndDist - sectionStartDist;

    const radian_t fwdAngleDiff   = normalizePM180(sectionBoundaries.second->pose.angle - sectionBoundaries.first->pose.angle);
    const radian_t targetFwdAngle = sectionBoundaries.first->pose.angle + map(sectionStartDist.get(), 0.0f, sectionLength.get(), radian_t(0), fwdAngleDiff);

    ControlData controlData;
    controlData.speed                    = map(sectionStartDist.get(), 0.0f, sectionLength.get(), sectionBoundaries.first->speed, sectionBoundaries.second->speed);
    controlData.rampTime                 = millisecond_t(0);
    controlData.rearSteerEnabled         = true;
    controlData.lineControl.actual.pos   = lineSign * linePoint.distance(car.pose.pos);
    controlData.lineControl.actual.angle = normalize(sectionLineAngle - car.pose.angle, PI, true);
    controlData.lineControl.target.pos   = millimeter_t(0);
    controlData.lineControl.target.angle = normalize(sectionLineAngle - targetFwdAngle, PI, true);

    return controlData;
}

bool Trajectory::finished(const CarProps& car, const LineInfo& lineInfo, const meter_t lineDetectedDistanceThreshold) const {

    const meter_t residualDistance = this->length() - this->coveredDistance();
    const LinePattern& pattern     = (car.speed >= m_per_sec_t(0) ? lineInfo.front : lineInfo.rear).pattern;

    return residualDistance <= meter_t(0) || (residualDistance <= lineDetectedDistanceThreshold && LinePattern::NONE != pattern.type);
}

std::pair<Trajectory::configs_t::const_iterator, Trajectory::configs_t::const_iterator> Trajectory::getSectionBoundaries(const CarProps& car) const {
    const configs_t::const_iterator closestConfig = this->getClosestConfig(car.pose.pos);
    configs_t::const_iterator otherConfig;

    if (closestConfig == this->configs_.begin()) {
        otherConfig = std::next(closestConfig);
    } else if (closestConfig == this->configs_.back()) {
        otherConfig = std::prev(closestConfig);
    } else {
        const configs_t::const_iterator prevConfig = std::prev(closestConfig);
        const configs_t::const_iterator nextConfig = std::next(closestConfig);

        if (std::distance(this->sectionStartConfig_, prevConfig) > std::ptrdiff_t{ 0 })
        {
            // If the section start is before the previous configuration,
            // then the car is between the previous and the closest configuration.
            otherConfig = prevConfig;
        } else if (this->sectionStartConfig_ == prevConfig) {
            // If the section start is the previous configuration,
            // then checks if the distance between the closest and the previous configuration has been covered.
            // If yes, then the closest configuration has been reached, and the trajectory should now
            // lead the car towards the next configuration.
            otherConfig = car.distance - this->carDistanceAtLastConfig_ >= closestConfig->pose.pos.distance(prevConfig->pose.pos) ? nextConfig : prevConfig;
        } else {
            // If the section start is after the previous configuration (probably the closest configuration is the section start),
            // then the car is between the closest and the next configuration.
            otherConfig = nextConfig;
        }
    }

    return {
        min(closestConfig, otherConfig),
        max(closestConfig, otherConfig)
    };
}

Trajectory::configs_t::const_iterator Trajectory::getClosestConfig(const point2m& pos) const {
    configs_t::const_iterator closest = this->configs_.end();
    meter_t minDist = micro::numeric_limits<meter_t>::infinity();

    for (configs_t::const_iterator it = this->sectionStartConfig_; it != this->configs_.end(); ++it) {
        const meter_t dist = pos.distance(it->pose.pos);
        if (dist < minDist) {
            minDist = dist;
            closest = it;
        } else {
            break; // if we have passed the closest config, there is no point for searching anymore
        }
    }

    return closest;
}

radian_t Trajectory::toCarOrientation(const radian_t fwdAngle, const m_per_sec_t speed)
{
    return speed >= m_per_sec_t(0) ? fwdAngle : normalize360(fwdAngle + PI);
}

} // namespace micro
