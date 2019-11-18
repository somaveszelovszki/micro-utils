#include <micro/hw/Servo.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {
namespace hw {

constexpr uint32_t PWM_DUTY_0  = 800;      // PWM duty for 0 radians (timer period is 20000)
constexpr uint32_t PWM_DUTY_PI = 2200;     // PWM duty for PI radians (timer period is 20000)

void Servo::write(radian_t angle) {
    const radian_t clamped = micro::clamp(angle, this->angle_min, this->angle_max);

    if (this->angle_ != clamped) {
        this->angle_ = clamped;
        uint32_t pwm = map(this->angle_, radian_t::zero(), PI, PWM_DUTY_0, PWM_DUTY_PI);
        __HAL_TIM_SET_COMPARE(this->htim, this->chnl, pwm);
    }
}

} // namespace hw
} // namespace micro
