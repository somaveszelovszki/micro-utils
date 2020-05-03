#include <micro/math/unit_utils.hpp>

#include "utils.hpp"

using namespace micro;

TEST(units, constructors) {
    EXPECT_EQ(0.0f, meter_t().get());
    EXPECT_EQ(10.0f, meter_t(10.0f).get());
    EXPECT_EQ(micro::numeric_limits<float>::infinity(), micro::numeric_limits<radian_t>::infinity().get());
}

TEST(units, arithmetics) {
    EXPECT_NEAR_UNIT(meter_t(1), centimeter_t(50) + centimeter_t(50), millimeter_t(1));
    EXPECT_NEAR_UNIT(meter_t(1), centimeter_t(150) - centimeter_t(50), millimeter_t(1));
    EXPECT_EQ(m_per_sec_t(1), meter_t(1) / second_t(1));
    EXPECT_EQ(meter_t(1), m_per_sec_t(1) * second_t(1));
}
