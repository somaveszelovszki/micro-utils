#pragma once

#if defined OS_FREERTOS // system manager is only supported if FreeRTOS is available

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

class SystemManager {
public:
    typedef uint8_t programState_t;
    typedef sorted_vec<taskState_t, 16, TaskStateComparator> taskStates_t;

    static SystemManager& instance();

    programState_t programState() const { return this->programState_; }
    void setProgramState(const programState_t programState) { this->programState_ = programState; }

    void registerTask();

    void notify(const bool state);

    taskStates_t failingTasks() const;

private:
    SystemManager() : programState_(0) {}

    mutable mutex_t mutex_;
    taskStates_t taskStates_;
    programState_t programState_;
};

} // namespace micro

#endif // OS_FREERTOS
