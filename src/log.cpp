#if defined OS_FREERTOS // logging is only supported if FreeRTOS is available

#include <micro/container/vec.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/math/numeric.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/log.hpp>
#include <micro/utils/str_utils.hpp>

#include <cstdarg>

namespace micro {

constexpr uint32_t STR_MAX_LEN_INT        = 1 + 10;         // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_DEC  = 1 + 8;          // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_FRAC = 4;              // fraction
constexpr uint32_t STR_MAX_LEN_FLOAT      = 1 + 8 + 1 + 4;  // sign + decimal + '.' + fragment

Log Log::instance() {
    static Log instance_;
    return instance_;
}

void Log::setMinLevel(const level_t minLevel) {
    this->minLevel_ = minLevel;
}

void Log::vprint(level_t level, const char *format, va_list args) {
    if (level >= this->minLevel_)
    {
        char msg[LOG_MSG_MAX_SIZE];
        const char *levelStr = to_string(level);
        uint32_t len = strncpy_until(msg, levelStr, strlen(levelStr));
        len += vsprint(&msg[len], LOG_MSG_MAX_SIZE - len - 3, format, args);
        len += strncpy_until(&msg[len], LOG_SEPARATOR_SEQ, ARRAY_SIZE(LOG_SEPARATOR_SEQ));
        this->queue_.send(msg, millisecond_t(0));
    }
}

void Log::print(level_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    this->vprint(level, format, args);
    va_end(args);
}

bool Log::receive(message_t& msg) {
    return this->queue_.receive(msg, millisecond_t(0));
}

const char* to_string(const Log::level_t& level) {
    const char *result = nullptr;
    switch (level) {
        case Log::level_t::Debug:   result = "[D]"; break;
        case Log::level_t::Info:    result = "[I]"; break;
        case Log::level_t::Warning: result = "[W]"; break;
        case Log::level_t::Error:   result = "[E]"; break;
        default:                    result = "[?]"; break;
    }
    return result;
}

} // namespace micro

#endif // OS_FREERTOS
