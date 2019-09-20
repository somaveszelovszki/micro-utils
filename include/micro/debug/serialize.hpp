#pragma once

#include <micro/utils/types.hpp>
#include <micro/utils/point2.hpp>
#include <micro/utils/CarProps.hpp>

namespace micro {

typedef uint32_t (*serialize_func)(char * const, const uint32_t size, const void * const);
typedef uint32_t (*deserialize_func)(const char * const, void * const);

struct serializer_t {
    serialize_func serialize;
    deserialize_func deserialize;
};

template <typename T> struct serialization {};

#define SERIALIZATION_STRUCT(type) template <> struct serialization<type> { static constexpr serializer_t serializer = { serialize_ ## type, deserialize_ ## type }; };

uint32_t serialize_bool(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_bool(const char * const stream, void * const value);
SERIALIZATION_STRUCT(bool)

uint32_t serialize_int8_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_int8_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(int8_t)

uint32_t serialize_int16_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_int16_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(int16_t)

uint32_t serialize_int32_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_int32_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(int32_t)

uint32_t serialize_int64_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_int64_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(int64_t)

uint32_t serialize_uint8_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_uint8_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(uint8_t)

uint32_t serialize_uint16_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_uint16_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(uint16_t)

uint32_t serialize_uint32_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_uint32_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(uint32_t)

uint32_t serialize_float(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_float(const char * const stream, void * const value);
SERIALIZATION_STRUCT(float)

uint32_t serialize_double(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_double(const char * const stream, void * const value);
SERIALIZATION_STRUCT(double)

uint32_t serialize_m_per_sec_t(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_m_per_sec_t(const char * const stream, void * const value);
SERIALIZATION_STRUCT(m_per_sec_t)

uint32_t serialize_CarProps(char * const stream, const uint32_t size, const void * const value);
uint32_t deserialize_CarProps(const char * const stream, void * const value);
SERIALIZATION_STRUCT(CarProps)

} // namespace micro
