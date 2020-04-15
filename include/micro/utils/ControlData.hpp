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
        OrientedLine baseline;
        millimeter_t offset;
        radian_t angle;
    } lineControl;

    struct {
        // direct control
        radian_t frontWheelAngle;
        radian_t rearWheelAngle;
    } directControl;
};

} // namespace micro
