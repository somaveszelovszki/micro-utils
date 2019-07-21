#include <micro/panel/handshake.hpp>
#include <micro/utils/arrays.hpp>
#include <micro/bsp/tim.hpp>

namespace micro {

static constexpr uint32_t HANDSHAKE_RX_SIZE = 4;

Status handshake(uart_handle_t *huart, const uint8_t *txBuffer, uint32_t txSize) {

    uint8_t rxBuffer[HANDSHAKE_RX_SIZE];
    Status status;

    micro::UART_Stop_DMA(huart);
    if (isOk(status = micro::UART_Transmit(huart, txBuffer, txSize, millisecond_t(2))) &&
            isOk(status = micro::UART_Receive(huart, rxBuffer, HANDSHAKE_RX_SIZE, millisecond_t(2)))) {

        status = micro::isZeroArray(rxBuffer, HANDSHAKE_RX_SIZE) ? Status::OK : Status::INVALID_DATA;
    }

    return status;
}

}  // namespace micro
