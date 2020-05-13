#pragma once

#include <micro/container/vec.hpp>

#include "serialize.hpp"

#define STR_MAX_LEN_PARAM_NAME 32u     // Maximum length of global parameter name.
#define MAX_NUM_GLOBAL_PARAMS  128u    // Maximum number of global parameters, that can be set from monitoring app.

namespace micro {

struct Param {

    Param();

    Param(const char *name, uint8_t *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize);

    Param(const Param& other) = default;

    Param& operator=(const Param& other) = default;

    char name[STR_MAX_LEN_PARAM_NAME];
    uint8_t *buf;
    uint8_t size;
    serialize_func serialize;
    deserialize_func deserialize;
};

class Params {
public:
    template <typename T>
    void registerParam(const char *name, T& value) {
        this->values.push_back(Param(
            name,
            reinterpret_cast<uint8_t*>(&value),
            sizeof(T),
            micro::serialize<T>,
            micro::deserialize<T>
        ));
    }

    uint32_t serializeAll(char * const str, uint32_t size);
    uint32_t deserializeAll(const char * const str, uint32_t size);

private:
    static void skipWhiteSpaces(const char * const str, uint32_t& idx);

    Param* get(const char *name);

    vec<Param, MAX_NUM_GLOBAL_PARAMS> values;
};

} // namespace micro
