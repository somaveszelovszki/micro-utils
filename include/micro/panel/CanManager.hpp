#pragma once

#if defined STM32F4

#include <micro/container/map.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/queue.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>

#include <functional>

namespace micro {

struct canFrame_t {
    typedef uint16_t id_t;
    union {
        CAN_RxHeaderTypeDef rx;
        CAN_TxHeaderTypeDef tx;
    } header;
    alignas(8) uint8_t data[8];
};

class CanManager {
public:
    typedef sorted_vec<uint16_t, 16> filters_t;
    typedef uint8_t subscriberId_t;

    explicit CanManager(CAN_HandleTypeDef * const hcan, const uint32_t rxFifo, const millisecond_t rxTimeout);

    subscriberId_t registerSubscriber(const filters_t& rxFilters);

    bool read(const subscriberId_t subscriberId, canFrame_t& frame);

    template <typename T>
    void send(const T& data) {
        CAN_TxHeaderTypeDef txHeader = can::buildHeader<T>();
        uint32_t txMailbox = 0;
        HAL_CAN_AddTxMessage(this->hcan_, &txHeader, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&data)), &txMailbox);
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
    CAN_HandleTypeDef * const hcan_;
    const uint32_t rxFifo_;
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
