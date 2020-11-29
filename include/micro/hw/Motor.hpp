#pragma once

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
    explicit Motor(const T& maxValue)
        : value_(0)
        , maxValue_(maxValue) {}

    void setValue(const T& value) {
        this->value_ = micro::clamp(value, -this->maxValue_, this->maxValue_);
    }

    T value() const {
        return this->value_;
    }

    T maxValue() const {
        return this->maxValue_;
    }

    virtual void start() = 0;
    virtual void write(const T& output) = 0;
    virtual void stop() = 0;

    virtual ~Motor() = default;

private:
    T value_;
    T maxValue_;
};

/* @brief Controls a motor using the standard servo interface.
 **/
template <typename T>
class ServoInterfaceMotor : public Motor<T> {
public:
    ServoInterfaceMotor(const micro::timer_t& timer, const uint32_t chnl, uint32_t pwmOffset, const T& positiveTransferRate, const T& negativeTransferRate, const T& maxValue)
        : Motor<T>(maxValue)
        , timer_(timer)
        , chnl_(chnl)
        , pwmOffset_(pwmOffset)
        , positiveTransferRate_(positiveTransferRate)
        , negativeTransferRate_(negativeTransferRate) {}

    void start() override {}

    void write(const T& value) override {
        this->setValue(value);
        const uint32_t pwm = this->pwmOffset_ + micro::round(this->value() / (this->value() >= T(0) ? this->positiveTransferRate_ : this->negativeTransferRate_));
        timer_setCompare(this->timer_, this->chnl_, pwm);
    }

    void stop() override {}

private:
    const micro::timer_t timer_;
    const uint32_t chnl_;
    const uint32_t pwmOffset_;
    const T positiveTransferRate_;   // The transfer rate between the requested value and the output PWM relative the offset in the positive direction
    const T negativeTransferRate_;   // The transfer rate between the requested value and the output PWM relative the offset in the positive direction
};

} // namespace hw
} // namespace micro
