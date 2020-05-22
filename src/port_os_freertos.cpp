#if defined OS_FREERTOS

#include <micro/math/numeric.hpp>
#include <micro/port/task.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace micro {

#if !defined STM32
context_t getContext() { return context_t::TASK; }
#endif // !STM32

interruptStatus_t os_enterCritical() {
    interruptStatus_t status = 0;
    if (getContext() == context_t::ISR) {
        status = taskENTER_CRITICAL_FROM_ISR();
    } else {
        taskENTER_CRITICAL();
    }
    return status;
}

void os_exitCritical(const interruptStatus_t status) {
    if (getContext() == context_t::ISR) {
        taskEXIT_CRITICAL_FROM_ISR(status);
    } else {
        taskEXIT_CRITICAL();
    }
}

void os_sleep(const millisecond_t delay) {
    vTaskDelay(static_cast<TickType_t>(micro::round(delay.get())));
}

} // namespace micro

#endif // OS_FREERTOS
