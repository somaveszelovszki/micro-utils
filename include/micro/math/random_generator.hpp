#pragma once

#include <micro/utils/types.hpp>

namespace micro {

class random_generator {
public:
    explicit random_generator(const uint16_t seed = 0);

    void seed(const uint16_t seed);

    float get();

private:
    uint16_t lfsr_;
};

} // namespace micro

