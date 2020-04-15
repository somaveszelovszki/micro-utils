#include <micro/panel/vehicleCanTypes.hpp>
#include <micro/math/unit_utils.hpp>

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

micro::can::detail::Lines convert(const micro::Lines& in) {

    micro::can::detail::Lines out;
    uint32_t i = 0;

    for (; i < in.size(); ++i) {
        out.values[i].pos_mm_9p4 = static_cast<int16_t>(in[i].pos.get() * SHIFT_4);
        out.values[i].id = in[i].id;
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
        millimeter_t(in.startDist_mm),
        in.isPatternPending
    };
}

micro::can::detail::LinePattern convert(const micro::LinePattern& in) {
    return micro::can::detail::LinePattern{
        static_cast<uint8_t>(in.type),
        static_cast<int8_t>(in.dir),
        static_cast<int8_t>(in.side),
        static_cast<uint32_t>(static_cast<millimeter_t>(in.startDist).get()),
        in.isPending,
        0 // reserved
    };
}

ServoOffsets::ServoOffsets(const radian_t frontSteeringServoOffset, const radian_t rearSteeringServoOffset, const radian_t extraServoOffset)
    : frontSteeringServoOffset_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(frontSteeringServoOffset).get() * detail::SHIFT_8))
    , rearSteeringServoOffset_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(rearSteeringServoOffset).get() * detail::SHIFT_8))
    , extraServoOffset_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(extraServoOffset).get() * detail::SHIFT_8)) {}

void ServoOffsets::acquire(radian_t& frontSteeringServoOffset, radian_t& rearSteeringServoOffset, radian_t& extraServoOffset) const {
    frontSteeringServoOffset = degree_t(this->frontSteeringServoOffset_deg_8p8 / detail::SHIFT_8);
    rearSteeringServoOffset  = degree_t(this->rearSteeringServoOffset_deg_8p8 / detail::SHIFT_8);
    extraServoOffset         = degree_t(this->extraServoOffset_deg_8p8 / detail::SHIFT_8);
}

MotorControlParams::MotorControlParams(const float controller_P, const float controller_I, const float controller_Imax)
    : controller_P_8p16(static_cast<uint32_t>(controller_P * detail::SHIFT_16))
    , controller_I_8p16(static_cast<uint32_t>(controller_I * detail::SHIFT_16))
    , controller_Imax_8p8(static_cast<uint32_t>(controller_Imax * detail::SHIFT_8)) {}

void MotorControlParams::acquire(float& controller_P, float& controller_I, float& controller_Imax) const {
    controller_P    = this->controller_P_8p16 / detail::SHIFT_16;
    controller_I    = this->controller_I_8p16 / detail::SHIFT_16;
    controller_Imax = this->controller_Imax_8p8 / detail::SHIFT_8;
}

} // namespace detail

LateralControl::LateralControl(const radian_t frontSteeringServoTargetAngle, const radian_t rearSteeringServoTargetAngle, const radian_t extraServoTargetAngle)
    : frontSteeringServoTargetAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(frontSteeringServoTargetAngle).get() * detail::SHIFT_8))
    , rearSteeringServoTargetAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(rearSteeringServoTargetAngle).get() * detail::SHIFT_8))
    , extraServoTargetAngle_deg_8p8(static_cast<int16_t>(static_cast<degree_t>(extraServoTargetAngle).get() * detail::SHIFT_8)) {}

void LateralControl::acquire(radian_t& frontSteeringServoTargetAngle, radian_t& rearSteeringServoTargetAngle, radian_t& extraServoTargetAngle) const {
    frontSteeringServoTargetAngle = degree_t(this->frontSteeringServoTargetAngle_deg_8p8 / detail::SHIFT_8);
    rearSteeringServoTargetAngle  = degree_t(this->rearSteeringServoTargetAngle_deg_8p8 / detail::SHIFT_8);
    extraServoTargetAngle         = degree_t(this->extraServoTargetAngle_deg_8p8 / detail::SHIFT_8);
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

LateralState::LateralState(const radian_t frontSteeringServoAngle, const radian_t rearSteeringServoAngle, const radian_t extraServoAngle)
    : frontSteeringServoAngle_deg_8p8(static_cast<uint16_t>(static_cast<degree_t>(frontSteeringServoAngle).get() * detail::SHIFT_8))
    , rearSteeringServoAngle_deg_8p8(static_cast<uint16_t>(static_cast<degree_t>(rearSteeringServoAngle).get() * detail::SHIFT_8))
    , extraServoAngle_deg_8p8(static_cast<uint16_t>(static_cast<degree_t>(extraServoAngle).get() * detail::SHIFT_8)) {}

void LateralState::acquire(radian_t& frontSteeringServoAngle, radian_t& rearSteeringServoAngle, radian_t& extraServoAngle) const {
    frontSteeringServoAngle = degree_t(this->frontSteeringServoAngle_deg_8p8 / detail::SHIFT_8);
    rearSteeringServoAngle  = degree_t(this->rearSteeringServoAngle_deg_8p8 / detail::SHIFT_8);
    extraServoAngle         = degree_t(this->extraServoAngle_deg_8p8 / detail::SHIFT_8);
}

LongitudinalState::LongitudinalState(const m_per_sec_t speed, const meter_t distance)
    : speed_mmps(static_cast<int16_t>(static_cast<mm_per_sec_t>(speed).get()))
    , reserved(0)
    , distance_mm(static_cast<uint32_t>(static_cast<millimeter_t>(distance).get())) {}

void LongitudinalState::acquire(m_per_sec_t& speed, meter_t& distance) const {
    speed    = mm_per_sec_t(this->speed_mmps);
    distance = millimeter_t(this->distance_mm);
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
