#pragma once

#include "Line.hpp"

namespace micro {

/* @brief Stores data that is sent to the ControlTask.
 **/
struct ControlData {

    enum class controlType_t : uint8_t {
        Line   = 1,
        Direct = 2
    };

    m_per_sec_t speed;
    millisecond_t rampTime;
    controlType_t controlType = controlType_t::Line;

    struct {
        OrientedLine actual;    // actual line offset and angle - relative to car: line offset to the right is POSITIVE, counter-clockwise line angle is POSITIVE
        OrientedLine target;    // target line offset and angle - relative to car: line offset to the right is POSITIVE, counter-clockwise line angle is POSITIVE
    } lineControl;

    struct {
        // direct control
        radian_t frontWheelAngle;
        radian_t rearWheelAngle;
    } directControl;
};

} // namespace micro
