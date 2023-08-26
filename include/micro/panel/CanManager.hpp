#pragma once

#include <cstring>
#include <functional>

#include <etl/vector.h>

#include <micro/container/map.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/port/can.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/queue.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

namespace micro {

#define MAX_NUM_CAN_SUBSCRIBERS 4
#define MAX_NUM_CAN_FILTERS     12

typedef sorted_vec<canFrame_t::id_t, MAX_NUM_CAN_FILTERS> CanFrameIds;

struct CanSubscriber {
    typedef uint8_t id_t;

    static constexpr id_t INVALID_ID = 0xff;

    struct Filter {
        canFrame_t::id_t id;
        millisecond_t lastActivityTime;
    };

    typedef sorted_map<canFrame_t::id_t, Filter, MAX_NUM_CAN_FILTERS> Filters;

    id_t id;
    Filters rxFilters, txFilters;
    ring_buffer<canFrame_t, MAX_NUM_CAN_FILTERS> rxFrames;

    CanSubscriber(const id_t id = INVALID_ID, const CanFrameIds& rxFilters = {}, const CanFrameIds& txFilters = {});

    bool hasTimedOut() const;
};

class CanManager {
public:
    explicit CanManager(const can_t& can);

    CanSubscriber::id_t registerSubscriber(const CanFrameIds& rxFilters, const CanFrameIds& txFilters);

    bool read(const CanSubscriber::id_t subscriberId, canFrame_t& frame);

    template<typename T, typename ...Args>
    void send(const CanSubscriber::id_t subscriberId, Args&&... args) {
        std::lock_guard<criticalSection_t> lock(this->criticalSection_);

        if (this->isValid(subscriberId)) {
            this->sendFrame<T>(this->subscribers_[subscriberId].txFilters.at(T::id()), std::forward<Args>(args)...);
        }
    }

    template<typename T, typename ...Args>
    void periodicSend(const CanSubscriber::id_t subscriberId, Args&&... args) {
        std::lock_guard<criticalSection_t> lock(this->criticalSection_);

        if (this->isValid(subscriberId)) {
            CanSubscriber::Filter *filter = this->subscribers_[subscriberId].txFilters.at(T::id());
            if (filter && getTime() - filter->lastActivityTime >= T::period()) {
                this->sendFrame<T>(filter, std::forward<Args>(args)...);
            }
        }
    }

    void onFrameReceived();

    bool hasTimedOut(const CanSubscriber::id_t subscriberId) const;

private:
    bool isValid(const CanSubscriber::id_t subscriberId) const {
        return subscriberId < this->subscribers_.size();
    }

    template<typename T, typename ...Args>
    void sendFrame(CanSubscriber::Filter *filter, Args&&... args) {
        if (filter) {
            const T data(std::forward<Args>(args)...);
            const auto frame = can_buildFrame(T::id(), reinterpret_cast<const uint8_t*>(&data), sizeof(T));
            can_transmit(this->can_, frame);
            filter->lastActivityTime = getTime();
        }
    }

    mutable criticalSection_t criticalSection_;
    can_t can_;
    etl::vector<CanSubscriber, MAX_NUM_CAN_SUBSCRIBERS> subscribers_;
};

class CanFrameHandler {
public:
    typedef std::function<void(const uint8_t * const)> handler_fn_t;

    void registerHandler(const canFrame_t::id_t frameId, const handler_fn_t& handler);

    void handleFrame(const canFrame_t& rxFrame);

    CanFrameIds identifiers() const;

private:
    typedef sorted_map<canFrame_t::id_t, handler_fn_t, MAX_NUM_CAN_FILTERS> Handlers;
    Handlers handlers_;
};

}  // namespace micro
