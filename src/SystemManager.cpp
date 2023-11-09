#include <micro/debug/SystemManager.hpp>

#include <algorithm>
#include <mutex>

#include <micro/log/log.hpp>
#include <micro/port/task.hpp>

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
    {
        std::scoped_lock lock{registerMutex_};
        this->taskStates_.insert(std::make_pair(getCurrentTaskId(), false));
    }
    os_sleep(millisecond_t(200));
}

void SystemManager::notify(const bool state) {
    if (const auto it = taskStates_.find(getCurrentTaskId()); it != taskStates_.end()) {
        it->second = state;
    }
}

bool SystemManager::ok() const {
    return std::all_of(taskStates_.begin(), taskStates_.end(), [now = getTime()](const auto& entry){
        const auto& state = entry.second;
        return now - state.timestamp() < millisecond_t(50) && state.value();
    });
}

} // namespace micro
