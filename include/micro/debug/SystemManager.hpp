#pragma once

#if defined OS_FREERTOS // system manager is only supported if FreeRTOS is available

#include <micro/container/vec.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/state.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace micro {

class SystemManager {
public:
    struct TaskState {
        TaskStatus_t details;
        state_t<bool> ok;
    };

    struct TaskStateComparator {
        constexpr bool operator()(const TaskState& a, const TaskState& b) const             { return a.details.xHandle < b.details.xHandle; };
        constexpr bool operator()(const TaskState& state, const TaskHandle_t& handle) const { return state.details.xHandle < handle; };
        constexpr bool operator()(const TaskHandle_t& handle, const TaskState& state) const { return handle < state.details.xHandle; };
    };

    typedef uint8_t programState_t;
    typedef sorted_vec<TaskState, 16, TaskStateComparator> TaskStates;

    static SystemManager& instance();

    programState_t programState() const;

    void setProgramState(const programState_t programState);

    void registerTask();

    void notify(const bool state);

    TaskStates failingTasks() const;

private:
    SystemManager() : programState_(0) {}

    mutable mutex_t mutex_;
    TaskStates taskStates_;
    programState_t programState_;
};

} // namespace micro

#endif // OS_FREERTOS
