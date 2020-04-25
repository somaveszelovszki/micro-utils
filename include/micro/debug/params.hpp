#pragma once

#include <micro/container/vec.hpp>
#include <micro/utils/atomic.hpp>

#include "serialize.hpp"

#include <cstring>

#define STR_MAX_LEN_PARAM_NAME 32u     // Maximum length of global parameter name.
#define MAX_NUM_GLOBAL_PARAMS  128u    // Maximum number of global parameters, that can be set from monitoring app.

namespace micro {

struct Param {

    Param()
        : name("")
        , hmutex(nullptr)
        , buf(nullptr)
        , size(0)
        , serialize(nullptr)
        , deserialize(nullptr) {}

    Param(const char *name, SemaphoreHandle_t hmutex, uint8_t *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize)
        : name("")
        , hmutex(hmutex)
        , buf(buf)
        , size(size)
        , serialize(serialize)
        , deserialize(deserialize) {
        strncpy(const_cast<char*>(this->name), name, STR_MAX_LEN_PARAM_NAME);
    }

    Param(const Param& other) = default;

    Param& operator=(const Param& other) {
        memcpy(this, &other, sizeof(Param));
        return *this;
    }

    const char name[STR_MAX_LEN_PARAM_NAME];
    SemaphoreHandle_t hmutex;
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
        this->values.push_back(this->fillParamStruct(name, value));
    }

    uint32_t serializeAll(char * const str, uint32_t size);
    uint32_t deserializeAll(const char * const str, uint32_t size);

private:

    Param* get(const char *name);

    template <typename T>
    Param fillParamStruct(const char *name, T *value) {
        return Param(
            name,
            nullptr,
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
