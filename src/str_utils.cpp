#include <micro/utils/str_utils.hpp>
#include <micro/math/numeric.hpp>
#include <micro/utils/arrays.hpp>

#include <cstdarg>
#include <cstring>

namespace micro {

constexpr uint32_t STR_MAX_LEN_INT        = 1 + 10;         // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_DEC  = 1 + 8;          // sign + decimal
constexpr uint32_t STR_MAX_LEN_FLOAT_FRAC = 4;              // fraction
constexpr uint32_t STR_MAX_LEN_FLOAT      = 1 + 8 + 1 + 4;  // sign + decimal + '.' + fragment

uint32_t atoi(const char * const s, int32_t *pResult) {
    uint32_t idx = 0;

    const uint32_t len = strlen(s);

    if (len > 0) {
        bool neg = s[0] == '-';
        if (neg || s[0] == '+') {
            idx = 1;
        }
        *pResult = 0;
        for (; idx < len; ++idx) {
            char c = s[idx];
            if (c < '0' || c > '9') {
                break;
            }
            *pResult *= 10;
            *pResult += static_cast<int32_t>(c - '0');
        }

        if (neg) {
            *pResult *= -1;
        }
    }

    return idx;
}

uint32_t atof(const char * const s, float *pResult) {
    int32_t dec, frac;

    const uint32_t len = strlen(s);

    uint32_t idx = 0;
    bool neg = s[0] == '-';
    if (neg || s[0] == '+') {
        idx = 1;
    }

    idx += atoi(&s[idx], &dec);

    if ('.' == s[idx]) {
        if (++idx < len) {  // idx is incremented because of the dot character before the fraction

            uint32_t fracCount = atoi(&s[idx], &frac);
            if (fracCount > 0) {
                idx += fracCount;
                *pResult = dec + frac / powerOf(10.0f, fracCount);
            } else {
                idx = 0;    // if no fraction has been parsed, string is invalid
            }
        } else {
            idx = 0;    // invalid floating point string
        }
    } else {
        *pResult = static_cast<float>(dec);
    }

    if (neg) {
        *pResult *= -1;
    }

    return idx;
}

uint32_t itoa(int32_t n, char *const s, uint32_t size, uint32_t padding) {
    bool sign;

    if ((sign = n < 0))
        n = -n;

    uint32_t idx = 0;
    do {
        s[idx++] = '0' + (n % 10);
        if (padding) {
            --padding;
        }
    } while ((n /= 10) > 0 && idx < size) ;

    while(padding--) {
        s[idx++] = '0';
    }

    if (idx < size) {
        if (sign) {
            s[idx++] = '-';
        }
    } else if (sign || n != 0) {
        idx = 0;    // buffer full
    }

    s[idx] = '\0';

    reverse(s, idx);
    return idx;
}

uint32_t ftoa(float n, char * const s, uint32_t size, uint32_t padding) {

    (void)size; // TODO

    uint32_t idx = 0;
    uint32_t decLen, fracLen;
    uint32_t sign;       // offset for sign

    if ((sign = (n < 0.0f) ? 1 : 0)) {
        n = -n;
        s[idx++] = '-';
    }

    int32_t dec = static_cast<int32_t>(n);
    int32_t frac = static_cast<int32_t>((n - static_cast<float>(dec)) * powerOf(10, padding));
    if ((decLen = itoa(dec, &s[idx], STR_MAX_LEN_FLOAT_DEC)) > 0) {
        idx += decLen;
        s[idx++] = '.';
        if ((fracLen = itoa(frac, &s[idx], STR_MAX_LEN_FLOAT_FRAC, padding)) > 0) {
            idx += fracLen;
        } else {
            idx = 0;
        }
    } else {
        idx = 0;
    }

    return idx;
}

uint32_t strncpy_until(char * const dest, const char * const src, const uint32_t size, const char delimiter) {
    uint32_t i;
    for (i = 0; i < size; ++i) {
        const char c = src[i];
        if (delimiter == c || '\0' == c) {
            break;
        }
        dest[i] = c;
    }
    if (i < size) dest[i] = '\0';
    return i;
}

uint32_t vsprint(char * const str, const uint32_t size, const char *format, va_list args) {

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
            case 'u':
                w += micro::itoa(va_arg(args, int), str + w, size - w);
                break;

            case 'f':
                w += micro::ftoa(static_cast<float>(va_arg(args, double)), str + w, size - w);
                break;

            default:
                // unsupported printf modifier
                break;
            }
        }

        ++r;
    }

    str[w] = '\0';
    return w;
}

uint32_t sprint(char * const str, const uint32_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const uint32_t result = vsprint(str, size, format, args);
    va_end(args);
    return result;
}

} // namespace micro
