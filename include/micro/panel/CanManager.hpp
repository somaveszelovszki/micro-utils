#pragma once

#if defined STM32F4

#include <micro/container/map.hpp>
#include <micro/port/can.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/queue.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

#include <cstring>
#include <functional>

namespace micro {

class CanManager {
public:
    typedef sorted_vec<uint16_t, 16> filters_t;
    typedef uint8_t subscriberId_t;

    CanManager(const can_t& can, const millisecond_t rxTimeout);

    subscriberId_t registerSubscriber(const filters_t& rxFilters);

    bool read(const subscriberId_t subscriberId, canFrame_t& frame);

    template <typename T>
    void send(const T& data) {
        canFrame_t frame;
        frame.header.tx = can::buildHeader<T>();
        memcpy(frame.data, reinterpret_cast<const uint8_t*>(&data), sizeof(T));
        can_transmit(this->can_, frame);
    }

    bool hasRxTimedOut() const {
        return this->rxWatchdog_.hasTimedOut();
    }

    void onFrameReceived();

private:
    struct subscriber_t {
        filters_t rxFilters_;
        queue_t<canFrame_t, 16> rxFrames_;
    };

    mutable mutex_t mutex_;
    can_t can_;
    WatchdogTimer rxWatchdog_;
    vec<subscriber_t, 4> subscribers_;
};

class CanFrameHandler {
public:
    typedef std::function<void(const uint8_t * const)> handler_fn_t;

    void registerHandler(const canFrame_t::id_t frameId, const handler_fn_t& handler);

    void handleFrame(const canFrame_t& rxFrame);

    CanManager::filters_t identifiers() const;

private:
    typedef sorted_map<canFrame_t::id_t, handler_fn_t, 16> handlers_t;
    handlers_t handlers_;
};

}  // namespace micro

#endif // STM32F4
