#pragma once

#include <etl/string.h>

#include <micro/container/map.hpp>
#include <micro/port/mutex.hpp>
#include <micro/port/sync_barrier.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/state.hpp>

namespace micro {

class TaskMonitor {
public:
    explicit TaskMonitor(const size_t numTasks)
        : initializationBarrier_{static_cast<uint32_t>((1 << numTasks) - 1)}
    {}

    void registerInitializedTask();
    void notify(const bool ok);
    bool ok() const;

private:
    struct State {
        etl::string<64> name;
        state_t<bool> ok;
    };

    mutex_t registerMutex_;
    sync_barrier_t initializationBarrier_;
    micro::map<taskId_t, State, 16> taskStates_;
};

} // namespace micro
