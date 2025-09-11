#include <algorithm>
#include <micro/debug/TaskMonitor.hpp>
#include <micro/log/log.hpp>
#include <micro/port/task.hpp>
#include <mutex>

namespace micro {

void TaskMonitor::registerInitializedTask() {
    const auto event = [this]() {
        std::scoped_lock lock{registerMutex_};
        taskStates_.insert(std::make_pair(getCurrentTaskId(), false));
        return (1 << (taskStates_.size() - 1));
    }();

    initializationBarrier_.sync(event);
}

void TaskMonitor::notify(const bool state) {
    taskStates_.at(getCurrentTaskId()) = state;
}

bool TaskMonitor::ok() const {
    return std::all_of(taskStates_.begin(), taskStates_.end(),
                       [now = getTime()](const auto& entry) {
                           const auto& state = entry.second;
                           return now - state.timestamp() < millisecond_t(50) && state.value();
                       });
}

} // namespace micro
