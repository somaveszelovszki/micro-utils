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
    return this->angle_ - this->offset_;
}

void Servo::write(const radian_t angle) {
    this->targetAngle_ = micro::clamp(this->offset_ + angle, this->offset_ - this->maxDelta_, this->offset_ + this->maxDelta_);
    uint32_t pwm = map(this->targetAngle_, radian_t(0), PI, this->pwm0_, this->pwm180_);
    __HAL_TIM_SET_COMPARE(this->htim_, this->chnl_, pwm);
}

} // namespace hw
} // namespace micro
