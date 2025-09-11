#include <micro/container/set.hpp>
#include <micro/test/utils.hpp>

namespace {

template <typename T> void test() {
    micro::set<T, 4> values;
    values.insert(T(1));
    values.insert(T(2));
    values.insert(T(3));
    values.insert(T(4));

    ASSERT_EQ(4, values.size());
    EXPECT_EQ(T(1), *values.begin());
    EXPECT_EQ(T(2), *std::next(values.begin(), 1));
    EXPECT_EQ(T(3), *std::next(values.begin(), 2));
    EXPECT_EQ(T(4), *std::next(values.begin(), 3));
}

TEST(set, alignment_uint8_t) {
    test<uint8_t>();
}

TEST(set, alignment_uint16_t) {
    test<uint16_t>();
}

TEST(set, alignment_uint32_t) {
    test<uint32_t>();
}

TEST(set, alignment_uint64_t) {
    test<uint64_t>();
}

} // namespace
