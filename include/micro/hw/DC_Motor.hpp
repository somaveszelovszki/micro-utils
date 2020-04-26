#pragma once

#include <micro/utils/units.hpp>

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_tim.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#endif

namespace micro {
namespace hw {
/* @brief Controls a DC motor using a timer configured in PWM mode.
 **/
class DC_Motor {

public:
    DC_Motor(TIM_HandleTypeDef *htim, const uint32_t chnl1, const uint32_t chnl2, const float maxDuty);

    void write(float duty);

private:
    TIM_HandleTypeDef *htim_;
    const uint32_t chnl1_;
    const uint32_t chnl2_;
    const float maxDuty_;
};
} // namespace hw
} // namespace micro
