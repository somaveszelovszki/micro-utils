#include <micro/hw/DcMotor.hpp>
#include <micro/port/task.hpp>

namespace micro {
namespace hw {

ServoInterfaceDcMotor::ServoInterfaceDcMotor(const micro::timer_t& timer, const uint32_t chnl)
    : ServoInterfaceMotor<float>(timer, chnl, 1500, 1.0f / 500, 1.0f) {}

BridgeDcMotor::BridgeDcMotor(const micro::timer_t& timer, const uint32_t chnl1, const uint32_t chnl2, const float maxDuty)
    : BridgeDcMotor(timer, chnl1, chnl2, gpio_t{}, gpio_t{}, maxDuty, false) {}

BridgeDcMotor::BridgeDcMotor(const micro::timer_t& timer, const uint32_t chnl1, const uint32_t chnl2, const gpio_t& shutdown1, const gpio_t& shutdown2,
    const float maxDuty, const bool shutdownPinsEnabled)
    : Motor<float>(maxDuty)
    , timer_(timer)
    , chnl1_(chnl1)
    , chnl2_(chnl2)
    , shutdown1_(shutdown1)
    , shutdown2_(shutdown2)
    , shutdownPinsEnabled_(shutdownPinsEnabled) {}

void BridgeDcMotor::start() {
    if (this->shutdownPinsEnabled_) {
        gpio_write(this->shutdown1_, gpioPinState_t::SET);
        gpio_write(this->shutdown2_, gpioPinState_t::SET);
    }
}

void BridgeDcMotor::write(const float& duty) {
    this->setValue(duty);

    criticalSection_t criticalSection;
    criticalSection.lock();
    timer_setDuty(this->timer_, this->chnl1_, this->value());
    timer_setDuty(this->timer_, this->chnl2_, 1.0f - this->value());
    criticalSection.unlock();
}

void BridgeDcMotor::stop() {
    if (this->shutdownPinsEnabled_) {
        gpio_write(this->shutdown1_, gpioPinState_t::RESET);
        gpio_write(this->shutdown2_, gpioPinState_t::RESET);
    }
}

} // namespace hw
} // namespace micro
