#pragma once

#include <micro/port/timer.hpp>

namespace micro {
namespace hw {
/* @brief Controls a servo motor using a timer configured in PWM mode.
 **/
class Servo {

public:
    Servo(const timer_t& timer, uint32_t chnl, uint32_t pwmCenter, const radian_t transferRate, radian_t maxAngle, rad_per_sec_t maxAngVel);

    radian_t maxAngle() const {
        return this->maxAngle_;
    }

    radian_t angle();

    void write(const radian_t angle);

private:
    const timer_t timer_;           // The timer used for PWM generation
    const uint32_t chnl_;           // The timer channel used for PWM generation
    const uint32_t pwmCenter_;      // The PWM value for the center position
    const radian_t transferRate_;   // The transfer rate between the PWM input and the angle output
    const radian_t maxAngle_;       // The maximum angle - relative to the center
    const rad_per_sec_t maxAngVel_; // The maximum angular velocity

    radian_t angle_;
    radian_t targetAngle_;
    microsecond_t prevAngleUpdateTime_;
};
} // namespace hw
} // namespace micro
