#pragma once

#if defined STM32F4

#include <micro/math/unit_utils.hpp>
#include <micro/utils/LinePattern.hpp>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>

namespace micro {
namespace can {
namespace detail {

struct Lines {
    struct {
        int16_t pos_mm_9p4 : 13;
        uint8_t id         : 3;
    } __attribute__((packed)) values[Line::MAX_NUM_LINES];
} __attribute__((packed));

struct LinePattern {
    uint8_t  type         : 4;
    int8_t   dir          : 2;
    int8_t   side         : 2;
    uint32_t startDist_mm : 24;
    bool isPatternPending : 1;
    uint8_t reserved      : 7;
} __attribute__((packed));

struct ServoParams {
    int16_t offset_deg_8p8   : 16;
    int16_t maxDelta_deg_8p8 : 16;

    ServoParams(const radian_t offset, const radian_t maxDelta);
    void acquire(radian_t& offset, radian_t& maxDelta) const;

} __attribute__((packed));

struct MotorControlParams {
    uint32_t controller_P_8p24 : 32;
    uint32_t controller_I_8p24 : 32;

    MotorControlParams(const float controller_P, const float controller_I);
    void acquire(float& controller_P, float& controller_I) const;

} __attribute__((packed));

} // namespace detail

template <typename T>
CAN_TxHeaderTypeDef buildHeader() {
    CAN_TxHeaderTypeDef header;
    header.StdId = T::id();
    header.ExtId = 0;
    header.IDE   = CAN_ID_STD;
    header.RTR   = CAN_RTR_DATA;
    header.DLC   = sizeof(T);
    header.TransmitGlobalTime = DISABLE;
    return header;
}

struct LateralControl {
    static constexpr uint16_t id() { return 0x301; }
    static constexpr millisecond_t period()  { return millisecond_t(2); }
    static constexpr millisecond_t timeout() { return millisecond_t(10); }

    int16_t frontWheelTargetAngle_deg_8p8 : 16;
    int16_t rearWheelTargetAngle_deg_8p8  : 16;
    int16_t extraServoTargetAngle_deg_8p8 : 16;

    LateralControl(const radian_t frontWheelTargetAngle, const radian_t rearWheelTargetAngle, const radian_t extraServoTargetAngle);
    void acquire(radian_t& frontWheelTargetAngle, radian_t& rearWheelTargetAngle, radian_t& extraServoTargetAngle) const;

} __attribute__((packed));

struct LongitudinalControl {
    static constexpr uint16_t id() { return 0x302; }
    static constexpr millisecond_t period()  { return millisecond_t(10); }
    static constexpr millisecond_t timeout() { return millisecond_t(25); }

    int16_t  targetSpeed_mmps       : 15;
    bool     useSafetyEnableSignal  : 1;
    uint16_t targetSpeedRampTime_ms : 16;

    LongitudinalControl(const m_per_sec_t targetSpeed, const bool useSafetyEnableSignal, const millisecond_t targetSpeedRampTime);
    void acquire(m_per_sec_t& targetSpeed, bool& useSafetyEnableSignal, millisecond_t& targetSpeedRampTime) const;

} __attribute__((packed));

struct FrontLines {
    static constexpr uint16_t id() { return 0x401; }
    static constexpr millisecond_t period()  { return millisecond_t(0); }
    static constexpr millisecond_t timeout() { return millisecond_t(10); }

    detail::Lines lines;

    FrontLines(const Lines& lines);
    void acquire(Lines& lines) const;

} __attribute__((packed));

struct RearLines {
    static constexpr uint16_t id() { return 0x402; }
    static constexpr millisecond_t period()  { return millisecond_t(0); }
    static constexpr millisecond_t timeout() { return millisecond_t(10); }

    detail::Lines lines;

    RearLines(const Lines& lines);
    void acquire(Lines& lines) const;

} __attribute__((packed));

struct LateralState {
    static constexpr uint16_t id() { return 0x403; }
    static constexpr millisecond_t period()  { return millisecond_t(5); }
    static constexpr millisecond_t timeout() { return millisecond_t(20); }

