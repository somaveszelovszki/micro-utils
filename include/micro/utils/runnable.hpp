#pragma once

#include <micro/bsp/tim.hpp>
#include <micro/utils/arrays.hpp>
#include <micro/utils/units.hpp>
#include <type_traits>

namespace micro {

class runnable_t {

public:
    runnable_t() : period(time_t::ZERO()) {}

    explicit runnable_t(micro::time_t _period) : period(_period) {}

    /* @brief Updates time and calculates time difference between previous and current run.
     * @note This is an internal method, do not call it explicitly!
     * @returns Time difference between the previous and the current run.
     **/
    micro::time_t updateTimeDiff(){
        micro::time_t prev = this->lastRunTime;
        return (this->lastRunTime = micro::getExactTime()) - prev;
    }

    bool shouldRun() const {
        return micro::getTime() - this->lastRunTime >= this->period;
    }

protected:
    micro::time_t period;
    micro::time_t lastRunTime;   // The time of the last run.
};

} // namespace micro
