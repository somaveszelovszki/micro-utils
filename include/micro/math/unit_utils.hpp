#pragma once

#include <micro/utils/units.hpp>
#include <micro/math/numeric.hpp>
#include <micro/math/constants.hpp>

namespace micro {

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
 * @brief Gets absolute of the value.
 * @param value The value.
 * @returns The absolute of the value.
 */
template <typename T>
inline constexpr typename std::enable_if<T::is_dim_class, T>::type abs(const T& value) {
    return T(abs(value.template get<true>()));
}

/**
 * @brief Gets sign of the value.
 * @restrict Type must be a unit class.
 * @param value The value.
 * @returns The sign of the value.
 */
template <typename T>
inline constexpr typename std::enable_if<T::is_dim_class, Sign>::type sgn(const T& value) {
    return sgn(value.template get<true>());
}

template <typename T>
inline constexpr typename std::enable_if<T::is_dim_class, bool>::type isinf(const T& value) {
    return isinf(value.template get<true>());
}

template <typename T>
inline constexpr typename std::enable_if<T::is_dim_class, bool>::type isnan(const T& value) {
    return isnan(value.template get<true>());
}

/**
 * @brief Checks if given value equals the reference with the default epsilon tolerance.
 * @restrict Types must be unit classes of the same dimension.
 * @tparam T Numeric type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param ref The reference.
 */
template <typename T1, typename T2>
inline constexpr typename std::enable_if<T1::is_dim_class && T2::is_dim_class && T1::dim == T2::dim, bool>::type eq(const T1& value, const T2& ref) {
    return eq(value, ref, T1(detail::COMMON_EQ_ABS_EPS, nullptr));
}

/**
 * @brief Checks if given value equals zero with the given epsilon tolerance.
 * @restrict Type must be a unit class.
 * @tparam T Numeric type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param eps The epsilon tolerance - 0.0001f by default.
 */
template <typename T1, typename T2>
inline constexpr typename std::enable_if<T1::is_dim_class && T2::is_dim_class && T1::dim == T2::dim, bool>::type isZero(const T1& value, const T2 eps) {
    return eq(value, T1::zero(), eps);
}

/**
 * @brief Checks if given value equals zero with the given epsilon tolerance.
 * @restrict Type must be a unit class.
 * @tparam T Numeric type of the value, the reference and the epsilon tolerance.
 * @param value The value to compare to the reference.
 * @param eps The epsilon tolerance - 0.0001f by default.
 */
template <typename T>
inline constexpr typename std::enable_if<T::is_dim_class, bool>::type isZero(const T& value) {
    return eq(value, T::zero());
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
    while(value < radian_t::zero()) {
        value += 2 * PI;
    }
    return value;
}

inline radian_t normalize180(radian_t value) {
    while(value >= PI) {
        value -= PI;
    }
    while(value < radian_t::zero()) {
        value += PI;
    }
    return value;
}

inline bool eqWithOverflow360(radian_t value, radian_t ref, radian_t eps) {
    return eq(value, ref, eps) || eq(value + 2 * PI, ref, eps) || eq(value - 2 * PI, ref, eps);
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
        result = radian_t::zero();
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
        result = radian_t::zero();
    }

    return result;
}

inline bool isMultipleOf90(radian_t value, radian_t eps) {

    return eqWithOverflow360(value, radian_t::zero(), eps)
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

inline radian_t straighten(radian_t angle, radian_t eps) {
    if (eq(angle, PI_2, eps)) {
        angle = PI_2;
    } else if (eq(angle, PI, eps)) {
        angle = PI;
    } else if (eq(angle, 3 * PI_2, eps)) {
        angle = 3 * PI_2;
    } else if (isZero(angle, eps) || eq(angle, 2 * PI, eps)) {
        angle = radian_t::zero();
    }

    return angle;
}
} // namespace micro

