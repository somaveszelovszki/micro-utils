#ifndef MICRO_PANEL_LINE_DETECT_PANEL_DATA
#define MICRO_PANEL_LINE_DETECT_PANEL_DATA

#include "lines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LINE_DETECT_PANEL_FLAG_INDICATOR_LEDS_ENABLED 0x01
    
typedef struct {
    uint8_t flags;
} lineDetectPanelDataIn_t;
    
typedef struct {
    Lines lines;
} lineDetectPanelDataOut_t;

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_LINE_DETECT_PANEL_DATA
