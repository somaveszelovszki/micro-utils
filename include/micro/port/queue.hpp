#pragma once

#include <micro/math/unit_utils.hpp>
#include "task.hpp"

#if defined OS_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif // OS_FREERTOS

namespace micro {

#if defined OS_FREERTOS

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
    
#else // !OS_FREERTOS

template <typename T, uint32_t>
class queue_t {
public:
    bool receive(T&, const millisecond_t) { return true; }
    bool peek(T&, const millisecond_t) { return true; }
    void overwrite(const T&) {}
    void send(const T&, const millisecond_t) {}
};

#endif // !OS_FREERTOS

} // namespace micro
