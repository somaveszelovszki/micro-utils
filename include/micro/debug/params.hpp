#pragma once

#include <micro/container/vec.hpp>
#include <micro/port/mutex.hpp>
#include <micro/utils/log.hpp>

#include "serialize.hpp"

#include <mutex>

#define STR_MAX_LEN_PARAM_NAME 32u
#define MAX_NUM_GLOBAL_PARAMS  128u
#define MAX_PARAM_SIZE_BYTES   4u

namespace micro {

struct Param {
    Param();

    Param(const char *name, void *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize, exchange_func exchange);

    Param(const Param& other) = default;

    Param& operator=(const Param& other) = default;

    bool checkChange();

    char name[STR_MAX_LEN_PARAM_NAME];
    void *buf;
    uint8_t size;
    uint8_t prev[MAX_PARAM_SIZE_BYTES];
    uint32_t serializedCount;
    serialize_func serialize;
    deserialize_func deserialize;
    exchange_func exchange;
};

struct ParamNameComparator {
    bool operator()(const char * const a, const char * const b) const;
    bool operator()(const Param& a, const Param& b) const;
    bool operator()(const Param& param, const char * const name) const;
    bool operator()(const char * const name, const Param& param) const;
};

class Params {
public:
    explicit Params(const char prefix);

    template <typename T>
    void registerParam(const char *name, T& value) {
        if (sizeof(T) <= MAX_PARAM_SIZE_BYTES) {
            std::lock_guard<mutex_t> lock(this->mutex_);
            this->values_.insert(Param(
                name,
                reinterpret_cast<uint8_t*>(&value),
                sizeof(T),
                micro::Serializer<T>::serialize,
                micro::Serializer<T>::deserialize,
                micro::Serializer<T>::exchange
            ));
        } else {
            LOG_WARN("Cannot register param '%s'. Size is above limit.", name);
        }
    }

    uint32_t serializeAll(char * const str, uint32_t size);
    uint32_t deserializeAll(const char * const str, uint32_t size);

private:
    typedef sorted_vec<Param, MAX_NUM_GLOBAL_PARAMS, ParamNameComparator> values_t;

    static void skipWhiteSpaces(const char * const str, uint32_t& idx);

    static void skipParam(const char * const str, uint32_t& idx);

    mutex_t mutex_;
    char prefix_;
    values_t values_;
};

#define REGISTER_PARAM(params, var) params.registerParam(#var, var)

} // namespace micro
