#pragma once

#include <micro/utils/types.hpp>

namespace micro {

class irandom_generator {
public:
    virtual ~irandom_generator() = default;

    /**
     * Gets random value.
     * @return A random value in range [0 1)
     */
    virtual float operator()() = 0;
};

class random_generator : public irandom_generator {
public:
    explicit random_generator(const uint16_t seed = 0);
    random_generator(const random_generator&) = default;
    random_generator(random_generator&&) = default;
    random_generator& operator=(const random_generator&) = default;
    random_generator& operator=(random_generator&&) = default;
    ~random_generator() = default;

    void seed(const uint16_t seed);

    float operator()() override;

private:
    uint16_t lfsr_;
};

class fixed_number_generator : public micro::irandom_generator {
public:
    explicit fixed_number_generator(const float value) : value_{value} {}
    float operator()() override { return value_; }
    void setValue(const float value) { value_ = value; }

private:
    float value_{};
};

} // namespace micro

