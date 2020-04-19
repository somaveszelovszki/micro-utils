#ifndef MICRO_PANEL_PANEL_VERSION
#define MICRO_PANEL_PANEL_VERSION

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef uint8_t panelVersion_t;

panelVersion_t panelVersion_get(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_PANEL_PANEL_VERSION
