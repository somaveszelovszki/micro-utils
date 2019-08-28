#pragma once

#include <micro/container/vec.hpp>
#include <micro/utils/types.hpp>
#include <micro/bsp/uart.hpp>
#include <micro/bsp/queue.hpp>

#include <cstdarg>

namespace micro {

/* @brief Prints a string to the result string.
 * Supported modifiers : %s, %c, %d, %f
 * @param str The result string.
 * @param size The size of the result string.
 * @param format The string format.
 * @param args Additional parameters.
 **/
void vsprint(char * const str, const uint32_t size, const char *format, va_list args);

/* @brief Prints a string to the result string.
 * Supported modifiers : %s, %c, %d, %f
 * @param str The result string.
 * @param size The size of the result string.
 * @param format The string format.
 * @params Additional parameters.
 **/
void sprint(char * const str, const uint32_t size, const char *format, ...);

} // namespace micro
