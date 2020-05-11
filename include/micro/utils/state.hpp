#pragma once

#include <micro/utils/units.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

template <typename T>
class state_t {
public:
    state_t() {}

    state_t(const T& value, const millisecond_t timestamp)
        : value_(value)
        , timestamp_(timestamp) {}

    state_t(const T& value) : state_t(value, getExactTime()) {}

    state_t& operator=(const T& val) {
        this->value_     = val;
        this->timestamp_ = getExactTime();
        return *this;
    }

    void set(const T& value, const millisecond_t timestamp) {
        this->value_ = value;
        this->timestamp_ = timestamp;
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
