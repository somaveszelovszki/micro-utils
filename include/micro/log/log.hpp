#pragma once

#include <utility>

#include <micro/format/format.hpp>
#include <micro/port/queue.hpp>
#include <micro/utils/types.hpp>

namespace micro {

enum class LogLevel : uint8_t {
    Debug   = 0x01,
    Info    = 0x02,
    Warning = 0x03,
    Error   = 0x04
};

const char* to_string(const LogLevel level);

class Log {
public:
    static constexpr char SEPARATOR = '\n';
    static constexpr size_t MAX_MESSAGE_SIZE = 128;

    using Message = char[MAX_MESSAGE_SIZE];

    static Log& instance();

    void setMinLevel(const LogLevel minLevel);
    bool receive(Message& msg);

    template <typename ...Args>
    void enqueue(const LogLevel level, const char * const formatStr, Args&&... args) {
        if (level >= minLevel_) {
            Message msg;
            format_to(msg, level, formatStr, std::forward<Args>(args)...);
            queue_.send(msg, millisecond_t(0));
        }
    }

    template <typename ...Args>
    static void format_to(Message& buffer, const LogLevel level, const char * const formatStr, Args&&... args) {
        auto idx = format_to_n(buffer, MAX_MESSAGE_SIZE, "{}:", to_string(level));
        idx += format_to_n(&buffer[idx], MAX_MESSAGE_SIZE - idx, formatStr, std::forward<Args>(args)...);
        format_to_n(&buffer[idx], MAX_MESSAGE_SIZE - idx, "{}", SEPARATOR);
    }

private:
    Log() = default;

private:
    queue_t<Message, 12> queue_;
    LogLevel minLevel_{LogLevel::Debug};
};

#define LOG_DEBUG(format, ...)   ::micro::Log::instance().enqueue(::micro::LogLevel::Debug,   format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    ::micro::Log::instance().enqueue(::micro::LogLevel::Info,    format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)    ::micro::Log::instance().enqueue(::micro::LogLevel::Warning, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)   ::micro::Log::instance().enqueue(::micro::LogLevel::Error,   format, ##__VA_ARGS__)

} // namespace micro
