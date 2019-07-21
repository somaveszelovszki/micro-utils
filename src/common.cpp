#include <micro/task/common.hpp>

namespace micro {
namespace task {


bool error_ = false;    // Indicates if an error has occurred.

void setErrorFlag() {
    error_ = true;
}

bool hasErrorHappened() {
    return error_;
}

} // namespace task
} // namespace micro
