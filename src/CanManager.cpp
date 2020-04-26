#include <micro/panel/CanManager.hpp>

#if defined STM32F4

namespace micro {

CanManager::CanManager(CAN_HandleTypeDef * const hcan, const uint32_t rxFifo, const millisecond_t rxTimeout)
    : hcan_(hcan)
    , rxFifo_(rxFifo)
    , rxWatchdog_(rxTimeout) {}

void CanManager::registerHandler(const uint16_t id, const canFrame_handler_fn_t& handler) {
    this->handlers_.put(id, handler);
}

void CanManager::handleIncomingFrames() {
    CAN_RxHeaderTypeDef rxHeader;
    alignas(8) uint8_t rxData[8];

    while (HAL_CAN_GetRxFifoFillLevel(this->hcan_, this->rxFifo_)) {
        if (HAL_OK == HAL_CAN_GetRxMessage(this->hcan_, this->rxFifo_, &rxHeader, rxData)) {
            this->rxWatchdog_.reset();
            this->handleIncomingFrame(rxHeader.StdId, rxData);
        }
    }
}

void CanManager::handleIncomingFrame(const uint32_t id, const uint8_t * const data) {
    canFrame_handler_fn_t *handler = this->handlers_.get(id);
    if (handler) {
        (*handler)(data);
    }
}

}  // namespace micro

#endif // STM32F4
