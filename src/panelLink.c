#include <micro/math/numeric.h>
#include <micro/panel/panelLink.h>

#include <string.h>

static uint8_t calcChecksum(const void *data, uint8_t dataSize) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < dataSize - 1; ++i) {
        sum += ((const uint8_t*)data)[i] * (i + 1);
    }
    return sum;
}

void panelLink_initialize(panelLink_t *link, panelLinkRole_t role, UART_HandleTypeDef *huart,
    void *rxDataBuffer1, void *rxDataBuffer2, uint32_t rxDataSize, uint32_t rxTimeoutMs,
    void *txDataBuffer, uint32_t txDataSize, uint32_t txPeriodMs) {

    link->role                   = role;
    link->huart                  = huart;
    link->startData.cmd          = '\0';
    link->rxDataBuffer           = rxDataBuffer1;
    link->rxAccessibleDataBuffer = rxDataBuffer2;
    link->rxDataSize             = rxDataSize;
    link->rxTimeoutMs            = rxTimeoutMs;
    link->lastRxTime             = HAL_GetTick();
    link->txDataBuffer           = txDataBuffer;
    link->txDataSize             = txDataSize;
    link->txPeriodMs             = txPeriodMs;
    link->lastTxTime             = HAL_GetTick();
    link->state                  = PanelLinkState_Disconnected;
    link->isAvailable            = false;
}

bool panelLink_isConnected(const panelLink_t *link) {
    return PanelLinkState_Connected == link->state;
}

bool panelLink_shouldSend(const panelLink_t *link) {
    return panelLink_isConnected(link) && HAL_GetTick() - link->lastTxTime >= link->txPeriodMs;
}

void panelLink_send(panelLink_t *link, const void *txData) {
    if (panelLink_isConnected(link)) {
        memcpy(link->txDataBuffer, txData, link->txDataSize - 1);
        ((uint8_t*)link->txDataBuffer)[link->txDataSize - 1] = calcChecksum(link->txDataBuffer, link->txDataSize);
        HAL_UART_Transmit_DMA(link->huart, (uint8_t*)link->txDataBuffer, link->txDataSize);
        link->lastTxTime = HAL_GetTick();
    }
}

void panelLink_onNewRxData(panelLink_t *link) {
    if ((uint8_t*)&link->startData == link->huart->pRxBuffPtr ||
        (link->huart->pRxBuffPtr)[link->rxDataSize - 1] == calcChecksum(link->huart->pRxBuffPtr, link->rxDataSize)) {

        memcpy(link->rxAccessibleDataBuffer, link->rxDataBuffer, link->rxDataSize);
        link->isAvailable = true;
        link->lastRxTime = HAL_GetTick();
    }
}

bool panelLink_readAvailable(panelLink_t *link, void *rxData) {
    bool available = false;
    if (panelLink_isConnected(link) && link->isAvailable) {
        memcpy(rxData, link->rxAccessibleDataBuffer, link->rxDataSize);
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
            HAL_UART_Transmit_DMA(link->huart, (uint8_t*)&link->startData, sizeof(link->startData));
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
        } else if (HAL_GetTick() - link->lastRxTime > link->rxTimeoutMs) {
            HAL_UART_AbortReceive_IT(link->huart);
            link->state = PanelLinkState_Disconnected;
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
