#pragma once

#include <micro/port/timer.hpp>
#include <micro/utils/units.hpp>

namespace micro {
namespace hw {
/* @brief Controls a DC motor using a timer configured in PWM mode.
 **/
class DC_Motor {

public:
    DC_Motor(const timer_t& timer, const uint32_t chnl1, const uint32_t chnl2, const float maxDuty);

    void write(const float duty);

private:
    const timer_t timer_;
    const uint32_t chnl1_;
    const uint32_t chnl2_;
    const float maxDuty_;
};
} // namespace hw
} // namespace micro
