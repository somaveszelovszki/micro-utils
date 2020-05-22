#pragma once

#include <micro/port/timer.hpp>

namespace micro {
namespace hw {
/* @brief Controls a DC motor using a timer configured in PWM mode.
 **/
class Encoder {

public:
    explicit Encoder(const timer_t& timer);

    void update();

    int64_t absPos() const {
        return this->absPos_;
    }

    uint64_t numIncr() const {
        return this->numIncr_;
    }

    int32_t lastDiff() const {
        return this->lastDiff_;
    }

private:
    const timer_t timer_;
    int64_t absPos_;
    uint64_t numIncr_;
    uint32_t prevPos_;
    int32_t lastDiff_;
};
} // namespace hw
} // namespace micro
