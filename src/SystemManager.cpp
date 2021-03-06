#if defined OS_FREERTOS // task monitor interface is only supported if FreeRTOS is available

#include <micro/debug/SystemManager.hpp>
#include <micro/utils/log.hpp>

#include <mutex>

extern void *pxCurrentTCB;

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
    std::lock_guard<mutex_t> lock(this->mutex_);

    TaskState state;
    vTaskGetInfo(pxCurrentTCB, &state.details, pdFALSE, eInvalid);
    state.ok = false;
    this->taskStates_.insert(state);
}

void SystemManager::notify(const bool state) {
    std::lock_guard<mutex_t> lock(this->mutex_);
    const TaskStates::iterator it = std::lower_bound(this->taskStates_.begin(), this->taskStates_.end(), pxCurrentTCB, TaskStateComparator{});
    if (it->details.xHandle == pxCurrentTCB) {
        it->ok = state;
    }
}

SystemManager::TaskStates SystemManager::failingTasks() const {
    std::lock_guard<mutex_t> lock(this->mutex_);
    TaskStates result;
    for (const TaskState& state : this->taskStates_) {
        if (getTime() - state.ok.timestamp() > millisecond_t(50) || !state.ok.value()) {
            result.insert(state);
        }
    }
    return result;
}

} // namespace micro

#endif // OS_FREERTOS
