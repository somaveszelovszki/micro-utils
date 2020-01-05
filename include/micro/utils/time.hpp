#pragma once

#include "units.hpp"

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_tim.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#endif

namespace micro {

/* @brief Gets time since system startup.
 * @returns Time since system startup.
 **/
millisecond_t getTime();

/* @brief Gets exact time since system startup.
 * @returns Time since system startup.
 **/
microsecond_t getExactTime();

/* @brief Gets exact time since system startup.
 * @returns Time since system startup.
 **/
microsecond_t getExactTime_ISR();

} // namespace micro
