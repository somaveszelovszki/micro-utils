#ifndef MICRO_UTILS_PORT_HAL_H
#define MICRO_UTILS_PORT_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined BSP_LIB_HAL

#if defined STM32F0
#include <stm32f0xx_hal.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#endif

#else

typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef void CAN_HandleTypeDef;
typedef void GPIO_TypeDef;
typedef void I2C_HandleTypeDef;
typedef void SPI_HandleTypeDef;
typedef void TIM_HandleTypeDef;
typedef void UART_HandleTypeDef;

#define GPIO_PIN_RESET  0
#define GPIO_PIN_SET    1

#define HAL_CAN_GetRxMessage(...)

#define HAL_I2C_Mem_Read(...)
#define HAL_I2C_Mem_Write(...)

#define HAL_GPIO_ReadPin(...)   GPIO_PIN_RESET
#define HAL_GPIO_WritePin(...)
#define HAL_GPIO_TogglePin(...)

#define HAL_SPI_Transmit(...)
#define HAL_SPI_Receive(...)
#define HAL_SPI_TransmitReceive(...)
#define HAL_SPI_Transmit_IT(...)
#define HAL_SPI_Receive_IT(...)
#define HAL_SPI_TransmitReceive_IT(...)
#define HAL_SPI_Transmit_DMA(...)
#define HAL_SPI_Receive_DMA(...)
#define HAL_SPI_TransmitReceive_DMA(...)

#define HAL_GetTick() 0
#define __HAL_TIM_GET_COUNTER(...)  0
#define __HAL_TIM_SET_COMPARE(...)

#endif

#ifdef __cplusplus
}
#endif

#endif // MICRO_UTILS_PORT_HAL_H
