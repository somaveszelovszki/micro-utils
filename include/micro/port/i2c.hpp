#pragma once

#include <micro/utils/types.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_i2c.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_i2c.h>
#endif

namespace micro {

#if defined STM32

struct i2c_t {
    I2C_HandleTypeDef *handle;
};

#else // !STM32

struct i2c_t {};

#endif // !STM32

Status i2c_masterReceive(const i2c_t& i2c, const uint16_t devAddr, uint8_t * const rxBuf, const uint32_t size);
Status i2c_masterTransmit(const i2c_t& i2c, const uint16_t devAddr, const uint8_t * const txBuf, const uint32_t size);
Status i2c_memoryRead(const i2c_t& i2c, const uint16_t devAddr, const uint16_t memAddr, const uint16_t memAddrSize, uint8_t * const rxBuf, const uint32_t size);
Status i2c_memoryWrite(const i2c_t& i2c, const uint16_t devAddr, const uint16_t memAddr, const uint16_t memAddrSize, const uint8_t * const txBuf, const uint32_t size);

} // namespace micro
