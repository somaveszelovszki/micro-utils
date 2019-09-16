#ifndef MICRO_UTILS_PANEL_LINE_DETECT_PANEL_DATA
#define MICRO_UTILS_PANEL_LINE_DETECT_PANEL_DATA

#include "lines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define LINE_DETECT_PANEL_COMMAND_START 'S'
    
typedef struct {
    char command;
} lineDetectPanelDataIn_t;
    
typedef struct {
    Lines lines;
} lineDetectPanelDataOut_t;

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_UTILS_PANEL_LINE_DETECT_PANEL_DATA
