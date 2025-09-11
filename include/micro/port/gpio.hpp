#pragma once

#include <micro/utils/types.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#endif

namespace micro {

#if defined STM32

struct gpio_t {
    GPIO_TypeDef* instance = nullptr;
    uint16_t pin           = 0;
};

#else // !STM32

struct gpio_t {};

#endif // !STM32

enum class gpioPinState_t : uint8_t { RESET = 0, SET = 1 };

inline gpioPinState_t operator!(const gpioPinState_t& state) {
    return state == gpioPinState_t::SET ? gpioPinState_t::RESET : gpioPinState_t::SET;
}

Status gpio_read(const gpio_t& gpio, gpioPinState_t& OUT state);
Status gpio_write(const gpio_t& gpio, const gpioPinState_t state);
Status gpio_toggle(const gpio_t& gpio);

} // namespace micro
