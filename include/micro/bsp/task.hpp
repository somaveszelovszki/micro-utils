#pragma once

namespace micro {

struct task_handle_t { void *ptr; }; // Task handle - underlying instance type is OS library dependent.

void taskResume(task_handle_t htask);

void taskResumeAll();

void taskSuspend(task_handle_t htask);

void taskSuspendAll();

void taskDelete(task_handle_t htask);

void taskDeleteCurrent();

} // namespace micro
