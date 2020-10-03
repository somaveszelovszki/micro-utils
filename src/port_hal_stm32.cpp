#if defined STM32

#include <micro/math/numeric.hpp>
#include <micro/port/can.hpp>
#include <micro/port/gpio.hpp>
#include <micro/port/i2c.hpp>
#include <micro/port/spi.hpp>
#include <micro/port/timer.hpp>
#include <micro/port/uart.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_spi.h>
#include <stm32f4xx_hal_tim.h>
#include <stm32f4xx_hal_uart.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_i2c.h>
#include <stm32f0xx_hal_tim.h>
#include <stm32f0xx_hal_uart.h>
#endif

#if defined __GNUC__
#include "cmsis_gcc.h"
#else
#error "STM32 with any other compiler than GCC is not supported!"
#endif // __GNUC__

namespace micro {

timer_t timer_system = { nullptr };

Status toStatus(HAL_StatusTypeDef status) {
    Status result = Status::ERROR;
    switch (status) {
    case HAL_OK:      result = Status::OK;      break;
    case HAL_ERROR:   result = Status::ERROR;   break;
    case HAL_BUSY:    result = Status::BUSY;    break;
    case HAL_TIMEOUT: result = Status::TIMEOUT; break;
    default:          result = Status::ERROR;   break;
    }
    return result;
}

// CAN

#if defined STM32F4

Status can_transmit(const can_t& can, const canFrame_t& frame) {
    uint32_t txMailbox = 0;
    return toStatus(HAL_CAN_AddTxMessage(can.handle, const_cast<CAN_TxHeaderTypeDef*>(&frame.header.tx), const_cast<uint8_t*>(frame.data), &txMailbox));

}

Status can_receive(const can_t& can, canFrame_t& OUT frame) {
    return toStatus(HAL_CAN_GetRxMessage(can.handle, CAN_RX_FIFO0, &frame.header.rx, frame.data));
}

#else // !STM32F4

Status can_transmit(const can_t&, const canFrame_t&) { return Status::OK; }
Status can_receive(const can_t&, canFrame_t& OUT) { return Status::OK; }

#endif // !STM32F4

// GPIO

Status gpio_read(const gpio_t& gpio, gpioPinState_t& OUT state) {
    state = HAL_GPIO_ReadPin(reinterpret_cast<GPIO_TypeDef*>(gpio.instance), gpio.pin) == GPIO_PIN_SET ? gpioPinState_t::SET : gpioPinState_t::RESET;
    return Status::OK;
}

Status gpio_write(const gpio_t& gpio, const gpioPinState_t state) {
    HAL_GPIO_WritePin(reinterpret_cast<GPIO_TypeDef*>(gpio.instance), gpio.pin, state == gpioPinState_t::SET ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return Status::OK;
}

Status gpio_toggle(const gpio_t& gpio) {
    HAL_GPIO_TogglePin(reinterpret_cast<GPIO_TypeDef*>(gpio.instance), gpio.pin);
    return Status::OK;
}

// I2C

Status i2c_masterReceive(const i2c_t& i2c, const uint16_t devAddr, uint8_t * const rxBuf, const uint32_t size) {
    return toStatus(HAL_I2C_Master_Receive_DMA(i2c.handle, devAddr, rxBuf, size));
}

Status i2c_masterTransmit(const i2c_t& i2c, const uint16_t devAddr, const uint8_t * const txBuf, const uint32_t size) {
    return toStatus(HAL_I2C_Master_Transmit_DMA(i2c.handle, devAddr, const_cast<uint8_t*>(txBuf), size));

}

Status i2c_memoryRead(const i2c_t& i2c, const uint16_t devAddr, const uint16_t memAddr, const uint16_t memAddrSize, uint8_t * const rxBuf, const uint32_t size) {
    return toStatus(HAL_I2C_Mem_Read_DMA(i2c.handle, devAddr, memAddr, memAddrSize, rxBuf, size));
}

Status i2c_memoryWrite(const i2c_t& i2c, const uint16_t devAddr, const uint16_t memAddr, const uint16_t memAddrSize, const uint8_t * const txBuf, const uint32_t size) {
    return toStatus(HAL_I2C_Mem_Write_DMA(i2c.handle, devAddr, memAddr, memAddrSize, const_cast<uint8_t*>(txBuf), size));
}

// SPI

#if defined STM32F4

Status spi_exchange(const spi_t& spi, const uint8_t * const txBuf, uint8_t * const rxBuf, const uint32_t size) {
    return toStatus(
        txBuf == nullptr ? HAL_SPI_Receive_DMA(spi.handle, rxBuf, size) :
        rxBuf == nullptr ? HAL_SPI_Transmit_DMA(spi.handle, const_cast<uint8_t*>(txBuf), size) :
        HAL_SPI_TransmitReceive_DMA(spi.handle, const_cast<uint8_t*>(txBuf), rxBuf, size)
    );
}

#else // !STM32F4

Status spi_exchange(const spi_t&, const uint8_t * const, uint8_t * const, const uint32_t) { return Status::OK; }

#endif // !STM32F4

// TIMER

void time_init(const timer_t& timer) {
    timer_system = timer;
}

millisecond_t getTime() {
    return millisecond_t(HAL_GetTick());
}

microsecond_t getExactTime() {
    __disable_irq();
    const microsecond_t time = static_cast<microsecond_t>(getTime()) + microsecond_t(__HAL_TIM_GET_COUNTER(timer_system.handle));
    __enable_irq();
    return time;
}

Status timer_getCounter(const timer_t& timer, uint32_t& OUT cntr) {
    cntr = __HAL_TIM_GET_COUNTER(timer.handle);
    return Status::OK;
}

Status timer_setCounter(const timer_t& timer, const uint32_t cntr) {
    __HAL_TIM_SET_COUNTER(timer.handle, cntr);
    return Status::OK;

}

Status timer_getCompare(const timer_t& timer, const uint32_t channel, uint32_t& OUT compare) {
    compare = __HAL_TIM_GET_COMPARE(timer.handle, channel);
    return Status::OK;
}

Status timer_setCompare(const timer_t& timer, const uint32_t channel, const uint32_t compare) {
    __HAL_TIM_SET_COMPARE(timer.handle, channel, compare);
    return Status::OK;
}

Status timer_setDuty(const timer_t& timer, const uint32_t channel, const float duty) {
    return timer_setCompare(timer, channel, map<float, uint32_t>(duty, 0.0f, 1.0f, 0, timer.handle->Instance->ARR - 1));
}

// UART

Status uart_receive(const uart_t& uart, uint8_t * const rxBuf, const uint32_t size) {
    return toStatus(HAL_UART_Receive_DMA(uart.handle, rxBuf, size));
}

Status uart_transmit(const uart_t& uart, const uint8_t * const txBuf, const uint32_t size) {
    return toStatus(HAL_UART_Transmit_DMA(uart.handle, const_cast<uint8_t*>(txBuf), size));
}

Status uart_stopReceive(const uart_t& uart) {
    return toStatus(HAL_UART_AbortReceive_IT(uart.handle));
}

Status uart_stopTransmit(const uart_t& uart) {
    return toStatus(HAL_UART_AbortTransmit_IT(uart.handle));
}

} // namespace micro

#endif // STM32
