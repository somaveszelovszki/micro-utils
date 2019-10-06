#pragma once

#include "serialize.hpp"

#include <micro/container/vec.hpp>
#include <micro/utils/atomic.hpp>
#include <micro/utils/typeinfo.hpp>

#include <cstring>

#define STR_MAX_LEN_PARAM_NAME 32u     // Maximum length of global parameter name.
#define STR_MAX_LEN_PARAM_TYPE 32u     // Maximum length of global parameter name.
#define MAX_NUM_GLOBAL_PARAMS  128u    // Maximum number of global parameters, that can be set from monitoring app.

namespace micro {

struct Param {

    Param(const char *name, const char *type, osMutexId hmutex, uint8_t *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize)
        : name("")
        , type("")
        , hmutex(hmutex)
        , buf(buf)
        , size(size)
        , serialize(serialize)
        , deserialize(deserialize) {
        strncpy(const_cast<char*>(this->name), name, STR_MAX_LEN_PARAM_NAME);
        strncpy(const_cast<char*>(this->type), type, STR_MAX_LEN_PARAM_TYPE);
    }

    Param(const Param& other)
        : name("")
        , type("")
        , hmutex(other.hmutex)
        , buf(other.buf)
        , size(other.size)
        , serialize(other.serialize)
        , deserialize(other.deserialize) {
        strncpy(const_cast<char*>(this->name), other.name, STR_MAX_LEN_PARAM_NAME);
        strncpy(const_cast<char*>(this->type), other.type, STR_MAX_LEN_PARAM_TYPE);
    }

    const char name[STR_MAX_LEN_PARAM_NAME];
    const char type[STR_MAX_LEN_PARAM_TYPE];
    osMutexId hmutex;
    uint8_t * const buf;
    const uint8_t size;
    serialize_func serialize;
    deserialize_func deserialize;
};

class Params {

public:

    template <typename T>
    void registerParam(const char *name, T *value) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable.");
        this->values.append(this->fillParamStruct(name, value));
    }

    uint32_t serializeAll(char * const str, uint32_t size);
    uint32_t deserializeAll(const char * const str);

private:

    Param* get(const char *name);

    template <typename T>
    Param fillParamStruct(const char *name, T *value) {
        return Param(
            name,
            micro::typeinfo<T>::name(),
            { nullptr },
            reinterpret_cast<uint8_t*>(value),
            sizeof(T),
            micro::serialize<T>,
            micro::deserialize<T>
        );
    }

    template <typename T>
    Param fillParamStruct(const char *name, atomic<T> *value) {
        T *value_ptr = const_cast<T*>(value->wait_ptr());
        value->release_ptr();
        return Param(
            name,
            micro::typeinfo<T>::name(),
            value->getMutex(),
            reinterpret_cast<uint8_t*>(value_ptr),
            sizeof(T),
            micro::serialize<T>,
            micro::deserialize<T>
        );
    }

    vec<Param, MAX_NUM_GLOBAL_PARAMS> values;
};

} // namespace micro
