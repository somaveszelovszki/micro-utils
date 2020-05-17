#pragma once

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#endif

#if !defined STM32

#define GPIO_PIN_RESET          0
#define GPIO_PIN_SET            1
#define GPIO_TypeDef            void
#define HAL_GPIO_ReadPin(...)   GPIO_PIN_RESET
#define HAL_GPIO_WritePin(...)
#define HAL_GPIO_TogglePin(...)

#endif // STM32

namespace micro {

struct gpio_t {
    GPIO_TypeDef *instance;
    uint16_t pin;
};

} // namespace micro

