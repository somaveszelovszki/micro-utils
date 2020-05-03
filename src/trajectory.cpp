#include <micro/math/linalg.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/trajectory.hpp>

namespace micro {

void Trajectory::setStartConfig(const config_t& start, meter_t currentDist) {
    if (!this->configs_.size()) {
        this->configs_.push_back(start);
        this->carDistanceAtLastConfig_ = currentDist;
        this->coveredDistanceUntilLastConfig_ = meter_t(0);
        this->carDistanceSinceLastConfig_ = meter_t(0);
    }
}

void Trajectory::appendLine(const config_t& dest) {
    const meter_t diff = dest.pose.pos.distance(this->configs_.back()->pose.pos);
    if (diff > centimeter_t(1)) {
        this->length_ += diff;
        this->configs_.push_back(dest);
    }
}

void Trajectory::appendCircle(const point2m& center, radian_t angle, m_per_sec_t destSpeed, uint32_t numSections) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const vec2m relativeVec = lastCfg->pose.pos - center;

    for (uint32_t i = 1; i <= numSections; ++i) {
        const m_per_sec_t currentSpeed    = map<uint32_t, m_per_sec_t>(i, 0, numSections, lastCfg->speed, destSpeed);
        const vec2m currentRelativeVec    = relativeVec.rotate(map<uint32_t, radian_t>(i, 0, numSections, radian_t(0), angle));
        const radian_t currentOrientation = currentRelativeVec.getAngle() + sgn(angle) * PI_2;

        this->appendLine(config_t{ { center + currentRelativeVec, currentOrientation }, currentSpeed });
    }
}

void Trajectory::appendSineArc(const config_t& dest, radian_t fwdAngle, orientationUpdate_t orientationUpdate, uint32_t numSections, radian_t sineStart, radian_t sineEnd) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const point2m c1_ = lastCfg->pose.pos.rotate(-fwdAngle);
    const point2m c2_ = dest.pose.pos.rotate(-fwdAngle);

    const meter_t dx = c2_.X - c1_.X;
    const meter_t dy = c2_.Y - c1_.Y;

    configs_t::const_iterator prevCfg = lastCfg;

    for (uint32_t i = 1; i <= numSections; ++i) {
        const meter_t x_ = c1_.X + static_cast<float>(i) / numSections * dx;
        const meter_t y_ = c1_.Y + dy * (1 - cos(map<float, radian_t>(i, 0, numSections, sineStart, sineEnd))) / 2;
        const point2m currentPoint        = point2m{ x_, y_ }.rotate(fwdAngle);
        const m_per_sec_t currentSpeed    = map<uint32_t, m_per_sec_t>(i, 0, numSections, lastCfg->speed, dest.speed);
        const radian_t currentOrientation = (currentPoint - prevCfg->pose.pos).getAngle();

        this->appendLine(config_t{ { currentPoint, currentOrientation }, currentSpeed });
        prevCfg = this->configs_.back();
    }
}

ControlData Trajectory::update(const CarProps car) {

    const configs_t::const_iterator closestConfig = this->getClosestConfig(car.pose.pos);
    const point2f closestConfigPosRaw(closestConfig->pose.pos);
    const meter_t closestDist = car.pose.pos.distance(closestConfig->pose.pos);

    configs_t::const_iterator nextConfig;

    if (closestConfig == this->configs_.begin()) {
        nextConfig = closestConfig + 1;
    } else if (closestConfig == this->configs_.back()) {
        nextConfig = closestConfig - 1;
    } else {
        const float prevRelativeDist = car.pose.pos.distance((closestConfig - 1)->pose.pos) / closestDist;
        const float nextRelativeDist = car.pose.pos.distance((closestConfig + 1)->pose.pos) / closestDist;

        if (prevRelativeDist < nextRelativeDist) {
            nextConfig = closestConfig - 1;
        } else {
            nextConfig = closestConfig + 1;
        }
    }

    const configs_t::const_iterator newSectionStartConfig = min(closestConfig, nextConfig);
    const meter_t otherDist = car.pose.pos.distance(nextConfig->pose.pos);

    if (newSectionStartConfig > this->sectionStartConfig_) {
        for (configs_t::const_iterator it = this->sectionStartConfig_; it != newSectionStartConfig; ++it) {
            this->coveredDistanceUntilLastConfig_ += (it + 1)->pose.pos.distance(it->pose.pos);
        }
        this->sectionStartConfig_ = newSectionStartConfig;
        this->carDistanceAtLastConfig_ = car.distance;
    }

    this->carDistanceSinceLastConfig_ = car.distance - this->carDistanceAtLastConfig_;

    const line2f carCenterLine(car.pose.pos, car.pose.pos + vec2m{ meter_t(0), meter_t(1) }.rotate(car.pose.angle));
    const radian_t fwdAngle         = car.speed >= m_per_sec_t(0) ? car.pose.angle : car.pose.angle + PI;
    const line2f sectionLine        = line2f(closestConfigPosRaw, point2f(nextConfig->pose.pos));
    const radian_t sectionLineAngle = sectionLine.getAngle();
    const point2m linePoint         = lineLine_intersection(sectionLine, carCenterLine);
    const radian_t linePointAngle   = (linePoint - car.pose.pos).getAngle();
    const Sign lineSign             = eqWithOverflow360(linePointAngle, fwdAngle - PI_2, PI_2) ? Sign::POSITIVE : Sign::NEGATIVE;

    ControlData controlData;
    controlData.speed                      = map(closestDist.get(), 0.0f, (closestDist + otherDist).get(), closestConfig->speed, nextConfig->speed);
    controlData.rampTime                   = millisecond_t(0);
    controlData.controlType                = ControlData::controlType_t::Line;
    controlData.lineControl.baseline.pos   = linePoint.distance(car.pose.pos) * lineSign;
    controlData.lineControl.baseline.angle = normalize360(fwdAngle - sectionLineAngle);
    controlData.lineControl.offset         = millimeter_t(0);
    controlData.lineControl.angle          = normalize360(closestConfig->pose.angle - sectionLineAngle);
    return controlData;
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

} // namespace micro
