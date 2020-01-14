#ifndef MICRO_PANEL_MOTOR_PANEL_DATA
#define MICRO_PANEL_MOTOR_PANEL_DATA

#include <micro/utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
#define MOTOR_PANEL_FLAG_USE_SAFETY_SIGNAL 0x01

typedef struct {
    float controller_P;
    float controller_I;
    float controller_integral_max;
    int16_t targetSpeed_mmps;
    uint8_t flags;
} __attribute__((packed)) motorPanelDataIn_t;
    
#define MOTOR_PANEL_LINK_RX_PERIOD_MS   20u
#define MOTOR_PANEL_LINK_RX_TIMEOUT_MS  50u

typedef struct {
    int32_t distance_mm;
    int16_t actualSpeed_mmps;
    bool isMotorEnabled;
} __attribute__((packed)) motorPanelDataOut_t;

#define MOTOR_PANEL_LINK_TX_PERIOD_MS   5u
#define MOTOR_PANEL_LINK_TX_TIMEOUT_MS  20u

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_MOTOR_PANEL_DATA
