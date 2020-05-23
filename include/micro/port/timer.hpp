#pragma once

#include <micro/utils/units.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#elif defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_tim.h>
#endif

namespace micro {

#if defined STM32

struct timer_t {
    TIM_HandleTypeDef *handle;
};

#else // !STM32

struct timer_t {};

#endif // !STM32

void time_init(const timer_t& timer);

/* @brief Gets time since system startup.
 * @returns Time since system startup.
 **/
millisecond_t getTime();

/* @brief Gets exact time since system startup.
 * @returns Exact time since system startup.
 **/
microsecond_t getExactTime();

Status timer_getCounter(const timer_t& timer, uint32_t& OUT cntr);
Status timer_setCounter(const timer_t& timer, const uint32_t cntr);
Status timer_getCompare(const timer_t& timer, const uint32_t channel, uint32_t& OUT compare);
Status timer_setCompare(const timer_t& timer, const uint32_t channel, const uint32_t compare);
Status timer_setDuty(const timer_t& timer, const uint32_t channel, const float duty);

} // namespace micro
