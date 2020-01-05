#ifndef MICRO_PANEL_LINE_DETECT_PANEL_DATA
#define MICRO_PANEL_LINE_DETECT_PANEL_DATA

#include "lines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LINE_DETECT_PANEL_FLAG_INDICATOR_LEDS_ENABLED 0x01
    
typedef struct {
    uint8_t flags;
} __attribute__((packed)) lineDetectPanelDataIn_t;

#define LINE_DETECT_PANEL_LINK_RECV_PERIOD_MS   400u
    
typedef struct {
    lines_t lines;
} __attribute__((packed)) lineDetectPanelDataOut_t;

#define LINE_DETECT_PANEL_LINK_SEND_PERIOD_MS   0u // sends data as soon as they are available

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_LINE_DETECT_PANEL_DATA
