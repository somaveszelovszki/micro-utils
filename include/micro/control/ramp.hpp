#pragma once

#include <micro/math/unit_utils.hpp>
#include <micro/utils/timer.hpp>

#include <utility>

namespace micro {

template <typename T>
class ramp_t {
public:
    T update(const T& actual, const T& target, const microsecond_t duration) {
        if (target != this->target_ || duration != this->duration_) {
            this->start_     = actual;
            this->target_    = target;
            this->startTime_ = getExactTime();
            this->duration_  = duration;
        }

        return this->duration_ != microsecond_t(0) ?
            micro::lerp(getExactTime(), this->startTime_, this->startTime_ + this->duration_, this->start_, this->target_) :
            this->target_;
    }

private:
    T start_;
    T target_;
    microsecond_t startTime_;
    microsecond_t duration_;
};

} // namespace micro
