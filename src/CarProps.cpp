#include <micro/math/linalg.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/CarProps.hpp>

namespace micro {

radian_t CarProps::getSpeedAngle(const meter_t frontRearPivotDist) const {
    const vec2m centerToFrontPivotCenter =
        vec2m{frontRearPivotDist / 2, meter_t(0)}.rotate(this->pose.angle);
    const point2m frontPivotCenter = this->pose.pos + centerToFrontPivotCenter;
    const point2m rearPivotCenter  = this->pose.pos - centerToFrontPivotCenter;

    radian_t carFwdAngle; // the car's forward angle (independent from the current speed sign)

    if (eq(this->frontWheelAngle, this->rearWheelAngle)) {
        carFwdAngle = this->pose.angle + this->frontWheelAngle;

    } else {
        const line2m frontWheelPerpendicularLine(frontPivotCenter,
                                                 this->pose.angle + this->frontWheelAngle + PI_2);
        const line2m rearWheelPerpendicularLine(rearPivotCenter,
                                                this->pose.angle + this->rearWheelAngle + PI_2);

        const point2m trajectoryCircleCenter =
            lineLine_intersection(frontWheelPerpendicularLine, rearWheelPerpendicularLine);

        carFwdAngle = (trajectoryCircleCenter - this->pose.pos).getAngle() + PI_2;
        if (!eqWithOverflow360(this->pose.angle, carFwdAngle, PI_2)) {
            carFwdAngle += PI;
        }
    }

    return normalize360(this->speed >= m_per_sec_t(0) ? carFwdAngle : carFwdAngle + PI);
}

vec2mps CarProps::getSpeedVector(const meter_t frontRearPivotDist) const {
    return vec2mps{abs(this->speed), m_per_sec_t(0)}.rotate(
        this->getSpeedAngle(frontRearPivotDist));
}

} // namespace micro
