
#include <micro/debug/DebugLed.hpp>

namespace micro {

DebugLed::DebugLed(GPIO_TypeDef *gpio, uint16_t gpioPin)
    : gpio_(gpio)
    , gpioPin_(gpioPin)
    , timer_(period_NOK() / 2) {}

void DebugLed::update(const bool isOk) {
    this->timer_.setPeriod((isOk ? period_OK() : period_NOK()) / 2);
    if (this->timer_.checkTimeout()) {
        HAL_GPIO_TogglePin(this->gpio_, this->gpioPin_);
    }
}

} // namespace micro
