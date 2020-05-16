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

        // performs mapping from target to start so that 0 duration mapping results in the target being returned
        return map(getExactTime(), this->startTime_ + this->duration_, this->startTime_, this->target_, this->start_);
    }

private:
    T start_;
    T target_;
    microsecond_t startTime_;
    microsecond_t duration_;
};

} // namespace micro
