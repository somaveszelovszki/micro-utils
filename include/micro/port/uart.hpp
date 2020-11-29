#pragma once

#include <micro/utils/types.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_uart.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_uart.h>
#endif

namespace micro {

#if defined STM32

struct uart_t {
    UART_HandleTypeDef *handle = nullptr;
};

#else // !STM32

struct uart_t {};

#endif // !STM32

Status uart_receive(const uart_t& uart, uint8_t * const rxBuf, const uint32_t size);
Status uart_transmit(const uart_t& uart, const uint8_t * const txBuf, const uint32_t size);
Status uart_stopReceive(const uart_t& uart);
Status uart_stopTransmit(const uart_t& uart);

} // namespace micro