    uint16_t frontWheelAngle_deg_8p8 : 16;
    uint16_t rearWheelAngle_deg_8p8  : 16;
    uint16_t extraServoAngle_deg_8p8 : 16;

    LateralState(const radian_t frontWheelAngle, const radian_t rearWheelAngle, const radian_t extraServoAngle);
    void acquire(radian_t& frontWheelAngle, radian_t& rearWheelAngle, radian_t& extraServoAngle) const;

} __attribute__((packed));

struct LongitudinalState {
    static constexpr uint16_t id() { return 0x404; }
    static constexpr millisecond_t period()  { return millisecond_t(5); }
    static constexpr millisecond_t timeout() { return millisecond_t(20); }

    int16_t  speed_mmps  : 15;
    uint8_t  reserved    : 1;
    uint32_t distance_mm : 24;

    LongitudinalState(const m_per_sec_t speed, const meter_t distance);
    void acquire(m_per_sec_t& speed, meter_t& distance) const;

} __attribute__((packed));

struct FrontLinePattern {
    static constexpr uint16_t id() { return 0x405; }
    static constexpr millisecond_t period()  { return millisecond_t(5); }
    static constexpr millisecond_t timeout() { return millisecond_t(20); }

    detail::LinePattern pattern;

    FrontLinePattern(const LinePattern& pattern);
    void acquire(LinePattern& pattern) const;

} __attribute__((packed));

struct RearLinePattern {
    static constexpr uint16_t id() { return 0x406; }
    static constexpr millisecond_t period()  { return millisecond_t(5); }
    static constexpr millisecond_t timeout() { return millisecond_t(20); }

    detail::LinePattern pattern;

    RearLinePattern(const LinePattern& pattern);
    void acquire(LinePattern& pattern) const;

} __attribute__((packed));

struct LineDetectControl {
    static constexpr uint16_t id() { return 0x501; }
    static constexpr millisecond_t period()  { return millisecond_t(50); }
    static constexpr millisecond_t timeout() { return millisecond_t(200); }

    bool     indicatorLedsEnabled : 1;
    uint8_t  scanRangeRadius      : 5; // Radius of the line scan (number of sensors to each direction) - 0 means all sensors
    uint8_t  domain               : 1;
    uint8_t  reserved             : 1;

    LineDetectControl(const bool indicatorLedsEnabled, const uint8_t scanRangeRadius, const linePatternDomain_t domain);
    void acquire(bool& indicatorLedsEnabled, uint8_t& scanRangeRadius, linePatternDomain_t& domain) const;

} __attribute__((packed));

struct SetFrontWheelParams : public detail::ServoParams {
    static constexpr uint16_t id() { return 0x601; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::ServoParams::ServoParams;

} __attribute__((packed));

struct SetRearWheelParams : public detail::ServoParams {
    static constexpr uint16_t id() { return 0x602; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::ServoParams::ServoParams;

} __attribute__((packed));

struct SetExtraServoParams : public detail::ServoParams {
    static constexpr uint16_t id() { return 0x603; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::ServoParams::ServoParams;

} __attribute__((packed));

struct SetMotorControlParams : public detail::MotorControlParams {
    static constexpr uint16_t id() { return 0x604; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::MotorControlParams::MotorControlParams;

} __attribute__((packed));

struct FrontWheelParams : public detail::ServoParams {
    static constexpr uint16_t id() { return 0x701; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::ServoParams::ServoParams;

} __attribute__((packed));

struct RearWheelParams : public detail::ServoParams {
    static constexpr uint16_t id() { return 0x702; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::ServoParams::ServoParams;

} __attribute__((packed));

struct ExtraServoParams : public detail::ServoParams {
    static constexpr uint16_t id() { return 0x703; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::ServoParams::ServoParams;

} __attribute__((packed));

struct MotorControlParams : public detail::MotorControlParams {
    static constexpr uint16_t id() { return 0x704; }
    static constexpr millisecond_t period()  { return micro::numeric_limits<millisecond_t>::infinity(); }
    static constexpr millisecond_t timeout() { return micro::numeric_limits<millisecond_t>::infinity(); }

    using detail::MotorControlParams::MotorControlParams;

} __attribute__((packed));

} // namespace can
} // namespace micro

#endif // STM32F4
