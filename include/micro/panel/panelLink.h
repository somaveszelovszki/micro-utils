#ifndef MICRO_UTILS_PANEL_LINK_H
#define MICRO_UTILS_PANEL_LINK_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_uart.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_uart.h>
#endif

typedef enum {
    PanelLinkRole_Active,
    PanelLinkRole_Passive
} panelLinkRole_t;

typedef enum {
    PanelLinkState_Disconnected,
    PanelLinkState_WaitingRx,
    PanelLinkState_Connected,
} panelLinkState_t;

typedef struct {
    panelLinkRole_t role;
    UART_HandleTypeDef *huart;
    void *rxDataBuffer;
    uint32_t rxDataSize;
    uint32_t rxTimeoutMs;
    uint32_t lastRxTime;
    void *txDataBuffer;
    uint32_t txDataSize;
    uint32_t txPeriodMs;
    uint32_t lastTxTime;
    panelLinkState_t state;
    bool isAvailable;
} panelLink_t;

void panelLink_initialize(panelLink_t *link, panelLinkRole_t role, UART_HandleTypeDef *huart,
    void *rxDataBuffer, uint32_t rxDataSize, uint32_t rxTimeoutMs,
    void *txDataBuffer, uint32_t txDataSize, uint32_t txPeriodMs);

bool panelLink_isConnected(const panelLink_t *link);

bool panelLink_shouldSend(const panelLink_t *link);

void panelLink_send(panelLink_t *link, const void *txData);

void panelLink_onNewRxData(panelLink_t *link, const uint32_t size);

void panelLink_onRxError(panelLink_t *link);

bool panelLink_readAvailable(panelLink_t *link, void *rxData);

void panelLink_update(panelLink_t *link);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MICRO_UTILS_PANEL_LINK_H