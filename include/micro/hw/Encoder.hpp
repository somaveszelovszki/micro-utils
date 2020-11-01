#pragma once

#include <micro/port/timer.hpp>

namespace micro {
namespace hw {
/* @brief Controls a DC motor using a timer configured in PWM mode.
 **/
class Encoder {

public:
    explicit Encoder(const timer_t& timer);

    void initialize();

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
    uint32_t timerPos();

    const timer_t timer_;
    uint32_t period_;
    int64_t absPos_;
    uint64_t numIncr_;
    uint32_t prevPos_;
    int32_t lastDiff_;
    bool initialized_;
};
} // namespace hw
} // namespace micro
