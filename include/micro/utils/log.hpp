#pragma once

#include <micro/container/vec.hpp>
#include <micro/utils/types.hpp>
#include <micro/bsp/uart.hpp>
#include <micro/bsp/queue.hpp>

#include <cstdarg>

#define LOG_ENABLED 1
#define MIN_LOG_LEVEL LogLevel_Debug

#define LOG_MSG_MAX_SIZE 128

namespace micro {

#if LOG_ENABLED

/* @brief Structure for storing debug messages.
 */
struct LogMessage {
    logLevel_t level;                     // The log message content.
    vec<char, LOG_MSG_MAX_SIZE> text;   // The log message text.
};

/* @brief Prints a debug code and a string to the console through USART.
 * Supported modifiers : %s, %c, %d, %f
 * @param level The log level.
 * @param format The string format.
 * @param args Additional parameters.
 **/
void vprintlog(logLevel_t level, const char *format, va_list args);

/* @brief Prints a debug code and a string to the console through USART.
 * Supported modifiers : %s, %c, %d, %f
 * @param level The log level.
 * @param format The string format.
 * @params Additional parameters.
 **/
void printlog(logLevel_t level, const char *format, ...);

#else

#define vprintlog(...)
#define printlog(...)

#endif // LOG_ENABLED

} // namespace micro

void log_init(micro::queue_handle_t *_logQueue);

#if MIN_LOG_LEVEL <= LogLevel_Debug
#define LOG_DEBUG(format, ...) micro::printlog(LogLevel_Debug, format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if MIN_LOG_LEVEL <= LogLevel_Info
#define LOG_INFO(format, ...) micro::printlog(LogLevel_Info, format, ##__VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if MIN_LOG_LEVEL <= LogLevel_Warning
#define LOG_WARNING(format, ...) micro::printlog(LogLevel_Warning, format, ##__VA_ARGS__)
#else
#define LOG_WARNING(...)
#endif

#if MIN_LOG_LEVEL <= LogLevel_Error
#define LOG_ERROR(format, ...) micro::printlog(LogLevel_Error, format, ##__VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif
