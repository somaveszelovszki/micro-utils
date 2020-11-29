#include <micro/hw/ServoMotor.hpp>

namespace micro {
namespace hw {

Servo::Servo(const micro::timer_t& timer, uint32_t chnl, uint32_t pwmCenter, const radian_t transferRate, radian_t maxAngle, rad_per_sec_t maxAngVel)
    : ServoInterfaceMotor<radian_t>(timer, chnl, pwmCenter, transferRate, maxAngle)
    , maxAngVel_(maxAngVel) {}

radian_t Servo::angle() const {
    return this->actualAngle_;
}

void Servo::write(const radian_t& angle) {

    // updates actual angle
    const microsecond_t now    = getExactTime();
    const radian_t maxDiff     = this->maxAngVel_ * (now - this->prevAngleUpdateTime_);
    this->actualAngle_        += clamp(this->value() - this->actualAngle_, -maxDiff, maxDiff);
    this->prevAngleUpdateTime_ = now;

    ServoInterfaceMotor<radian_t>::write(angle);
}

} // namespace hw
} // namespace micro
