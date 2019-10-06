#pragma once

#include <micro/hw/Servo.hpp>

namespace micro {
namespace hw {

/* @brief Controls a steering servo motor using a timer configured in PWM mode.
 **/
class SteeringServo : private Servo {
private:
    const radian_t angle_mid;     // The middle position angle.
    const float servoWheelTransition;

public:
    /* @brief Constructor - initializes timer handle, channel, middle servo angle and delta maximum wheel angle.
     * @param htim The handle for the timer used for PWM generation.
     * @param chnl The timer channel used for PWM generation.
     * @param angle_mid The middle servo angle.
     * @param wheelAngle_d_max The maximum delta wheel angle.
     **/
    SteeringServo(TIM_HandleTypeDef *htim, uint32_t chnl, radian_t angle_mid, radian_t wheelAngle_d_max, float servoWheelTransition)
        : Servo(htim, chnl, angle_mid - wheelAngle_d_max / servoWheelTransition, angle_mid + wheelAngle_d_max / servoWheelTransition)
        , angle_mid(angle_mid)
        , servoWheelTransition(servoWheelTransition) {}

    /* @brief Writes wheel angle - converts value to servo angle and writes it to the PWM pin.
     * @param wheelAngle The wheel angle to write.
     **/
    void writeWheelAngle(radian_t wheelAngle) {
        this->write(this->angle_mid + wheelAngle / this->servoWheelTransition);
    }

    /* @brief Writes the middle angle to the servo, thus positioning the wheels middle.
     **/
    void positionMiddle() {
        this->write(this->angle_mid);
    }

    /* @brief Gets wheel angle - converts servo angle to wheel angle and returns it.
     * @returns The wheel angle.
     **/
    radian_t getWheelAngle() const {
        return (this->getAngle() - this->angle_mid) * this->servoWheelTransition;
    }
};
} // namespace hw
} // namespace micro
