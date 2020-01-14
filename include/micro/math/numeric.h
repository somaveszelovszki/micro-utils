#ifndef MICRO_UTILS_NUMERIC_H
#define MICRO_UTILS_NUMERIC_H

#include <micro/utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : (-a))
#define AVG(a, b) ((a) / 2 + (b) / 2)

#define CLAMP(value, bound1, bound2)    \
((bound1) <= (bound2) ?                 \
MAX(MIN((value), (bound2)), (bound1)) : \
MIN(MAX((value), (bound2)), (bound1)))

#define MAP(value, from_low, from_high, to_low, to_high) \
((to_low) + (CLAMP((value), (from_low), (from_high)) - (from_low)) * ((to_high) - (to_low)) / ((from_high) - (from_low)))

int32_t round_to_int(float value);

void array_average_min_max(const uint8_t *data, uint32_t size, uint8_t *avg_, uint8_t *min_, uint8_t *max_);

#define COMPARE_FUNCTION(_abbr_) int compare_ ## _abbr_(const void *a, const void *b);

COMPARE_FUNCTION(u8)
COMPARE_FUNCTION(u16)
COMPARE_FUNCTION(u32)
COMPARE_FUNCTION(i8)
COMPARE_FUNCTION(i16)
COMPARE_FUNCTION(i32)
COMPARE_FUNCTION(float)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_UTILS_NUMERIC_H
