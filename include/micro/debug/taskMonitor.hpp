#pragma once

#if defined OS_FREERTOS // task monitor interface is only supported if FreeRTOS is available

#include <micro/container/vec.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/state.hpp>

namespace micro {

struct taskState_t {
    TaskStatus_t details;
    state_t<bool> ok;
};

struct TaskStateComparator {
    constexpr bool operator()(const taskState_t& a, const taskState_t& b) const { return a.details.xHandle < b.details.xHandle; };
    constexpr bool operator()(const taskState_t& state, const TaskHandle_t& handle) const { return state.details.xHandle < handle; };
    constexpr bool operator()(const TaskHandle_t& handle, const taskState_t& state) const { return handle < state.details.xHandle; };
};

class TaskMonitor {
public:
    typedef sorted_vec<taskState_t, 16, TaskStateComparator> taskStates_t;

    static TaskMonitor& instance();

    void registerTask();

    void notify(const bool state);

    taskStates_t failingTasks() const;

private:
    TaskMonitor() {}

    mutable mutex_t mutex_;
    taskStates_t taskStates_;
};

void taskMonitor_notify(const bool state);

} // namespace micro

#endif // OS_FREERTOS
