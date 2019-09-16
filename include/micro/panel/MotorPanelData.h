#ifndef MICRO_PANEL_MOTOR_PANEL_DATA
#define MICRO_PANEL_MOTOR_PANEL_DATA

#include <micro/utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
#define FLAG_USE_SAFETY_SIGNAL 0x01

typedef struct {
    int16_t targetSpeed_mmps;
    uint16_t controller_Ti_us;
    float controller_Kc;
    uint8_t flags;
} motorPanelDataIn_t;
    
typedef struct {
    int16_t actualSpeed_mmps;
} motorPanelDataOut_t;

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_MOTOR_PANEL_DATA
