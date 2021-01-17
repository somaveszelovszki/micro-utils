#pragma once

#include <micro/port/task.hpp>
#include <micro/utils/types.hpp>
#include <micro/utils/point2.hpp>
#include <micro/utils/CarProps.hpp>
#include <micro/utils/str_utils.hpp>

#include <cstring>

namespace micro {

typedef uint32_t (*serialize_func)(char * const, const uint32_t size, const void * const);
typedef uint32_t (*deserialize_func)(const char * const, void * const);

template <typename T, typename partial = void> struct Serializer {};

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
};

template <>
struct Serializer<char> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        memcpy(stream, static_cast<const char*>(value), 1);
        stream[1] = '\0';
        return 1;
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        *static_cast<char*>(value) = stream[0];
        return 1;
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
};

template <typename T>
struct Serializer<T, typename std::enable_if<std::is_integral<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        return micro::itoa(static_cast<int32_t>(*static_cast<const T*>(value)), stream, size);
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        int32_t n;
        uint32_t idx = 0;
        idx++; // '"'
        idx += micro::atoi(&stream[idx], &n);
        if (idx > 1) {
            *static_cast<T*>(value) = static_cast<T>(n);
            idx++; // '"'
        } else {
            idx = 0;
        }
        return idx;
    }
};

template <typename T>
struct Serializer<T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        return micro::ftoa(*static_cast<const float*>(value), stream, size);
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
};

template <typename T>
struct Serializer<T, typename std::enable_if<is_unit<T>::value>::type> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        return Serializer<typename T::value_type>::serialize(stream, size, value);
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        return Serializer<typename T::value_type>::deserialize(stream, value);
    }
};

template <>
struct Serializer<CarProps> {
    static uint32_t serialize(char * const stream, const uint32_t size, const void * const value) {
        const interruptStatus_t interruptStatus = os_enterCritical();
        const CarProps car = *static_cast<const CarProps*>(value);
        os_exitCritical(interruptStatus);

        return sprint(stream, size, "[%f,%f,%f,%f,%f,%f,%f,%f,%f]",
            car.pose.pos.X.get(),
            car.pose.pos.Y.get(),
            static_cast<degree_t>(car.pose.angle).get(),
            car.speed.get(),
            car.distance.get(),
            car.orientedDistance.get(),
            static_cast<degree_t>(car.frontWheelAngle).get(),
            static_cast<degree_t>(car.rearWheelAngle).get(),
            static_cast<deg_per_sec_t>(car.yawRate).get());
    }

    static uint32_t deserialize(const char * const stream, void * const value) {
        CarProps car;
        float n;

        uint32_t idx = strlen("{\"pose\":{\"pos_m\":{\"X\":");
        idx += micro::atof(&stream[idx], &n);
        car.pose.pos.X = meter_t(n);

        idx += strlen(",\"Y\":");
        idx += micro::atof(&stream[idx], &n);
        car.pose.pos.Y = meter_t(n);

        idx += strlen("},\"angle_deg\":");
        idx += micro::atof(&stream[idx], &n);
        car.pose.angle = degree_t(n);

        idx += strlen("},\"speed_mps\":");
        idx += micro::atof(&stream[idx], &n);
        car.speed = m_per_sec_t(n);

        idx += strlen(",\"distance_m\":");
        idx += micro::atof(&stream[idx], &n);
        car.distance = meter_t(n);

        idx += strlen(",\"orientedDistance_m\":");
        idx += micro::atof(&stream[idx], &n);
        car.orientedDistance = meter_t(n);

        idx += strlen(",\"frontWheelAngle_deg\":");
        idx += micro::atof(&stream[idx], &n);
        car.frontWheelAngle = degree_t(n);

        idx += strlen(",\"rearWheelAngle_deg\":");
        idx += micro::atof(&stream[idx], &n);
        car.rearWheelAngle = degree_t(n);

        idx += strlen(",\"yawRate_degps\":");
        idx += micro::atof(&stream[idx], &n);
        car.yawRate = deg_per_sec_t(n);

        idx += strlen("}");

        const interruptStatus_t interruptStatus = os_enterCritical();
        *static_cast<CarProps*>(value) = car;
        os_exitCritical(interruptStatus);

        return idx;
    }
};

} // namespace micro
