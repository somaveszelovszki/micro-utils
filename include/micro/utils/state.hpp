#pragma once

#include <micro/utils/units.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

template <typename T>
class state_t {
public:
    state_t(const T& val)
        : value_(val)
        , timestamp_(getTime()) {}

    state_t& operator=(const T& val) {
        this->value_     = val;
        this->timestamp_ = getTime();
        return *this;
    }

    const T& value() const {
        return this->value_;
    }

    millisecond_t timestamp() const {
        return this->timestamp_;
    }

private:
    T value_;
    millisecond_t timestamp_;
};

} // namespace micro
