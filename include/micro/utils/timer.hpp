#pragma once

#include "time.hpp"
#include <micro/container/vec.hpp>

namespace micro {

#define MAX_NUM_TIMERS 64

class Timer {
public:
    Timer(void)
        : timedOut_(false)
        , isRunning_(false) {
        Timer::instances.push_back(this);
    }

    void setPeriod(const millisecond_t period) {
        this->period_ = period;
    }

    void start(const millisecond_t period) {
        this->startTime_= getTime();
        this->period_ = period;
        this->timedOut_ = false;
        this->isRunning_ = true;
    }

    void stop(void) {
        this->isRunning_ = false;
    }

    bool checkTimeout(void) {
        bool timedOut = this->timedOut_;
        this->timedOut_ = false;
        return timedOut;
    }

    static void updateAll(void) {
        for (Timer *t : Timer::instances) {
            t->update();
        }
    }

private:
    static vec<Timer*, MAX_NUM_TIMERS> instances;

    void update(void) {
        if (this->isRunning_ && getTime() - this->startTime_ > this->period_) {
            this->startTime_ = getTime();
            this->timedOut_ = true;
        }
    }

    millisecond_t startTime_;
    millisecond_t period_;
    bool timedOut_;
    bool isRunning_;
};

} // namespace micro
