#include <micro/math/random_generator.hpp>

namespace micro {

random_generator::random_generator(const microsecond_t seedTime) : lfsr_(0) {
    this->seed(seedTime);
}

void random_generator::seed(const microsecond_t seedTime) {
    this->lfsr_ = 0xACE1u + static_cast<uint16_t>(static_cast<uint64_t>(seedTime.get()));
}

float random_generator::get() {
    // Uses Fibonacci LFSRs @see https://en.wikipedia.org/wiki/Linear-feedback_shift_register
    // taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1
    const uint16_t bit = ((lfsr_ >> 0) ^ (lfsr_ >> 2) ^ (lfsr_ >> 3) ^ (lfsr_ >> 5)) & 1u;
    lfsr_ = (lfsr_ >> 1) | (bit << 15);
    return static_cast<float>(lfsr_) / 65536u;
}

} // namespace micro
