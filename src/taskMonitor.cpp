#if defined OS_FREERTOS // task monitor interface is only supported if FreeRTOS is available

#include <micro/debug/taskMonitor.hpp>
#include <micro/utils/log.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <mutex>

extern void *pxCurrentTCB;

namespace micro {

TaskMonitor& TaskMonitor::instance() {
    static TaskMonitor instance_;
    return instance_;
}

void TaskMonitor::registerTask() {
    std::lock_guard<mutex_t> lock(this->mutex_);

    taskState_t state;
    vTaskGetInfo(pxCurrentTCB, &state.details, pdFALSE, eInvalid);
    state.ok = false;
    this->taskStates_.insert(state);
}

void TaskMonitor::notify(const bool state) {
    std::lock_guard<mutex_t> lock(this->mutex_);
    const taskStates_t::iterator it = std::lower_bound(this->taskStates_.begin(), this->taskStates_.end(), pxCurrentTCB, TaskStateComparator{});
    if (it->details.xHandle == pxCurrentTCB) {
        it->ok = state;
    }
}

TaskMonitor::taskStates_t TaskMonitor::failingTasks() const {
    std::lock_guard<mutex_t> lock(this->mutex_);
    taskStates_t result;
    for (const taskState_t& state : this->taskStates_) {
        if (getTime() - state.ok.timestamp() > millisecond_t(50) || !state.ok.value()) {
            result.insert(state);
        }
    }
    return result;
}

} // namespace micro

#endif // OS_FREERTOS
