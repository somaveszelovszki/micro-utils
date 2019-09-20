#pragma once

#include <micro/bsp/uart.hpp>

namespace micro {

template <typename T_toPanel, typename T_fromPanel>
class Panel {
public:
    explicit Panel(uart_handle_t huart) : huart(huart) {}

    void start(const T_toPanel& out) {
        UART_Receive_DMA(this->huart, reinterpret_cast<uint8_t*>(&this->inData), sizeof(T_fromPanel));
        this->send(out);
    }

    void send(const T_toPanel& out) {
        UART_Transmit_DMA(this->huart, reinterpret_cast<const uint8_t*>(&out), sizeof(T_toPanel));
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

private:
    uart_handle_t huart;
    millisecond_t lastRecvTime;
    T_fromPanel inData;
};

}  // namespace micro
