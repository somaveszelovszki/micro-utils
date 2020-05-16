#pragma once

#include <micro/port/hal.h>
#include <micro/utils/timer.hpp>

#if defined STM32F0
#include <stm32f0xx_hal_gpio.h>
#elif defined STM32F4
#include <stm32f4xx_hal_gpio.h>
#endif

namespace micro {

class DebugLed {
public:
    static constexpr millisecond_t period_OK()  { return millisecond_t(500); }
    static constexpr millisecond_t period_NOK() { return millisecond_t(1000); }

    DebugLed(GPIO_TypeDef *gpio, uint16_t gpioPin);

    void update(const bool isOk);

private:
    GPIO_TypeDef * const gpio_;
    const uint16_t gpioPin_;
    Timer timer_;
};

} // namespace micro
