#pragma once

#include "types.hpp"

#ifdef OS_FREERTOS

#include <FreeRTOS.h>
#include <task.h>

#define os_taskSuspendAll() vTaskSuspendAll()
#define os_taskResumeAll()  xTaskResumeAll()
#define os_delay(ms)        vTaskDelay(ms)

#else

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_tim.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#endif

#define os_taskSuspendAll()
#define os_taskResumeAll()
#define os_delay(ms)        HAL_Delay(ms)

#endif // OS_FREERTOS

