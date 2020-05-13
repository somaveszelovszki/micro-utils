#pragma once

#include <micro/utils/types.hpp>
#include <micro/utils/point2.hpp>
#include <micro/utils/CarProps.hpp>
#include <micro/utils/str_utils.hpp>

#include <cstring>

namespace micro {

typedef uint32_t (*serialize_func)(char * const, const uint32_t size, const void * const);
typedef uint32_t (*deserialize_func)(const char * const, void * const);

template <typename T>
inline typename std::enable_if<std::is_same<T, bool>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    uint32_t result = 0;
    if (value) {
        strncpy(stream, "true", size);
        result = 4;
    } else {
        strncpy(stream, "false", size);
        result = 5;
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, bool>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
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

template <typename T>
inline typename std::enable_if<std::is_same<T, int8_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int8_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int8_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<int8_t*>(value) = static_cast<int8_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int16_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int16_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int16_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<int16_t*>(value) = static_cast<int16_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int32_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(*static_cast<const int32_t*>(value), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int32_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<int32_t*>(value) = n;
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int64_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int64_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int64_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<int64_t*>(value) = static_cast<int64_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint8_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint8_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint8_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<uint8_t*>(value) = static_cast<uint8_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint16_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint16_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint16_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<uint16_t*>(value) = static_cast<uint16_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint32_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint32_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint32_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<uint32_t*>(value) = static_cast<uint32_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint64_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint64_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint64_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atoi(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<uint64_t*>(value) = static_cast<uint64_t>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
}

template <typename T>
inline typename std::enable_if<std::is_same<T, float>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::ftoa(*static_cast<const float*>(value), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, float>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    float n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atof(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<float*>(value) = n;
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, double>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::ftoa(static_cast<float>(*static_cast<const double*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, double>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    float n;
    uint32_t idx = 0;
    idx++; // '"'
    idx += micro::atof(&stream[idx], &n);
    if (idx > 1) {
        *static_cast<double*>(value) = static_cast<double>(n);
        idx++; // '"'
    } else {
        idx = 0;
    }
    return idx;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, CarProps>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    const CarProps * const car = static_cast<const CarProps*>(value);
    return sprint(stream, size,
        "{\"pose\":{\"pos_m\":{\"X\":%f,\"Y\":%f},\"angle_deg\":%f},\"speed_mps\":%f}",
        car->pose.pos.X.get(),
        car->pose.pos.Y.get(),
        static_cast<degree_t>(car->pose.angle).get(),
        car->speed.get()
    );
}

template <typename T>
inline typename std::enable_if<std::is_same<T, CarProps>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    CarProps * const car = static_cast<CarProps*>(value);
    float n;

    uint32_t idx = strlen("{\"pose\":{\"pos_m\":{\"X\":");
    idx += micro::atof(&stream[idx], &n);
    car->pose.pos.X = meter_t(n);

    idx += strlen(",\"Y\":");
    idx += micro::atof(&stream[idx], &n);
    car->pose.pos.Y = meter_t(n);

    idx += strlen("},\"angle_deg\":");
    idx += micro::atof(&stream[idx], &n);
    car->pose.angle = degree_t(n);

    idx += strlen("},\"speed_mps\":");
    idx += micro::atof(&stream[idx], &n);
    car->speed = m_per_sec_t(n);

    idx += strlen("}");

    return idx;
}

template <typename T>
inline typename std::enable_if<is_unit<T>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return serialize<typename T::value_type>(stream, size, value);
}

template <typename T>
inline typename std::enable_if<is_unit<T>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    return deserialize<typename T::value_type>(stream, value);
}

} // namespace micro
