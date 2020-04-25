#pragma once

#include <micro/math/numeric.hpp>
#include <micro/utils/units.hpp>

namespace micro {

static constexpr float SQRT_2 = std::sqrt(2.0f);
static constexpr float SQRT_3 = std::sqrt(3.0f);

constexpr radian_t PI = radian_t(3.14159265358979323846);
constexpr radian_t PI_2 = PI / 2;
constexpr radian_t PI_4 = PI / 4;

const m_per_sec2_t G = m_per_sec2_t(9.81f);  // Gravitational acceleration.

template <typename T>
struct numeric_limits<T, typename std::enable_if<T::is_dim_class, void>::type> {
    static constexpr T min()       { return std::numeric_limits<typename T::value_type>::min();       }
    static constexpr T max()       { return std::numeric_limits<typename T::value_type>::max();       }
    static constexpr T quiet_Nan() { return std::numeric_limits<typename T::value_type>::quiet_NaN(); }
    static constexpr T infinity()  { return std::numeric_limits<typename T::value_type>::infinity();  }
    static constexpr T epsilon()   { return std::numeric_limits<typename T::value_type>::epsilon();   }
};

/**
 * @brief Gets value.
 * @param value The value.
 * @returns The value.
 */
template <typename T>
inline constexpr typename std::enable_if<T::is_dim_class, float>::type valueOf(const T& value) {
    return value.template get<true>();
}

/**
 * @brief Calculates the hypotenuse of a triangle using the Pythagorean theory.
 * @restrict Type must be a unit class.
 * @tparam T The type of the values.
 * @param a The length of the first leg of the triangle.
 * @param b The length of the other leg of the triangle.
 * @returns The length of the hypotenuse of the triangle.
 */
template <typename T1, typename T2>
inline typename std::enable_if<T1::is_dim_class && T2::is_dim_class && T1::dim == T2::dim, T1>::type pythag(const T1& a, const T2& b) {
    const float _a = a.template get<true>();
    const float _b = static_cast<T1>(b).template get<true>();
    return T1(std::sqrt(_a * _a + _b * _b), nullptr);
}

/**
 * @brief Calculates vector length using the Pythagorean theory.
 * @restrict Type must be a unit class.
 * @tparam T The type of the values.
 * @param a The length of the first coordinate.
 * @param b The length of the second coordinate.
 * @param c The length of the third coordinate.
 * @returns The length of the vector.
 */
template <typename T1, typename T2, typename T3>
inline typename std::enable_if<T1::is_dim_class && T2::is_dim_class && T3::is_dim_class && T1::dim == T2::dim && T1::dim == T3::dim, T1>::type pythag(const T1& a, const T2& b, const T3& c) {
    const float _a = a.template get<true>();
    const float _b = static_cast<T1>(b).template get<true>();
    const float _c = static_cast<T1>(c).template get<true>();
    return T1(std::sqrt(_a * _a + _b * _b + _c * _c), nullptr);
}

/* @brief Calculates sine of given angle.
 * @param value The angle.
 * @returns The sine of the angle.
 **/
inline constexpr float sin(const radian_t& value) {
    return std::sin(value.template get<true>());
}

/* @brief Calculates arc-sine of given value.
 * @param value The value.+
 * @returns The arc-sine of the value.
 **/
inline constexpr radian_t asin(float value) {
    return radian_t(std::asin(value)) ;
}

/* @brief Calculates cosine of given angle.
 * @param value The angle.
 * @returns The cosine of the angle.
 **/
inline constexpr float cos(const radian_t& value) {
    return std::cos(value.template get<true>()) ;
}

/* @brief Calculates arc-cosine of given value.
 * @param value The value.
 * @returns The arc-cosine of the value.
 **/
inline constexpr radian_t acos(float value) {
    return radian_t(std::acos(value)) ;
}

/* @brief Calculates tangent of given angle.
 * @param value The angle.
 * @returns The tangent of the angle.
 **/
inline constexpr float tan(const radian_t& value) {
    return std::tan(value.template get<true>());
}

/* @brief Calculates arc-tangent of given value.
 * @param value The value.
 * @returns The arc-tangent of the value.
 **/
inline constexpr radian_t atan(float value) {
    return radian_t(std::atan(value));
}

template <typename T>
inline constexpr typename std::enable_if<std::is_arithmetic<T>::value, radian_t>::type atan2(const T& y, const T& x) {
    return radian_t(std::atan2(static_cast<float>(y), static_cast<float>(x)));
}

template <typename T1, typename T2>
inline constexpr typename std::enable_if<T1::is_dim_class && T2::is_dim_class && T1::dim == T2::dim, radian_t>::type atan2(const T1& y, const T2& x) {
    return radian_t(atan2(y.template get<true>(), static_cast<T1>(x).template get<true>()));
}

inline radian_t normalizePM180(radian_t value) {
    while(value >= PI) {
        value -= 2 * PI;
    }
    while(value < -PI) {
        value += 2 * PI;
    }
    return value;
}

inline radian_t normalize360(radian_t value) {
    while(value >= 2 * PI) {
        value -= 2 * PI;
    }
    while(value < radian_t(0)) {
        value += 2 * PI;
    }
    return value;
}

inline radian_t normalize180(radian_t value) {
    while(value >= PI) {
        value -= PI;
    }
    while(value < radian_t(0)) {
        value += PI;
    }
    return value;
}

inline bool eqWithOverflow360(radian_t value, radian_t ref, radian_t eps) {
    return abs(normalizePM180(value - ref)) <= eps;
}

inline bool eqWithOverflow180(radian_t value, radian_t ref, radian_t eps) {
    return eq(value, ref, eps) || eq(value + PI, ref, eps) || eq(value - PI, ref, eps);
}

inline radian_t round45(radian_t value) {
    static constexpr radian_t EPS = PI_4 / 2;
    radian_t result;

    if (eqWithOverflow360(value, PI_4, EPS)) {
        result = PI_4;
    } else if (eqWithOverflow360(value, PI_2, EPS)) {
        result = PI_2;
    } else if (eqWithOverflow360(value, 3 * PI_4, EPS)) {
        result = 3 * PI_4;
    } else if (eqWithOverflow360(value, PI, EPS)) {
        result = PI;
    } else if (eqWithOverflow360(value, 5 * PI_4, EPS)) {
        result = 5 * PI_4;
    } else if (eqWithOverflow360(value, 3 * PI_2, EPS)) {
        result = 3 * PI_2;
    } else if (eqWithOverflow360(value, 7 * PI_4, EPS)) {
        result = 7 * PI_4;
    } else {
        result = radian_t(0);
    }

    return result;
}

inline radian_t round90(radian_t value) {
    static constexpr radian_t EPS = PI_4;
    radian_t result;

    if (eqWithOverflow360(value, PI_2, EPS)) {
        result = PI_2;
    } else if (eqWithOverflow360(value, PI, EPS)) {
        result = PI;
    } else if (eqWithOverflow360(value, 3 * PI_2, EPS)) {
        result = 3 * PI_2;
    } else {
        result = radian_t(0);
    }

    return result;
}

inline bool isMultipleOf90(radian_t value, radian_t eps) {

    return eqWithOverflow360(value, radian_t(0), eps)
        || eqWithOverflow360(value, PI_2, eps)
        || eqWithOverflow360(value, PI, eps)
        || eqWithOverflow360(value, 3 * PI_2, eps);
}

/* @brief Calculates square of the vector length using the Pythagorean theory.
 * @param a The length of the first leg of the triangle.
 * @param b The length of the other leg of the triangle.
 * @returns The length of the hypotenuse of the triangle.
 **/
inline distance_t pythag_square(distance_t a, distance_t b) {
    return centimeter_t(pythag_square(centimeter_t(a).template get<true>(), centimeter_t(b).template get<true>()));
}

/* @brief Calculates vector length using the Pythagorean theory.
 * @param a The length of the first leg of the triangle.
 * @param b The length of the other leg of the triangle.
 * @returns The length of the hypotenuse of the triangle.
 **/
inline distance_t pythag(distance_t a, distance_t b) {
    return centimeter_t(pythag(centimeter_t(a).template get<true>(), centimeter_t(b).template get<true>()));
}

/* @brief Calculates square of the vector length using the Pythagorean theory.
 * @param a The length of the first coordinate.
 * @param b The length of the second coordinate.
 * @param c The length of the third coordinate.
 * @returns The length of the vector.
 **/
inline distance_t pythag_square(distance_t a, distance_t b, distance_t c) {
    return centimeter_t(pythag_square(centimeter_t(a).template get<true>(), centimeter_t(b).template get<true>(), centimeter_t(c).template get<true>()));
}

/* @brief Calculates vector length using the Pythagorean theory.
 * @param a The length of the first coordinate.
 * @param b The length of the second coordinate.
 * @param c The length of the third coordinate.
 * @returns The length of the vector.
 **/
inline distance_t pythag(distance_t a, distance_t b, distance_t c) {
    return centimeter_t(pythag(centimeter_t(a).template get<true>(), centimeter_t(b).template get<true>(), centimeter_t(c).template get<true>()));
}

} // namespace micro

