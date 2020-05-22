#pragma once

#include <micro/port/timer.hpp>

namespace micro {
namespace hw {
/* @brief Controls a servo motor using a timer configured in PWM mode.
 **/
class Servo {

public:
    Servo(const timer_t& timer, uint32_t chnl, uint32_t pwm0, uint32_t pwm180, rad_per_sec_t maxAngVel, radian_t offset, radian_t maxDelta)
        : timer_(timer)
        , chnl_(chnl)
        , pwm0_(pwm0)
        , pwm180_(pwm180)
        , maxAngVel_(maxAngVel)
        , offset_(offset)
        , maxDelta_(maxDelta) {}

    radian_t offset() const {
        return offset_;
    }

    radian_t maxDelta() const {
        return this->maxDelta_;
    }

    radian_t angle();

    void setOffset(const radian_t offset) {
        this->offset_ = offset;
    }

    void setMaxDelta(const radian_t maxDelta) {
        this->maxDelta_ = maxDelta;
    }

    void write(const radian_t angle);

private:
    const timer_t timer_;           // The timer used for PWM generation.
    const uint32_t chnl_;           // The timer channel used for PWM generation.
    const uint32_t pwm0_;           // The PWM value for 0 degrees.
    const uint32_t pwm180_;         // The PWM value for 180 degrees.
    const rad_per_sec_t maxAngVel_; // The max angular velocity of the servo.

    radian_t offset_;
    radian_t maxDelta_;
    radian_t angle_;
    radian_t targetAngle_;
    microsecond_t prevAngleUpdateTime_;
};
} // namespace hw
} // namespace micro
