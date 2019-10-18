#ifndef MICRO_UTILS_TIME_INIT_H
#define MICRO_UTILS_TIME_INIT_H

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>

void micro_time_init(TIM_HandleTypeDef *htim);

#endif // MICRO_UTILS_TIME_INIT_H
