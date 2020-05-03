#pragma once

#if defined OS_FREERTOS

#include <FreeRTOS.h>
#include <task.h>

#define os_taskSuspendAll() vTaskSuspendAll()
#define os_taskResumeAll()  xTaskResumeAll()
#define os_enterCritical()  taskENTER_CRITICAL()
#define os_exitCritical()   taskEXIT_CRITICAL()
#define os_delay(ms)        vTaskDelay(ms)

#elif defined BSP_LIB_HAL

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

#else

#define os_taskSuspendAll()
#define os_taskResumeAll()
#define os_enterCritical()
#define os_exitCritical()
#define os_delay(ms)

#endif

namespace micro {

template <typename ptr>
void waitReady(const volatile ptr& item) {
    while (!item) {
        os_delay(1);
    }
}

} // namespace micro
