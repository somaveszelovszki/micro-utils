#pragma once

#if defined STM32F4
#include <stm32f446xx.h>
#elif defined STM32F0
#include <stm32f0xx.h>
#endif

namespace micro {
inline bool isInterrupt()
{
#if defined STM32F4 || defined STM32F0
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
#else
    return false;
#endif
}
} // namespace micro


#if defined OS_FREERTOS

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <string.h>

namespace micro {

template <typename T, uint32_t size>
class queue_t {
public:
    queue_t() {
        xQueueCreateStatic(size, sizeof(T), this->queueStorageBuffer_, &this->queueBuffer_);
    }

    bool receive(T& value) {
        return xQueueReceive(this->handle(), &value, 0);
    }

    void overwrite(const T& value) {
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueOverwriteFromISR(this->handle(), &value, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xQueueOverwrite(this->handle(), &value);
        }
    }

    void send(const T& value) {
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(this->handle(), &value, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xQueueSend(this->handle(), &value, 0);
        }
    }

private:
    QueueHandle_t handle() {
        return &this->queueBuffer_;
    }

    uint8_t queueStorageBuffer_[size * sizeof(T)];
    StaticQueue_t queueBuffer_;
};

class mutex_t {
public:
    mutex_t() {
        xSemaphoreCreateMutexStatic(&this->semphrBuffer_);
    }

    void lock() {
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreTakeFromISR(this->handle(), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xSemaphoreTake(this->handle(), portMAX_DELAY);
        }
    }

    void release() {
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(this->handle(), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xSemaphoreGive(this->handle());
        }
    }

private:
    SemaphoreHandle_t handle() {
        return &this->semphrBuffer_;
    }

    StaticSemaphore_t semphrBuffer_;
};

} // namespace micro

#define os_taskSuspendAll() vTaskSuspendAll()
#define os_taskResumeAll()  xTaskResumeAll()
#define os_enterCritical()  taskENTER_CRITICAL()
#define os_exitCritical()   taskEXIT_CRITICAL()
#define os_delay(ms)        vTaskDelay(ms)

#else // !OS_FREERTOS

#include <cstdint>

namespace micro {
class mutex_t {
public:
    void lock() {}
    void release() {}
};

template <typename T, uint32_t>
class queue_t {
public:
    bool receive(T&);
    void overwrite(const T&);
    void send(const T& );
};
} // namespace micro

#define os_taskSuspendAll()
#define os_taskResumeAll()

#if defined BSP_LIB_HAL

#if defined STM32F0
#include <stm32f0xx_hal.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#endif

#ifdef __GNUC__
#include "cmsis_gcc.h"
#endif // __GNUC__

#define os_enterCritical()  __disable_irq()
#define os_exitCritical()   __enable_irq()
#define os_delay(ms)        HAL_Delay(ms)

#else // !BSP_LIB_HAL

#define os_enterCritical()
#define os_exitCritical()
#define os_delay(...)

#endif // !BSP_LIB_HAL
#endif // !OS_FREERTOS

namespace micro {

class lock_guard_t {
public:
    explicit lock_guard_t(mutex_t& mtx) : mtx_(mtx) {
        this->mtx_.lock();
    }

    ~lock_guard_t() {
        this->mtx_.release();
    }
private:
    mutex_t& mtx_;
};

template <typename ptr>
void waitReady(const volatile ptr& item) {
    while (!item) {
        os_delay(1);
    }
}

} // namespace micro
