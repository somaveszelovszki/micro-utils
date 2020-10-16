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
constexpr uint8_t VERSION_INVALID  = 0xff;

panelVersion_t version = VERSION_INVALID;

panelVersion_t getPanelVersion(void) {
#if defined STM32
    if (VERSION_INVALID == version) {
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

        panelVersion_t candidates[3];
        for (uint8_t c = 0; c < 3; ++c) {
            candidates[c] = 0;
            for (uint8_t b = 0; b < NUM_VERSION_BITS; ++b) {
                gpioPinState_t pinState;
                gpio_read(PINS[b], pinState);
                candidates[c] |= ((pinState == gpioPinState_t::SET ? 1 : 0) << b);
            }
        }

        version = candidates[0] == candidates[1] ? candidates[0] :
                  candidates[0] == candidates[2] ? candidates[0] :
                  candidates[1] == candidates[2] ? candidates[1] :
                  VERSION_INVALID;
    }
#endif // STM2

    return version;
}

} // namespace micro
