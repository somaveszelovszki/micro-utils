#include <micro/panel/vehicleCanTypes.hpp>
#include <micro/utils/types.hpp>

namespace micro {
namespace can {
namespace detail {

constexpr float SHIFT_1  = 2.0f;
constexpr float SHIFT_2  = 4.0f;
constexpr float SHIFT_3  = 8.0f;
constexpr float SHIFT_4  = 16.0f;
constexpr float SHIFT_5  = 32.0f;
constexpr float SHIFT_6  = 64.0f;
constexpr float SHIFT_7  = 128.0f;
constexpr float SHIFT_8  = 256.0f;
constexpr float SHIFT_16 = 65536.0f;
constexpr float SHIFT_24 = 16777216.0f;

micro::can::detail::Lines convert(const micro::Lines& in) {

    micro::can::detail::Lines out;
    uint32_t i = 0;

    for (const auto& line : in) {
        out.values[i].pos_mm_9p4 = static_cast<int16_t>(line.pos.get() * SHIFT_4);
        out.values[i].id = line.id;
        i++;
    }

    for (; i < ARRAY_SIZE(out.values); ++i) {
        out.values[i].pos_mm_9p4 = 0;
        out.values[i].id = 0;
    }

    return out;
}

micro::Lines convert(const micro::can::detail::Lines& in) {

    micro::Lines out;

    for (uint8_t i = 0; i < ARRAY_SIZE(in.values); ++i) {
        if (in.values[i].id != 0) {
            out.insert(Line{ millimeter_t(in.values[i].pos_mm_9p4 / SHIFT_4), in.values[i].id });
        } else {
            break;
        }
    }

    return out;
}

micro::LinePattern convert(const micro::can::detail::LinePattern& in) {
    return micro::LinePattern{
        static_cast<micro::LinePattern::type_t>(in.type),
        static_cast<micro::Sign>(in.dir),
        static_cast<micro::Direction>(in.side),
        millimeter_t(in.startDist_mm)
    };
}

micro::can::detail::LinePattern convert(const micro::LinePattern& in) {
    return micro::can::detail::LinePattern{
        static_cast<uint8_t>(in.type),
        static_cast<int8_t>(in.dir),
        static_cast<int8_t>(in.side),
        static_cast<uint32_t>(static_cast<millimeter_t>(in.startDist).get()),
        0 // reserved
    };
}

MotorControlParams::MotorControlParams(const float controller_P, const float controller_I)
    : controller_P_8p24(static_cast<uint32_t>(controller_P * detail::SHIFT_24))
    , controller_I_8p24(static_cast<uint32_t>(controller_I * detail::SHIFT_24)) {}

void MotorControlParams::acquire(float& controller_P, float& controller_I) const {
    controller_P = this->controller_P_8p24 / detail::SHIFT_24;
    controller_I = this->controller_I_8p24 / detail::SHIFT_24;
}

} // namespace detail

LateralControl::LateralControl(const radian_t frontWheelTargetAngle, const radian_t rearWheelTargetAngle, const radian_t extraServoTargetAngle)
    : frontWheelTargetAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(frontWheelTargetAngle).get() * detail::SHIFT_8))
    , rearWheelTargetAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(rearWheelTargetAngle).get() * detail::SHIFT_8))
    , extraServoTargetAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(extraServoTargetAngle).get() * detail::SHIFT_8)) {}

void LateralControl::acquire(radian_t& frontWheelTargetAngle, radian_t& rearWheelTargetAngle, radian_t& extraServoTargetAngle) const {
    frontWheelTargetAngle = degree_t(this->frontWheelTargetAngle_deg_8p8 / detail::SHIFT_8);
    rearWheelTargetAngle  = degree_t(this->rearWheelTargetAngle_deg_8p8 / detail::SHIFT_8);
    extraServoTargetAngle = degree_t(this->extraServoTargetAngle_deg_8p8 / detail::SHIFT_8);
}

