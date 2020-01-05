#include <micro/panel/panelLink.h>

void panelLink_initialize(panelLink_t *link, UART_HandleTypeDef *huart,
    void *rxData, uint32_t rxDataSize, uint32_t rxPeriodMs,
    const void *txData, uint32_t txDataSize, uint32_t txPeriodMs) {

    link->huart         = huart;
    link->startData.cmd = '\0';
    link->rxData      = rxData;
    link->rxDataSize  = rxDataSize;
    link->rxPeriodMs  = rxPeriodMs;
    link->lastRxTime  = HAL_GetTick();
    link->txData      = txData;
    link->txDataSize  = txDataSize;
    link->txPeriodMs  = txPeriodMs;
    link->lastTxTime  = HAL_GetTick();
    link->isConnected   = false;
    link->isAvailable   = false;

    HAL_UART_Receive_DMA(link->huart, (uint8_t*)&link->startData, sizeof(link->startData));
}

bool panelLink_shouldSend(const panelLink_t *link) {
    return link->isConnected && HAL_GetTick() - link->lastTxTime >= link->txPeriodMs;
}

void panelLink_send(panelLink_t *link) {
    if (link->isConnected) {
        HAL_UART_Transmit_DMA(link->huart, (uint8_t*)link->txData, link->txDataSize);
        link->lastTxTime = HAL_GetTick();
    }
}

void panelLink_onNewCmd(panelLink_t *link) {
    if (link->isConnected) {
        link->isAvailable = true;
    } else if (PANEL_START == link->startData.cmd) {
        HAL_UART_AbortReceive_IT(link->huart);
        HAL_UART_Receive_DMA(link->huart, (uint8_t*)link->rxData, link->rxDataSize);
        link->isConnected = true;
    }
    link->lastRxTime = HAL_GetTick();
}

bool panelLink_checkAvailable(panelLink_t *link) {
    bool available = false;
    if (link->isConnected && link->isAvailable) {
        link->isAvailable = false;
        available = true;
    }
    return available;
}

void panelLink_checkConnection(panelLink_t *link) {
    if (link->isConnected && HAL_GetTick() - link->lastRxTime >= link->rxPeriodMs * 3) {
        HAL_UART_AbortReceive_IT(link->huart);
        HAL_UART_Receive_DMA(link->huart, (uint8_t*)&link->startData, sizeof(link->startData));
        link->isConnected = false;
    }
}
