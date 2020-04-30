#pragma once

#include "types.hpp"

#ifdef OS_FREERTOS

#include <FreeRTOS.h>
#include <task.h>

#define os_taskSuspendAll() vTaskSuspendAll()
#define os_taskResumeAll()  xTaskResumeAll()
#define os_enterCritical()  taskENTER_CRITICAL()
#define os_exitCritical()   taskEXIT_CRITICAL()
#define os_delay(ms)        vTaskDelay(ms)

#else

#if defined STM32F0
#include <stm32f0xx_hal.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#endif

#ifdef __GNUC__
#include "cmsis_gcc.h"
#endif // __GNUC__

#define os_taskSuspendAll()
#define os_taskResumeAll()
#define os_enterCritical()  __disable_irq()
#define os_exitCritical()   __enable_irq()
#define os_delay(ms)        HAL_Delay(ms)

#endif // OS_FREERTOS

