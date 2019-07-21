#pragma once

#include <micro/bsp/uart.hpp>

namespace micro {

Status handshake(uart_handle_t *huart, const uint8_t *txBuffer, uint32_t txSize);

}  // namespace micro
