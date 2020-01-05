#ifndef MICRO_PANEL_MOTOR_PANEL_DATA
#define MICRO_PANEL_MOTOR_PANEL_DATA

#include <micro/utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
#define MOTOR_PANEL_FLAG_USE_SAFETY_SIGNAL 0x01

typedef struct {
    float controller_Kc;
    int16_t targetSpeed_mmps;
    uint16_t controller_Ti_us;
    uint8_t flags;
} __attribute__((packed)) motorPanelDataIn_t;
    
#define MOTOR_PANEL_LINK_RECV_PERIOD_MS   10u

typedef struct {
    int32_t distance_mm;
    int16_t targetSpeed_mmps;
    int16_t actualSpeed_mmps;
    bool isMotorEnabled;
} __attribute__((packed)) motorPanelDataOut_t;

#define MOTOR_PANEL_LINK_SEND_PERIOD_MS   10u

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_MOTOR_PANEL_DATA
