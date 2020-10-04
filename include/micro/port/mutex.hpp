#pragma once

#include <micro/math/unit_utils.hpp>
#include "task.hpp"

#if defined OS_FREERTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif // OS_FREERTOS

namespace micro {

#if defined OS_FREERTOS

class mutex_t {
public:
    mutex_t() {
        xSemaphoreCreateMutexStatic(&this->semphrBuffer_);
    }

    bool lock(const millisecond_t timeout = micro::numeric_limits<millisecond_t>::infinity()) {
        bool success = false;
        if (getContext() == context_t::ISR) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            success = !!xSemaphoreTakeFromISR(this->handle(), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            success = !!xSemaphoreTake(this->handle(), micro::isinf(timeout) ? portMAX_DELAY : micro::round(timeout.get()));
        }
        return success;
    }

    bool unlock() {
        bool success = false;
        if (getContext() == context_t::ISR) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            success = !!xSemaphoreGiveFromISR(this->handle(), &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            success = !!xSemaphoreGive(this->handle());
        }
        return success;
    }

private:
    SemaphoreHandle_t handle() {
        return &this->semphrBuffer_;
    }

    StaticSemaphore_t semphrBuffer_;
};

#else // !OS_FREERTOS

class mutex_t {
public:
    bool lock(const millisecond_t = micro::numeric_limits<millisecond_t>::infinity()) { return true; }
    void unlock() {}
};

#endif // !OS_FREERTOS

} // namespace micro
