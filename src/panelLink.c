#include <micro/math/numeric.h>
#include <micro/panel/panelLink.h>

#include <string.h>

void panelLink_initialize(panelLink_t *link, panelLinkRole_t role, UART_HandleTypeDef *huart,
    void *rxDataBuffer, uint32_t rxDataSize, uint32_t rxTimeoutMs,
    void *txDataBuffer, uint32_t txDataSize, uint32_t txPeriodMs) {

    link->role          = role;
    link->huart         = huart;
    link->rxDataBuffer  = rxDataBuffer;
    link->rxDataSize    = rxDataSize;
    link->rxTimeoutMs   = rxTimeoutMs;
    link->lastRxTime    = 0;
    link->txDataBuffer  = txDataBuffer;
    link->txDataSize    = txDataSize;
    link->txPeriodMs    = txPeriodMs;
    link->lastTxTime    = 0;
    link->state         = PanelLinkState_Disconnected;
    link->isAvailable   = false;
}

bool panelLink_isConnected(const panelLink_t *link) {
    return PanelLinkState_Connected == link->state;
}

bool panelLink_shouldSend(const panelLink_t *link) {
    // communication can only be started by the active panel,
    // passive panel must wait for link to get connected
    return (PanelLinkRole_Active == link->role || panelLink_isConnected(link)) &&
        (0 == link->lastTxTime || HAL_GetTick() - link->lastTxTime >= link->txPeriodMs);
}

void panelLink_send(panelLink_t *link, const void *txData) {
    memcpy(link->txDataBuffer, txData, link->txDataSize);
    HAL_UART_Transmit_DMA(link->huart, (uint8_t*)link->txDataBuffer, link->txDataSize);
    link->lastTxTime = HAL_GetTick();
}

void panelLink_onNewRxData(panelLink_t *link, const uint32_t size) {
    if (size == link->rxDataSize) {
        link->isAvailable = true;
        link->lastRxTime = HAL_GetTick();
    }
    HAL_UART_Receive_DMA(link->huart, (uint8_t*)link->rxDataBuffer, link->rxDataSize);
}

void panelLink_onRxError(panelLink_t *link) {
    HAL_UART_Receive_DMA(link->huart, (uint8_t*)link->rxDataBuffer, link->rxDataSize);
}

bool panelLink_readAvailable(panelLink_t *link, void *rxData) {
    bool available = false;
    if (link->isAvailable) {
        link->isAvailable = false;
        memcpy(rxData, link->rxDataBuffer, link->rxDataSize);
        available = true;
    }
    return available;
}

void panelLink_update(panelLink_t *link) {
    switch (link->state) {

    case PanelLinkState_Disconnected:
        HAL_UART_Receive_DMA(link->huart, (uint8_t*)link->rxDataBuffer, link->rxDataSize);
        link->lastRxTime = 0;
        link->lastTxTime = 0;
        link->state = PanelLinkState_WaitingRx;
        break;

    case PanelLinkState_WaitingRx:
        if (link->lastRxTime != 0 && HAL_GetTick() - link->lastRxTime < link->rxTimeoutMs) {
            link->state = PanelLinkState_Connected;
        }
        break;

    case PanelLinkState_Connected:
        if (HAL_GetTick() - link->lastRxTime > link->rxTimeoutMs) {
            HAL_UART_AbortReceive_IT(link->huart);
            link->state = PanelLinkState_Disconnected;
        }
        break;
    }
}
