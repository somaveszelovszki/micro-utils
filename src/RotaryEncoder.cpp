#include <micro/hw/RotaryEncoder.hpp>
#include <micro/utils/numeric.hpp>

namespace micro {

hw::RotaryEncoder::RotaryEncoder(tim_handle_t *_htim)
    : htim(_htim)
    , prevPos(0)
    , offset(0) {}

Status hw::RotaryEncoder::initialize() {
    this->offset = this->prevPos = micro::getTimerCounter(this->htim);
    return Status::OK;
}

int32_t hw::RotaryEncoder::getDiff() {
    static constexpr int32_t MAX_VALUE = 65536;

    int32_t currentPos = micro::getTimerCounter(this->htim), diff = currentPos - this->prevPos;

    this->prevPos = currentPos;

    // checks overflow and updates difference if needed
    if (abs(diff) > MAX_VALUE / 2) {
        int32_t dp = diff + MAX_VALUE, dm = diff - MAX_VALUE;
        diff = abs(dp) <= abs(dm) ? dp : dm;
    }

    return diff;
}

} // namespace micro
