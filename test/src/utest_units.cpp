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

TEST(units, numeric_limits) {
    EXPECT_EQ(std::numeric_limits<float>::min(), micro::numeric_limits<millimeter_t>::min().get());
    EXPECT_EQ(std::numeric_limits<float>::max(), micro::numeric_limits<m_per_sec_t>::max().get());
    EXPECT_TRUE(micro::isnan(micro::numeric_limits<celsius_t>::quiet_NaN()));
    EXPECT_EQ(std::numeric_limits<float>::infinity(), micro::numeric_limits<mm_per_sec2_t>::infinity().get());
    EXPECT_EQ(std::numeric_limits<float>::epsilon(), micro::numeric_limits<radian_t>::epsilon().get());
}

TEST(units, raw_type) {
    EXPECT_EQ(-10.0f, raw_type<radian_t>::get(radian_t(-10.0f)));
}

TEST(units, base) {
    EXPECT_EQ(degree_t(-1), min(degree_t(-1), degree_t(0)));
    EXPECT_EQ(degree_t(0), max(degree_t(-1), degree_t(0)));
    EXPECT_NEAR_UNIT(degree_t(10), avg(degree_t(5), degree_t(15)), degree_t(0.0001));

    EXPECT_TRUE(isBtw(m_per_sec_t(10), m_per_sec_t(0), m_per_sec_t(100)));
    EXPECT_FALSE(isBtw(m_per_sec_t(1000), m_per_sec_t(0), m_per_sec_t(100)));

    EXPECT_EQ(radian_t(1), clamp(radian_t(0), radian_t(1), radian_t(3)));
    EXPECT_EQ(radian_t(1), clamp(radian_t(1), radian_t(1), radian_t(3)));
    EXPECT_EQ(radian_t(2), clamp(radian_t(2), radian_t(1), radian_t(3)));
    EXPECT_EQ(radian_t(3), clamp(radian_t(3), radian_t(1), radian_t(3)));
    EXPECT_EQ(radian_t(3), clamp(radian_t(4), radian_t(1), radian_t(3)));

    EXPECT_TRUE(isInRange(millimeter_t(9), millimeter_t(10), 0.2f));
    EXPECT_TRUE(isInRange(millimeter_t(10), millimeter_t(10), 0.2f));
    EXPECT_TRUE(isInRange(millimeter_t(11), millimeter_t(10), 0.2f));
    EXPECT_FALSE(isInRange(millimeter_t(13), millimeter_t(10), 0.2f));
}

TEST(units, map) {
    EXPECT_NEAR(10.0f, map(radian_t(1.0f), radian_t(2.0f), radian_t(4.0f), 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(10.0f, map(radian_t(2.0f), radian_t(2.0f), radian_t(4.0f), 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(13.0f, map(radian_t(3.0f), radian_t(2.0f), radian_t(4.0f), 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(radian_t(4.0f), radian_t(2.0f), radian_t(4.0f), 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(radian_t(5.0f), radian_t(2.0f), radian_t(4.0f), 10.f, 16.0f), 0.0001f);

    EXPECT_NEAR_UNIT(radian_t(10.0f), map(1.0f, 2.0f, 4.0f, radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(10.0f), map(2.0f, 2.0f, 4.0f, radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(13.0f), map(3.0f, 2.0f, 4.0f, radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(16.0f), map(4.0f, 2.0f, 4.0f, radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(16.0f), map(5.0f, 2.0f, 4.0f, radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));

    EXPECT_NEAR_UNIT(radian_t(10.0f), map(m_per_sec_t(1.0f), m_per_sec_t(2.0f), m_per_sec_t(4.0f), radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(10.0f), map(m_per_sec_t(2.0f), m_per_sec_t(2.0f), m_per_sec_t(4.0f), radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(13.0f), map(m_per_sec_t(3.0f), m_per_sec_t(2.0f), m_per_sec_t(4.0f), radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(16.0f), map(m_per_sec_t(4.0f), m_per_sec_t(2.0f), m_per_sec_t(4.0f), radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
    EXPECT_NEAR_UNIT(radian_t(16.0f), map(m_per_sec_t(5.0f), m_per_sec_t(2.0f), m_per_sec_t(4.0f), radian_t(10.f), radian_t(16.0f)), radian_t(0.0001f));
}
