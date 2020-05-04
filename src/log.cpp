#if defined OS_FREERTOS // logging is only supported if FreeRTOS is available

#include <micro/container/vec.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/math/numeric.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/log.hpp>
#include <micro/utils/str_utils.hpp>

#include <cstdarg>
#include <cstring>

namespace micro {

constexpr uint32_t STR_MAX_LEN_INT        = 1 + 10;         // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_DEC  = 1 + 8;          // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_FRAC = 4;              // fraction
constexpr uint32_t STR_MAX_LEN_FLOAT      = 1 + 8 + 1 + 4;  // sign + decimal + '.' + fragment

QueueHandle_t queue = nullptr;
logLevel_t minLevel = logLevel_t::Debug;

void log_init(QueueHandle_t logQueue, const logLevel_t minLogLevel) {
    queue    = logQueue;
    minLevel = minLogLevel;
}

void vprintlog(logLevel_t level, const char *format, va_list args) {

    micro::waitReady(queue);

    if (level >= minLevel)
    {
        char msg[LOG_MSG_MAX_SIZE];
        const char *levelStr = to_string(level);
        uint32_t len = strlen(levelStr);
        strncpy(msg, levelStr, len);
        len += vsprint(&msg[len], LOG_MSG_MAX_SIZE - len, format, args);
        if (len < LOG_MSG_MAX_SIZE - 3) {
            msg[len++] = '$';
            msg[len++] = '\r';
            msg[len++] = '\n';
            msg[len++] = '\0';
        }
        xQueueSend(queue, msg, 1);
    }
}

void printlog(logLevel_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintlog(level, format, args);
    va_end(args);
}

} // namespace micro

#endif // OS_FREERTOS
