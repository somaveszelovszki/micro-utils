#include "micro/port/task.hpp"
#include <micro/debug/SystemManager.hpp>
#include <micro/utils/log.hpp>

#include <mutex>

namespace micro {

SystemManager& SystemManager::instance() {
    static SystemManager instance_;
    return instance_;
}

SystemManager::programState_t SystemManager::programState() const {
    return this->programState_;
}

void SystemManager::setProgramState(const programState_t programState) {
    this->programState_ = programState;
}

void SystemManager::registerTask() {
    std::scoped_lock lock{mutex_};
    const auto info = getCurrentTaskInfo();
    this->taskStates_.insert(std::make_pair(info.id, TaskState{info, {false}}));
}

void SystemManager::notify(const bool state) {
    std::scoped_lock lock{mutex_};
    if (const auto it = taskStates_.find(getCurrentTaskId()); it != taskStates_.end()) {
        it->second.ok = state;
    }
}

auto SystemManager::failingTasks() const -> TaskStates {
    std::scoped_lock lock{mutex_};
    TaskStates result;
    for (const auto& [id, state] : taskStates_) {
        if (getTime() - state.ok.timestamp() > millisecond_t(50) || !state.ok.value()) {
            result.push_back(state);
        }
    }
    return result;
}

} // namespace micro
