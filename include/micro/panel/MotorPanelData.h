#ifndef MICRO_PANEL_MOTOR_PANEL_DATA
#define MICRO_PANEL_MOTOR_PANEL_DATA

#include <micro/utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
#define MOTOR_PANEL_FLAG_USE_SAFETY_SIGNAL 0x01

typedef struct {
    int16_t targetSpeed_mmps;
    uint16_t controller_Ti_us;
    float controller_Kc;
    uint8_t flags;
} motorPanelDataIn_t;

#define dataSize_motorPanelDataIn 9
    
typedef struct {
    int32_t distance_mm;
    int16_t actualSpeed_mmps;
} motorPanelDataOut_t;

#define dataSize_motorPanelDataOut 6

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_MOTOR_PANEL_DATA
