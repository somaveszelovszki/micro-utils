#if defined STM32

#include <cmath>
#include <micro/port/task.hpp>

#if defined STM32F4
#include <stm32f446xx.h>
#include <stm32f4xx_hal.h>
#elif defined STM32F0
#include <stm32f0xx.h>
#include <stm32f0xx_hal.h>
#endif

#if defined __GNUC__
#include "cmsis_gcc.h"
#else
#error "STM32 with any other compiler than GCC is not supported!"
#endif // __GNUC__

namespace micro {

context_t getContext() {
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ? context_t::ISR : context_t::TASK;
}

#if !defined OS_FREERTOS

interruptStatus_t os_enterCritical() {
    __disable_irq();
    return 0;
}

void os_exitCritical(const interruptStatus_t) {
    __enable_irq();
}

void os_sleep(const millisecond_t delay) {
    HAL_Delay(static_cast<uint32_t>(std::lround(delay.get())));
}

#endif // !OS_FREERTOS

} // namespace micro

#endif // STM32
