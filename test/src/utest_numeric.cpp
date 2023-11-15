#include <gtest/gtest.h>
#include <micro/math/numeric.hpp>

#include <micro/test/utils.hpp>

using namespace micro;

TEST(numeric, numeric_limits) {
    EXPECT_EQ(std::numeric_limits<int32_t>::min(), micro::numeric_limits<int32_t>::min());
    EXPECT_EQ(std::numeric_limits<int32_t>::max(), micro::numeric_limits<int32_t>::max());
    EXPECT_TRUE(micro::isnan(micro::numeric_limits<float>::quiet_NaN()));
    EXPECT_EQ(std::numeric_limits<int32_t>::infinity(), micro::numeric_limits<int32_t>::infinity());
    EXPECT_EQ(std::numeric_limits<int32_t>::epsilon(), micro::numeric_limits<int32_t>::epsilon());
}

TEST(numeric, raw_type) {
    EXPECT_EQ(-10.0f, raw_type<float>::get(-10.0f));
    EXPECT_EQ(10, raw_type<int32_t>::get(10));
}

TEST(numeric, base) {
    EXPECT_EQ(-1, min(-1, 0));
    EXPECT_EQ(-1.0f, min(-1.0f, 10.0f));

    EXPECT_EQ(0, max(-1, 0));
    EXPECT_EQ(10.0f, max(-1.0f, 10.0f));

    EXPECT_EQ(200, avg<uint8_t>(190, 210));
    EXPECT_EQ(5.0f, avg(0.0f, 10.0f));

    EXPECT_TRUE(isBtw(0.0, -1.0, 1.0));
    EXPECT_TRUE(isBtw(-1.0, -1.0, 1.0));
    EXPECT_TRUE(isBtw(1.0, -1.0, 1.0));
    EXPECT_FALSE(isBtw(2.0, -1.0, 1.0));

    EXPECT_EQ(1, clamp(0, 1, 3));
    EXPECT_EQ(1, clamp(1, 1, 3));
    EXPECT_EQ(2, clamp(2, 1, 3));
    EXPECT_EQ(3, clamp(3, 1, 3));
    EXPECT_EQ(3, clamp(4, 1, 3));

    EXPECT_TRUE(isInRange(9, 10, 0.2f));
    EXPECT_TRUE(isInRange(10, 10, 0.2f));
    EXPECT_TRUE(isInRange(11, 10, 0.2f));
    EXPECT_FALSE(isInRange(13, 10, 0.2f));
}

