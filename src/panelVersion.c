#include <micro/panel/panelVersion.h>

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#endif

extern void Error_Handler(void);

#define NUM_VERSION_BITS 4

typedef struct {
    GPIO_TypeDef *gpio;
    uint16_t pin;
} gpio_t;

void panelVersion_check(const uint8_t expected) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

#ifdef STM32F4
    gpio_t PINS[NUM_VERSION_BITS] = {
        { GPIOB, GPIO_PIN_12 },
        { GPIOB, GPIO_PIN_13 },
        { GPIOB, GPIO_PIN_14 },
        { GPIOB, GPIO_PIN_15 }
    };

    __HAL_RCC_GPIOB_CLK_ENABLE();
#endif

    for (uint8_t i = 0; i < NUM_VERSION_BITS; ++i) {
        GPIO_InitStruct.Pin = PINS[i].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(PINS[i].gpio, &GPIO_InitStruct);
    }

    uint8_t version = 0;
    for (uint8_t i = 0; i < NUM_VERSION_BITS; ++i) {
        version |= ((HAL_GPIO_ReadPin(PINS[i].gpio, PINS[i].pin) == GPIO_PIN_SET ? 1 : 0) << i);
    }

    if (expected != version) {
        Error_Handler();
    }
}
