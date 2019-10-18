#pragma once

#include "units.hpp"

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>

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
