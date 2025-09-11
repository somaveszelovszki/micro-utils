#pragma once

#include <micro/port/timer.hpp>

namespace micro {

class Timer {
  public:
    Timer(void) : isRunning_(false) {}

    explicit Timer(const millisecond_t period) : isRunning_(false) { this->start(period); }

    bool isRunning() const { return this->isRunning_; }

    millisecond_t startTime() const { return this->startTime_; }

    void setPeriod(const millisecond_t period) { this->period_ = period; }

    millisecond_t period() const { return this->period_; }

    void start(const millisecond_t period) {
        this->startTime_ = getTime();
        this->period_    = period;
        this->isRunning_ = true;
    }

    void restart() { this->start(this->period_); }

    void stop(void) { this->isRunning_ = false; }

    bool checkTimeout(void) {
        bool hasTimedOut = false;
        if (this->isRunning_) {
            if (getTime() - this->startTime_ > this->period_) {
                do {
                    this->startTime_ += this->period_;
                } while (getTime() - this->startTime_ > this->period_);

                hasTimedOut = true;
            }
        }
        return hasTimedOut;
    }

  protected:
    millisecond_t startTime_;
    millisecond_t period_;
    bool isRunning_;
};

class WatchdogTimer : private Timer {
  public:
    using Timer::isRunning;
    using Timer::restart;
    using Timer::start;
    using Timer::stop;
    using Timer::Timer;

    void setTimeout(const millisecond_t timeout) { this->setPeriod(timeout); }

    millisecond_t timeout() const { return this->period(); }

    void reset() { this->startTime_ = getTime(); }

    bool hasTimedOut() const {
        return this->isRunning_ && getTime() - this->startTime_ > this->period_;
    }
};

} // namespace micro
