#if !defined OS_FREERTOS && !defined STM32

#include <micro/port/task.hpp>

namespace micro {

context_t getContext() { return context_t::TASK; }
taskId_t getCurrentTaskId() { return 0; }
TaskInfo getCurrentTaskInfo() { return { 0, {} }; }
interruptStatus_t os_enterCritical() { return 0; }
void os_exitCritical(const interruptStatus_t) {}
void os_sleep(const millisecond_t) {}

} // namespace micro

#endif // !OS_FREERTOS && !STM32
