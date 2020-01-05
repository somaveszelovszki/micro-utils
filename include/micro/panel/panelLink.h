#ifndef MICRO_UTILS_PANEL_LINK_H
#define MICRO_UTILS_PANEL_LINK_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "panelData.h"

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_uart.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_uart.h>
#endif

typedef struct {
    UART_HandleTypeDef *huart;
    panelStartData_t startData;
    void *rxData;
    uint32_t rxDataSize;
    uint32_t rxPeriodMs;
    uint32_t lastRxTime;
    const void *txData;
    uint32_t txDataSize;
    uint32_t txPeriodMs;
    uint32_t lastTxTime;
    bool isConnected;
    bool isAvailable;
} panelLink_t;

void panelLink_initialize(panelLink_t *link, UART_HandleTypeDef *huart,
    void *rxData, uint32_t rxDataSize, uint32_t rxPeriodMs,
    const void *txData, uint32_t txDataSize, uint32_t txPeriodMs);

bool panelLink_shouldSend(const panelLink_t *link);

void panelLink_send(panelLink_t *link);

void panelLink_onNewCmd(panelLink_t *link);

bool panelLink_checkAvailable(panelLink_t *link);

void panelLink_checkConnection(panelLink_t *link);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_UTILS_PANEL_LINK_H
