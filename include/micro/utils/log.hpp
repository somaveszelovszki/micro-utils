#pragma once

#include <cfg_micro.hpp>

#if LOG_ENABLED

#include <micro/utils/types.hpp>

#include <FreeRTOS.h>
#include <queue.h>

#include <cstdarg>

namespace micro {

constexpr uint8_t LOG_MSG_MAX_SIZE = 128;

void log_init(QueueHandle_t logQueue);

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

} // namespace micro

#else

namespace micro {

#define vprintlog(...)
#define printlog(...)
#define log_init(...)

} // namespace micro

#endif // LOG_ENABLED

#if MIN_LOG_LEVEL <= LogLevel_Debug && LOG_ENABLED
#define LOG_DEBUG(format, ...) micro::printlog(LogLevel_Debug, format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if MIN_LOG_LEVEL <= LogLevel_Info && LOG_ENABLED
#define LOG_INFO(format, ...) micro::printlog(LogLevel_Info, format, ##__VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if MIN_LOG_LEVEL <= LogLevel_Warning && LOG_ENABLED
#define LOG_WARNING(format, ...) micro::printlog(LogLevel_Warning, format, ##__VA_ARGS__)
#else
#define LOG_WARNING(...)
#endif

#if MIN_LOG_LEVEL <= LogLevel_Error && LOG_ENABLED
#define LOG_ERROR(format, ...) micro::printlog(LogLevel_Error, format, ##__VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif
