#ifndef MICRO_UTILS_TYPES_H
#define MICRO_UTILS_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* @brief Concatenates values.
 * @param a The first value.
 * @param b The second value.
 **/
#define CONCAT(a,b) a##b

void onHardFault();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_UTILS_TYPES_H
