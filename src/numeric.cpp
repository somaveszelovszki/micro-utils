#include <micro/math/numeric.hpp>

namespace micro {

uint32_t incr_overflow(uint32_t value, uint32_t incr, const uint32_t exclusive_max) {
    return value < exclusive_max - incr ? value + incr : incr - (exclusive_max - value);
}

uint32_t decr_underflow(uint32_t value, uint32_t decr, const uint32_t exclusive_max) {
    return value >= decr ? value - decr : exclusive_max - (decr - value);
}

uint32_t incr_overflow(uint32_t value, const uint32_t exclusive_max) {
    return incr_overflow(value, 1, exclusive_max);
}

uint32_t decr_underflow(uint32_t value, const uint32_t exclusive_max) {
    return decr_underflow(value, 1, exclusive_max);
}

uint32_t diff_overflow(uint32_t a, uint32_t b, const uint32_t exclusive_max) {
    return a >= b ? a - b : exclusive_max - b + a;
}

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x) {
    if (x != 0.0f) {
        float halfx = 0.5f * x;
        long i      = *(long*)&x;
        i           = 0x5f3759df - (i >> 1);
        x           = *(float*)&i;
        x           = x * (1.5f - (halfx * x * x));
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

} // namespace micro
