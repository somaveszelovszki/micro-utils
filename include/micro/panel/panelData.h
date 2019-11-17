#ifndef MICRO_PANEL_PANEL_DATA
#define MICRO_PANEL_PANEL_DATA

#include "lines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define PANEL_START 'S'
    
typedef struct {
    union {
        struct {
            uint8_t cmd;
        };
        uint32_t data32[1];
    };
} panelStartData_t;

#define dataSize_panelStartData 1

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_PANEL_DATA
