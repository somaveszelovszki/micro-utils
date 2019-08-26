#pragma once

#include <micro/hw/Servo.hpp>

namespace micro {
namespace hw {

/* @brief Controls a steering servo motor using a timer configured in PWM mode.
 **/
class SteeringServo : private Servo {
private:
    const radian_t angle_mid;     // The middle position angle.
    const float32_t servoWheelTransition_;

public:
    /* @brief Constructor - initializes timer handle, channel, middle servo angle and delta maximum wheel angle.
     * @param _htim The handle for the timer used for PWM generation.
     * @param _chnl The timer channel used for PWM generation.
     * @param _angle_mid The middle servo angle.
     * @param _wheelAngle_d_max The maximum delta wheel angle.
     **/
    SteeringServo(tim_handle_t _htim, tim_channel_t _chnl, radian_t _angle_mid, radian_t _wheelAngle_d_max, float32_t servoWheelTransition)
        : Servo(_htim, _chnl, _angle_mid - _wheelAngle_d_max / servoWheelTransition, _angle_mid + _wheelAngle_d_max / servoWheelTransition)
        , angle_mid(_angle_mid)
        , servoWheelTransition_(servoWheelTransition) {}

    /* @brief Writes wheel angle - converts value to servo angle and writes it to the PWM pin.
     * @param wheelAngle The wheel angle to write.
     **/
    void writeWheelAngle(radian_t wheelAngle) {
        this->write(this->angle_mid + wheelAngle / this->servoWheelTransition_);
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
        return (this->getAngle() - this->angle_mid) * this->servoWheelTransition_;
    }
};
} // namespace hw
} // namespace micro
