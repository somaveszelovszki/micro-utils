#include <micro/panel/panelVersion.hpp>
#include <micro/port/gpio.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rcc.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_rcc.h>
#endif

namespace micro {

constexpr uint8_t NUM_VERSION_BITS = 4;

panelVersion_t version = 0xff;

panelVersion_t getPanelVersion(void) {
#if defined STM32
    if (0xff == version) {
        GPIO_InitTypeDef GPIO_InitStruct = {0};

#if defined STM32F0
        gpio_t PINS[NUM_VERSION_BITS] = {
            micro::gpio_t{ GPIOA, GPIO_PIN_0  },
            micro::gpio_t{ GPIOA, GPIO_PIN_1  },
            micro::gpio_t{ GPIOA, GPIO_PIN_12 },
            micro::gpio_t{ GPIOA, GPIO_PIN_15 }
        };
        __HAL_RCC_GPIOA_CLK_ENABLE();

#elif defined STM32F4
        gpio_t PINS[NUM_VERSION_BITS] = {
            micro::gpio_t{ GPIOB, GPIO_PIN_12 },
            micro::gpio_t{ GPIOB, GPIO_PIN_13 },
            micro::gpio_t{ GPIOB, GPIO_PIN_14 },
            micro::gpio_t{ GPIOB, GPIO_PIN_15 }
        };
        __HAL_RCC_GPIOB_CLK_ENABLE();
#endif

        for (uint8_t i = 0; i < NUM_VERSION_BITS; ++i) {
            GPIO_InitStruct.Pin = PINS[i].pin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(reinterpret_cast<GPIO_TypeDef*>(PINS[i].instance), &GPIO_InitStruct);
        }

        version = 0;
        for (uint8_t i = 0; i < NUM_VERSION_BITS; ++i) {
            gpioPinState_t pinState;
            gpio_read(PINS[i], pinState);
            version |= ((pinState == gpioPinState_t::SET ? 1 : 0) << i);
        }
    }
#endif // STM2

    return version;
}

} // namespace micro
