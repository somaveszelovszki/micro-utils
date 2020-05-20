#pragma once

#if defined OS_FREERTOS // params interface is only supported if FreeRTOS is available

#include <micro/container/vec.hpp>
#include <micro/port/mutex.hpp>

#include "serialize.hpp"

#include <mutex>

#define STR_MAX_LEN_PARAM_NAME 32u
#define MAX_NUM_GLOBAL_PARAMS  64u

namespace micro {

struct Param {
    enum class permission_t : uint8_t {
        ReadOnly  = 0,
        ReadWrite = 1
    };

    Param();

    Param(const char *name, const permission_t permission, uint8_t *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize);

    Param(const Param& other) = default;

    Param& operator=(const Param& other) = default;

    char name[STR_MAX_LEN_PARAM_NAME];
    permission_t permission;
    void *buf;
    uint8_t size;
    serialize_func serialize;
    deserialize_func deserialize;
};

struct ParamNameComparator {
    constexpr bool operator()(const Param& a, const Param& b) const;
    constexpr bool operator()(const Param& param, const char * const name) const;
    constexpr bool operator()(const char * const name, const Param& param) const;
};

class Params {
public:
    static Params instance();

    template <typename T>
    void registerParam(const char *name, T& value, const Param::permission_t permission) {
        std::lock_guard<mutex_t> lock(this->mutex_);
        this->values_.insert(Param(
            name,
            permission,
            reinterpret_cast<uint8_t*>(&value),
            sizeof(T),
            micro::serialize<T>,
            micro::deserialize<T>
        ));
    }

    void serializeAll(char * const str, uint32_t size);
    void deserializeAll(const char * const str, uint32_t size);

private:
    typedef sorted_vec<Param, MAX_NUM_GLOBAL_PARAMS, ParamNameComparator> values_t;

    Params() {}

    static void skipWhiteSpaces(const char * const str, uint32_t& idx);

    static void skipParam(const char * const str, uint32_t& idx);

    mutex_t mutex_;
    values_t values_;
};

#define REGISTER_READ_ONLY_PARAM(var)  micro::Params::instance().registerParam(#var, var, Param::permission_t::ReadOnly)
#define REGISTER_READ_WRITE_PARAM(var) micro::Params::instance().registerParam(#var, var, Param::permission_t::ReadWrite)

} // namespace micro

#endif // OS_FREERTOS
