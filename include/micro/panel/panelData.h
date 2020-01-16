#ifndef MICRO_PANEL_PANEL_DATA
#define MICRO_PANEL_PANEL_DATA

#include "lines.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define PANEL_START 'S'

typedef struct {
    char cmd;
} __attribute__((packed)) panelStartData_t;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_PANEL_PANEL_DATA
