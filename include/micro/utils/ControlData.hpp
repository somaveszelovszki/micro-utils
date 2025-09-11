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
        OrientedLine actual; // actual line offset and angle - relative to car: line offset to the
                             // right is POSITIVE, counter-clockwise line angle is POSITIVE
        OrientedLine target; // target line offset and angle - relative to car: line offset to the
                             // right is POSITIVE, counter-clockwise line angle is POSITIVE
    } lineControl;
};

#define EXPECT_EQ_MICRO_CONTROL_DATA(expected, result)                                             \
    EXPECT_NEAR_UNIT_DEFAULT(expected.speed, result.speed);                                        \
    EXPECT_NEAR_UNIT_DEFAULT(expected.rampTime, result.rampTime);                                  \
    EXPECT_EQ(expected.rearSteerEnabled, result.rearSteerEnabled);                                 \
    EXPECT_EQ_MICRO_ORIENTED_LINE(expected.lineControl.actual, result.lineControl.actual);         \
    EXPECT_EQ_MICRO_ORIENTED_LINE(expected.lineControl.target, result.lineControl.target);

} // namespace micro
