#pragma once

#include <micro/utils/time_init.h>
#include <micro/utils/time.hpp>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>

#include <FreeRTOS.h>
#include <task.h>

namespace micro {

TIM_HandleTypeDef *tim_system = nullptr;

millisecond_t getTime() {
    return millisecond_t(HAL_GetTick());
}

microsecond_t getExactTime() {
    taskENTER_CRITICAL();
    const microsecond_t time = static_cast<microsecond_t>(getTime()) + microsecond_t(__HAL_TIM_GET_COUNTER(tim_system));
    taskEXIT_CRITICAL();
    return time;
}

microsecond_t getExactTime_ISR() {
    UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    const microsecond_t time = static_cast<microsecond_t>(getTime()) + microsecond_t(__HAL_TIM_GET_COUNTER(tim_system));
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
    return time;
}

extern "C" void micro_time_init(TIM_HandleTypeDef *htim) {
    tim_system = htim;
}

} // namespace micro
