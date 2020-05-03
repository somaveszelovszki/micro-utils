#pragma once

#include <micro/port/hal.h>
#include <micro/utils/units.hpp>

#if defined STM32F0
#include <stm32f0xx_hal_tim.h>
#elif defined STM32F4
#include <stm32f4xx_hal_tim.h>
#endif

namespace micro {
namespace hw {
/* @brief Controls a DC motor using a timer configured in PWM mode.
 **/
class Encoder {

public:
    Encoder(TIM_HandleTypeDef *htim);

    void update();

    int64_t absPos() const {
        return this->absPos_;
    }

    uint64_t numIncr() const {
        return this->numIncr_;
    }

    int32_t lastDiff() const {
        return this->lastDiff_;
    }

private:
    TIM_HandleTypeDef *htim_;
    int64_t absPos_;
    uint64_t numIncr_;
    uint32_t prevPos_;
    int32_t lastDiff_;
};
} // namespace hw
} // namespace micro
