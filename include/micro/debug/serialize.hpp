#pragma once

#include <micro/utils/types.hpp>
#include <micro/utils/point2.hpp>
#include <micro/utils/str_utils.hpp>

#include <cstring>

namespace micro {

typedef uint32_t (*serialize_func)(char * const, const uint32_t size, const void * const);
typedef uint32_t (*deserialize_func)(const char * const, void * const);
typedef bool (*exchange_func)(void * const, const void * const);

template <typename T, typename partial = void> struct Serializer {};

template <typename T>
bool raw_buffer_exchange(void * const obj, const void * new_value) {
    T& obj_ = *static_cast<T*>(obj);
    const T& new_value_ = *static_cast<const T*>(new_value);

    const bool changed = obj_ != new_value_;
    obj_ = new_value_;
    return changed;
}

template <>
struct Serializer<bool> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        uint32_t result = 0;
        if (*static_cast<const bool*>(value)) {
            strncpy(stream, "true", size);
            result = 4;
        } else {
            strncpy(stream, "false", size);
            result = 5;
        }
        return result;
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        uint32_t result = 0;
        if (!strncmp(stream, "true", 4)) {
            *static_cast<bool*>(value) = true;
            result = 4;
        } else if (!strncmp(stream, "false", 5)) {
            *static_cast<bool*>(value) = false;
            result = 5;
        }
        return result;
    }

    static bool exchange(void * const obj, const void * new_value) {
        return raw_buffer_exchange<bool>(obj, new_value);
    }
};

template <>
struct Serializer<char> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        stream[0] = '"';
        memcpy(&stream[1], static_cast<const char*>(value), 1);
        stream[2] = '"';
        stream[3] = '\0';
        return 1;
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        *static_cast<char*>(value) = stream[1];
        return 3;
    }

    static bool exchange(void * const obj, const void * new_value) {
        return raw_buffer_exchange<char>(obj, new_value);
    }
};

template <typename T>
struct Serializer<T, typename std::enable_if<std::is_enum<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        return Serializer<typename std::underlying_type<T>::type>::serialize(stream, size, value);
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        return Serializer<typename std::underlying_type<T>::type>::deserialize(stream, value);
    }

    static bool exchange(void * const obj, const void * new_value) {
        return raw_buffer_exchange<T>(obj, new_value);
    }
};

template <typename T>
struct Serializer<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        const float n = static_cast<float>(*static_cast<const T*>(value));
        return micro::ftoa(n, stream, size);
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        float n;
        uint32_t idx = 0;
        idx++; // '"'
        idx += micro::atof(&stream[idx], &n);
        if (idx > 1) {
            *static_cast<T*>(value) = static_cast<T>(n);
            idx++; // '"'
        } else {
            idx = 0;
        }
        return idx;
    }

    static bool exchange(void * const obj, const void * new_value) {
        return raw_buffer_exchange<T>(obj, new_value);
    }
};

template <typename T>
struct Serializer<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        const float n = static_cast<float>(*static_cast<const T*>(value));
        return Serializer<float>::serialize(stream, size, &n);
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        float n;
        const uint32_t result = Serializer<float>::deserialize(stream, &n);
        *static_cast<T*>(value) = static_cast<T>(micro::round(n));
        return result;
    }

    static bool exchange(void * const obj, const void * new_value) {
        return raw_buffer_exchange<T>(obj, new_value);
    }
};

template <typename T>
struct Serializer<T, typename std::enable_if<is_unit<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        return Serializer<typename T::value_type>::serialize(stream, size, value);
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        return Serializer<typename T::value_type>::deserialize(stream, value);
    }

    static bool exchange(void * const obj, const void * new_value) {
        return raw_buffer_exchange<T>(obj, new_value);
    }
};

} // namespace micro
