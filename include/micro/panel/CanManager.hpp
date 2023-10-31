#pragma once

#include <functional>
#include <mutex>
#include <optional>

#include <etl/circular_buffer.h>
#include <etl/map.h>
#include <etl/set.h>
#include <etl/vector.h>

#include <micro/port/can.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/queue.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

namespace micro {

#define MAX_NUM_CAN_SUBSCRIBERS 4
#define MAX_NUM_CAN_FILTERS     12

using CanFrameIds = etl::set<canFrameId_t, MAX_NUM_CAN_FILTERS>;

struct CanSubscriber {
    typedef uint8_t id_t;

    static constexpr id_t INVALID_ID = 0xff;

    struct Filter {
        canFrameId_t id;
        millisecond_t lastActivityTime;
    };

    using Filters = etl::map<canFrameId_t, Filter, MAX_NUM_CAN_FILTERS>;

    id_t id;
    Filters rxFilters, txFilters;
    etl::circular_buffer<canFrame_t, MAX_NUM_CAN_FILTERS> rxFrames;

    CanSubscriber(const id_t id = INVALID_ID, const CanFrameIds& rxFilters = {}, const CanFrameIds& txFilters = {});

    bool hasTimedOut() const;
};

class CanManager {
public:
    explicit CanManager(const can_t& can);

    CanSubscriber::id_t registerSubscriber(const CanFrameIds& rxFrameIds, const CanFrameIds& txFrameIds);

    std::optional<canFrame_t> read(const CanSubscriber::id_t subscriberId);

    template<typename T, typename ...Args>
    void send(const CanSubscriber::id_t subscriberId, Args&&... args) {
        send<T>(subscriberId, false, std::forward<Args>(args)...);
    }

    template<typename T, typename ...Args>
    void periodicSend(const CanSubscriber::id_t subscriberId, Args&&... args) {
        send<T>(subscriberId, true, std::forward<Args>(args)...);
    }

    void onFrameReceived();

    bool hasTimedOut(const CanSubscriber::id_t subscriberId) const;

private:
    bool isValid(const CanSubscriber::id_t subscriberId) const {
        return subscriberId < subscribers_.size();
    }

    template<typename T, typename ...Args>
    void send(const CanSubscriber::id_t subscriberId, const bool checkPeriod, Args&&... args) {
        std::scoped_lock lock(criticalSection_);

        if (isValid(subscriberId)) {
            auto& txFilters = subscribers_[subscriberId].txFilters;
            if (auto it = txFilters.find(T::id()); it != txFilters.end()) {
                const auto now = getTime();
                if (!checkPeriod || (getTime() - std::exchange(it->second.lastActivityTime, now) >= T::period())) {
                    const T data(std::forward<Args>(args)...);
                    const auto frame = can_buildFrame(T::id(), reinterpret_cast<const uint8_t*>(&data), sizeof(T));
                    can_transmit(can_, frame);
                }
            }
        }
    }

    mutable criticalSection_t criticalSection_;
    can_t can_;
    etl::vector<CanSubscriber, MAX_NUM_CAN_SUBSCRIBERS> subscribers_;
};

class CanFrameHandler {
public:
    typedef std::function<void(const uint8_t * const)> handler_fn_t;

    void registerHandler(const canFrameId_t frameId, const handler_fn_t& handler);

    void handleFrame(const canFrame_t& rxFrame);

    CanFrameIds identifiers() const;

private:
    etl::map<canFrameId_t, handler_fn_t, MAX_NUM_CAN_FILTERS> handlers_;
};

}  // namespace micro
