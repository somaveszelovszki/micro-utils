#include <micro/hw/HC_SR04_Ultrasonic.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/port/timer.hpp>

namespace micro {
namespace hw {

HC_SR04_Ultrasonic::HC_SR04_Ultrasonic(const gpio_t& gpio_trigger, const gpio_t& gpio_echo)
    : gpio_trigger_(gpio_trigger), gpio_echo_(gpio_echo) {
}

void HC_SR04_Ultrasonic::initialize() {
    gpio_write(this->gpio_trigger_, gpioPinState_t::RESET);
}

meter_t HC_SR04_Ultrasonic::readDistance() {
    static constexpr microsecond_t ECHO_OVERHEAD = microsecond_t(450); // Overhead before echo.
    static constexpr meter_t MAX_DIST            = meter_t(2);
    static constexpr m_per_sec_t SOUND_SPEED     = m_per_sec_t(343);

    meter_t distance = micro::numeric_limits<meter_t>::infinity();

    const microsecond_t triggerTime = getExactTime();
    gpio_write(this->gpio_trigger_, gpioPinState_t::SET);
    while (getExactTime() - triggerTime < microsecond_t(10)) {
    }
    gpio_write(this->gpio_trigger_, gpioPinState_t::RESET);

    if (this->echoSemaphore_.take(millisecond_t(20))) {
        if (this->echoTime_ - triggerTime > ECHO_OVERHEAD) {
            distance = (this->echoTime_ - triggerTime - ECHO_OVERHEAD) * SOUND_SPEED / 2;
        }

        if (distance > MAX_DIST) {
            distance = micro::numeric_limits<meter_t>::infinity();
        }
    }

    return distance;
}

void HC_SR04_Ultrasonic::onEchoReceived() {
    this->echoTime_ = micro::getExactTime();
    this->echoSemaphore_.give();
}

} // namespace hw
} // namespace micro
