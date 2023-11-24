#include <micro/debug/TaskMonitor.hpp>

#include <algorithm>

#include <micro/log/log.hpp>
#include <micro/port/task.hpp>

namespace micro {

void TaskMonitor::registerInitializedTask() {
    const auto event = (1 << taskStates_.size());
    taskStates_.insert(std::make_pair(getCurrentTaskId(), false));
    initializationBarrier_.sync(event);
}

void TaskMonitor::notify(const bool state) {
    if (const auto it = taskStates_.find(getCurrentTaskId()); it != taskStates_.end()) {
        it->second = state;
    }
}

bool TaskMonitor::ok() const {
    return std::all_of(taskStates_.begin(), taskStates_.end(), [now = getTime()](const auto& entry){
        const auto& state = entry.second;
        return now - state.timestamp() < millisecond_t(50) && state.value();
    });
}

} // namespace micro
