#ifndef MICRO_PANEL_LINE_DETECT_PANEL_DATA
#define MICRO_PANEL_LINE_DETECT_PANEL_DATA

#include "lines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LINE_DETECT_PANEL_FLAG_INDICATOR_LEDS_ENABLED 0x01
    
typedef struct {
    union {
        struct {
            uint8_t flags;
        };
        uint32_t data32[1];
    };
} lineDetectPanelDataIn_t;

#define dataSize_lineDetectPanelDataIn 1
    
typedef struct {
    union {
        struct {
            linePositions_t lines;
        };
        uint32_t data32[1];
    };
} lineDetectPanelDataOut_t;

#define dataSize_lineDetectPanelDataOut 4

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_LINE_DETECT_PANEL_DATA
