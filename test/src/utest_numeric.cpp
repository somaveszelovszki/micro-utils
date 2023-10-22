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

TEST(numeric, map) {
    EXPECT_NEAR(10.0f, map(1.0f, 2.0f, 4.0f, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(10.0f, map(2.0f, 2.0f, 4.0f, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(13.0f, map(3.0f, 2.0f, 4.0f, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(4.0f, 2.0f, 4.0f, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(5.0f, 2.0f, 4.0f, 10.f, 16.0f), 0.0001f);

    EXPECT_NEAR(10.0f, map(1, 2, 4, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(10.0f, map(2, 2, 4, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(13.0f, map(3, 2, 4, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(4, 2, 4, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(5, 2, 4, 10.f, 16.0f), 0.0001f);

    EXPECT_NEAR(10.0f, map(1u, 2u, 4u, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(10.0f, map(2u, 2u, 4u, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(13.0f, map(3u, 2u, 4u, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(4u, 2u, 4u, 10.f, 16.0f), 0.0001f);
    EXPECT_NEAR(16.0f, map(5u, 2u, 4u, 10.f, 16.0f), 0.0001f);

    EXPECT_NEAR(10, map(1, 2, 4, 10, 16), 0.0001f);
    EXPECT_NEAR(10, map(2, 2, 4, 10, 16), 0.0001f);
    EXPECT_NEAR(13, map(3, 2, 4, 10, 16), 0.0001f);
    EXPECT_NEAR(16, map(4, 2, 4, 10, 16), 0.0001f);
    EXPECT_NEAR(16, map(5, 2, 4, 10, 16), 0.0001f);

    EXPECT_NEAR(10u, map(1u, 2u, 4u, 10u, 16u), 0.0001f);
    EXPECT_NEAR(10u, map(2u, 2u, 4u, 10u, 16u), 0.0001f);
    EXPECT_NEAR(13u, map(3u, 2u, 4u, 10u, 16u), 0.0001f);
    EXPECT_NEAR(16u, map(4u, 2u, 4u, 10u, 16u), 0.0001f);
    EXPECT_NEAR(16u, map(5u, 2u, 4u, 10u, 16u), 0.0001f);

    EXPECT_EQ(10.0f, map(1.0f, 1.0f, 4.0f, 10.f, 16.0f));
    EXPECT_EQ(10.0f, map(1, 1, 1, 10.f, 16.0f));
    EXPECT_EQ(10,    map(1, 1, 1, 10, 16));
    EXPECT_EQ(10u,   map(1u, 1u, 1u, 10u, 16u));
}
