#include <micro/hw/Encoder.hpp>
#include <micro/math/numeric.hpp>

namespace micro {
namespace hw {

Encoder::Encoder(const timer_t& timer)
    : timer_(timer), period_(0), absPos_(0), numIncr_(0), prevPos_(0), lastDiff_(0),
      initialized_(false) {
}

void Encoder::initialize() {
    timer_setCounter(this->timer_, 0);
    timer_getPeriod(this->timer_, this->period_);

    this->absPos_      = 0;
    this->numIncr_     = 0;
    this->prevPos_     = 0;
    this->lastDiff_    = 0;
    this->initialized_ = true;
}

void Encoder::update() {
    if (this->initialized_) {
        uint32_t pos;
        timer_getCounter(this->timer_, pos);

        const uint32_t diffP = diff_overflow(pos, this->prevPos_, this->period_);
        const uint32_t diffN = diff_overflow(this->prevPos_, pos, this->period_);

        this->lastDiff_ =
            diffP <= diffN ? static_cast<int32_t>(diffP) : -static_cast<int32_t>(diffN);
        this->absPos_ += static_cast<int64_t>(this->lastDiff_);
        this->numIncr_ += static_cast<int64_t>(abs(this->lastDiff_));
        this->prevPos_ = pos;
    }
}

} // namespace hw
} // namespace micro
