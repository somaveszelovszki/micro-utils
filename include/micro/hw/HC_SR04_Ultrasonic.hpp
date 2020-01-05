#pragma once

#include <micro/utils/units.hpp>

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#endif

namespace micro {
namespace hw {

class HC_SR04_Ultrasonic {

public:
    HC_SR04_Ultrasonic(GPIO_TypeDef *gpio_trigger, uint16_t gpioPin_trigger, GPIO_TypeDef *gpio_echo, uint16_t gpioPin_echo);

    void initialize();

    void startMeasurement();

    void onEchoReceived();

    centimeter_t getDistance() const {
        return this->distance;
    }

private:
    GPIO_TypeDef *gpio_trigger;
    uint16_t gpioPin_trigger;
    GPIO_TypeDef *gpio_echo;
    uint16_t gpioPin_echo;

    microsecond_t lastStartTime;
    bool busy;
    centimeter_t distance;
};

} // namespace hw
} // namespace micro
