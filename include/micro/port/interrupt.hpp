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

bool isInterrupt();

} // namespace micro
