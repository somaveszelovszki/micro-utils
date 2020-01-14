#ifndef MICRO_PANEL_LINE_DETECT_PANEL_DATA
#define MICRO_PANEL_LINE_DETECT_PANEL_DATA

#include "lines.h"

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LINE_DETECT_PANEL_FLAG_INDICATOR_LEDS_ENABLED 0x01

typedef struct {
    uint8_t flags;
} __attribute__((packed)) lineDetectPanelDataIn_t;

#define LINE_DETECT_PANEL_LINK_IN_PERIOD_MS   UINT_MAX
#define LINE_DETECT_PANEL_LINK_IN_TIMEOUT_MS  UINT_MAX
    
typedef struct {
    lines_t lines;
} __attribute__((packed)) lineDetectPanelDataOut_t;

#define LINE_DETECT_PANEL_LINK_OUT_PERIOD_MS   0u
#define LINE_DETECT_PANEL_LINK_OUT_TIMEOUT_MS  15u

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_LINE_DETECT_PANEL_DATA
