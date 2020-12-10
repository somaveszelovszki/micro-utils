#pragma once

#include <micro/utils/types.hpp>
#include <micro/utils/units.hpp>

#include <cmath>
#include <limits>
#include <type_traits>

namespace micro {

template <typename T, typename partial = void>
struct numeric_limits {};

template <typename T>
struct numeric_limits<T, typename std::enable_if<std::is_arithmetic<T>::value, void>::type> {
    static constexpr T min()       { return std::numeric_limits<T>::min();       }
    static constexpr T max()       { return std::numeric_limits<T>::max();       }
    static constexpr T quiet_NaN() { return std::numeric_limits<T>::quiet_NaN(); }
    static constexpr T infinity()  { return std::numeric_limits<T>::infinity();  }
    static constexpr T epsilon()   { return std::numeric_limits<T>::epsilon();   }
};

template <typename T, typename partial = void> struct raw_type {
    typedef T type;
    static constexpr type get(const T& value) { return value; }
};

template <typename T, typename std::enable_if<std::is_enum<T>::value, void>::type* = nullptr>
typename std::underlying_type<T>::type enum_cast(const T& value) {
    return static_cast<typename std::underlying_type<T>::type>(value);
}

// ---------------------------------------- Type-independent functions (same implementation for unit classes) ----------------------------------------

/**
 * @brief Gets minimum of the two values.
 * @param a The first value.
 * @param b The second value.
 * @returns The minimum of the two values.
 */
template <typename T>
inline constexpr T min(const T& a, const T& b) {
    return a < b ? a : b;
}

/**
 * @brief Gets maximum of the two values.
 * @param a The first value.
 * @param b The second value.
 * @returns The maximum of the two values.
 */
template <typename T>
inline constexpr T max(const T& a, const T& b) {
    return a > b ? a : b;
}

/**
 * @brief Gets average of the two values.
 * @param a The first value.
 * @param b The second value.
 * @returns The average of the two values.
 */
template <typename T1, typename T2>
inline constexpr auto avg(const T1& a, const T2& b) -> decltype(b + (a - b) / 2) {
    return a > b ? b + (a - b) / 2 : a + (b - a) / 2;
}

/**
 * @brief Checks if a value is between the given boundaries.
 * @tparam T Type of the elements.
 * @param value The value to check.
 * @param b1 The first boundary.
 * @param b2 The second boundary.
 * @returns Boolean value indicating if the value is between the boundaries.
 */
template <typename T1, typename T2, typename T3>
inline constexpr bool isBtw(const T1& value, const T2& b1, const T3& b2) {
    return b2 >= b1 ? value >= b1 && value <= b2 : value >= b2 && value <= b1;
}

/**
 * @brief clamps value between the given boundaries.
 * @tparam T Type of the elements.
 * @param value The value to clamp.
 * @param b1 The first boundary.
 * @param b2 The second boundary.
 * @returns The clampd value.
 */
template <typename T>
inline constexpr T clamp(const T& value, const T& b1, const T& b2) {
    return b2 > b1 ? min(max(value, b1), b2) : min(max(value, b2), b1);
}

/**
 * @brief Checks if value is in a given range of the reference value.
 * @tparam T Numeric type of the value and the reference.
 * @param value The value to compare to the reference.
 * @param ref The reference value.
 * @param relErr The permitted relative error.
 */
template <typename T1, typename T2>
inline constexpr bool isInRange(const T1& value, const T2& ref, float relErr) {
    return isBtw(value, ref * (1.0f - relErr), ref * (1.0f + relErr));
}

/**
 * @brief Checks if given value equals the reference with the given epsilon tolerance.
 * @tparam T Type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param ref The reference.
 * @param eps The epsilon tolerance.
 */
template <typename T1, typename T2, typename T3>
inline constexpr bool eq(const T1& value, const T2& ref, const T3& eps) {
    return (value >= ref - eps) && (value <= ref + eps);
}

/**
 * @brief Checks if given value equals the reference with the default epsilon tolerance.
 * @restrict Type must be arithmetic.
 * @tparam T Numeric type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param ref The reference.
 */
template <typename T1, typename T2>
inline constexpr bool eq(const T1& value, const T2& ref) {
    return eq(value, ref, micro::numeric_limits<T1>::epsilon());
}

/**
 * @brief Checks if given value equals zero with the given epsilon tolerance.
 * @restrict Type must be arithmetic.
 * @tparam T Numeric type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param eps The epsilon tolerance - 0.0001f by default.
 */
template <typename T1, typename T2>
inline constexpr bool isZero(const T1& value, const T2& eps) {
    return eq(value, T1(0), eps);
}

/**
 * @brief Checks if given value equals zero with the given epsilon tolerance.
 * @restrict Type must be arithmetic.
 * @tparam T Numeric type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param eps The epsilon tolerance - 0.0001f by default.
 */
template <typename T>
inline constexpr bool isZero(const T& value) {
    return eq(value, T(0));
}

/**
 * @brief Maps value from from a given range to another.
 * @tparam S Numeric type of the source value and the source range boundaries.
 * @tparam R Numeric type of the result value and the result range boundaries.
 * @param value The value to map.
 * @param from1 First boundary of the source range.
 * @param from2 Second boundary of the source range.
 * @param to1 First boundary of the destination range.
 * @param to2 Second boundary of the destination range.
 * @returns The mapped value.
 */
template <typename S, typename R>
inline constexpr typename std::enable_if<!std::is_integral<S>::value && !std::is_unsigned<S>::value && !std::is_unsigned<R>::value, R>::type
map(const S& value, const S& from1, const S& from2, const R& to1, const R& to2) {
    return isZero(from2 - from1) ? to1 : to1 + (clamp(value, from1, from2) - from1) / (from2 - from1) * (to2 - to1);
}

/**
 * @brief Maps value from from a given range to another.
 * @tparam S Numeric type of the source value and the source range boundaries.
 * @tparam R Numeric type of the result value and the result range boundaries.
 * @param value The value to map.
 * @param from1 First boundary of the source range.
 * @param from2 Second boundary of the source range.
 * @param to1 First boundary of the destination range.
 * @param to2 Second boundary of the destination range.
 * @returns The mapped value.
 */
template <typename S, typename R>
inline constexpr typename std::enable_if<std::is_integral<S>::value && !std::is_unsigned<S>::value && !std::is_unsigned<R>::value, R>::type
map(const S& value, const S& from1, const S& from2, const R& to1, const R& to2) {
    return from1 == from2 ? to1 : to1 + (clamp(value, from1, from2) - from1) * (to2 - to1) / (from2 - from1);
}

/**
 * @brief Maps value from from a given range to another.
 * @tparam S Numeric type of the source value and the source range boundaries.
 * @tparam R Numeric type of the result value and the result range boundaries.
 * @param value The value to map.
 * @param from1 First boundary of the source range.
 * @param from2 Second boundary of the source range.
 * @param to1 First boundary of the destination range.
 * @param to2 Second boundary of the destination range.
 * @returns The mapped value.
 */
template <typename S, typename R>
inline constexpr typename std::enable_if<std::is_unsigned<S>::value || std::is_unsigned<R>::value, R>::type
map(const S& value, const S& from1, const S& from2, const R& to1, const R& to2) {

    const S clamped = clamp(value, from1, from2);
    R result = to1;

    if (from2 > from1) {
        if (to2 >= to1) {
            result = to1 + (clamped - from1) * (to2 - to1) / (from2 - from1);
        } else {
            result = to1 - (clamped - from1) * (to1 - to2) / (from2 - from1);
        }
    } else if (from2 < from1) {
        if (to2 >= to1) {
            result = to1 + (from1 - clamped) * (to2 - to1) / (from1 - from2);
        } else {
            result = to1 - (from1 - clamped) * (to1 - to2) / (from1 - from2);
        }
    }

    return result;
}

/**
 * @brief Calculates square of the vector length using the Pythagorean theory.
 * @tparam T The type of the values.
 * @param a The length of the first leg of the triangle.
 * @param b The length of the other leg of the triangle.
 * @returns The length of the hypotenuse of the triangle.
 */
template <typename T>
inline constexpr auto pythag_square(const T& a, const T& b) -> decltype (a * b) {
    return a * a + b * b;
}

/**
 * @brief Calculates square of the vector length using the Pythagorean theory.
 * @tparam T The type of the values.
 * @param a The length of the first coordinate.
 * @param b The length of the second coordinate.
 * @param c The length of the third coordinate.
 * @returns The length of the vector.
 */
template <typename T>
inline constexpr auto pythag_square(const T& a, const T& b, const T& c) -> decltype (a * b) {
    return a * a + b * b + c * c;
}

// ---------------------------------------- Type-dependent functions (different implementations for unit classes) ----------------------------------------

union quaternion_t {
    struct { float w,  x,  y,  z;  };
    struct { float q0, q1, q2, q3; };
};

/**
 * @brief Gets absolute of the value.
 * @param value The value.
 * @returns The absolute of the value.
 */
template <typename T>
inline constexpr T abs(const T& value) {
    return value >= T(0) ? value : -value;
}

/**
 * @brief Gets sign of the value.
 * @restrict Type must be arithmetic.
 * @param value The value.
 * @returns The sign of the value.
 */
template <typename T>
inline constexpr Sign sgn(const T& value) {
    return value >= T(0) ? Sign::POSITIVE : Sign::NEGATIVE;
}

template <typename T>
inline constexpr bool isinf(const T& value) {
    return std::isinf(raw_type<T>::get(value));
}

template <typename T>
inline constexpr bool isnan(const T& value) {
    return std::isnan(raw_type<T>::get(value));
}

/**
 * @brief Calculates the hypotenuse of a triangle using the Pythagorean theory.
 * @restrict Type must be arithmetic.
 * @tparam T The type of the values.
 * @param a The length of the first leg of the triangle.
 * @param b The length of the other leg of the triangle.
 * @returns The length of the hypotenuse of the triangle.
 */
template <typename T>
inline constexpr typename std::enable_if<std::is_arithmetic<T>::value, T>::type pythag(const T& a, const T& b) {
    return T(std::sqrt(a * a + b * b));
}

/**
 * @brief Calculates vector length using the Pythagorean theory.
 * @restrict Type must be arithmetic.
 * @tparam T The type of the values.
 * @param a The length of the first coordinate.
 * @param b The length of the second coordinate.
 * @param c The length of the third coordinate.
 * @returns The length of the vector.
 */
template <typename T>
inline constexpr typename std::enable_if<std::is_arithmetic<T>::value, T>::type pythag(const T& a, const T& b, const T& c) {
    return static_cast<T>(std::sqrt(a * a + b * b + c * c));
}

inline constexpr int32_t round(const float value) {
    return static_cast<int32_t>(value >= 0.0f ? value + 0.5f : value - 0.5f);
}

inline constexpr int32_t round_down(const float value) {
    return value >= 0.0f ? round(value - 0.5f) : round(value + 0.5f);
}

inline constexpr int32_t round_up(const float value) {
    return value >= 0.0f ? round(value + 0.49999f) : round(value - 0.49999f);
}

/**
 * @brief Calculates power.
 * @param value
 * @param pow
 * @return
 */
template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
inline T powerOf(const T& value, uint32_t pow) {
    T result(1);
    for (uint32_t i = 0; i < pow; ++i) {
        result *= value;
    }
    return result;
}

uint32_t incr_overflow(uint32_t value, uint32_t incr, const uint32_t exclusive_max);
uint32_t decr_underflow(uint32_t value, uint32_t decr, const uint32_t exclusive_max);

uint32_t incr_overflow(uint32_t value, const uint32_t exclusive_max);
uint32_t decr_underflow(uint32_t value, const uint32_t exclusive_max);

uint32_t diff_overflow(uint32_t a, uint32_t b, const uint32_t exclusive_max);

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x);

void normalize(float& a, float& b);

void normalize(float& a, float& b, float& c);

void normalize(float& a, float& b, float& c, float& d);

} // namespace micro

