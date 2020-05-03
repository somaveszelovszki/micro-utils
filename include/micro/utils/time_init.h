#ifndef MICRO_UTILS_TIME_INIT_H
#define MICRO_UTILS_TIME_INIT_H

#include <micro/port/hal.h>

#if defined STM32F0
#include <stm32f0xx_hal_tim.h>
#elif defined STM32F4
#include <stm32f4xx_hal_tim.h>
#endif

void micro_time_init(TIM_HandleTypeDef *htim);

#endif // MICRO_UTILS_TIME_INIT_H
