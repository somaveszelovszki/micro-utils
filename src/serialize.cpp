#include <micro/debug/serialize.hpp>
#include <micro/utils/str_utils.hpp>
#include <micro/utils/CarProps.hpp>

#include <cstring>

namespace micro {

uint32_t serialize_bool(char * const stream, const uint32_t size, const void * const value) {
    const uint32_t result = size > 0 ? 1 : 0;
    if (result > 0) {
        stream[0] = *static_cast<const bool*>(value) ? 't' : 'f';
    }
    return result;
}

uint32_t deserialize_bool(const char * const stream, void * const value) {
    const uint32_t result = strlen(stream) > 0 ? 1 : 0;
    if (result > 0) {
        *static_cast<bool*>(value) = stream[0] == 't' ? true : false;
    }
    return result;
}

uint32_t serialize_int8_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int8_t*>(value)), stream, size);
}

uint32_t deserialize_int8_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<int8_t*>(value) = static_cast<int8_t>(n);
    }
    return result;
}

uint32_t serialize_int16_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int16_t*>(value)), stream, size);
}

uint32_t deserialize_int16_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<int16_t*>(value) = static_cast<int16_t>(n);
    }
    return result;
}

uint32_t serialize_int32_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(*static_cast<const int32_t*>(value), stream, size);
}

uint32_t deserialize_int32_t(const char * const stream, void * const value) {
    return micro::atoi(stream, static_cast<int32_t*>(value));
}

uint32_t serialize_int64_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const int64_t*>(value)), stream, size);
}

uint32_t deserialize_int64_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<int64_t*>(value) = static_cast<int64_t>(n);
    }
    return result;
}

uint32_t serialize_uint8_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint8_t*>(value)), stream, size);
}

uint32_t deserialize_uint8_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    *static_cast<uint8_t*>(value) = static_cast<uint8_t>(n);
    return result;
}

uint32_t serialize_uint16_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint16_t*>(value)), stream, size);
}

uint32_t deserialize_uint16_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<uint16_t*>(value) = static_cast<uint16_t>(n);
    }
    return result;
}

uint32_t serialize_uint32_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint32_t*>(value)), stream, size);
}

uint32_t deserialize_uint32_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<uint32_t*>(value) = static_cast<uint32_t>(n);
    }
    return result;
}

uint32_t serialize_uint64_t(char * const stream, const uint32_t size, const void * const value) {
    return micro::itoa(static_cast<int32_t>(*static_cast<const uint64_t*>(value)), stream, size);
}

uint32_t deserialize_uint64_t(const char * const stream, void * const value) {
    int32_t n;
    const uint32_t result = micro::atoi(stream, &n);
    if (result > 0) {
        *static_cast<uint64_t*>(value) = static_cast<uint64_t>(n);
    }
    return result;
}

uint32_t serialize_float(char * const stream, const uint32_t size, const void * const value) {
    return micro::ftoa(*static_cast<const float*>(value), stream, size);
}

uint32_t deserialize_float(const char * const stream, void * const value) {
    return micro::atof(stream, static_cast<float*>(value));
}

uint32_t serialize_double(char * const stream, const uint32_t size, const void * const value) {
    return micro::ftoa(static_cast<float>(*static_cast<const double*>(value)), stream, size);
}

uint32_t deserialize_double(const char * const stream, void * const value) {
    float n;
    const uint32_t result = micro::atof(stream, &n);
    if (result > 0) {
        *static_cast<double*>(value) = static_cast<double>(n);
    }
    return result;
}

uint32_t serialize_CarProps(char * const stream, const uint32_t size, const void * const value) {
    const CarProps * const car = static_cast<const CarProps*>(value);
    return sprint(stream, size,
        "{\"pose\":{\"pos\":{\"X\":%f,\"Y\":%f},\"angle\":%f},\"speed\":%f}",
        car->pose.pos.X.get(),
        car->pose.pos.Y.get(),
        car->pose.angle.get(),
        car->speed.get()
    );
}

uint32_t deserialize_CarProps(const char * const stream, void * const value) {
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

} // namespace micro
