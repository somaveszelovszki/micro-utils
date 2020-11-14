#include <micro/math/linalg.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/LinePattern.hpp>
#include <micro/utils/trajectory.hpp>

namespace micro {

void Trajectory::setStartConfig(const config_t& start, meter_t currentDist) {
    this->configs_.clear();
    this->configs_.push_back(start);
    this->carDistanceAtLastConfig_ = currentDist;
    this->coveredDistanceUntilLastConfig_ = meter_t(0);
    this->carDistanceSinceLastConfig_ = meter_t(0);
}

void Trajectory::appendLine(const config_t& dest) {
    const meter_t diff = dest.pose.pos.distance(this->configs_.back()->pose.pos);
    if (diff > centimeter_t(1)) {
        this->length_ += diff;
        this->configs_.push_back(dest);
    }
}

void Trajectory::appendCircle(const point2m& center, radian_t angle, m_per_sec_t destSpeed) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const vec2m relativeVec = lastCfg->pose.pos - center;

    const uint32_t numSections = (relativeVec.length() * angle) / centimeter_t(5);

    for (uint32_t i = 1; i <= numSections; ++i) {
        const m_per_sec_t currentSpeed    = map<uint32_t, m_per_sec_t>(i, 0, numSections, lastCfg->speed, destSpeed);
        const vec2m currentRelativeVec    = relativeVec.rotate(map<uint32_t, radian_t>(i, 0, numSections, radian_t(0), angle));
        const radian_t currentOrientation = currentRelativeVec.getAngle() + sgn(angle) * PI_2;

        this->appendLine(config_t{ { center + currentRelativeVec, currentOrientation }, currentSpeed });
    }
}

void Trajectory::appendSineArc(const config_t& dest, radian_t fwdAngle, orientationUpdate_t orientationUpdate, radian_t sineStart, radian_t sineEnd) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const point2m c1_ = lastCfg->pose.pos.rotate(-fwdAngle);
    const point2m c2_ = dest.pose.pos.rotate(-fwdAngle);

    const meter_t dx = c2_.X - c1_.X;
    const meter_t dy = c2_.Y - c1_.Y;

    const uint32_t numSections = (dx + dy) / centimeter_t(5);

    configs_t::const_iterator prevCfg = lastCfg;

    for (uint32_t i = 1; i <= numSections; ++i) {
        const meter_t x_ = c1_.X + static_cast<float>(i) / numSections * dx;
        const meter_t y_ = c1_.Y + dy * (1 - cos(map<float, radian_t>(i, 0, numSections, sineStart, sineEnd))) / 2;
        const point2m currentPoint        = point2m{ x_, y_ }.rotate(fwdAngle);
        const m_per_sec_t currentSpeed    = map<uint32_t, m_per_sec_t>(i, 0, numSections, lastCfg->speed, dest.speed);
        const radian_t currentOrientation = orientationUpdate == orientationUpdate_t::FIX_ORIENTATION ? lastCfg->pose.angle : (currentPoint - prevCfg->pose.pos).getAngle();

        this->appendLine(config_t{ { currentPoint, currentOrientation }, currentSpeed });
        prevCfg = this->configs_.back();
    }
}

