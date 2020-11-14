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
        , prevValue_(value)
        , timestamp_(timestamp) {}

    state_t(const T& value) : state_t(value, getTime()) {}

    state_t(const state_t&) = default;

    state_t& operator=(const T& value) {
        this->set(value, getTime());
        return *this;
    }

    state_t& operator=(const state_t&) = default;

    operator T() const {
        return this->value_;
    }

    bool changed() const {
        return this->value_ != this->prevValue_;
    }

    void set(const T& value, const millisecond_t timestamp) {
        this->prevValue_ = this->value_;
        this->value_     = value;
        this->timestamp_ = timestamp;
    }

    const T& value() const {
        return this->value_;
    }

    const T& prev() const {
        return this->prevValue_;
    }

    millisecond_t timestamp() const {
        return this->timestamp_;
    }

private:
    T value_;
    T prevValue_;
    millisecond_t timestamp_;
};

} // namespace micro
