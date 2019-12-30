#include <micro/hw/Servo.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {
namespace hw {

void Servo::write(radian_t angle) {
    const radian_t clamped = micro::clamp(this->offset_ + angle, this->offset_ - this->max_delta_, this->offset_ + this->max_delta_);
    this->angle_ = clamped - this->offset_;
    uint32_t pwm = map(clamped, radian_t::zero(), PI, this->pwm0_, this->pwm180_);
    __HAL_TIM_SET_COMPARE(this->htim_, this->chnl_, pwm);
}

} // namespace hw
} // namespace micro
