#include <micro/debug/TaskMonitor.hpp>

#include <algorithm>
#include <mutex>

#include <micro/log/log.hpp>
#include <micro/port/task.hpp>

namespace micro {

void TaskMonitor::registerInitializedTask() {
    const auto event = [this](){
        const auto info = getCurrentTaskInfo();
        std::scoped_lock lock{registerMutex_};
        taskStates_.insert(std::make_pair(info.id, State{info.name, false}));
        return (1 << (taskStates_.size() - 1));
    }();

    initializationBarrier_.sync(event);
}

void TaskMonitor::notify(const bool state) {
    if (const auto it = taskStates_.find(getCurrentTaskId()); it != taskStates_.end()) {
        it->second.ok = state;
    }
}

bool TaskMonitor::ok() const {
    return std::all_of(taskStates_.begin(), taskStates_.end(), [now = getTime()](const auto& entry){
        const auto& state = entry.second.ok;
        return now - state.timestamp() < millisecond_t(50) && state.value();
    });
}

} // namespace micro
