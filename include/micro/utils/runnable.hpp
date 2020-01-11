#pragma once

#include <micro/utils/arrays.hpp>
#include <micro/utils/time.hpp>

#include <type_traits>

namespace micro {

class runnable_t {

public:
    runnable_t() : period(millisecond_t::zero()) {}

    explicit runnable_t(millisecond_t period) : period(period) {}

    /* @brief Updates time and calculates time difference between previous and current run.
     * @note This is an internal method, do not call it explicitly!
     * @returns Time difference between the previous and the current run.
     **/
    millisecond_t updateTimeDiff(){
        millisecond_t prev = this->lastRunTime;
        return (this->lastRunTime = getTime()) - prev;
    }

    bool shouldRun() const {
        return getTime() - this->lastRunTime >= this->period;
    }

protected:
    micro::millisecond_t period;
    micro::millisecond_t lastRunTime;   // The time of the last run.
};

} // namespace micro
