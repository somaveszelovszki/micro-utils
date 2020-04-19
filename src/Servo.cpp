#include <micro/hw/Servo.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {
namespace hw {

void Servo::write(const radian_t angle) {
    this->angle_ = micro::clamp(this->offset_ + angle, this->offset_ - this->maxDelta_, this->offset_ + this->maxDelta_);
    uint32_t pwm = map(this->angle_, radian_t(0), PI, this->pwm0_, this->pwm180_);
    __HAL_TIM_SET_COMPARE(this->htim_, this->chnl_, pwm);
}

} // namespace hw
} // namespace micro
