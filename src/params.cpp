#include <micro/debug/params.hpp>
#include <micro/utils/log.hpp>
#include <micro/math/numeric.hpp>
#include <micro/utils/arrays.hpp>
#include <micro/utils/str_utils.hpp>
#include <micro/utils/task.hpp>

#include <cstring>

namespace micro {

static void skipWhiteSpaces(const char * const str, uint32_t& idx) {
    while('\r' == str[idx] || '\n' == str[idx] || ' ' == str[idx]) { ++idx; }
}

Param::Param()
    : name("")
    , buf(nullptr)
    , size(0)
    , serialize(nullptr)
    , deserialize(nullptr) {}

Param::Param(const char *name, uint8_t *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize)
    : name("")
    , buf(buf)
    , size(size)
    , serialize(serialize)
    , deserialize(deserialize) {
    strncpy(const_cast<char*>(this->name), name, STR_MAX_LEN_PARAM_NAME);
}

Param& Param::operator=(const Param& other) {
    memcpy(this, &other, sizeof(Param));
    return *this;
}

uint32_t Params::serializeAll(char * const str, uint32_t size) {
    uint32_t idx = 0;
    str[idx++] = '{';

    for (uint32_t i = 0; i < this->values.size(); ++i) {
        Param& p = this->values[i];

        str[idx++] = '"';

        strncpy(&str[idx], p.name, size - idx);
        idx += strlen(p.name);

        str[idx++] = '"';
        str[idx++] = ':';

        os_taskSuspendAll();
        idx += p.serialize(&str[idx], size - idx, p.buf);
        os_taskResumeAll();

        if (idx >= size) break;

        if (i < this->values.size() - 1) {
            str[idx++] = ',';
            if (idx >= size) break;
        }

        str[idx++] = '\r';
        str[idx++] = '\n';
    }

    str[idx++] = '}';

    return idx;
}

uint32_t Params::deserializeAll(const char * const str, uint32_t size) {

    uint32_t idx = 0;
    idx++; // '{'

    char name[STR_MAX_LEN_PARAM_NAME];

    while (idx < size) {
        idx = micro::indexOf('"', str, size, idx) + 1;
        idx += strcpy_until(name, &str[idx], min(size - idx, static_cast<uint32_t>(STR_MAX_LEN_PARAM_NAME)), '"');
        idx++; // '"'
        idx++; // ':'

        Param * const p = this->get(name);
        if (!p) {
            LOG_ERROR("Unknown parameter name: '%s'.", name);
            break;
        }

        os_taskSuspendAll();
        idx += p->deserialize(&str[idx], p->buf);
        os_taskResumeAll();

        skipWhiteSpaces(str, idx);

        if ('}' == str[idx]) {
            idx++; // '$'
            skipWhiteSpaces(str, idx);
            break;
        } else {
            idx++; // ','
        }
    }

    return idx;
}

Param* Params::get(const char *name) {
    Param *param = nullptr;
    for (Param& p : this->values) {
        if (strcmp(p.name, name) == 0) {
            param = &p;
            break;
        }
    }
    return param;
}

} // namespace micro
