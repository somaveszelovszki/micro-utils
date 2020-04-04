#pragma once

#include "PanelLink.hpp"

namespace micro {

struct MotorInPanelLinkData : public PanelLinkData {

    // temporary fields
    float controller_P;
    float controller_I;
    float controller_integral_max;
    // ----------

    int16_t  targetSpeed_mmps               : 15;
    bool     useSafetyEnableSignal          : 1;
    uint16_t targetSpeedRampTime_s_per_128  : 10;
    uint16_t frontServoTargetAngle_deg_per4 : 10;
    uint16_t rearServoTargetAngle_deg_per4  : 10;
    uint16_t extraServoTargetAngle_deg_per4 : 10;

    static constexpr microsecond_t period()  { return millisecond_t(5);  }
    static constexpr microsecond_t timeout() { return millisecond_t(12); }

} __attribute__((packed));

struct MotorOutPanelLinkData : public micro::PanelLinkData {

    int16_t  speed_mmps               : 15;
    uint8_t  reserved1                : 1;
    uint32_t distance_mm              : 24;
    uint16_t frontServoAngle_deg_per4 : 10;
    uint16_t rearServoAngle_deg_per4  : 10;
    uint16_t extraServoAngle_deg_per4 : 10;
    uint8_t  reserved2                : 2;

    static constexpr microsecond_t period()  { return millisecond_t(5);  }
    static constexpr microsecond_t timeout() { return millisecond_t(12); }

} __attribute__((packed));

} // namespace micro
