#ifndef MICRO_PANEL_LINES_H
#define MICRO_PANEL_LINES_H

#include <micro/utils/types.h>

#define MAX_NUM_LINES 3

typedef struct {
    int8_t pos_mm;
} linePosition_t;

typedef struct {
    uint8_t numLines;
    linePosition_t values[MAX_NUM_LINES];
} linePositions_t;

#endif // MICRO_PANEL_LINES_H
