#pragma once

#include <micro/utils/units.hpp>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>

namespace micro {
namespace hw {
/* @brief Controls a servo motor using a timer configured in PWM mode.
 **/
class Servo {

public:
    /* @brief Constructor - sets timer handle, timer channel, minimum and maximum angles.
     * @param htim The handle for the timer used for PWM generation.
     * @param chnl The timer channel used for PWM generation.
     * @param offset_ The offset.
     * @param max_delta The maximum delta angle.
     **/
    Servo(TIM_HandleTypeDef *htim, uint32_t chnl, radian_t offset_, radian_t max_delta_)
        : htim(htim)
        , chnl(chnl)
        , offset_(offset_)
        , max_delta_(max_delta_) {}

    radian_t offset() const {
        return offset_;
    }

    /* @brief Writes angle to the servo motor - converts value to PWM duty, and writes PWM pin.
     * @param _ang The angle to write.
     **/
    void write(radian_t angle);

    /* @brief Gets servo angle.
     * @returns The servo angle.
     **/
    radian_t angle() const {
        return this->angle_;
    }

private:
    TIM_HandleTypeDef *htim;  // The handle for the timer used for PWM generation.
    const uint32_t chnl;      // The timer channel used for PWM generation.

    const radian_t offset_;   // The offset.
    radian_t max_delta_;      // The maximum delta angle.
    radian_t angle_;          // The current angle.
};
} // namespace hw
} // namespace micro
