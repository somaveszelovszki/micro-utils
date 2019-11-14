#pragma once

#include <micro/utils/types.hpp>

#include <cstdarg>

#define LOG_ENABLED 1
#define MIN_LOG_LEVEL LogLevel_Debug

#define LOG_MSG_MAX_SIZE 512

namespace micro {

#if LOG_ENABLED

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
