#include <micro/utils/trajectory.hpp>
#include <micro/linalg/linalg.hpp>

namespace micro {

ControlData Trajectory::getControl(const CarProps& car) {
    ControlData controlData;

    const point2m optoRowCenterPos = car.pose.pos + vec2m(this->optoCarCenterDist, meter_t(0)).rotate(car.pose.angle);
    const point2f optoRowCenterPosRaw = point2f(optoRowCenterPos);

    const configs_t::const_iterator closestConfig = this->getClosestConfig(optoRowCenterPos);

    const point2f closestConfigPosRaw(closestConfig->pose.pos);
    line2f sectionLine;

    if (closestConfig == this->configs.begin()) {
        sectionLine = line2f(closestConfigPosRaw, point2f((closestConfig + 1)->pose.pos));
        controlData.speed = avg(closestConfig->speed, (closestConfig + 1)->speed);
        this->sectionStartConfig = closestConfig;

    } else if (closestConfig == this->configs.back()) {
        sectionLine = line2f(closestConfigPosRaw, point2f((closestConfig - 1)->pose.pos));
        controlData.speed = avg(closestConfig->speed, (closestConfig - 1)->speed);
        this->sectionStartConfig = closestConfig - 1;

    } else {
        const line2f l1 = line2f(closestConfigPosRaw, point2f((closestConfig - 1)->pose.pos));
        const line2f l2 = line2f(closestConfigPosRaw, point2f((closestConfig + 1)->pose.pos));

        if (distance(l1, optoRowCenterPosRaw) < distance(l2, optoRowCenterPosRaw)) {
            sectionLine = l1;
            controlData.speed = avg(closestConfig->speed, (closestConfig - 1)->speed);
            this->sectionStartConfig = closestConfig - 1;
        } else {
            sectionLine = l2;
            controlData.speed = avg(closestConfig->speed, (closestConfig + 1)->speed);
            this->sectionStartConfig = closestConfig;
        }
    }

    const line2f optoLine(optoRowCenterPosRaw, optoRowCenterPosRaw + vec2f(0.0f, this->optoCarCenterDist.get()).rotate(car.pose.angle));
    const point2m linePoint(lineLine_intersection(sectionLine, optoLine));

    const Sign lineSign = -(optoRowCenterPos - car.pose.pos).getAngleSign(closestConfig->pose.pos - car.pose.pos);

    controlData.baseline.pos = linePoint.distance(optoRowCenterPos) * static_cast<int8_t>(lineSign);
    controlData.baseline.id = 0;
    controlData.offset = millimeter_t(0);
    controlData.angle = radian_t(0);

    return controlData;
}

Trajectory::configs_t::const_iterator Trajectory::getClosestConfig(const point2m& pos) const {
    configs_t::const_iterator closest = this->configs.end();
    meter_t minDist = meter_t::infinity();

    for (configs_t::const_iterator it = this->sectionStartConfig; it != this->configs.end(); ++it) {
        const meter_t dist = pos.distance(it->pose.pos);
        if (dist < minDist) {
            minDist = dist;
            closest = it;
        }
    }

    return closest;
}

} // namespace micro
