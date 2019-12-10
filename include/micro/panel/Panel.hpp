#pragma once

#include <micro/panel/panelData.h>
#include <micro/utils/time.hpp>
#include <micro/math/unit_utils.hpp>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#include <FreeRTOS.h>
#include <task.h>

namespace micro {

template <typename T_toPanel, uint32_t size_toPanel, typename T_fromPanel, uint32_t size_fromPanel>
class Panel {
public:
    explicit Panel(UART_HandleTypeDef *huart)
        : huart(huart)
        , newValueReceived(false) {}

    void start(void) {
        HAL_UART_Receive_DMA(this->huart, reinterpret_cast<uint8_t*>(&this->inBuffer), size_fromPanel);
        panelStartData_t startData;
        startData.cmd = PANEL_START;
        HAL_UART_Transmit_DMA(this->huart, reinterpret_cast<uint8_t*>(&startData), dataSize_panelStartData);
    }

    void waitResponse() {
        do {
            vTaskDelay(1);
        } while(!this->newValueReceived);
    }

    void send(const T_toPanel& out) {
        HAL_UART_Transmit_DMA(this->huart, reinterpret_cast<uint8_t*>(const_cast<T_toPanel*>(&out)), size_toPanel);
    }

    void onDataReceived(void) {
        this->inData = inBuffer;
        this->newValueReceived = true;
    }

    bool hasNewValue(void) const {
        return this->newValueReceived;
    }

    T_fromPanel acquireLastValue(void) {
        taskENTER_CRITICAL();
        const T_fromPanel data = this->inData;
        this->newValueReceived = false;
        taskEXIT_CRITICAL();
        return data;
    }

private:
    UART_HandleTypeDef *huart;
    volatile bool newValueReceived;
    T_fromPanel inBuffer, inData;
};

}  // namespace micro
