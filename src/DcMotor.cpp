#include <micro/hw/DcMotor.hpp>
#include <micro/port/task.hpp>

namespace micro {
namespace hw {

ServoInterfaceDcMotor::ServoInterfaceDcMotor(const micro::timer_t& timer, const uint32_t chnl)
    : ServoInterfaceMotor<float>(timer, chnl, 1500, 1.0f / 500, 1.0f / 500, 1.0f) {
}

BridgeDcMotor::BridgeDcMotor(const micro::timer_t& timer, const uint32_t chnl1,
                             const uint32_t chnl2, const float maxDuty)
    : BridgeDcMotor(timer, chnl1, chnl2, gpio_t{}, gpio_t{}, maxDuty, false) {
}

BridgeDcMotor::BridgeDcMotor(const micro::timer_t& timer, const uint32_t chnl1,
                             const uint32_t chnl2, const gpio_t& shutdown1, const gpio_t& shutdown2,
                             const float maxDuty, const bool shutdownPinsEnabled)
    : Motor<float>(), timer_(timer), chnl1_(chnl1), chnl2_(chnl2), shutdown1_(shutdown1),
      shutdown2_(shutdown2), shutdownPinsEnabled_(shutdownPinsEnabled), maxDuty_(maxDuty) {
}

void BridgeDcMotor::start() {
    if (this->shutdownPinsEnabled_) {
        gpio_write(this->shutdown1_, gpioPinState_t::SET);
        gpio_write(this->shutdown2_, gpioPinState_t::SET);
    }
}

void BridgeDcMotor::write(const float& duty) {
    const float duty1 = micro::lerp(duty, -1.0f, 1.0f, 1.0f - this->maxDuty_, this->maxDuty_);

    criticalSection_t criticalSection;
    criticalSection.lock();
    timer_setDuty(this->timer_, this->chnl1_, duty1);
    timer_setDuty(this->timer_, this->chnl2_, 1.0f - duty1);
    criticalSection.unlock();
}

void BridgeDcMotor::stop() {
    if (this->shutdownPinsEnabled_) {
        gpio_write(this->shutdown1_, gpioPinState_t::RESET);
        gpio_write(this->shutdown2_, gpioPinState_t::RESET);
    }
}

BidirectionalPwmDcMotor::BidirectionalPwmDcMotor(const micro::timer_t& timer, const uint32_t chnl,
                                                 const micro::gpio_t& dir, const float maxDuty)
    : Motor<float>(), timer_(timer), chnl_(chnl), dir_(dir), maxDuty_(maxDuty) {
}

void BidirectionalPwmDcMotor::start() {
}

void BidirectionalPwmDcMotor::write(const float& duty) {
    const float clampedDuty = micro::lerp(duty, -1.0f, 1.0f, 1.0f - this->maxDuty_, this->maxDuty_);

    criticalSection_t criticalSection;
    criticalSection.lock();
    timer_setDuty(this->timer_, this->chnl_, clampedDuty);
    gpio_write(this->dir_, clampedDuty >= 0.0f ? gpioPinState_t::SET : gpioPinState_t::RESET);
    criticalSection.unlock();
}

void BidirectionalPwmDcMotor::stop() {
}

} // namespace hw
} // namespace micro
