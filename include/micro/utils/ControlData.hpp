#pragma once

#include "Line.hpp"

namespace micro {

/* @brief Stores data that is sent to the ControlTask.
 **/
struct ControlData {
    m_per_sec_t speed;
    millisecond_t rampTime;
    bool rearSteerEnabled = false;

    struct {
        OrientedLine actual;    // actual line offset and angle - relative to car: line offset to the right is POSITIVE, counter-clockwise line angle is POSITIVE
        OrientedLine target;    // target line offset and angle - relative to car: line offset to the right is POSITIVE, counter-clockwise line angle is POSITIVE
    } lineControl;
};

} // namespace micro
