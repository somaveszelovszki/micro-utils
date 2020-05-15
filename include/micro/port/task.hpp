#pragma once

#include <micro/math/unit_utils.hpp>

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

    bool receive(T& value, const millisecond_t timeout) {
        return xQueueReceive(this->handle(), &value, micro::round(timeout.get()));
    }

    bool peek(T& value, const millisecond_t timeout) {
        return xQueuePeek(this->handle(), &value, micro::round(timeout.get()));
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

    void send(const T& value, const millisecond_t timeout) {
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(this->handle(), &value, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            xQueueSend(this->handle(), &value, micro::round(timeout.get()));
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

    bool lock(const millisecond_t timeout = micro::numeric_limits<millisecond_t>::infinity()) {
        bool success = false;
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            success = xSemaphoreTakeFromISR(this->handle(), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            success = xSemaphoreTake(this->handle(), micro::isinf(timeout) ? portMAX_DELAY : micro::round(timeout.get()));
        }
        return success;
    }

    void unlock() {
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

class semaphore_t {
public:
    semaphore_t() {
        xSemaphoreCreateBinaryStatic(&this->semphrBuffer_);
        this->give();
    }

    bool take(const millisecond_t timeout) {
        bool success = false;
        if (isInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            success = xSemaphoreTakeFromISR(this->handle(), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            success = xSemaphoreTake(this->handle(), micro::isinf(timeout) ? portMAX_DELAY : micro::round(timeout.get()));
        }
        return success;
    }

    void give() {
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
    bool lock(const millisecond_t = micro::numeric_limits<millisecond_t>::infinity()) { return true; }
    void unlock() {}
};

class semaphore_t {
public:
    bool take(const millisecond_t) { return true; }
    void give() {}
};

template <typename T, uint32_t>
class queue_t {
public:
    bool receive(T&, const millisecond_t);
    bool peek(T&, const millisecond_t);
    void overwrite(const T&);
    void send(const T&, const millisecond_t);
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
