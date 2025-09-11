#pragma once

#include "task.hpp"

#include <cmath>
#include <micro/math/unit_utils.hpp>

#if defined OS_FREERTOS
#include <FreeRTOS.h>
#include <queue.h>
#endif // OS_FREERTOS

namespace micro {

#if defined OS_FREERTOS

template <typename T, uint32_t size> class queue_t {
  public:
    queue_t() {
        xQueueCreateStatic(size, sizeof(T), this->queueStorageBuffer_, &this->queueBuffer_);
    }

    bool receive(T& value,
                 const millisecond_t timeout = micro::numeric_limits<millisecond_t>::infinity()) {
        return xQueueReceive(this->handle(), &value,
                             micro::isinf(timeout) ? portMAX_DELAY : std::lround(timeout.get()));
    }

    bool peek(T& value,
              const millisecond_t timeout = micro::numeric_limits<millisecond_t>::infinity()) {
        return xQueuePeek(this->handle(), &value,
                          micro::isinf(timeout) ? portMAX_DELAY : std::lround(timeout.get()));
    }

    bool overwrite(const T& value) {
        bool success = false;
        if (getContext() == context_t::ISR) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            success = !!xQueueOverwriteFromISR(this->handle(), &value, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            success = !!xQueueOverwrite(this->handle(), &value);
        }
        return success;
    }

    bool send(const T& value,
              const millisecond_t timeout = micro::numeric_limits<millisecond_t>::infinity()) {
        bool success = false;
        if (getContext() == context_t::ISR) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            success = !!xQueueSendFromISR(this->handle(), &value, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            success =
                !!xQueueSend(this->handle(), &value,
                             micro::isinf(timeout) ? portMAX_DELAY : std::lround(timeout.get()));
        }
        return success;
    }

  public:
    QueueHandle_t handle() { return &this->queueBuffer_; }

    uint8_t queueStorageBuffer_[size * sizeof(T)];
    StaticQueue_t queueBuffer_;
};

#else // !OS_FREERTOS

template <typename T, uint32_t> class queue_t {
  public:
    bool receive(T&, const millisecond_t = micro::numeric_limits<millisecond_t>::infinity()) {
        return true;
    }
    bool peek(T&, const millisecond_t = micro::numeric_limits<millisecond_t>::infinity()) {
        return true;
    }
    void overwrite(const T&) {}
    void send(const T&, const millisecond_t = micro::numeric_limits<millisecond_t>::infinity()) {}
};

#endif // !OS_FREERTOS

} // namespace micro
