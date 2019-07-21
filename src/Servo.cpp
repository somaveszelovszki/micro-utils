#include <micro/hw/Servo.hpp>
#include <micro/bsp/tim.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/unit_utils.hpp>

namespace micro {

namespace {
constexpr uint32_t PWM_DUTY_0  = 800;      // PWM duty for 0 radians (timer period is 20000)
constexpr uint32_t PWM_DUTY_PI = 2200;     // PWM duty for PI radians (timer period is 20000)
} // namespace

void hw::Servo::write(radian_t _ang) {
    _ang = micro::clamp(_ang, this->angle_min, this->angle_max);

    if (this->_angle != _ang) {
        this->_angle = _ang;
        uint32_t pwm = micro::scale(this->_angle.get(), 0.0f, PI.get(), PWM_DUTY_0, PWM_DUTY_PI);
        micro::writePWM(this->htim, this->chnl, pwm);
    }
}

} // namespace micro
