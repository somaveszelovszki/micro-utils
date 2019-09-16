#ifndef MICRO_UTILS_TYPES_H
#define MICRO_UTILS_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* @brief Concatenates values.
 * @param a The first value.
 * @param b The second value.
 **/
#define CONCAT(a,b) a##b

typedef float float32_t;    // 32-bit floating point number
typedef double float64_t;   // 64-bit floating point number

void onHardFault();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_UTILS_TYPES_H
