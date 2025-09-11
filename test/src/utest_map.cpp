#include <micro/container/map.hpp>
#include <micro/test/utils.hpp>

namespace {

template <typename T> void test() {
    micro::map<T, T, 4> values;
    values.insert({T(1), T(10)});
    values.insert({T(2), T(20)});
    values.insert({T(3), T(30)});
    values.insert({T(4), T(40)});

    ASSERT_EQ(4, values.size());
    EXPECT_EQ_PAIR(std::make_pair(T(1), T(10)), (*values.begin()));
    EXPECT_EQ_PAIR(std::make_pair(T(2), T(20)), (*std::next(values.begin(), 1)));
    EXPECT_EQ_PAIR(std::make_pair(T(3), T(30)), (*std::next(values.begin(), 2)));
    EXPECT_EQ_PAIR(std::make_pair(T(4), T(40)), (*std::next(values.begin(), 3)));
}

TEST(map, alignment_uint8_t) {
    test<uint8_t>();
}

TEST(map, alignment_uint16_t) {
    test<uint16_t>();
}

TEST(map, alignment_uint32_t) {
    test<uint32_t>();
}

TEST(map, alignment_uint64_t) {
    test<uint64_t>();
}

} // namespace
