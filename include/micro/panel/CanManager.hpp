#pragma once

#include <functional>
#include <mutex>
#include <optional>

#include <etl/circular_buffer.h>

#include <micro/container/map.hpp>
#include <micro/container/set.hpp>
#include <micro/container/vector.hpp>
#include <micro/port/can.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/queue.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

namespace micro {

#define MAX_NUM_CAN_SUBSCRIBERS 4
#define MAX_NUM_CAN_FILTERS 12

using CanFrameIds = micro::set<canFrameId_t, MAX_NUM_CAN_FILTERS>;

struct CanSubscriber {
    using Id                       = uint8_t;
    static constexpr Id INVALID_ID = 0xff;

    struct Filter {
        canFrameId_t id;
        millisecond_t lastActivityTime;
    };

    using Filters = micro::map<canFrameId_t, Filter, MAX_NUM_CAN_FILTERS>;

    Filters rxFilters, txFilters;
    etl::circular_buffer<canFrame_t, MAX_NUM_CAN_FILTERS> rxFrames;

    CanSubscriber(const CanFrameIds& rxFilters = {}, const CanFrameIds& txFilters = {});

    bool hasTimedOut() const;
};

class CanManager {
  public:
    explicit CanManager(const can_t& can);

    CanSubscriber::Id registerSubscriber(const CanFrameIds& rxFrameIds,
                                         const CanFrameIds& txFrameIds);

    std::optional<canFrame_t> read(const CanSubscriber::Id subscriberId);

    template <typename T, typename... Args>
    void send(const CanSubscriber::Id subscriberId, Args&&... args) {
        send<T>(subscriberId, false, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    void periodicSend(const CanSubscriber::Id subscriberId, Args&&... args) {
        send<T>(subscriberId, true, std::forward<Args>(args)...);
    }

    void onFrameReceived();

    bool hasTimedOut(const CanSubscriber::Id subscriberId) const;

  private:
    bool isValid(const CanSubscriber::Id subscriberId) const {
        return subscriberId < subscribers_.size();
    }

    template <typename T, typename... Args>
    void send(const CanSubscriber::Id subscriberId, const bool checkPeriod, Args&&... args) {
        std::scoped_lock lock(criticalSection_);

        if (!isValid(subscriberId)) {
            return;
        }

        auto& txFilters = subscribers_[subscriberId].txFilters;
        if (auto it = txFilters.find(T::id()); it != txFilters.end()) {
            auto& filter   = it->second;
            const auto now = getTime();
            if (!checkPeriod || (now - filter.lastActivityTime) >= T::period()) {
                filter.lastActivityTime = now;
                const T data(std::forward<Args>(args)...);
                const auto frame =
                    can_buildFrame(T::id(), reinterpret_cast<const uint8_t*>(&data), sizeof(T));
                can_transmit(can_, frame);
            }
        }
    }

    mutable criticalSection_t criticalSection_;
    can_t can_;
    micro::vector<CanSubscriber, MAX_NUM_CAN_SUBSCRIBERS> subscribers_;
};

class CanFrameHandler {
  public:
    typedef std::function<void(const uint8_t* const)> handler_fn_t;

    void registerHandler(const canFrameId_t frameId, const handler_fn_t& handler);

    void handleFrame(const canFrame_t& rxFrame);

    CanFrameIds identifiers() const;

  private:
    micro::map<canFrameId_t, handler_fn_t, MAX_NUM_CAN_FILTERS> handlers_;
};

} // namespace micro
