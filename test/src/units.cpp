#include "gtest/gtest.h"

#include <micro/math/unit_utils.hpp>

using namespace micro;

TEST(units, constructors) {
    EXPECT_EQ(0.0f, meter_t().get());
    EXPECT_EQ(10.0f, meter_t(10.0f).get());
    EXPECT_EQ(std::numeric_limits<float>::infinity(), std::numeric_limits<radian_t>::infinity().get());
}
