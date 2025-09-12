#include <mutex>

#include <micro/panel/CanManager.hpp>
#include <micro/utils/algorithm.hpp>

namespace micro {

millisecond_t timeout(const canFrameId_t id) {
    millisecond_t result(0);

    if (can::LateralControl::id() == id)
        result = can::LateralControl::timeout();
    else if (can::LongitudinalControl::id() == id)
        result = can::LongitudinalControl::timeout();
    else if (can::FrontLines::id() == id)
        result = can::FrontLines::timeout();
    else if (can::RearLines::id() == id)
        result = can::RearLines::timeout();
    else if (can::LateralState::id() == id)
        result = can::LateralState::timeout();
    else if (can::LongitudinalState::id() == id)
        result = can::LongitudinalState::timeout();
    else if (can::FrontLinePattern::id() == id)
        result = can::FrontLinePattern::timeout();
    else if (can::RearLinePattern::id() == id)
        result = can::RearLinePattern::timeout();
    else if (can::LineDetectControl::id() == id)
        result = can::LineDetectControl::timeout();
    else if (can::SetMotorControlParams::id() == id)
        result = can::SetMotorControlParams::timeout();

    return result;
}

CanSubscriber::CanSubscriber(const CanFrameIds& rxFrameIds, const CanFrameIds& txFrameIds) {
    for (const auto id : rxFrameIds) {
        rxFilters.insert(std::make_pair(id, Filter{id, millisecond_t(0)}));
    }

    for (const auto id : txFrameIds) {
        txFilters.insert(std::make_pair(id, Filter{id, millisecond_t(0)}));
    }
}

bool CanSubscriber::hasTimedOut() const {
    bool timedOut           = false;
    const millisecond_t now = getTime();

    for (Filters::const_iterator it = rxFilters.begin(); it != rxFilters.end(); ++it) {
        if (now - it->second.lastActivityTime > timeout(it->second.id)) {
            timedOut = true;
            break;
        }
    }

    return timedOut;
}

CanManager::CanManager(const can_t& can) : can_(can) {
}

CanSubscriber::Id CanManager::registerSubscriber(const CanFrameIds& rxFilters,
                                                 const CanFrameIds& txFilters) {
    std::scoped_lock lock(criticalSection_);
    subscribers_.emplace_back(rxFilters, txFilters);
    return static_cast<CanSubscriber::Id>(subscribers_.size() - 1);
}

std::optional<canFrame_t> CanManager::read(const CanSubscriber::Id subscriberId) {
    std::scoped_lock lock(criticalSection_);

    if (!isValid(subscriberId)) {
        return std::nullopt;
    }

    auto& rxFrames = subscribers_[subscriberId].rxFrames;

    if (rxFrames.empty()) {
        return std::nullopt;
    }

    const auto frame = std::move(rxFrames.front());
    rxFrames.pop();
    return frame;
}

void CanManager::onFrameReceived() {
    std::scoped_lock lock(criticalSection_);

    canFrame_t rxFrame;
    if (isOk(can_receive(can_, rxFrame))) {
        for (auto& subscriber : subscribers_) {
            if (auto it = subscriber.rxFilters.find(can_getId(rxFrame));
                it != subscriber.rxFilters.end()) {
                subscriber.rxFrames.push(rxFrame);
                it->second.lastActivityTime = getTime();
            }
        }
    }
}

bool CanManager::hasTimedOut(const CanSubscriber::Id subscriberId) const {
    return isValid(subscriberId) && subscribers_[subscriberId].hasTimedOut();
}

void CanFrameHandler::registerHandler(const canFrameId_t frameId, const handler_fn_t& handler) {
    handlers_.insert(std::make_pair(frameId, handler));
}

void CanFrameHandler::handleFrame(const canFrame_t& rxFrame) {
    if (auto it = handlers_.find(can_getId(rxFrame)); it != handlers_.end()) {
        (it->second)(rxFrame.data);
    }
}

CanFrameIds CanFrameHandler::identifiers() const {
    CanFrameIds ids;
    for (const auto& entry : handlers_) {
        ids.insert(entry.first);
    }
    return ids;
}

} // namespace micro
