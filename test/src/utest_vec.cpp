#include <micro/container/vec.hpp>

#include "utils.hpp"

using namespace micro;

TEST(vec, constructors) {
    vec<float, 10> v1;
    EXPECT_EQ(0, v1.size());
    EXPECT_EQ(10, v1.capacity());

    vec<uint16_t, 5> v2 = { 1, 2, 3 };
    EXPECT_EQ(3, v2.size());
    EXPECT_EQ(1, v2[0]);
    EXPECT_EQ(2, v2[1]);
    EXPECT_EQ(3, v2[2]);

    vec<uint16_t, 5> v3(v2);
    EXPECT_EQ(3, v3.size());
    EXPECT_EQ(1, v3[0]);
    EXPECT_EQ(2, v3[1]);
    EXPECT_EQ(3, v3[2]);
}

TEST(vec, iterators) {
    vec<uint16_t, 5> v1 = { 1, 2, 3 };
    EXPECT_EQ(1, *v1.begin());
    EXPECT_EQ(3, *v1.back());
    EXPECT_EQ(v1.back(), std::prev(v1.end()));
}

TEST(vec, modify) {
    vec<uint16_t, 5> v1 = { 1 };
    EXPECT_EQ(1, v1.size());
    EXPECT_EQ(1, v1[0]);

    v1.emplace_back(2);
    EXPECT_EQ(2, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);

    v1.emplace(v1.end(), 3);
    EXPECT_EQ(3, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);
    EXPECT_EQ(3, v1[2]);

    v1.push_back(5);
    EXPECT_EQ(4, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);
    EXPECT_EQ(3, v1[2]);
    EXPECT_EQ(5, v1[3]);

    v1.insert(v1.back(), 4);
    EXPECT_EQ(5, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);
    EXPECT_EQ(3, v1[2]);
    EXPECT_EQ(4, v1[3]);
    EXPECT_EQ(5, v1[4]);

    v1.erase(v1.begin());
    EXPECT_EQ(4, v1.size());
    EXPECT_EQ(2, v1[0]);
    EXPECT_EQ(3, v1[1]);
    EXPECT_EQ(4, v1[2]);
    EXPECT_EQ(5, v1[3]);

    v1.clear();
    EXPECT_EQ(0, v1.size());

    vec<uint32_t, 2> v2 = { 1, 2 };
    v1.push_back(v2.begin(), v2.end());
    EXPECT_EQ(2, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);
}

TEST(sorted_vec, constructors) {
    sorted_vec<float, 10> v1;
    EXPECT_EQ(0, v1.size());
    EXPECT_EQ(10, v1.capacity());

    sorted_vec<uint16_t, 5> v2 = { 1, 3, 2 };
    EXPECT_EQ(3, v2.size());
    EXPECT_EQ(1, v2[0]);
    EXPECT_EQ(2, v2[1]);
    EXPECT_EQ(3, v2[2]);

    sorted_vec<uint16_t, 5> v3(v2);
    EXPECT_EQ(3, v3.size());
    EXPECT_EQ(1, v3[0]);
    EXPECT_EQ(2, v3[1]);
    EXPECT_EQ(3, v3[2]);
}

TEST(sorted_vec, iterators) {
    sorted_vec<uint16_t, 5> v1 = { 1, 3, 2 };
    EXPECT_EQ(1, *v1.begin());
    EXPECT_EQ(3, *v1.back());
    EXPECT_EQ(v1.back(), std::prev(v1.end()));
}

TEST(sorted_vec, modify) {
    sorted_vec<uint16_t, 5> v1 = { 1 };
    EXPECT_EQ(1, v1.size());
    EXPECT_EQ(1, v1[0]);

    v1.insert(2);
    EXPECT_EQ(2, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);

    vec<uint32_t, 2> v2 = { 3, 1 };
    v1.insert(v2.begin(), v2.end());
    EXPECT_EQ(4, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(1, v1[1]);
    EXPECT_EQ(2, v1[2]);
    EXPECT_EQ(3, v1[3]);

    v1.erase(v1.begin());
    EXPECT_EQ(3, v1.size());
    EXPECT_EQ(1, v1[0]);
    EXPECT_EQ(2, v1[1]);
    EXPECT_EQ(3, v1[2]);

    v1.clear();
    EXPECT_EQ(0, v1.size());
}