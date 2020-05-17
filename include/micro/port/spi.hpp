#pragma once

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_spi.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_spi.h>
#endif

#if !defined STM32

#define SPI_HandleTypeDef void
#define HAL_SPI_Receive_DMA(...)
#define HAL_SPI_Transmit_DMA(...)
#define HAL_SPI_TransmitReceive_DMA(...)

#endif // STM32

