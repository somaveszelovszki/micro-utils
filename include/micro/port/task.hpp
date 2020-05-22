#pragma once

#include <micro/utils/units.hpp>

namespace micro {

enum class context_t : uint8_t {
    TASK,
    ISR
};

typedef uint32_t interruptStatus_t;

context_t getContext();
interruptStatus_t os_enterCritical();
void os_exitCritical(const interruptStatus_t savedInterruptStatus);
void os_sleep(const millisecond_t delay);

} // namespace micro
