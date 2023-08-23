#pragma once

#if defined OS_FREERTOS // logging is only supported if FreeRTOS is available

#include <micro/port/queue.hpp>
#include <micro/utils/types.hpp>

#include <cstdarg>

namespace micro {

inline constexpr char LOG_SEPARATOR[] = "\r\n";

constexpr uint8_t LOG_MSG_MAX_SIZE   = 128;
constexpr uint8_t LOG_QUEUE_MAX_SIZE = 16;

class Log {
public:
    typedef char message_t[LOG_MSG_MAX_SIZE];

    enum class level_t : uint8_t {
        Debug   = 0x01,
        Info    = 0x02,
        Warning = 0x03,
        Error   = 0x04
    };

    static Log& instance();

    void setMinLevel(const level_t minLevel);

    /* @brief Prints a debug code and a string to the console through USART.
     * Supported modifiers : %s, %c, %d, %f
     * @param level The log level.
     * @param format The string format.
     * @param args Additional parameters.
     **/
    void vprint(level_t level, const char *format, va_list args);

    /* @brief Prints a debug code and a string to the console through USART.
     * Supported modifiers : %s, %c, %d, %f
     * @param level The log level.
     * @param format The string format.
     * @params Additional parameters.
     **/
    void print(level_t level, const char *format, ...);

    bool receive(message_t& msg);

private:
    Log() : minLevel_(level_t::Debug) {}

    queue_t<message_t, LOG_QUEUE_MAX_SIZE> queue_;
    level_t minLevel_;
};

const char* to_string(const Log::level_t& level);

} // namespace micro

#define LOG_DEBUG(format, ...)   micro::Log::instance().print(micro::Log::level_t::Debug,   format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    micro::Log::instance().print(micro::Log::level_t::Info,    format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)    micro::Log::instance().print(micro::Log::level_t::Warning, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)   micro::Log::instance().print(micro::Log::level_t::Error,   format, ##__VA_ARGS__)

#else

#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#define LOG_WARN(format, ...)
#define LOG_ERROR(format, ...)

#endif // OS_FREERTOS
