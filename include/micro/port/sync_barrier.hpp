#pragma once

#include <cmath>
#include <micro/math/unit_utils.hpp>

#if defined OS_FREERTOS
#include <FreeRTOS.h>
#include <event_groups.h>
#endif // OS_FREERTOS

namespace micro {

#if defined OS_FREERTOS

class sync_barrier_t {
  public:
    explicit sync_barrier_t(const uint32_t allEvents)
        : allEvents_{static_cast<TickType_t>(allEvents)} {
        xEventGroupCreateStatic(&this->eventGroupBuffer_);
    }

    bool sync(const uint32_t event,
              const millisecond_t timeout = micro::numeric_limits<millisecond_t>::infinity()) {
        const auto events =
            xEventGroupSync(this->handle(), static_cast<TickType_t>(event), allEvents_,
                            micro::isinf(timeout) ? portMAX_DELAY : std::lround(timeout.get()));
        return events == allEvents_;
    }

  private:
    EventGroupHandle_t handle() { return &this->eventGroupBuffer_; }

    StaticEventGroup_t eventGroupBuffer_;
    TickType_t allEvents_{};
};

#else // !OS_FREERTOS

class sync_barrier_t {
  public:
    explicit sync_barrier_t(const uint32_t) {}
    bool sync(const uint32_t,
              const millisecond_t = micro::numeric_limits<millisecond_t>::infinity()) {
        return true;
    }
};

#endif // !OS_FREERTOS

} // namespace micro
