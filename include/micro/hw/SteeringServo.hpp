#pragma once

#include <micro/hw/Servo.hpp>

namespace micro {
namespace hw {

/* @brief Controls a steering servo motor using a timer configured in PWM mode.
 **/
class SteeringServo : private Servo {

public:
    /* @brief Constructor - initializes timer handle, channel, middle servo angle and delta maximum wheel angle.
     * @param htim The handle for the timer used for PWM generation.
     * @param chnl The timer channel used for PWM generation.
     * @param offset_ The offset.
     * @param wheelAngle_d_max The maximum delta wheel angle.
     **/
    SteeringServo(TIM_HandleTypeDef *htim, uint32_t chnl, radian_t offset_, radian_t wheelAngle_d_max, float servoWheelTransition)
        : Servo(htim, chnl, offset_, wheelAngle_d_max / servoWheelTransition)
        , servoWheelTransition(servoWheelTransition) {}

    /* @brief Writes wheel angle - converts value to servo angle and writes it to the PWM pin.
     * @param wheelAngle The wheel angle to write.
     **/
    void writeWheelAngle(radian_t wheelAngle) {
        this->write(wheelAngle / this->servoWheelTransition);
    }

    /* @brief Gets wheel angle - converts servo angle to wheel angle and returns it.
     * @returns The wheel angle.
     **/
    radian_t wheelAngle() const {
        return this->angle() * this->servoWheelTransition;
    }

private:
    const float servoWheelTransition;
};
} // namespace hw
} // namespace micro
