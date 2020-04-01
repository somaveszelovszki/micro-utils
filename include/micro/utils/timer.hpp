#pragma once

#include "time.hpp"
#include <micro/container/vec.hpp>

namespace micro {

class Timer {
public:
    Timer(void) : isRunning_(false) {}

    void setPeriod(const millisecond_t period) {
        this->period_ = period;
    }

    void start(const millisecond_t period) {
        this->startTime_= getTime();
        this->period_ = period;
        this->isRunning_ = true;
    }

    void stop(void) {
        this->isRunning_ = false;
    }

    bool checkTimeout(void) {
        bool hasTimedOut = false;
        if (this->isRunning_ && getTime() - this->startTime_ > this->period_) {
            this->startTime_ += this->period_;
            hasTimedOut = true;
        }
        return hasTimedOut;
    }

private:
    millisecond_t startTime_;
    millisecond_t period_;
    bool isRunning_;
};

} // namespace micro
