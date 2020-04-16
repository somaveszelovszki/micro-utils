#pragma once

#include <micro/container/map.hpp>
#include <micro/utils/timer.hpp>

#include "vehicleCanTypes.hpp"

#include <functional>

namespace micro {

typedef std::function<void(const uint8_t * const)> canFrame_handler_fn_t;

class CanManager {
public:
    explicit CanManager(CAN_HandleTypeDef * const hcan, const uint32_t rxFifo, const millisecond_t rxTimeout);

    void registerHandler(const uint16_t id, const canFrame_handler_fn_t& handler);

    void handleIncomingFrames();

    template <typename T>
    void send(const T& data) {
        CAN_TxHeaderTypeDef txHeader = can::buildHeader<T>();
        uint32_t txMailbox = 0;
        HAL_CAN_AddTxMessage(hcan_, &txHeader, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&data)), &txMailbox);
    }

    bool hasRxTimedOut() const {
        return this->rxWatchdog_.hasTimedOut();
    }

private:
    void handleIncomingFrame(const uint32_t id, const uint8_t * const data);

    typedef sorted_map<uint16_t, canFrame_handler_fn_t, 64> handler_map_t;
    CAN_HandleTypeDef * const hcan_;
    const uint32_t rxFifo_;
    handler_map_t handlers_;
    WatchdogTimer rxWatchdog_;
};

}  // namespace micro
