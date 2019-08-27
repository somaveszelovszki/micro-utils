#include <micro/utils/log.hpp>

#if LOG_ENABLED

#include <micro/container/vec.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/convert.hpp>
#include <cfg_os.hpp>

#include <cstdarg>
#include <cstring>

namespace micro {
namespace debug {

constexpr uint32_t STR_MAX_LEN_INT        = 1 + 10;         // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_DEC  = 1 + 8;          // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_FRAC = 4;              // fraction
constexpr uint32_t STR_MAX_LEN_FLOAT      = 1 + 8 + 1 + 4;  // sign + decimal + '.' + fragment

void vprintlog(logLevel_t level, const char *format, va_list args) {
    LogMessage msg;

    char numBuf[micro::max(STR_MAX_LEN_INT, STR_MAX_LEN_FLOAT) + 1];
    uint32_t n = 0; // will store the index of the current character

    if (level >= MIN_LOG_LEVEL)
    {
        msg.level = level;

        while (format[n] != '\0') {

            if (format[n] != '%')
                msg.text.append(format[n]);
            else {
                switch (format[++n]) {
                case 's':
                {
                    const char *str = va_arg(args, char*);
                    msg.text.append(str, str + strlen(str));
                    break;
                }

                case 'c':
                    msg.text.append(static_cast<char>(va_arg(args, int)));
                    break;

                case 'd':
                {
                    const uint32_t len = micro::itoa(va_arg(args, int), numBuf, STR_MAX_LEN_INT);
                    if (len > 0) {
                        msg.text.append(numBuf, numBuf + strlen(numBuf));
                    }
                    break;
                }

                case 'f':
                {
                    const uint32_t len = micro::ftoa(static_cast<float32_t>(va_arg(args, double)), numBuf, STR_MAX_LEN_FLOAT_DEC, STR_MAX_LEN_FLOAT_FRAC);
                    if (len > 0) {
                        msg.text.append(numBuf, numBuf + strlen(numBuf));
                    }
                    break;
                }
                default:
                    // unsupported printf modifier
                    break;
                }
            }

            ++n;
        }

        if (msg.text.append('\0')) {
            micro::queueSend(cfg::queue_Log, &msg);
        }
    }
}

void printlog(logLevel_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintlog(level, format, args);
    va_end(args);
}

} // namespace debug
} // namespace micro

#endif // LOG_ENABLED
