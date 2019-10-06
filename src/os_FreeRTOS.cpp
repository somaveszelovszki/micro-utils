#ifdef OS_FREERTOS

#include <micro/bsp/tim.hpp>
#include <micro/bsp/it.hpp>
#include <micro/bsp/queue.hpp>
#include <micro/bsp/mutex.hpp>
#include <micro/bsp/task.hpp>

#include <cmsis_os.h>
extern "C" osMessageQId LogQueueHandle;

namespace micro {

void nonBlockingDelay(millisecond_t delay) {
    vTaskDelay(delay.get());
}

Status queueSend(const queue_handle_t hQueue, const void * const txBuffer){
    return xQueueSend(static_cast<osMessageQId>(hQueue.ptr), txBuffer, 0) > 0 ? Status::OK : Status::BUFFER_FULL;
}

Status queueReceive(const queue_handle_t hQueue, void * const rxBuffer){
    return xQueueReceive(static_cast<osMessageQId>(hQueue.ptr), rxBuffer, 0) > 0 ? Status::OK : Status::NO_NEW_DATA;
}

// IT

void enterCritical() {
    taskENTER_CRITICAL();
}

uint32_t enterCritical_ISR() {
    return static_cast<uint32_t>(taskENTER_CRITICAL_FROM_ISR());
}

void exitCritical() {
    taskEXIT_CRITICAL();
}

void exitCritical_ISR(uint32_t uxSavedInterruptStatus) {
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}

Status mutexTake(mutex_handle_t hMutex, millisecond_t timeout) {
    return xSemaphoreTake(static_cast<osMutexId>(hMutex.ptr), timeout.get()) == pdTRUE ? Status::OK : Status::TIMEOUT;
}

Status mutexTake_ISR(mutex_handle_t hMutex) {
    return xSemaphoreTakeFromISR(static_cast<osMutexId>(hMutex.ptr), nullptr) == pdTRUE ? Status::OK : Status::BUSY;
}

Status mutexRelease(mutex_handle_t hMutex)  {
    return xSemaphoreGive(static_cast<osMutexId>(hMutex.ptr)) == pdTRUE ? Status::OK : Status::ERROR;
}

Status mutexRelease_ISR(mutex_handle_t hMutex)  {
    return xSemaphoreGiveFromISR(static_cast<osMutexId>(hMutex.ptr), nullptr) == pdTRUE ? Status::OK : Status::ERROR;
}

void taskResume(task_handle_t hTask) {
    vTaskResume(static_cast<osThreadId>(hTask.ptr));
}

void taskResumeAll() {
    xTaskResumeAll();
}

void taskSuspend(task_handle_t hTask) {
    vTaskSuspend(static_cast<osThreadId>(hTask.ptr));
}

void taskSuspendAll() {
    vTaskSuspendAll();
}

void taskDelete(task_handle_t hTask) {
    vTaskDelete(static_cast<osThreadId>(hTask.ptr));
}

void taskDeleteCurrent() {
    vTaskDelete(nullptr);
}

} // namespace micro

#endif // OS_FREERTOS
