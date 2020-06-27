#include <micro/math/numeric.hpp>

namespace micro {

uint32_t add_overflow(uint32_t value, uint32_t incr, uint32_t exclusive_max) {
    value += incr;
    while(value >= exclusive_max) {
        value -= exclusive_max;
    }
    return value;
}

uint32_t sub_underflow(uint32_t value, uint32_t sub, uint32_t exclusive_max) {
    while(value >= exclusive_max) {
        value -= exclusive_max;
    }
    while(sub >= exclusive_max) {
        sub -= exclusive_max;
    }
    return value >= sub ? value - sub : value + exclusive_max - sub;
}

uint32_t incr_overflow(uint32_t value, uint32_t exclusive_max) {
    return ++value == exclusive_max ? 0 : value;
}

uint32_t decr_underflow(uint32_t value, uint32_t exclusive_max) {
    return value-- == 0 ? exclusive_max - 1 : value;
}

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x) {
    if (x != 0.0f) {
        float halfx = 0.5f * x;
        long i = *(long*)&x;
        i = 0x5f3759df - (i >> 1);
        x = *(float*)&i;
        x = x * (1.5f - (halfx * x * x));
    }
    return x;
}

void normalize(float& a, float& b) {
    const float norm = invSqrt(a * a + b * b);
    a *= norm;
    b *= norm;
}

void normalize(float& a, float& b, float& c) {
    const float norm = invSqrt(a * a + b * b + c * c);
    a *= norm;
    b *= norm;
    c *= norm;
}

void normalize(float& a, float& b, float& c, float& d) {
    const float norm = invSqrt(a * a + b * b + c * c + d * d);
    a *= norm;
    b *= norm;
    c *= norm;
    d *= norm;
}

random_generator_t::random_generator_t(const microsecond_t seedTime) : lfsr_(0) {
    this->seed(seedTime);
}

void random_generator_t::seed(const microsecond_t seedTime) {
    this->lfsr_ = 0xACE1u + static_cast<uint16_t>(static_cast<uint64_t>(seedTime.get()));
}

float random_generator_t::get() {
    // Uses Fibonacci LFSRs @see https://en.wikipedia.org/wiki/Linear-feedback_shift_register
    // taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1
    const uint16_t bit = ((lfsr_ >> 0) ^ (lfsr_ >> 2) ^ (lfsr_ >> 3) ^ (lfsr_ >> 5)) & 1u;
    lfsr_ = (lfsr_ >> 1) | (bit << 15);
    return map<uint32_t, float>(lfsr_, 0u, 65536u, 0.0f, 1.0f);
}

} // namespace micro
