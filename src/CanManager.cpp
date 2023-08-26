#include <micro/panel/CanManager.hpp>
#include <micro/utils/algorithm.hpp>

#include <mutex>

#if defined STM32F4

namespace micro {

constexpr uint8_t CanSubscriber::INVALID_ID;

millisecond_t timeout(const canFrame_t::id_t id) {
    millisecond_t result(0);

    if      (can::LateralControl::id()        == id) result = can::LateralControl::timeout();
    else if (can::LongitudinalControl::id()   == id) result = can::LongitudinalControl::timeout();
    else if (can::FrontLines::id()            == id) result = can::FrontLines::timeout();
    else if (can::RearLines::id()             == id) result = can::RearLines::timeout();
    else if (can::LateralState::id()          == id) result = can::LateralState::timeout();
    else if (can::LongitudinalState::id()     == id) result = can::LongitudinalState::timeout();
    else if (can::FrontLinePattern::id()      == id) result = can::FrontLinePattern::timeout();
    else if (can::RearLinePattern::id()       == id) result = can::RearLinePattern::timeout();
    else if (can::LineDetectControl::id()     == id) result = can::LineDetectControl::timeout();
    else if (can::SetMotorControlParams::id() == id) result = can::SetMotorControlParams::timeout();

    return result;
}

CanSubscriber::CanSubscriber(const id_t id, const CanFrameIds& rxFilters, const CanFrameIds& txFilters)
    : id(id) {

    for (const canFrame_t::id_t id : rxFilters) {
        this->rxFilters.emplace(id, { id, millisecond_t(0) });
    }

    for (const canFrame_t::id_t id : txFilters) {
        this->txFilters.emplace(id, { id, millisecond_t(0) });
    }
}

bool CanSubscriber::hasTimedOut() const {
    bool timedOut = false;
    const millisecond_t now = getTime();

    for (Filters::const_iterator it = this->rxFilters.begin(); it != this->rxFilters.end(); ++it) {
        if (now - it->second.lastActivityTime > timeout(it->second.id)) {
            timedOut = true;
            break;
        }
    }

    return timedOut;
}

CanManager::CanManager(const can_t& can)
    : can_(can) {}

CanSubscriber::id_t CanManager::registerSubscriber(const CanFrameIds& rxFilters, const CanFrameIds& txFilters) {
    std::lock_guard<criticalSection_t> lock(this->criticalSection_);
    return this->subscribers_.emplace_back(this->subscribers_.size(), rxFilters, txFilters).id;
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
        for (CanSubscriber& subscriber : this->subscribers_) {
            CanSubscriber::Filter *filter = subscriber.rxFilters.at(rxFrame.header.rx.StdId);
            if (filter) {
                subscriber.rxFrames.write(rxFrame);
                filter->lastActivityTime = getTime();
            }
        }
    }
}

bool CanManager::hasTimedOut(const CanSubscriber::id_t subscriberId) const {
    return this->isValid(subscriberId) && this->subscribers_[subscriberId].hasTimedOut();
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
