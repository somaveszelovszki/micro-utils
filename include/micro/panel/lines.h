#ifndef MICRO_PANEL_LINES_H
#define MICRO_PANEL_LINES_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <micro/utils/types.h>

#define MAX_NUM_LINES 3

typedef struct {
    int8_t pos_mm;
    uint8_t id;
} line_t;

typedef struct {
    uint8_t numLines;
    line_t values[MAX_NUM_LINES];
} lines_t;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_PANEL_LINES_H
