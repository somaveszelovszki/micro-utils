#pragma once

#include <micro/utils/types.hpp>
#include <micro/utils/units.hpp>

namespace micro {

class random_generator {
public:
    explicit random_generator(const microsecond_t seedTime = microsecond_t(0));

    void seed(const microsecond_t seedTime);

    float get();

private:
    uint16_t lfsr_;
};

} // namespace micro

