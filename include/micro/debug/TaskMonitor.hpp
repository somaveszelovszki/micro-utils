#pragma once

#include <micro/container/map.hpp>
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
    sync_barrier_t initializationBarrier_;
    micro::map<taskId_t, state_t<bool>, 16> taskStates_;
};

} // namespace micro
