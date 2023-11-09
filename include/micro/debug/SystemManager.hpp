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
    typedef uint8_t programState_t;

    static SystemManager& instance();

    programState_t programState() const;

    void setProgramState(const programState_t programState);

    void registerTask();

    void notify(const bool state);

    bool ok() const;

private:
    SystemManager() : programState_(0) {}

    mutex_t registerMutex_;
    etl::map<taskId_t, state_t<bool>, MAX_NUM_TASKS> taskStates_;
    programState_t programState_;
};

} // namespace micro
