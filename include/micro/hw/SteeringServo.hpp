#pragma once

#include <micro/hw/Servo.hpp>

namespace micro {
namespace hw {

/* @brief Controls a steering servo motor using a timer configured in PWM mode.
 **/
class SteeringServo : private Servo {

public:
    SteeringServo(const timer_t& timer, uint32_t chnl, uint32_t pwm0, uint32_t pwm180, rad_per_sec_t maxAngVel, radian_t wheelOffset, radian_t wheelAngle_d_max, float servoWheelTransferRate)
        : Servo(timer, chnl, pwm0, pwm180, maxAngVel, wheelOffset / servoWheelTransferRate, wheelAngle_d_max / servoWheelTransferRate)
        , servoWheelTransferRate_(servoWheelTransferRate) {}

    radian_t wheelOffset() const {
        return this->offset() * this->servoWheelTransferRate_;
    }

    radian_t wheelMaxDelta() const {
        return this->maxDelta() * this->servoWheelTransferRate_;
    }

    /* @brief Gets wheel angle - converts servo angle to wheel angle and returns it.
     * @returns The wheel angle.
     **/
    radian_t wheelAngle() {
        return this->angle() * this->servoWheelTransferRate_;
    }

    void setWheelOffset(const radian_t wheelOffset) {
        this->setOffset(wheelOffset / this->servoWheelTransferRate_);
    }

    void setWheelMaxDelta(const radian_t wheelMaxDelta) {
        this->setMaxDelta(wheelMaxDelta / this->servoWheelTransferRate_);
    }

    /* @brief Writes wheel angle - converts value to servo angle and writes it to the PWM pin.
     * @param wheelAngle The wheel angle to write.
     **/
    void writeWheelAngle(radian_t wheelAngle) {
        this->write(wheelAngle / this->servoWheelTransferRate_);
    }

private:
    const float servoWheelTransferRate_;
};
} // namespace hw
} // namespace micro
