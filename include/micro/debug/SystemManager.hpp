#pragma once

#include <etl/map.h>
#include <etl/vector.h>

#include <micro/port/mutex.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/state.hpp>

namespace micro {

class SystemManager {
    static constexpr size_t MAX_NUM_TASKS = 16;
public:
    struct TaskState {
        TaskInfo info;
        state_t<bool> ok;
    };

    typedef uint8_t programState_t;

    using TaskStates = etl::vector<TaskState, MAX_NUM_TASKS>;

    static SystemManager& instance();

    programState_t programState() const;

    void setProgramState(const programState_t programState);

    void registerTask();

    void notify(const bool state);

    TaskStates failingTasks() const;

private:
    SystemManager() : programState_(0) {}

    mutable mutex_t mutex_;
    etl::map<taskId_t, TaskState, MAX_NUM_TASKS> taskStates_;
    programState_t programState_;
};

} // namespace micro
