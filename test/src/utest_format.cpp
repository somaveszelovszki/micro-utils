#include <gtest/gtest.h>
#include <micro/format/format.hpp>
#include <micro/test/utils.hpp>
#include <micro/utils/types.hpp>

TEST(format, format_empty) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "");
    EXPECT_STREQ("", result);
}

TEST(format, format_no_args) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Hello");
    EXPECT_STREQ("Hello", result);
}

TEST(format, format_false) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", false);
    EXPECT_STREQ("Value is false.", result);
}

TEST(format, format_true) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", true);
    EXPECT_STREQ("Value is true.", result);
}

TEST(format, format_positive_integer) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", 42);
    EXPECT_STREQ("Value is 42.", result);
}

TEST(format, format_negative_integer) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", -42);
    EXPECT_STREQ("Value is -42.", result);
}

TEST(format, format_positive_float) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", 12.0345f);
    EXPECT_STREQ("Value is 12.0345.", result);
}

TEST(format, format_negative_float) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", -12.3400f);
    EXPECT_STREQ("Value is -12.3400.", result);
}

TEST(format, format_string) {
    char result[100];
    micro::format_to_n(result, ARRAY_SIZE(result), "Value is {}.", "hello");
    EXPECT_STREQ("Value is hello.", result);
}
