#include <micro/log/log.hpp>

#include <micro/test/utils.hpp>

using namespace micro;

TEST(format, format_debug) {
    Log::Message result;
    Log::format_to(result, LogLevel::Debug, "Value is {}.", false);
    EXPECT_STREQ("D:Value is false.\r\n", result);
}

TEST(format, format_info) {
    Log::Message result;
    Log::format_to(result, LogLevel::Info, "Value is {}.", true);
    EXPECT_STREQ("I:Value is true.\r\n", result);
}

TEST(format, format_warning) {
    Log::Message result;
    Log::format_to(result, LogLevel::Warning, "Value is {}.", 42);
    EXPECT_STREQ("W:Value is 42.\r\n", result);
}

TEST(format, format_error) {
    Log::Message result;
    Log::format_to(result, LogLevel::Error, "Value is {}.", -42);
    EXPECT_STREQ("E:Value is -42.\r\n", result);
}
