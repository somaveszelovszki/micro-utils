#include <micro/utils/trajectory.hpp>
#include <micro/linalg/linalg.hpp>

namespace micro {

void Trajectory::setStartConfig(const config_t& start) {
    if (!this->configs_.size()) {
        this->configs_.push_back(start);
    }
}

void Trajectory::appendLine(const config_t& dest) {
    this->length_ += dest.pos.distance(this->configs_.back()->pos);
    this->configs_.push_back(dest);
}

void Trajectory::appendCircle(const point2m& center, radian_t angle, m_per_sec_t destSpeed, uint32_t numSections) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const vec2m relativeVec = lastCfg->pos - center;

    for (uint32_t i = 0; i < numSections; ++i) {
        const radian_t currentAngle = map(i, 1ul, numSections, radian_t(0), angle);
        const m_per_sec_t currentSpeed = map(i, 1ul, numSections, lastCfg->speed, destSpeed);

        this->appendLine(config_t{ relativeVec.rotate(currentAngle), currentSpeed });
    }
}

void Trajectory::appendSineArc(const config_t& dest, radian_t fwdAngle, uint32_t numSections) {
    const configs_t::const_iterator lastCfg = this->configs_.back();
    const point2m c1_ = lastCfg->pos.rotate(-fwdAngle);
    const point2m c2_ = dest.pos.rotate(-fwdAngle);

    const meter_t dx = c2_.X - c1_.X;
    const meter_t dy = c2_.Y - c1_.Y;

    for (uint32_t i = 1; i <= numSections; ++i) {
        const meter_t x_ = c1_.X + static_cast<float>(i) / numSections * dx;
        const meter_t y_ = c1_.Y + dy * (1 - cos(static_cast<float>(i) / numSections * PI)) / 2;
        const m_per_sec_t currentSpeed = map(i, 1ul, numSections, lastCfg->speed, dest.speed);

        this->appendLine(config_t{ point2m(x_, y_).rotate(fwdAngle), currentSpeed });
    }

    this->appendLine(dest);
}

ControlData Trajectory::update(const CarProps car) {
    ControlData controlData;

    const vec2m currentCarCenterToOptoRowCenter = this->carCenterToOptoRowCenter_.rotate(car.pose.angle);
    const point2m optoRowCenterPos = car.pose.pos + currentCarCenterToOptoRowCenter;
    const point2f optoRowCenterPosRaw = point2f(optoRowCenterPos);

    const configs_t::const_iterator closestConfig = this->getClosestConfig(optoRowCenterPos);
    configs_t::const_iterator newSectionStartConfig = this->sectionStartConfig_;

    const point2f closestConfigPosRaw(closestConfig->pos);
    line2f sectionLine;

    if (closestConfig == this->configs_.begin()) {
        sectionLine = line2f(closestConfigPosRaw, point2f((closestConfig + 1)->pos));
        controlData.speed = avg(closestConfig->speed, (closestConfig + 1)->speed);
        newSectionStartConfig = closestConfig;

    } else if (closestConfig == this->configs_.back()) {
        sectionLine = line2f(closestConfigPosRaw, point2f((closestConfig - 1)->pos));
        controlData.speed = avg(closestConfig->speed, (closestConfig - 1)->speed);
        newSectionStartConfig = closestConfig - 1;

    } else {
        const line2f l1 = line2f(closestConfigPosRaw, point2f((closestConfig - 1)->pos));
        const line2f l2 = line2f(closestConfigPosRaw, point2f((closestConfig + 1)->pos));

        if (distance(l1, optoRowCenterPosRaw) < distance(l2, optoRowCenterPosRaw)) {
            sectionLine = l1;
            controlData.speed = avg(closestConfig->speed, (closestConfig - 1)->speed);
            newSectionStartConfig = closestConfig - 1;
        } else {
            sectionLine = l2;
            controlData.speed = avg(closestConfig->speed, (closestConfig + 1)->speed);
            newSectionStartConfig = closestConfig;
        }
    }

    if (newSectionStartConfig > this->sectionStartConfig_) {
        for (configs_t::const_iterator it = this->sectionStartConfig_; it != newSectionStartConfig; ++it) {
            this->coveredDistanceUntilLastConfig_ += (it + 1)->pos.distance(it->pos);
        }
        this->sectionStartConfig_ = newSectionStartConfig;
        this->carDistanceAtLastConfig_ = car.distance;
    }

    this->carDistanceSinceLastConfig_ = car.distance - this->carDistanceAtLastConfig_;

    const line2f optoLine(optoRowCenterPosRaw, optoRowCenterPosRaw + static_cast<vec2f>(currentCarCenterToOptoRowCenter).rotate(PI_2));
    const point2m linePoint(lineLine_intersection(sectionLine, optoLine));

    const radian_t angleDiff = normalize360(car.pose.pos.getAngle(optoRowCenterPos) - car.pose.pos.getAngle(linePoint));
    const Sign lineSign = angleDiff < PI ? Sign::POSITIVE : Sign::NEGATIVE;

    controlData.baseline.pos = linePoint.distance(optoRowCenterPos) * lineSign;
    controlData.baseline.id = 0;
    controlData.offset = millimeter_t(0);
    controlData.angle = radian_t(0);

    return controlData;
}

Trajectory::configs_t::const_iterator Trajectory::getClosestConfig(const point2m& pos) const {
    configs_t::const_iterator closest = this->configs_.end();
    meter_t minDist = meter_t::infinity();

    for (configs_t::const_iterator it = this->sectionStartConfig_; it != this->configs_.end(); ++it) {
        const meter_t dist = pos.distance(it->pos);
        if (dist < minDist) {
            minDist = dist;
            closest = it;
        }
    }

    return closest;
}

} // namespace micro
