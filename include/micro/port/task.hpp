#pragma once

#include <micro/math/unit_utils.hpp>

#if defined STM32F4
#include <stm32f446xx.h>
#include <stm32f4xx_hal.h>
#elif defined STM32F0
#include <stm32f0xx.h>
#include <stm32f0xx_hal.h>
#endif

#if defined STM32 && defined __GNUC__
#include "cmsis_gcc.h"
#endif // __GNUC__

#if defined OS_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#endif // OS_FREERTOS

namespace micro {

inline bool isInterrupt()
{
#if defined STM32
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
#else // !STM32
    return false;
#endif // !STM32
}

} // namespace micro

#if defined OS_FREERTOS

#define os_taskSuspendAll() vTaskSuspendAll()
#define os_taskResumeAll()  xTaskResumeAll()
#define os_enterCritical()  taskENTER_CRITICAL()
#define os_exitCritical()   taskEXIT_CRITICAL()
#define os_delay(ms)        vTaskDelay(ms)

#else // !OS_FREERTOS

#define os_taskSuspendAll()
#define os_taskResumeAll()

#if defined STM32

#if defined __GNUC__
#define os_enterCritical()  __disable_irq()
#define os_exitCritical()   __enable_irq()
#endif // __GNUC__

#define os_delay(ms)        HAL_Delay(ms)

#else // !STM32

#define os_enterCritical()
#define os_exitCritical()
#define os_delay(...)

#endif // !STM32
#endif // !OS_FREERTOS
