#include <micro/math/unit_utils.hpp>

#include "gtest/gtest.h"

template <typename T1, typename T2, typename T3>
inline typename std::enable_if<T1::dim != micro::Dimension::angle && micro::is_same_unit_dimension<T1, T2>::value && micro::is_same_unit_dimension<T1, T3>::value, void>::type
EXPECT_NEAR_UNIT(T1 a, T2 b, T3 eps) {
    const float _a   = a.template get<true>();
    const float _b   = static_cast<T1>(b).template get<true>();
    const float _eps = static_cast<T1>(eps).template get<true>();
    EXPECT_NEAR(_a, _b, _eps);
}

template <typename T1, typename T2, typename T3>
inline typename std::enable_if<T1::dim == micro::Dimension::angle && micro::is_same_unit_dimension<T1, T2>::value && micro::is_same_unit_dimension<T1, T3>::value, void>::type
EXPECT_NEAR_UNIT(T1 a, T2 b, T3 eps) {
    while(a > b + micro::PI) {
        b += 2 * micro::PI;
    }
    while(b > a + micro::PI) {
        a += 2 * micro::PI;
    }

    const float _a   = a.template get<true>();
    const float _b   = static_cast<T1>(b).template get<true>();
    const float _eps = static_cast<T1>(eps).template get<true>();
    EXPECT_NEAR(_a, _b, _eps);
}
