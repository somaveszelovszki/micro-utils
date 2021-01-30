#pragma once

#include <micro/utils/types.hpp>

namespace micro {

class random_generator {
public:
    explicit random_generator(const uint16_t seed = 0);

    void seed(const uint16_t seed);

    /**
     * Gets random value.
     * @return A random value in range [0 1)
     */
    float get();

    uint16_t get(const uint16_t inclusiveLow, const uint16_t exclusiveHigh);

private:
    uint16_t lfsr_;
};

} // namespace micro

