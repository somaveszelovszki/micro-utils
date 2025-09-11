
#include <micro/debug/DebugLed.hpp>

namespace micro {

DebugLed::DebugLed(const gpio_t& gpio) : gpio_(gpio), timer_(period_NOK() / 2) {
}

void DebugLed::update(const bool isOk) {
    this->timer_.setPeriod((isOk ? period_OK() : period_NOK()) / 2);
    if (this->timer_.checkTimeout()) {
        gpio_toggle(this->gpio_);
    }
}

} // namespace micro
