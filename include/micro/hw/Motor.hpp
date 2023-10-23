#pragma once

#include <cmath>

#include <micro/math/numeric.hpp>
#include <micro/port/gpio.hpp>
#include <micro/port/timer.hpp>

namespace micro {
namespace hw {

/* @brief Base class for all motor control types.
 **/
template <typename T>
class Motor {
public:
    virtual void start() = 0;
    virtual void write(const T& output) = 0;
    virtual void stop() = 0;

    virtual ~Motor() = default;
};

/* @brief Controls a motor using the standard servo interface.
 **/
template <typename T>
class ServoInterfaceMotor : public Motor<T> {
public:
    ServoInterfaceMotor(const micro::timer_t& timer, const uint32_t chnl, uint32_t pwmOffset, const T& positiveTransferRate, const T& negativeTransferRate, const T& maxValue)
        : Motor<T>()
        , timer_(timer)
        , chnl_(chnl)
        , pwmOffset_(pwmOffset)
        , positiveTransferRate_(positiveTransferRate)
        , negativeTransferRate_(negativeTransferRate)
        , maxValue_(maxValue) {}

    void start() override {}

    void write(const T& value) override {
        this->value_ = micro::clamp(value, -this->maxValue_, this->maxValue_);
        const uint32_t pwm = this->pwmOffset_ + std::lround(this->value_ / (this->value_ >= T(0) ? this->positiveTransferRate_ : this->negativeTransferRate_));
        timer_setCompare(this->timer_, this->chnl_, pwm);
    }

    void stop() override {}

    T value() const {
        return this->value_;
    }

    T maxValue() const {
        return this->maxValue_;
    }

private:
    const micro::timer_t timer_;
    const uint32_t chnl_;
    const uint32_t pwmOffset_;
    const T positiveTransferRate_;   // The transfer rate between the requested value and the output PWM relative the offset in the positive direction
    const T negativeTransferRate_;   // The transfer rate between the requested value and the output PWM relative the offset in the positive direction

    const T maxValue_;
    T value_;
};

} // namespace hw
} // namespace micro
