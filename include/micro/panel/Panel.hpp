#pragma once

#include <micro/utils/time.hpp>
#include <micro/utils/unit_utils.hpp>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#include <FreeRTOS.h>
#include <task.h>

namespace micro {

template <typename T_toPanel, typename T_fromPanel>
class Panel {
public:
    explicit Panel(UART_HandleTypeDef *huart)
        : huart(huart)
        , newValueReceived(false) {}

    void start(const T_toPanel& out) {
        HAL_UART_Receive_DMA(this->huart, reinterpret_cast<uint8_t*>(&this->inBuffer), sizeof(T_fromPanel));
        this->send(out);
    }

    void waitStart(void) {
        millisecond_t lastRecvTime = millisecond_t(0);
        do {
            vTaskDelay(1);
        } while(!this->newValueReceived);
    }

    void send(const T_toPanel& out) {
        HAL_UART_Transmit_DMA(this->huart, reinterpret_cast<uint8_t*>(const_cast<T_toPanel*>(&out)), sizeof(T_toPanel));
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
    bool newValueReceived;
    T_fromPanel inBuffer, inData;
};

}  // namespace micro
