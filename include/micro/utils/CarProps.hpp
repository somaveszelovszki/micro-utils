#pragma once

#include "point2.hpp"

namespace micro {

struct Pose {

    /* @brief Position of the car relative to its start position.
    **/
    point2m pos;

    /* @brief Car's orientation.
    @note Orientation is relative to the X axis!
     **/
    radian_t angle;
};

/* @brief Car properties.
 **/
struct CarProps  {
    /* @brief Default constructor - initializes fields.
     **/
    CarProps() {}

    Pose pose;                // The current pose (position and orientation) of the car.
    m_per_sec_t speed;        // The current speed of the car.
    meter_t distance;         // The distance that the car has driven since startup.
    meter_t orientedDistance; // The distance that the has driven with no major orientation change (in a straight line).
    radian_t frontWheelAngle;
    radian_t rearWheelAngle;
    rad_per_sec_t yawRate;

    radian_t getSpeedAngle(const meter_t frontRearPivotDist) const;

    vec2mps getSpeedVector(const meter_t frontRearPivotDist) const;
};

} // namespace micro
