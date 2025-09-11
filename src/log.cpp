#include <micro/log/log.hpp>
#include <micro/port/task.hpp>

namespace micro {

const char* to_string(const LogLevel level) {
    switch (level) {
    case LogLevel::Debug:
        return "D";
    case LogLevel::Info:
        return "I";
    case LogLevel::Warning:
        return "W";
    case LogLevel::Error:
        return "E";
    default:
        return "?";
    }
}

Log& Log::instance() {
    static Log instance_;
    return instance_;
}

void Log::setMinLevel(const LogLevel minLevel) {
    minLevel_ = minLevel;
}

bool Log::receive(Message& msg) {
    return queue_.receive(msg, millisecond_t(0));
}

} // namespace micro
