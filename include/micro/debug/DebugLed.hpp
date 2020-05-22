#pragma once

#include <micro/port/gpio.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

class DebugLed {
public:
    static constexpr millisecond_t period_OK()  { return millisecond_t(500); }
    static constexpr millisecond_t period_NOK() { return millisecond_t(1000); }

    explicit DebugLed(const gpio_t& gpio);

    void update(const bool isOk);

private:
    const gpio_t gpio_;
    Timer timer_;
};

} // namespace micro
