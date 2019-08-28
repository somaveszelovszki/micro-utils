#include <micro/utils/log.hpp>

#if LOG_ENABLED

#include <micro/utils/str_utils.hpp>
#include <micro/container/vec.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/convert.hpp>
#include <micro/bsp/uart.hpp>
#include <micro/bsp/queue.hpp>
#include <cfg_os.hpp>

#include <cstdarg>
#include <cstring>

namespace micro {

constexpr uint32_t STR_MAX_LEN_INT        = 1 + 10;         // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_DEC  = 1 + 8;          // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_FRAC = 4;              // fraction
constexpr uint32_t STR_MAX_LEN_FLOAT      = 1 + 8 + 1 + 4;  // sign + decimal + '.' + fragment

void vprintlog(logLevel_t level, const char *format, va_list args) {
    if (level >= MIN_LOG_LEVEL)
    {
        LogMessage msg;
        msg.level = level;
        vsprint(msg.text, LOG_MSG_MAX_SIZE, format, args);
        micro::queueSend(cfg::queue_Log, &msg);
    }
}

void printlog(logLevel_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintlog(level, format, args);
    va_end(args);
}

} // namespace micro

#endif // LOG_ENABLED
