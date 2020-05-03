#include <micro/utils/units.hpp>

#include "gtest/gtest.h"

template <typename T1, typename T2, typename T3>
inline typename std::enable_if<T1::is_dim_class && T2::is_dim_class && T3::is_dim_class && T1::dim == T2::dim && T1::dim == T3::dim, void>::type EXPECT_NEAR_UNIT(const T1& a, const T2& b, const T3& eps) {
    const float _a   = a.template get<true>();
    const float _b   = static_cast<T1>(b).template get<true>();
    const float _eps = static_cast<T1>(eps).template get<true>();
    EXPECT_NEAR(_a, _b, _eps);
}
