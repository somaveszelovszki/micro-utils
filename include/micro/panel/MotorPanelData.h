#ifndef MICRO_PANEL_MOTOR_PANEL_DATA
#define MICRO_PANEL_MOTOR_PANEL_DATA

#include <micro/utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    
#define MOTOR_PANEL_FLAG_USE_SAFETY_SIGNAL 0x01

typedef struct {
    union {
        struct {
            float controller_Kc;
            int16_t targetSpeed_mmps;
            uint16_t controller_Ti_us;
            uint8_t flags;
        };
        uint32_t data32[3];
    };
} motorPanelDataIn_t;

#define dataSize_motorPanelDataIn 9
    
typedef struct {
    union {
        struct {
            int32_t distance_mm;
            int16_t targetSpeed_mmps;
            int16_t actualSpeed_mmps;
            uint8_t isMotorEnabled;
        };
        uint32_t data32[2];
    };
} motorPanelDataOut_t;

#define dataSize_motorPanelDataOut 9

#ifdef __cplusplus
}
#endif // __cplusplus
    
#endif // MICRO_PANEL_MOTOR_PANEL_DATA
