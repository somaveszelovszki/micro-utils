#ifndef MICRO_PANEL_LINES_H
#define MICRO_PANEL_LINES_H

#include <micro/utils/types.h>

#define MAX_NUM_LINES 3

typedef struct {
    int8_t pos_mm;
} Line;

typedef struct {
    uint8_t numLines;
    Line values[MAX_NUM_LINES];
} Lines;

#endif // MICRO_PANEL_LINES_H
