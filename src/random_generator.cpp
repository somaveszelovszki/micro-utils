#include <micro/math/numeric.hpp>
#include <micro/math/random_generator.hpp>

namespace micro {

random_generator::random_generator(const uint16_t seed) : lfsr_(0) {
    this->seed(seed);
}

void random_generator::seed(const uint16_t seed) {
    this->lfsr_ = 0xACE1u + seed;
}

float random_generator::get() {
    // Uses Fibonacci LFSRs @see https://en.wikipedia.org/wiki/Linear-feedback_shift_register
    // taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1
    const uint16_t bit = ((lfsr_ >> 0) ^ (lfsr_ >> 2) ^ (lfsr_ >> 3) ^ (lfsr_ >> 5)) & 1u;
    lfsr_ = (lfsr_ >> 1) | (bit << 15);
    return static_cast<float>(lfsr_) / 65536u;
}

uint16_t random_generator::get(const uint16_t inclusiveLow, const uint16_t exclusiveHigh) {
    return micro::lerp(this->get(), 0.0f, 1.0f, inclusiveLow, exclusiveHigh);
}


} // namespace micro
