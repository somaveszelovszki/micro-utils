#include <micro/container/vector.hpp>
#include <micro/test/utils.hpp>

namespace {

template <typename T> void test() {
    micro::vector<T, 4> values;
    values.push_back(T(1));
    values.push_back(T(2));
    values.push_back(T(3));
    values.push_back(T(4));

    ASSERT_EQ(4, values.size());
    EXPECT_EQ(T(1), values[0]);
    EXPECT_EQ(T(2), values[1]);
    EXPECT_EQ(T(3), values[2]);
    EXPECT_EQ(T(4), values[3]);
}

TEST(vector, alignment_uint8_t) {
    test<uint8_t>();
}

TEST(vector, alignment_uint16_t) {
    test<uint16_t>();
}

TEST(vector, alignment_uint32_t) {
    test<uint32_t>();
}

TEST(vector, alignment_uint64_t) {
    test<uint64_t>();
}

} // namespace
