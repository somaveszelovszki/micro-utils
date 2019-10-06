#pragma once

#include <micro/bsp/tim.hpp>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

namespace micro {

template <typename T_toPanel, typename T_fromPanel>
class Panel {
public:
    explicit Panel(UART_HandleTypeDef *huart) : huart(huart) {}

    void start(const T_toPanel& out) {
        HAL_UART_Receive_DMA(this->huart, reinterpret_cast<uint8_t*>(&this->inData), sizeof(T_fromPanel));
        this->send(out);
    }

    void send(const T_toPanel& out) {
        HAL_UART_Transmit_DMA(this->huart, reinterpret_cast<uint8_t*>(const_cast<T_toPanel*>(&out)), sizeof(T_toPanel));
    }

    void onDataReceived(void) {
        this->lastRecvTime = micro::getTime();
    }

    T_fromPanel getLastValue(millisecond_t * const lastRecvTime = nullptr) const {
        if (lastRecvTime != nullptr)
        {
            *lastRecvTime = this->lastRecvTime;
        }
        return this->inData;
    }

    millisecond_t getLastRecvTime(void) const {
        return this->lastRecvTime;
    }

private:
    UART_HandleTypeDef *huart;
    millisecond_t lastRecvTime;
    T_fromPanel inData;
};

}  // namespace micro
