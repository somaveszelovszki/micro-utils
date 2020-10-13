#pragma once

#if defined STM32F4

#include <micro/container/map.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/port/can.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/queue.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

#include <cstring>
#include <functional>

namespace micro {

static constexpr uint32_t MAX_NUM_CAN_SUBSCRIBERS = 4;
static constexpr uint32_t MAX_NUM_CAN_FILTERS     = 16;

typedef sorted_vec<canFrame_t::id_t, MAX_NUM_CAN_FILTERS> CanFrameIds;

struct CanSubscriber {
    typedef uint8_t id_t;

    static constexpr id_t INVALID_ID = 0;

    struct Filter {
        canFrame_t::id_t id;
        millisecond_t lastActivityTime;
    };

    id_t id;
    sorted_map<canFrame_t::id_t, Filter, MAX_NUM_CAN_FILTERS> rxFilters, txFilters;
    ring_buffer<canFrame_t, 2 * MAX_NUM_CAN_FILTERS> rxFrames;

    explicit CanSubscriber(const id_t id = INVALID_ID, const CanFrameIds& rxFilters = {}, const CanFrameIds& txFilters = {});
};

class CanManager {
public:
    CanManager(const can_t& can, const millisecond_t rxTimeout);

    CanSubscriber::id_t registerSubscriber(const CanFrameIds& rxFilters, const CanFrameIds& txFilters);

    bool read(const CanSubscriber::id_t subscriberId, canFrame_t& frame);

    template<typename T, typename ...Args>
    void send(const CanSubscriber::id_t subscriberId, Args&&... args) {
        std::lock_guard<mutex_t> lock(this->mutex_);
        this->sendFrame<T>(this->subscribers_.at(subscriberId)->txFilters.at(T::id()), std::forward<Args>(args)...);
    }

    template<typename T, typename ...Args>
    void periodicSend(const CanSubscriber::id_t subscriberId, Args&&... args) {
        std::lock_guard<mutex_t> lock(this->mutex_);

        CanSubscriber::Filter *filter = this->subscribers_.at(subscriberId)->txFilters.at(T::id());
        if (filter && getTime() - filter->lastActivityTime >= T::period()) {
            this->sendFrame<T>(filter, std::forward<Args>(args)...);
        }
    }

    bool hasRxTimedOut() const {
        return this->rxWatchdog_.hasTimedOut();
    }

    void onFrameReceived();

private:
    CanSubscriber* subscriber(const CanSubscriber::id_t subscriberId);

    template<typename T, typename ...Args>
    void sendFrame(CanSubscriber::Filter *filter, Args&&... args) {
        if (filter) {
            const T data(std::forward<Args>(args)...);
            canFrame_t frame;
            frame.header.tx = can::buildHeader<T>();
            memcpy(frame.data, reinterpret_cast<const uint8_t*>(&data), sizeof(T));
            can_transmit(this->can_, frame);
            filter->lastActivityTime = getTime();
        }
    }

    mutable mutex_t mutex_;
    can_t can_;
    WatchdogTimer rxWatchdog_;
    sorted_map<CanSubscriber::id_t, CanSubscriber, MAX_NUM_CAN_SUBSCRIBERS> subscribers_;
};

class CanFrameHandler {
public:
    typedef std::function<void(const uint8_t * const)> handler_fn_t;

    void registerHandler(const canFrame_t::id_t frameId, const handler_fn_t& handler);

    void handleFrame(const canFrame_t& rxFrame);

    CanFrameIds identifiers() const;

private:
    typedef sorted_map<canFrame_t::id_t, handler_fn_t, 16> handlers_t;
    handlers_t handlers_;
};

}  // namespace micro

#endif // STM32F4
