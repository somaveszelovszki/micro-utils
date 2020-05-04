#pragma once

#if defined OS_FREERTOS // logging is only supported if FreeRTOS is available

#include <micro/utils/types.hpp>

#include <FreeRTOS.h>
#include <queue.h>

#include <cstdarg>

namespace micro {

constexpr uint8_t LOG_MSG_MAX_SIZE = 128;

void log_init(QueueHandle_t logQueue, const logLevel_t minLogLevel = logLevel_t::Debug);

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

#define LOG_DEBUG(format, ...)   micro::printlog(logLevel_t::Debug,   format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    micro::printlog(logLevel_t::Info,    format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) micro::printlog(logLevel_t::Warning, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)   micro::printlog(logLevel_t::Error,   format, ##__VA_ARGS__)

#else

#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#define LOG_WARNING(format, ...)
#define LOG_ERROR(format, ...)

#endif // OS_FREERTOS