TEST(numeric, lerp) {
    EXPECT_NEAR_DEFAULT(10.0f, lerp(1.0f, 2.0f, 4.0f, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(10.0f, lerp(2.0f, 2.0f, 4.0f, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(13.0f, lerp(3.0f, 2.0f, 4.0f, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(16.0f, lerp(4.0f, 2.0f, 4.0f, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(16.0f, lerp(5.0f, 2.0f, 4.0f, 10.f, 16.0f));

    EXPECT_NEAR_DEFAULT(10.0f, lerp(1, 2, 4, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(10.0f, lerp(2, 2, 4, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(13.0f, lerp(3, 2, 4, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(16.0f, lerp(4, 2, 4, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(16.0f, lerp(5, 2, 4, 10.f, 16.0f));

    EXPECT_NEAR_DEFAULT(10.0f, lerp(1u, 2u, 4u, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(10.0f, lerp(2u, 2u, 4u, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(13.0f, lerp(3u, 2u, 4u, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(16.0f, lerp(4u, 2u, 4u, 10.f, 16.0f));
    EXPECT_NEAR_DEFAULT(16.0f, lerp(5u, 2u, 4u, 10.f, 16.0f));

    EXPECT_NEAR_DEFAULT(10, lerp(1, 2, 4, 10, 16));
    EXPECT_NEAR_DEFAULT(10, lerp(2, 2, 4, 10, 16));
    EXPECT_NEAR_DEFAULT(13, lerp(3, 2, 4, 10, 16));
    EXPECT_NEAR_DEFAULT(16, lerp(4, 2, 4, 10, 16));
    EXPECT_NEAR_DEFAULT(16, lerp(5, 2, 4, 10, 16));

    EXPECT_NEAR_DEFAULT(10u, lerp(1u, 2u, 4u, 10u, 16u));
    EXPECT_NEAR_DEFAULT(10u, lerp(2u, 2u, 4u, 10u, 16u));
    EXPECT_NEAR_DEFAULT(13u, lerp(3u, 2u, 4u, 10u, 16u));
    EXPECT_NEAR_DEFAULT(16u, lerp(4u, 2u, 4u, 10u, 16u));
    EXPECT_NEAR_DEFAULT(16u, lerp(5u, 2u, 4u, 10u, 16u));

    EXPECT_EQ(10.0f, lerp(1.0f, 1.0f, 4.0f, 10.f, 16.0f));
    EXPECT_EQ(10.0f, lerp(1, 1, 1, 10.f, 16.0f));
    EXPECT_EQ(10,    lerp(1, 1, 1, 10, 16));
    EXPECT_EQ(10u,   lerp(1u, 1u, 1u, 10u, 16u));
}

TEST(numeric, normalize_into_periodic_interval) {
    EXPECT_NEAR_DEFAULT(10.0, normalize_into_periodic_interval(-710.0, -180.0, 180.0));
    EXPECT_NEAR_DEFAULT(10.0, normalize_into_periodic_interval(-350.0, -180.0, 180.0));
    EXPECT_NEAR_DEFAULT(-170.0, normalize_into_periodic_interval(-170.0, -180.0, 180.0));
    EXPECT_NEAR_DEFAULT(0.0, normalize_into_periodic_interval(0.0, -180.0, 180.0));
    EXPECT_NEAR_DEFAULT(10.0, normalize_into_periodic_interval(10.0, -180.0, 180.0));
    EXPECT_NEAR_DEFAULT(-170.0, normalize_into_periodic_interval(190.0, -180.0, 180.0));
    EXPECT_NEAR_DEFAULT(-170.0, normalize_into_periodic_interval(550.0, -180.0, 180.0));
}

TEST(numeric, equal_in_periodic_interval) {
    EXPECT_TRUE(equal_in_periodic_interval(0.0, -720.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(0.0, -360.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(0.0, 0.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(0.0, 360.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(0.0, 720.0, -180.0, 180.0, 0.0001));
    EXPECT_FALSE(equal_in_periodic_interval(0.0, 10.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(-350.0, 10.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(-170.0, 190.0, -180.0, 180.0, 0.0001));
    EXPECT_TRUE(equal_in_periodic_interval(-10.0, 0.0, 0.0, 100.0, 20.0));
    EXPECT_TRUE(equal_in_periodic_interval(10.0, 0.0, 0.0, 100.0, 20.0));
}

TEST(numeric, round_in_periodic_interval) {
    EXPECT_NEAR_DEFAULT(0.0, round_in_periodic_interval(-40.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(0.0, round_in_periodic_interval(0.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(0.0, round_in_periodic_interval(40.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(90.0, round_in_periodic_interval(50.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(90.0, round_in_periodic_interval(90.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(90.0, round_in_periodic_interval(130.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(180.0, round_in_periodic_interval(140.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(180.0, round_in_periodic_interval(180.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(180.0, round_in_periodic_interval(220.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(270.0, round_in_periodic_interval(230.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(270.0, round_in_periodic_interval(270.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(270.0, round_in_periodic_interval(310.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(0.0, round_in_periodic_interval(320.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(0.0, round_in_periodic_interval(360.0, 90.0, 0.0, 360.0));
    EXPECT_NEAR_DEFAULT(0.0, round_in_periodic_interval(400.0, 90.0, 0.0, 360.0));
}
