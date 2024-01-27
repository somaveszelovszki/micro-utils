#pragma once

// This format library implements a subset of the functionalities of
// the {fmt} library: https://fmt.dev/latest/index.html
// The reason for reimplementing it is that {fmt} has a very large code footprint,
// and it simply does not fit into the FLASH memory of some microcontrollers.

#include <algorithm>
#include <cctype>
#include <iterator>
#include <type_traits>
#include <utility>

#include <etl/string.h>

#include <micro/math/numeric.hpp>

namespace micro {

template <typename T>
struct buffer_sweeper {
    T *buffer{nullptr};
    size_t capacity{};
    size_t index{};

    T* begin() const { return &buffer[index]; }
    T* end() const { return &buffer[capacity]; }
    size_t size() const { return capacity - index; }
    bool empty() const { return !buffer || size() == 0; }
    void append(const T& value) { buffer[index++] = std::move(value); }
};

template <typename T, typename = void>
struct formatter;

template <typename T>
struct formatter_type : public std::decay<T> {};

template <>
struct formatter_type<const char*> {
    using type = char*;
};

template <typename T>
using formatter_type_t = typename formatter_type<T>::type;

struct format_context {
    buffer_sweeper<char> output;
    buffer_sweeper<const char> formatStr;

    void skip_until_format_block_end() {
        while (!formatStr.empty() && *formatStr.begin() != '}') {
            formatStr.index++;
        }

        if (*formatStr.begin() == '}') {
            formatStr.index++;
        }
    }

    void copy_until_format_block_begin() {
        const auto isSpecial = [](const auto it) { return *it == '{' || *it == '}'; };

        char prevSpecialChar = '\0';
        const auto isBlockBegin = [&prevSpecialChar](const auto it) {
            return *it == '{' && prevSpecialChar != '{' && *std::next(it) != '{';
        };

        while (!formatStr.empty() && !isBlockBegin(formatStr.begin())) {
            const auto it = formatStr.begin();

            if (isSpecial(it)) {
                if (prevSpecialChar != *it) {
                    prevSpecialChar = *it;
                } else {
                    prevSpecialChar = '\0';
                }
            }

            if (!prevSpecialChar) {
                output.append(*formatStr.begin());
            }

            formatStr.index++;
        }

        if (*formatStr.begin() == '{') {
            formatStr.index++;
        }
    }

    template <typename T>
    void format(const T& value) {
        formatter<formatter_type_t<T>>().format(value, *this);
        skip_until_format_block_end();
        copy_until_format_block_begin();
    }
};

template <>
struct formatter<bool> {
    void format(const bool value, format_context& ctx) const {
        if (ctx.output.empty()) {
            return;
        }

        const auto start = ctx.output.begin();
        const auto* text = value ? "true" : "false";
        etl::strncpy(start, text, ctx.output.size());
        ctx.output.index += etl::strlen(text);
    }
};


template <>
struct formatter<char> {
    void format(const char value, format_context& ctx) const {
        if (ctx.output.empty()) {
            return;
        }

        ctx.output.append(value);
    }
};

template <>
struct formatter<char*> {
    void format(const char * const value, format_context& ctx) const {
        for (size_t i = 0; !ctx.output.empty() && value[i] != '\0'; i++) {
            ctx.output.append(value[i]);
        }
    }
};

template <size_t N>
struct formatter<etl::string<N>> {
    void format(const etl::string<N>& value, format_context& ctx) const {
        formatter<char*>().format(value.c_str(), ctx);
    }
};

template <typename T>
struct formatter<T, std::enable_if_t<!std::is_same_v<T, bool> && std::is_integral_v<T>>> {
    void format(const T& value, format_context& ctx) const {
        this->format(value, ctx.output, this->parsePadding(ctx));
    }

    void format(T value, buffer_sweeper<char>& output, std::pair<char, size_t> padding) const {
        if (output.empty()) {
            return;
        }

        if (value < T(0)) {
            output.append('-');
            this->format(-value, output, padding);
            return;
        }

        const auto start = output.begin();

        auto& [paddingChar, paddingCount] = padding;

        do {
            output.append('0' + (value % T(10)));
            if (paddingCount != 0) {
                paddingCount--;
            }
        } while ((value /= T(10)) > T(0) && !output.empty()) ;

        while(paddingCount-- != 0 && !output.empty()) {
            output.append('0');
        }

        std::reverse(start, output.begin());
    }

    std::pair<char, size_t> parsePadding(const format_context& ctx) const {
        // Example: {:4}

        const auto start = ctx.formatStr.begin();

        if (ctx.formatStr.size() < 3
            || *start != ':'
            || !std::isdigit(*std::next(start))) {
            return std::make_pair('0', 0);
        }

        return std::make_pair(
            *std::next(start),
            static_cast<size_t>(*std::next(start, 2) - '0')
        );
    }
};

template <typename T>
struct formatter<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    void format(const T& value, format_context& ctx) const {
        if (ctx.output.empty()) {
            return;
        }

        if (value < T(0)) {
            ctx.output.append('-');
            this->format(-value, ctx);
            return;
        }

        const auto precision = this->parsePrecision(ctx);

        const int32_t dec = static_cast<int32_t>(value);
        const int32_t frac = static_cast<int32_t>(std::lround((value - static_cast<T>(dec)) * micro::pow(10, precision)));

        formatter<int32_t>().format(dec, ctx.output, std::make_pair('_', 0));
        
        if (ctx.output.size() < 1 + precision) {
            return;
        }

        ctx.output.append('.');
        formatter<int32_t>().format(frac, ctx.output, std::make_pair('0', precision));
    }

    size_t parsePrecision(const format_context& ctx) const {
        // Example: {:.4f}

        constexpr size_t kDefaultPrecision = 4;

        const auto start = ctx.formatStr.begin();

        if (ctx.formatStr.size() < 4
            || *start != ':'
            || *std::next(start) != '.'
            || !std::isdigit(*std::next(start, 2))
            || *std::next(start, 3) != 'f') {
            return kDefaultPrecision;
        }

        return static_cast<size_t>(*std::next(start, 2) - '0');
    }
};

template <typename ...Args>
size_t format_to_n(char * const output, const size_t size, const char * const formatStr, Args&&... args) {
    if (size == 0) {
        return 0;
    }

    format_context ctx{{output, size, 0}, {formatStr, etl::strlen(formatStr), 0}};
    ctx.copy_until_format_block_begin();
    (ctx.format(std::forward<Args>(args)), ...);

    ctx.output.index = std::min(ctx.output.index, ctx.output.capacity - 1);
    ctx.output.buffer[ctx.output.index] = '\0';
    return ctx.output.index;
}

} // namespace micro
