#include <micro/panel/CanManager.hpp>
#include <micro/utils/algorithm.hpp>

#include <mutex>

#if defined STM32F4

namespace micro {

void CanFrameHandler::registerHandler(const canFrame_t::id_t frameId, const handler_fn_t& handler) {
    this->handlers_.emplace(frameId, handler);
}

void CanFrameHandler::handleFrame(const canFrame_t& rxFrame) {
    handler_fn_t * const handler = this->handlers_.at(rxFrame.header.rx.StdId);
    if (handler) {
        (*handler)(rxFrame.data);
    }
}

CanManager::filters_t CanFrameHandler::identifiers() const {
    CanManager::filters_t ids;
    for (const handlers_t::entry_type& entry : this->handlers_) {
        ids.insert(entry.first);
    }
    return ids;
}

CanManager::CanManager(CAN_HandleTypeDef * const hcan, const uint32_t rxFifo, const millisecond_t rxTimeout)
    : hcan_(hcan)
    , rxFifo_(rxFifo)
    , rxWatchdog_(rxTimeout) {}

CanManager::subscriberId_t CanManager::registerSubscriber(const filters_t& rxFilters) {
    std::lock_guard<mutex_t> lock(this->mutex_);
    subscriber_t sub;
    sub.rxFilters_ = rxFilters;
    this->subscribers_.push_back(sub);
    return this->subscribers_.size() - 1;
}

bool CanManager::read(const subscriberId_t subscriberId, canFrame_t& frame) {
    std::lock_guard<mutex_t> lock(this->mutex_);
    return this->subscribers_[subscriberId].rxFrames_.receive(frame, millisecond_t(0));
}

void CanManager::onFrameReceived() {
    std::lock_guard<mutex_t> lock(this->mutex_);
    canFrame_t rxFrame;
    if (HAL_OK == HAL_CAN_GetRxMessage(this->hcan_, this->rxFifo_, &rxFrame.header.rx, rxFrame.data)) {
        this->rxWatchdog_.reset();

        for (subscriber_t& sub : this->subscribers_) {
            filters_t::const_iterator filter = std::lower_bound(sub.rxFilters_.begin(), sub.rxFilters_.end(), rxFrame.header.rx.StdId);
            if (filter != sub.rxFilters_.end() && *filter == rxFrame.header.rx.StdId) {
                sub.rxFrames_.send(rxFrame, millisecond_t(0));
            }
        }
    }
}

}  // namespace micro

#endif // STM32F4
