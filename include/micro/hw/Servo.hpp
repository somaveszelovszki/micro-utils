#pragma once

#include <micro/utils/units.hpp>

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#endif

namespace micro {
namespace hw {
/* @brief Controls a servo motor using a timer configured in PWM mode.
 **/
class Servo {

public:
    /* @brief Constructor - sets timer handle, timer channel, minimum and maximum angles.
     * @param htim The handle for the timer used for PWM generation.
     * @param chnl The timer channel used for PWM generation.
     * @param pwm0 The PWM value for 0 degrees.
     * @param pwm180 The PWM value for 180 degrees.
     * @param offset The offset.
     * @param maxDelta The maximum delta angle.
     **/
    Servo(TIM_HandleTypeDef *htim, uint32_t chnl, uint32_t pwm0, uint32_t pwm180, radian_t offset, radian_t maxDelta)
        : htim_(htim)
        , chnl_(chnl)
        , pwm0_(pwm0)
        , pwm180_(pwm180)
        , offset_(offset)
        , maxDelta_(maxDelta) {}

    radian_t offset() const {
        return offset_;
    }

    radian_t maxDelta() const {
        return this->maxDelta_;
    }

    radian_t angle() const {
        return this->angle_ - this->offset_;
    }

    void setOffset(const radian_t offset) {
        this->offset_ = offset;
    }

    void setMaxDelta(const radian_t maxDelta) {
        this->maxDelta_ = maxDelta;
    }

    void write(const radian_t angle);

private:
    TIM_HandleTypeDef *htim_;  // The handle for the timer used for PWM generation.
    const uint32_t chnl_;      // The timer channel used for PWM generation.
    const uint32_t pwm0_;      // The PWM value for 0 degrees.
    const uint32_t pwm180_;    // The PWM value for 180 degrees.

    radian_t offset_;          // The offset.
    radian_t maxDelta_;        // The maximum delta angle.
    radian_t angle_;           // The current angle.
};
} // namespace hw
} // namespace micro
