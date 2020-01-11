#include <micro/math/numeric.h>
#include <micro/panel/panelLink.h>

#include <string.h>

static bool hasRxTimedOut(const panelLink_t *link) {
    return HAL_GetTick() - link->lastRxTime >= link->timeout;
}

static bool hasTxTimedOut(const panelLink_t *link) {
    return HAL_GetTick() - link->lastTxTime >= link->timeout;
}

void panelLink_initialize(panelLink_t *link, panelLinkRole_t role, UART_HandleTypeDef *huart,
    void *rxDataBuffer, uint32_t rxDataSize, uint32_t rxPeriodMs,
    void *txDataBuffer, uint32_t txDataSize, uint32_t txPeriodMs) {

    link->role          = role;
    link->huart         = huart;
    link->startData.cmd = '\0';
    link->rxDataBuffer  = rxDataBuffer;
    link->rxDataSize    = rxDataSize;
    link->rxPeriodMs    = rxPeriodMs;
    link->lastRxTime    = HAL_GetTick();
    link->txDataBuffer  = txDataBuffer;
    link->txDataSize    = txDataSize;
    link->txPeriodMs    = txPeriodMs;
    link->lastTxTime    = HAL_GetTick();
    link->timeout       = MAX(MAX(link->rxPeriodMs, link->txPeriodMs) * 5, 100);
    link->state         = PanelLinkState_Disconnected;
    link->isAvailable   = false;
}

bool panelLink_isConnected(const panelLink_t *link) {
    return PanelLinkState_Connected == link->state;
}

bool panelLink_shouldSend(const panelLink_t *link) {
    return panelLink_isConnected(link) && !hasRxTimedOut(link) && HAL_GetTick() - link->lastTxTime >= link->txPeriodMs;
}

void panelLink_send(panelLink_t *link, const void *txData) {
    if (panelLink_isConnected(link)) {
        memcpy(link->txDataBuffer, txData, link->txDataSize);
        HAL_UART_Transmit_DMA(link->huart, (uint8_t*)link->txDataBuffer, link->txDataSize);
        link->lastTxTime = HAL_GetTick();
    }
}

void panelLink_onNewRxData(panelLink_t *link) {
    link->isAvailable = true;
    link->lastRxTime = HAL_GetTick();
}

bool panelLink_readAvailable(panelLink_t *link, void *rxData) {
    bool available = false;
    if (panelLink_isConnected(link) && link->isAvailable) {
        memcpy(rxData, link->rxDataBuffer, link->rxDataSize);
        link->isAvailable = false;
        available = true;
    }
    return available;
}

void panelLink_update(panelLink_t *link) {
    switch (link->state) {

    case PanelLinkState_Disconnected:
        link->isAvailable = false;
        link->lastRxTime = link->lastTxTime = HAL_GetTick();

        if (PanelLinkRole_Master == link->role) {
            HAL_UART_Receive_DMA(link->huart, (uint8_t*)link->rxDataBuffer, link->rxDataSize);
            link->startData.cmd = PANEL_START;
            HAL_UART_Transmit(link->huart, (uint8_t*)&link->startData, sizeof(link->startData), 1);
        } else { // Slave
            link->startData.cmd = '\0';
            HAL_UART_Receive_DMA(link->huart, (uint8_t*)&link->startData, sizeof(link->startData));
        }
        link->state = PanelLinkState_WaitingData;
        break;

    case PanelLinkState_WaitingData:
        if (link->isAvailable) {
            if (PanelLinkRole_Master == link->role) {
                link->state = PanelLinkState_Connected;
            } else { // Slave
                link->isAvailable = false;
                if (PANEL_START == link->startData.cmd) {
                    HAL_UART_AbortReceive_IT(link->huart);
                    HAL_UART_Receive_DMA(link->huart, (uint8_t*)link->rxDataBuffer, link->rxDataSize);
                    link->lastTxTime = 0; // forces response to be sent as soon as possible
                    link->state = PanelLinkState_Connected;
                }
            }
        } else if (hasTxTimedOut(link)) {
            HAL_UART_AbortReceive_IT(link->huart);
            link->state = PanelLinkState_Disconnected;
        }
        break;

    case PanelLinkState_Connected:
        if (hasRxTimedOut(link) && hasTxTimedOut(link)) {
            HAL_UART_AbortReceive_IT(link->huart);
            link->state = PanelLinkState_Disconnected;
        }
        break;
    }
}
