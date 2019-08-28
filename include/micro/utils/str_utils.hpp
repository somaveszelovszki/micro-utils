#pragma once

#include <micro/container/vec.hpp>
#include <micro/utils/types.hpp>
#include <micro/bsp/uart.hpp>
#include <micro/bsp/queue.hpp>

#include <cstdarg>

namespace micro {

uint32_t atoi(const char * const s, int32_t *pResult);

uint32_t atof(const char * const s, float32_t *pResult);

/* @brief Converts integer to string.
 * @param n The source integer.
 * @param s The result string.
 * @param size Size of the result string.
 * @param padding The minimum number of printed digits - 0 by default.
 * @returns Number of characters written.
 **/
uint32_t itoa(int32_t n, char *const s, uint32_t size, uint32_t padding = 0);

uint32_t ftoa(float32_t n, char * const s, uint32_t size);

/* @brief Prints a string to the result string.
 * Supported modifiers : %s, %c, %d, %f
 * @param str The result string.
 * @param size The size of the result string.
 * @param format The string format.
 * @param args Additional parameters.
 * @returns Number of characters written.
 **/
uint32_t vsprint(char * const str, const uint32_t size, const char *format, va_list args);

/* @brief Prints a string to the result string.
 * Supported modifiers : %s, %c, %d, %f
 * @param str The result string.
 * @param size The size of the result string.
 * @param format The string format.
 * @params Additional parameters.
 * @returns Number of characters written.
 **/
uint32_t sprint(char * const str, const uint32_t size, const char *format, ...);

} // namespace micro
