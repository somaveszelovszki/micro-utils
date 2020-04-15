#pragma once

#include <micro/utils/LinePattern.hpp>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>

namespace micro {
namespace can {

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

    int16_t frontSteeringServoTargetAngle_deg_8p8 : 16;
    int16_t rearSteeringServoTargetAngle_deg_8p8  : 16;
    int16_t extraServoTargetAngle_deg_8p8         : 16;

    LateralControl(const radian_t frontSteeringServoTargetAngle, const radian_t rearSteeringServoTargetAngle, const radian_t extraServoTargetAngle);
    void acquire(radian_t& frontSteeringServoTargetAngle, radian_t& rearSteeringServoTargetAngle, radian_t& extraServoTargetAngle) const;

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

namespace detail {

struct Lines {
    struct {
        int16_t pos_mm_9p4 : 13;
        uint8_t id         : 3;
    } __attribute__((packed)) values[cfg::MAX_NUM_LINES];
} __attribute__((packed));

} // namespace detail

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

    uint16_t frontSteeringServoAngle_deg_8p8 : 16;
    uint16_t rearSteeringServoAngle_deg_8p8  : 16;
    uint16_t extraServoAngle_deg_8p8         : 16;

    LateralState(const radian_t frontSteeringServoAngle, const radian_t rearSteeringServoAngle, const radian_t extraServoAngle);
    void acquire(radian_t& frontSteeringServoAngle, radian_t& rearSteeringServoAngle, radian_t& extraServoAngle) const;

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

namespace detail {

struct LinePattern {
    uint8_t  type         : 4;
    int8_t   dir          : 2;
    int8_t   side         : 2;
    uint32_t startDist_mm : 24;
    bool isPatternPending : 1;
    uint8_t reserved      : 7;
} __attribute__((packed));

} // namespace detail

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

namespace detail {

struct ServoOffsets {
    int16_t frontSteeringServoOffset_deg_8p8 : 16;
    int16_t rearSteeringServoOffset_deg_8p8  : 16;
    int16_t extraServoOffset_deg_8p8         : 16;

    ServoOffsets(const radian_t frontSteeringServoOffset, const radian_t rearSteeringServoOffset, const radian_t extraServoOffset);
    void acquire(radian_t& frontSteeringServoOffset, radian_t& rearSteeringServoOffset, radian_t& extraServoOffset) const;

} __attribute__((packed));

} // namespace detail

struct SetServoOffsets : public detail::ServoOffsets {
    static constexpr uint16_t id() { return 0x601; }
    static constexpr millisecond_t period()  { return millisecond_t::infinity(); }
    static constexpr millisecond_t timeout() { return millisecond_t::infinity(); }

    using detail::ServoOffsets::ServoOffsets;

} __attribute__((packed));

struct ServoOffsets : public detail::ServoOffsets {
    static constexpr uint16_t id() { return 0x602; }
    static constexpr millisecond_t period()  { return millisecond_t::infinity(); }
    static constexpr millisecond_t timeout() { return millisecond_t::infinity(); }

    using detail::ServoOffsets::ServoOffsets;

} __attribute__((packed));

namespace detail {

struct MotorControlParams {

    uint32_t controller_P_8p16   : 24;
    uint32_t controller_I_8p16   : 24;
    uint16_t controller_Imax_8p8 : 16;

    MotorControlParams(const float controller_P, const float controller_I, const float controller_Imax);
    void acquire(float& controller_P, float& controller_I, float& controller_Imax) const;

} __attribute__((packed));

} // namespace detail

struct SetMotorControlParams : public detail::MotorControlParams {
    static constexpr uint16_t id() { return 0x603; }
    static constexpr millisecond_t period()  { return millisecond_t::infinity(); }
    static constexpr millisecond_t timeout() { return millisecond_t::infinity(); }

    using detail::MotorControlParams::MotorControlParams;

} __attribute__((packed));

struct MotorControlParams : public detail::MotorControlParams {
    static constexpr uint16_t id() { return 0x604; }
    static constexpr millisecond_t period()  { return millisecond_t::infinity(); }
    static constexpr millisecond_t timeout() { return millisecond_t::infinity(); }

    using detail::MotorControlParams::MotorControlParams;

} __attribute__((packed));

} // namespace can
} // namespace micro
