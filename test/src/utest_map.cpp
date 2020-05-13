#include <micro/container/map.hpp>

#include "utils.hpp"

using namespace micro;

TEST(unsorted_map, constructors) {
    unsorted_map<float, int32_t, 10> m1;
    EXPECT_EQ(0, m1.size());
    EXPECT_EQ(10, m1.capacity());

    unsorted_map<uint16_t, float, 5> m2 = { { 1, 1.0f }, { 2, 2.0f }, { 3, 3.0f } };
    EXPECT_EQ(3, m2.size());
    EXPECT_EQ(1.0f, *m2.at(1));
    EXPECT_EQ(2.0f, *m2.at(2));
    EXPECT_EQ(3.0f, *m2.at(3));
    EXPECT_EQ(nullptr, m2.at(4));

    unsorted_map<uint16_t, float, 5> m3(m2);
    EXPECT_EQ(3, m3.size());
    EXPECT_EQ(1.0f, *m3.at(1));
    EXPECT_EQ(2.0f, *m3.at(2));
    EXPECT_EQ(3.0f, *m3.at(3));
    EXPECT_EQ(nullptr, m3.at(4));
}

TEST(unsorted_map, iterators) {
    unsorted_map<uint16_t, float, 5> m1 = { { 1, 1.0f }, { 2, 2.0f }, { 3, 3.0f } };
    EXPECT_EQ(1, m1.begin()->first);
    EXPECT_EQ(1.0f, m1.begin()->second);
    EXPECT_EQ(3, m1.back()->first);
    EXPECT_EQ(3.0f, m1.back()->second);
    EXPECT_EQ(3, std::prev(m1.end())->first);
    EXPECT_EQ(3.0f, std::prev(m1.end())->second);
}

TEST(unsorted_map, modify) {
    unsorted_map<int32_t, int32_t, 5> m1 = { { 1, 1 } };
    EXPECT_EQ(1, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(nullptr, m1.at(2));

    m1.emplace(2, 2);
    EXPECT_EQ(2, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(2, *m1.at(2));

    m1.emplace(2, 3);
    EXPECT_EQ(2, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(3, *m1.at(2));

    m1.erase(m1.begin());
    EXPECT_EQ(1, m1.size());
    EXPECT_EQ(nullptr, m1.at(1));
    EXPECT_EQ(3, *m1.at(2));

    m1.clear();
    EXPECT_EQ(0, m1.size());
}

TEST(sorted_map, constructors) {
    sorted_map<float, int32_t, 10> m1;
    EXPECT_EQ(0, m1.size());
    EXPECT_EQ(10, m1.capacity());

    sorted_map<uint16_t, float, 5> m2 = { { 1, 1.0f }, { 3, 3.0f }, { 2, 2.0f } };
    EXPECT_EQ(3, m2.size());
    EXPECT_EQ(1.0f, *m2.at(1));
    EXPECT_EQ(2.0f, *m2.at(2));
    EXPECT_EQ(3.0f, *m2.at(3));
    EXPECT_EQ(nullptr, m2.at(4));

    sorted_map<uint16_t, float, 5> m3(m2);
    EXPECT_EQ(3, m3.size());
    EXPECT_EQ(1.0f, *m3.at(1));
    EXPECT_EQ(2.0f, *m3.at(2));
    EXPECT_EQ(3.0f, *m3.at(3));
    EXPECT_EQ(nullptr, m3.at(4));
}

TEST(sorted_map, iterators) {
    sorted_map<uint16_t, float, 5> m1 = { { 1, 1.0f }, { 3, 3.0f }, { 2, 2.0f } };
    EXPECT_EQ(1, m1.begin()->first);
    EXPECT_EQ(1.0f, m1.begin()->second);
    EXPECT_EQ(3, m1.back()->first);
    EXPECT_EQ(3.0f, m1.back()->second);
    EXPECT_EQ(3, std::prev(m1.end())->first);
    EXPECT_EQ(3.0f, std::prev(m1.end())->second);
}

TEST(sorted_map, modify) {
    sorted_map<int32_t, int32_t, 5> m1 = { { 1, 1 } };
    EXPECT_EQ(1, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(nullptr, m1.at(2));

    m1.emplace(3, 3);
    EXPECT_EQ(2, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(3, *m1.at(3));

    m1.emplace(2, 2);
    EXPECT_EQ(3, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(2, *m1.at(2));
    EXPECT_EQ(3, *m1.at(3));

    m1.emplace(2, 3);
    EXPECT_EQ(3, m1.size());
    EXPECT_EQ(1, *m1.at(1));
    EXPECT_EQ(3, *m1.at(2));
    EXPECT_EQ(3, *m1.at(3));

    m1.erase(m1.begin());
    EXPECT_EQ(2, m1.size());
    EXPECT_EQ(nullptr, m1.at(1));
    EXPECT_EQ(3, *m1.at(2));
    EXPECT_EQ(3, *m1.at(3));

    m1.clear();
    EXPECT_EQ(0, m1.size());
}

TEST(sorted_map, lerp) {
    sorted_map<float, float, 3> m1 = { { 1.0f, 2.0f }, { 2.0f, 4.0f }, { 3.0f, 6.0f } };
    EXPECT_NEAR(2.0f, m1.lerp(0.0f), 0.0001);
    EXPECT_NEAR(2.0f, m1.lerp(1.0f), 0.0001);
    EXPECT_NEAR(3.0f, m1.lerp(1.5f), 0.0001);
    EXPECT_NEAR(4.0f, m1.lerp(2.0f), 0.0001);
    EXPECT_NEAR(5.0f, m1.lerp(2.5f), 0.0001);
    EXPECT_NEAR(6.0f, m1.lerp(3.0f), 0.0001);
    EXPECT_NEAR(6.0f, m1.lerp(4.0f), 0.0001);
}
