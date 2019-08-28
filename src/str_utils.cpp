#include <micro/utils/str_utils.hpp>

#include <micro/container/vec.hpp>
#include <micro/container/ring_buffer.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/convert.hpp>
#include <cfg_os.hpp>

#include <cstdarg>
#include <cstring>

namespace micro {

constexpr uint32_t STR_MAX_LEN_INT        = 1 + 10;         // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_DEC  = 1 + 8;          // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_FRAC = 4;              // fraction
constexpr uint32_t STR_MAX_LEN_FLOAT      = 1 + 8 + 1 + 4;  // sign + decimal + '.' + fragment

void vsprint(char * const str, const uint32_t size, const char *format, va_list args) {

    uint32_t r = 0; // will store the index of the current read character
    uint32_t w = 0; // will store the index of the current written character

    while (format[r] != '\0') {

        if (format[r] != '%')
            str[w++] = format[r];
        else {
            switch (format[++r]) {
            case 's':
                strncpy(str + w, va_arg(args, char*), size - w);
                w = strlen(str);
                break;

            case 'c':
                str[w++] = static_cast<char>(va_arg(args, int));
                break;

            case 'd':
                w += micro::itoa(va_arg(args, int), str + w, STR_MAX_LEN_INT);
                break;

            case 'f':
                w += micro::ftoa(static_cast<float32_t>(va_arg(args, double)), str + w, STR_MAX_LEN_FLOAT_DEC, STR_MAX_LEN_FLOAT_FRAC);
                break;

            default:
                // unsupported printf modifier
                break;
            }
        }

        ++r;
    }

    str[w++] = '0';
}

void sprint(char * const str, const uint32_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsprint(str, size, format, args);
    va_end(args);
}

} // namespace micro
