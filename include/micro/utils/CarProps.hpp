#pragma once

#include <micro/utils/point2.hpp>

namespace micro {

struct Pose {

    /* @brief Position of the car relative to its start position.
    **/
    point2<meter_t> pos;

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
    CarProps()
        : pose{ { meter_t(0.0f), meter_t(0.0f) }, PI_2 }
        , speed(0.0f) {}

    Pose pose;          // The current pose (position and orientation) of the car.
    m_per_sec_t speed;  // The current speed of the car.
};
DEFINE_TYPEINFO(CarProps);

} // namespace micro
