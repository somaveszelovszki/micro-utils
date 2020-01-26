#pragma once

#include "point2.hpp"
#include "typeinfo.hpp"

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
DEFINE_TYPEINFO(Pose);

/* @brief Car properties.
 **/
struct CarProps  {
    /* @brief Default constructor - initializes fields.
     **/
    CarProps() {}

    Pose pose;         // The current pose (position and orientation) of the car.
    m_per_sec_t speed; // The current speed of the car.
    meter_t distance;  // The distance that the car has driven since startup.
};
DEFINE_TYPEINFO(CarProps);

} // namespace micro
