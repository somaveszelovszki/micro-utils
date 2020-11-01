#include <micro/panel/CanManager.hpp>
#include <micro/utils/algorithm.hpp>

#include <mutex>

#if defined STM32F4

namespace micro {

constexpr uint8_t CanSubscriber::INVALID_ID;

CanSubscriber::CanSubscriber(const id_t id, const CanFrameIds& rxFilters, const CanFrameIds& txFilters)
    : id(id) {

    for (const canFrame_t::id_t id : rxFilters) {
        this->rxFilters.emplace(id, { id, millisecond_t(0) });
    }

    for (const canFrame_t::id_t id : txFilters) {
        this->txFilters.emplace(id, { id, millisecond_t(0) });
    }
}

CanManager::CanManager(const can_t& can, const millisecond_t rxTimeout)
    : can_(can)
    , rxWatchdog_(rxTimeout) {}

CanSubscriber::id_t CanManager::registerSubscriber(const CanFrameIds& rxFilters, const CanFrameIds& txFilters) {
    std::lock_guard<criticalSection_t> lock(this->criticalSection_);
    return this->subscribers_.emplace_back(this->subscribers_.size(), rxFilters, txFilters)->id;
}

bool CanManager::read(const CanSubscriber::id_t subscriberId, canFrame_t& frame) {
    std::lock_guard<criticalSection_t> lock(this->criticalSection_);

    bool success = false;
    if (this->isValid(subscriberId)) {
        success = this->subscribers_[subscriberId].rxFrames.read(frame);
    }

    return success;
}

void CanManager::onFrameReceived() {
    std::lock_guard<criticalSection_t> lock(this->criticalSection_);

    canFrame_t rxFrame;
    if (isOk(can_receive(this->can_, rxFrame))) {
        this->rxWatchdog_.reset();

        for (CanSubscriber& subscriber : this->subscribers_) {
            CanSubscriber::Filter *filter = subscriber.rxFilters.at(rxFrame.header.rx.StdId);
            if (filter) {
                subscriber.rxFrames.write(rxFrame);
                filter->lastActivityTime = getTime();
            }
        }
    }
}

void CanFrameHandler::registerHandler(const canFrame_t::id_t frameId, const handler_fn_t& handler) {
    this->handlers_.emplace(frameId, handler);
}

void CanFrameHandler::handleFrame(const canFrame_t& rxFrame) {
    handler_fn_t * const handler = this->handlers_.at(rxFrame.header.rx.StdId);
    if (handler) {
        (*handler)(rxFrame.data);
    }
}

CanFrameIds CanFrameHandler::identifiers() const {
    CanFrameIds ids;
    for (const Handlers::entry_type& entry : this->handlers_) {
        ids.insert(entry.first);
    }
    return ids;
}

}  // namespace micro

#endif // STM32F4
