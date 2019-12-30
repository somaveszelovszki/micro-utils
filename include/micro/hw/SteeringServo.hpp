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
     * @param pwm0 The PWM value for 0 degrees.
     * @param pwm180 The PWM value for 180 degrees.
     * @param offset_ The offset.
     * @param wheelAngle_d_max The maximum delta wheel angle.
     * @param servoWheelTransferRate The transfer rate between the servo and the wheels.
     **/
    SteeringServo(TIM_HandleTypeDef *htim, uint32_t chnl, uint32_t pwm0, uint32_t pwm180, radian_t offset_, radian_t wheelAngle_d_max, float servoWheelTransferRate)
        : Servo(htim, chnl, pwm0, pwm180, offset_, wheelAngle_d_max / servoWheelTransferRate)
        , servoWheelTransferRate_(servoWheelTransferRate) {}

    /* @brief Writes wheel angle - converts value to servo angle and writes it to the PWM pin.
     * @param wheelAngle The wheel angle to write.
     **/
    void writeWheelAngle(radian_t wheelAngle) {
        this->write(wheelAngle / this->servoWheelTransferRate_);
    }

    /* @brief Gets wheel angle - converts servo angle to wheel angle and returns it.
     * @returns The wheel angle.
     **/
    radian_t wheelAngle() const {
        return this->angle() * this->servoWheelTransferRate_;
    }

private:
    const float servoWheelTransferRate_;
};
} // namespace hw
} // namespace micro
