#include <micro/hw/DC_Motor.hpp>
#include <micro/math/numeric.hpp>

namespace micro {
namespace hw {

DC_Motor::DC_Motor(const timer_t& timer, const uint32_t chnl1, const uint32_t chnl2, const float maxDuty)
    : timer_(timer)
    , chnl1_(chnl1)
    , chnl2_(chnl2)
    , maxDuty_(maxDuty) {

    this->write(0.0f);
}

void DC_Motor::write(const float duty) {
    const float duty1 = map(duty, -1.0f, 1.0f, 1.0f - this->maxDuty_, this->maxDuty_);
    timer_setDuty(this->timer_, this->chnl1_, duty1);
    timer_setDuty(this->timer_, this->chnl2_, 1.0f - duty1);
}

} // namespace hw
} // namespace micro
