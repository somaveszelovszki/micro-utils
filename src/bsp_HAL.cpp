#ifdef BSP_LIB_HAL

#include <micro/bsp/gpio.hpp>
#include <micro/bsp/tim.hpp>
#include <micro/bsp/dma.hpp>
#include <micro/bsp/adc.hpp>
#include <micro/bsp/i2c.hpp>
#include <micro/bsp/spi.hpp>
#include <micro/bsp/uart.hpp>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"

namespace micro {

/* @brief Converts HAL status to Status.
 * @param status The HAL status.
 * @returns The status as an Status.
 **/
Status toStatus(HAL_StatusTypeDef status) {
    return static_cast<Status>(status);
}

/* @brief Converts HAL status to Status.
 * @param status The HAL status.
 * @returns The status as an Status.
 **/
Status toStatus(HAL_SPI_StateTypeDef status) {
    Status result = Status::BUSY;
    switch(status) {
    case HAL_SPI_STATE_RESET:   result = Status::OK;    break;
    case HAL_SPI_STATE_READY:   result = Status::OK;    break;
    case HAL_SPI_STATE_ERROR:   result = Status::ERROR; break;
    default:                    result = Status::BUSY;  break;
    }
    return result;
}

millisecond_t getTime() {
    return millisecond_t(HAL_GetTick());
}

microsecond_t getExactTime() {
    return millisecond_t(0); // TODO
    //return getTime() + microsecond_t(getTimerCounter(cfg::tim_System));
}

void blockingDelay(millisecond_t delay) {
    HAL_Delay(delay.get());
}

Status writePWM(tim_handle_t htim, tim_channel_t channel, uint32_t duty) {
    __HAL_TIM_SET_COMPARE(static_cast<TIM_HandleTypeDef*>(htim.ptr), channel.value, duty);
    return Status::OK;
}

uint32_t getTimerCounter(tim_handle_t htim) {
    return __HAL_TIM_GET_COUNTER(static_cast<TIM_HandleTypeDef*>(htim.ptr));
}

void setTimerCounter(tim_handle_t htim, uint32_t cntr) {
    __HAL_TIM_SET_COUNTER(static_cast<TIM_HandleTypeDef*>(htim.ptr), cntr);
}

uint32_t getTimerCompare(tim_handle_t htim, tim_channel_t channel) {
    return __HAL_TIM_GET_COMPARE(static_cast<TIM_HandleTypeDef*>(htim.ptr), channel.value);
}

void GPIO_WritePin(const gpio_pin_struct& gpio, PinState pinState) {
    HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio.handle.ptr), static_cast<uint16_t>(gpio.pin.value), static_cast<GPIO_PinState>(pinState));
}

PinState GPIO_ReadPin(const gpio_pin_struct& gpio) {
    return static_cast<PinState>(HAL_GPIO_ReadPin(static_cast<GPIO_TypeDef*>(gpio.handle.ptr), static_cast<uint16_t>(gpio.pin.value)));
}

void GPIO_TogglePin(const gpio_pin_struct& gpio) {
    HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio.handle.ptr), static_cast<uint16_t>(gpio.pin.value));
}

Status ADC_SetChannel(adc_handle_t hadc, adc_channel_t channel) {
    ADC_ChannelConfTypeDef sConfig = { channel.value, 1, ADC_SAMPLETIME_3CYCLES, 0 };
    return toStatus(HAL_ADC_ConfigChannel(static_cast<ADC_HandleTypeDef*>(hadc.ptr), &sConfig));
}

Status ADC_ReadValue(adc_handle_t hadc, uint32_t *pResult) {
    Status status;
    ADC_HandleTypeDef *_hadc = static_cast<ADC_HandleTypeDef*>(hadc.ptr);

    if (isOk(status = toStatus(HAL_ADC_Start(_hadc))) &&
        isOk(status = toStatus(HAL_ADC_PollForConversion(_hadc, 1)))) {

        *pResult = HAL_ADC_GetValue(_hadc);
        status = toStatus(HAL_ADC_Stop(_hadc));
    }

    return status;
}

Status I2C_IsDeviceReady(i2c_handle_t hi2c, uint16_t address, uint32_t trials, millisecond_t timeout) {
    return toStatus(HAL_I2C_IsDeviceReady(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, trials, timeout.get()));
}

Status I2C_Master_Transmit(i2c_handle_t hi2c, uint16_t address, const uint8_t *txBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_I2C_Master_Transmit(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, const_cast<uint8_t*>(txBuffer), size, timeout.get()));
}

Status I2C_Master_Transmit_IT(i2c_handle_t hi2c, uint16_t address, const uint8_t *txBuffer, uint32_t size) {
    return toStatus(HAL_I2C_Master_Transmit_IT(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, const_cast<uint8_t*>(txBuffer), size));
}

Status I2C_Mem_Write(i2c_handle_t hi2c, uint16_t address, uint16_t memAddress, uint16_t memAddressSize, const uint8_t *txBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_I2C_Mem_Write(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, memAddress, memAddressSize, const_cast<uint8_t*>(txBuffer), size, timeout.get()));
}