ControlData Trajectory::update(const CarProps car) {

    const configs_t::const_iterator closestConfig = this->getClosestConfig(car.pose.pos);
    configs_t::const_iterator otherConfig;

    if (closestConfig == this->configs_.begin()) {
        otherConfig = closestConfig + 1;
    } else if (closestConfig == this->configs_.back()) {
        otherConfig = closestConfig - 1;
    } else {
        const meter_t closestDist = car.pose.pos.distance(closestConfig->pose.pos);

        const float prevRelativeDist = car.pose.pos.distance((closestConfig - 1)->pose.pos) / closestDist;
        const float nextRelativeDist = car.pose.pos.distance((closestConfig + 1)->pose.pos) / closestDist;

        if (prevRelativeDist < nextRelativeDist) {
            otherConfig = closestConfig - 1;
        } else {
            otherConfig = closestConfig + 1;
        }
    }

    const configs_t::const_iterator sectionStartConfig = min(closestConfig, otherConfig);
    const configs_t::const_iterator sectionEndConfig   = max(closestConfig, otherConfig);

    if (sectionStartConfig > this->sectionStartConfig_) {
        for (configs_t::const_iterator it = this->sectionStartConfig_; it != sectionStartConfig; ++it) {
            this->coveredDistanceUntilLastConfig_ += (it + 1)->pose.pos.distance(it->pose.pos);
        }
        this->sectionStartConfig_ = sectionStartConfig;
        this->carDistanceAtLastConfig_ = car.distance;
    }

    this->carDistanceSinceLastConfig_ = car.distance - this->carDistanceAtLastConfig_;

    const line2m carCenterLine(car.pose.pos, car.pose.pos + vec2m{ meter_t(0), meter_t(1) }.rotate(car.pose.angle));
    const radian_t fwdAngle         = car.speed >= m_per_sec_t(0) ? car.pose.angle : car.pose.angle + PI;
    const line2m sectionLine        = line2f(sectionStartConfig->pose.pos, sectionEndConfig->pose.pos);
    const radian_t sectionLineAngle = sectionLine.getAngle();
    const point2m linePoint         = lineLine_intersection(sectionLine, carCenterLine);
    const radian_t linePointAngle   = (linePoint - car.pose.pos).getAngle();
    const Sign lineSign             = eqWithOverflow360(linePointAngle, fwdAngle - PI_2, PI_2) ? Sign::POSITIVE : Sign::NEGATIVE;

    const meter_t sectionStartDist  = sectionStartConfig->pose.pos.distance(car.pose.pos);
    const meter_t sectionEndDist    = sectionEndConfig->pose.pos.distance(car.pose.pos);
    const meter_t sectionLength     = sectionEndDist - sectionStartDist;

    const radian_t fwdAngleDiff     = normalizePM180(sectionEndConfig->pose.angle - sectionStartConfig->pose.angle);
    const radian_t desiredFwdAngle  = sectionStartConfig->pose.angle + map(sectionStartDist.get(), 0.0f, sectionLength.get(), radian_t(0), fwdAngleDiff);

    ControlData controlData;
    controlData.speed                    = map(sectionStartDist.get(), 0.0f, sectionLength.get(), sectionStartConfig->speed, sectionEndConfig->speed);
    controlData.rampTime                 = millisecond_t(0);
    controlData.controlType              = ControlData::controlType_t::Line;
    controlData.lineControl.actual.pos   = linePoint.distance(car.pose.pos) * lineSign;
    controlData.lineControl.actual.angle = normalizePM180(sectionLineAngle - fwdAngle);
    controlData.lineControl.target.pos   = millimeter_t(0);
    controlData.lineControl.target.angle = normalizePM180(sectionLineAngle - desiredFwdAngle);
    return controlData;
}

bool Trajectory::finished(const CarProps& car, const LineInfo& lineInfo) const {
    static constexpr meter_t LINE_DETECTED_DISTANCE_LIMIT        = centimeter_t(15);
    static constexpr meter_t CENTER_LINE_DETECTED_DISTANCE_LIMIT = centimeter_t(40);
    static constexpr meter_t OVERFLOW_DISTANCE_LIMIT             = centimeter_t(150);
    static constexpr meter_t CENTER_LINE_POS_LIMIT               = centimeter_t(4);

    const Sign speedSign           = sgn(car.speed);
    const meter_t residualDistance = this->length() - this->coveredDistance();
    const bool lineDetected        = LinePattern::NONE != (Sign::POSITIVE == speedSign ? lineInfo.front.pattern : lineInfo.rear.pattern).type;

    bool finished = false;

    if (lineDetected) {
        if (residualDistance < LINE_DETECTED_DISTANCE_LIMIT) {
            finished = true;
        } else if (residualDistance < CENTER_LINE_DETECTED_DISTANCE_LIMIT) {
            const Lines& lines = Sign::POSITIVE == speedSign ? lineInfo.front.lines : lineInfo.rear.lines;
            for (const Line& line : lines) {
                if (abs(line.pos) < CENTER_LINE_POS_LIMIT) {
                    finished = true;
                    break;
                }
            }
        }
    } else if (residualDistance < -OVERFLOW_DISTANCE_LIMIT) {
        finished = true;
    }

    return finished;
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
