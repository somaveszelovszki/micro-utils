#pragma once

#include <micro/utils/types.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_spi.h>
#endif

#if !defined STM32

#define SPI_HandleTypeDef void
#define HAL_SPI_Receive_DMA(...)
#define HAL_SPI_Transmit_DMA(...)
#define HAL_SPI_TransmitReceive_DMA(...)

#endif // STM32

namespace micro {

#if defined STM32F4

struct spi_t {
    SPI_HandleTypeDef *handle;
};

#else // !STM32F4

struct spi_t {};

#endif // !STM32F4

Status spi_exchange(const spi_t& spi, const uint8_t * const txBuf, uint8_t * const rxBuf, const uint32_t size);

} // namespace micro