Status I2C_Master_Receive(i2c_handle_t hi2c, uint16_t address, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_I2C_Master_Receive(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, rxBuffer, size, timeout.get()));
}

Status I2C_Master_Receive_IT(i2c_handle_t hi2c, uint16_t address, uint8_t *rxBuffer, uint32_t size) {
    return toStatus(HAL_I2C_Master_Receive_IT(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, rxBuffer, size));
}

Status I2C_Mem_Read(i2c_handle_t hi2c, uint16_t address, uint16_t memAddress, uint16_t memAddressSize, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_I2C_Mem_Read(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), address, memAddress, memAddressSize, rxBuffer, size, timeout.get()));
}

Status I2C_Slave_Receive_DMA(i2c_handle_t hi2c, uint8_t *rxBuffer, uint32_t size) {
    return toStatus(HAL_I2C_Slave_Receive_DMA(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), rxBuffer, size));
}

Status I2C_Slave_Receive(i2c_handle_t hi2c, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_I2C_Slave_Receive(static_cast<I2C_HandleTypeDef*>(hi2c.ptr), rxBuffer, size, timeout.get()));
}

Status SPI_Receive(spi_handle_t hspi, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_SPI_Receive(static_cast<SPI_HandleTypeDef*>(hspi.ptr), rxBuffer, size, timeout.get()));
}

Status SPI_Transmit(spi_handle_t hspi, const uint8_t *txBuffer, uint32_t size, millisecond_t timeout) {
    Status status = toStatus(HAL_SPI_Transmit(static_cast<SPI_HandleTypeDef*>(hspi.ptr), const_cast<uint8_t*>(txBuffer), size, timeout.get()));
    if (isOk(status)) {
        while(HAL_SPI_GetState(static_cast<SPI_HandleTypeDef*>(hspi.ptr)) != HAL_SPI_STATE_READY) {}
    }
    return status;
}

Status SPI_TransmitReceive(spi_handle_t hspi, const uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_SPI_TransmitReceive(static_cast<SPI_HandleTypeDef*>(hspi.ptr), const_cast<uint8_t*>(txBuffer), rxBuffer, size, timeout.get()));
}

Status SPI_Receive_IT(spi_handle_t hspi, uint8_t *rxBuffer, uint32_t size) {
    return toStatus(HAL_SPI_Receive_IT(static_cast<SPI_HandleTypeDef*>(hspi.ptr), rxBuffer, size));
}

Status SPI_Transmit_IT(spi_handle_t hspi, const uint8_t *txBuffer, uint32_t size) {
    return toStatus(HAL_SPI_Transmit_IT(static_cast<SPI_HandleTypeDef*>(hspi.ptr), const_cast<uint8_t*>(txBuffer), size));
}

Status SPI_TransmitReceive_IT(spi_handle_t hspi, const uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t size) {
    return toStatus(HAL_SPI_TransmitReceive_IT(static_cast<SPI_HandleTypeDef*>(hspi.ptr), const_cast<uint8_t*>(txBuffer), rxBuffer, size));
}

Status SPI_GetState(spi_handle_t hspi) {
    return toStatus(HAL_SPI_GetState(static_cast<SPI_HandleTypeDef*>(hspi.ptr)));
}

void SPI_SetReady(spi_handle_t hspi) {
    static_cast<SPI_HandleTypeDef*>(hspi.ptr)->State = HAL_SPI_STATE_READY;
}

Status UART_Receive(uart_handle_t huart, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_UART_Receive(static_cast<UART_HandleTypeDef*>(huart.ptr), rxBuffer, size, timeout.get()));
}

Status UART_Receive_IT(uart_handle_t huart, uint8_t *rxBuffer, uint32_t size) {
    return toStatus(HAL_UART_Receive_IT(static_cast<UART_HandleTypeDef*>(huart.ptr), rxBuffer, size));
}

Status UART_Receive_DMA(uart_handle_t huart, uint8_t *rxBuffer, uint32_t size) {
    return toStatus(HAL_UART_Receive_DMA(static_cast<UART_HandleTypeDef*>(huart.ptr), rxBuffer, size));
}

Status UART_Transmit(uart_handle_t huart, const uint8_t *txBuffer, uint32_t size, millisecond_t timeout) {
    return toStatus(HAL_UART_Transmit(static_cast<UART_HandleTypeDef*>(huart.ptr), const_cast<uint8_t*>(txBuffer), size, timeout.get()));
}

Status UART_Transmit_IT(uart_handle_t huart, const uint8_t *txBuffer, uint32_t size) {
    return toStatus(HAL_UART_Transmit_IT(static_cast<UART_HandleTypeDef*>(huart.ptr), const_cast<uint8_t*>(txBuffer), size));
}

Status UART_Transmit_DMA(uart_handle_t huart, const uint8_t *txBuffer, uint32_t size) {
    return toStatus(HAL_UART_Transmit_DMA(static_cast<UART_HandleTypeDef*>(huart.ptr), const_cast<uint8_t*>(txBuffer), size));
}

Status UART_Stop_DMA(uart_handle_t huart) {
    return toStatus(HAL_UART_DMAStop(static_cast<UART_HandleTypeDef*>(huart.ptr)));
}

} // namespace micro

#endif // BSP_LIB_HAL