LongitudinalControl::LongitudinalControl(const m_per_sec_t targetSpeed, const bool useSafetyEnableSignal, const millisecond_t targetSpeedRampTime)
    : targetSpeed_mmps(static_cast<int16_t>(static_cast<mm_per_sec_t>(targetSpeed).get()))
    , useSafetyEnableSignal(useSafetyEnableSignal)
    , targetSpeedRampTime_ms(static_cast<uint16_t>(targetSpeedRampTime.get())) {}

void LongitudinalControl::acquire(m_per_sec_t& targetSpeed, bool& useSafetyEnableSignal, millisecond_t& targetSpeedRampTime) const {
    targetSpeed           = mm_per_sec_t(this->targetSpeed_mmps);
    useSafetyEnableSignal = this->useSafetyEnableSignal;
    targetSpeedRampTime   = millisecond_t(this->targetSpeedRampTime_ms);
}

FrontLines::FrontLines(const Lines& lines)
    : lines(detail::convert(lines)) {}

void FrontLines::acquire(Lines& lines) const {
    lines = detail::convert(this->lines);
}

RearLines::RearLines(const Lines& lines)
    : lines(detail::convert(lines)) {}

void RearLines::acquire(Lines& lines) const {
    lines = detail::convert(this->lines);
}

LateralState::LateralState(const radian_t frontWheelAngle, const radian_t rearWheelAngle, const radian_t extraServoAngle)
    : frontWheelAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(frontWheelAngle).get() * detail::SHIFT_8))
    , rearWheelAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(rearWheelAngle).get() * detail::SHIFT_8))
    , extraServoAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(extraServoAngle).get() * detail::SHIFT_8)) {}

void LateralState::acquire(radian_t& frontWheelAngle, radian_t& rearWheelAngle, radian_t& extraServoAngle) const {
    frontWheelAngle = degree_t(this->frontWheelAngle_deg_8p8 / detail::SHIFT_8);
    rearWheelAngle  = degree_t(this->rearWheelAngle_deg_8p8 / detail::SHIFT_8);
    extraServoAngle = degree_t(this->extraServoAngle_deg_8p8 / detail::SHIFT_8);
}

LongitudinalState::LongitudinalState(const m_per_sec_t speed, const bool remoteControlled, const meter_t distance)
    : speed_mmps(static_cast<int16_t>(static_cast<mm_per_sec_t>(speed).get()))
    , remoteControlled(remoteControlled)
    , distance_mm(static_cast<uint32_t>(static_cast<millimeter_t>(distance).get())) {}

void LongitudinalState::acquire(m_per_sec_t& speed, bool& remoteControlled, meter_t& distance) const {
    speed            = mm_per_sec_t(this->speed_mmps);
    remoteControlled = this->remoteControlled;
    distance         = millimeter_t(this->distance_mm);
}

FrontLinePattern::FrontLinePattern(const LinePattern& pattern)
    : pattern(detail::convert(pattern)) {}

void FrontLinePattern::acquire(LinePattern& pattern) const {
    pattern = detail::convert(this->pattern);
}

RearLinePattern::RearLinePattern(const LinePattern& pattern)
    : pattern(detail::convert(pattern)) {}

void RearLinePattern::acquire(LinePattern& pattern) const {
    pattern = detail::convert(this->pattern);
}

LineDetectControl::LineDetectControl(const bool indicatorLedsEnabled, const uint8_t scanRangeRadius, const linePatternDomain_t domain)
    : indicatorLedsEnabled(indicatorLedsEnabled)
    , scanRangeRadius(scanRangeRadius)
    , domain(static_cast<uint8_t>(domain))
    , reserved(0) {}

void LineDetectControl::acquire(bool& indicatorLedsEnabled, uint8_t& scanRangeRadius, linePatternDomain_t& domain) const {
    indicatorLedsEnabled = this->indicatorLedsEnabled;
    scanRangeRadius      = this->scanRangeRadius;
    domain               = static_cast<linePatternDomain_t>(this->domain);
}

} // namespace can
} // namespace micro
