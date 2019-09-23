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
    const uint32_t result = size > 0 ? 1 : 0;
    if (result > 0) {
        stream[0] = *static_cast<const bool*>(value) ? 't' : 'f';
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, bool>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    const uint32_t result = strlen(stream) > 0 ? 1 : 0;
    if (result > 0) {
        *static_cast<bool*>(value) = stream[0] == 't' ? true : false;
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
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<int8_t*>(value) = static_cast<int8_t>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int16_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int16_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int16_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<int16_t*>(value) = static_cast<int16_t>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int32_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(*static_cast<const int32_t*>(value), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int32_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    return micro::atoi(stream, static_cast<int32_t*>(value));
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int64_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int64_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, int64_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<int64_t*>(value) = static_cast<int64_t>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint8_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint8_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint8_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    *static_cast<uint8_t*>(value) = static_cast<uint8_t>(n);
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint16_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint16_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint16_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<uint16_t*>(value) = static_cast<uint16_t>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint32_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint32_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint32_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<uint32_t*>(value) = static_cast<uint32_t>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint64_t>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint64_t*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, uint64_t>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<uint64_t*>(value) = static_cast<uint64_t>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, float>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::ftoa(*static_cast<const float*>(value), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, float>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    return micro::atof(stream, static_cast<float*>(value));
}

template <typename T>
inline typename std::enable_if<std::is_same<T, double>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return micro::ftoa(static_cast<float>(*static_cast<const double*>(value)), stream, size);
}

template <typename T>
inline typename std::enable_if<std::is_same<T, double>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    float n;
    const uint32_t result = micro::atof(stream, &n);
    if (result > 0) {
        *static_cast<double*>(value) = static_cast<double>(n);
    }
    return result;
}

template <typename T>
inline typename std::enable_if<std::is_same<T, CarProps>::value, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    const CarProps * const car = static_cast<const CarProps*>(value);
    return sprint(stream, size,
        "{\"pose\":{\"pos\":{\"X\":%f,\"Y\":%f},\"angle\":%f},\"speed\":%f}",
        car->pose.pos.X.get(),
        car->pose.pos.Y.get(),
        car->pose.angle.get(),
        car->speed.get()
    );
}

template <typename T>
inline typename std::enable_if<std::is_same<T, CarProps>::value, uint32_t>::type deserialize(const char * const stream, void * const value) {
    CarProps * const car = static_cast<CarProps*>(value);
    float n;

    uint32_t idx = strlen("{\"pose\":{\"pos\":{\"X\":");
    idx += micro::atof(&stream[idx], &n);
    car->pose.pos.X = meter_t(n);

    idx += strlen(",\"Y\":");
    idx += micro::atof(&stream[idx], &n);
    car->pose.pos.Y = meter_t(n);

    idx += strlen("},\"angle\":");
    idx += micro::atof(&stream[idx], &n);
    car->pose.angle = radian_t(n);

    idx += strlen("},\"speed\":");
    idx += micro::atof(&stream[idx], &n);
    car->speed = m_per_sec_t(n);

    return idx;
}

template <typename T>
inline typename std::enable_if<T::is_dim_class, uint32_t>::type serialize(char * const stream, const uint32_t size, const void * const value) {
    return serialize<typename T::value_type>(stream, size, value);
}

template <typename T>
inline typename std::enable_if<T::is_dim_class, uint32_t>::type deserialize(const char * const stream, void * const value) {
    return deserialize<typename T::value_type>(stream, value);
}

} // namespace micro
