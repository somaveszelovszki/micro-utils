#include <micro/hw/Servo.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/utils/timer.hpp>

namespace micro {
namespace hw {

radian_t Servo::angle() {
    const microsecond_t now = getExactTime();
    const radian_t maxDiff  = this->maxAngVel_ * (now - this->prevAngleUpdateTime_);
    this->angle_ += clamp(this->targetAngle_ - this->angle_, -maxDiff, maxDiff);

    this->prevAngleUpdateTime_ = now;
    return this->angle_;
}

void Servo::write(const radian_t angle) {
    this->targetAngle_ = micro::clamp(angle, -this->maxAngle_, this->maxAngle_);
    const uint32_t pwm = this->pwmCenter_ + this->targetAngle_ / this->transferRate_;
    timer_setCompare(this->timer_, this->chnl_, pwm);
}

} // namespace hw
} // namespace micro
