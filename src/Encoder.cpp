#include <micro/hw/Encoder.hpp>
#include <micro/math/numeric.hpp>

namespace micro {
namespace hw {

Encoder::Encoder(TIM_HandleTypeDef *htim)
    : htim_(htim)
    , absPos_(0)
    , numIncr_(0)
    , prevPos_(0)
    , lastDiff_(0) {}

void Encoder::update() {
    const uint32_t pos = __HAL_TIM_GET_COUNTER(this->htim_);
    const uint32_t diffP = pos - this->prevPos_;
    const uint32_t diffN = this->prevPos_ - pos;

    this->lastDiff_ = diffP <= diffN ? static_cast<int32_t>(diffP) : -static_cast<int32_t>(diffN);
    this->absPos_  += static_cast<int64_t>(this->lastDiff_);
    this->numIncr_ += static_cast<int64_t>(abs(this->lastDiff_));
    this->prevPos_  = pos;
}

} // namespace hw
} // namespace micro
