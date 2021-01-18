#if defined OS_FREERTOS

#include <micro/debug/params.hpp>
#include <micro/utils/log.hpp>
#include <micro/math/numeric.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/algorithm.hpp>
#include <micro/utils/arrays.hpp>
#include <micro/utils/str_utils.hpp>

#include <cstring>

namespace micro {

constexpr char PARAMS_START_SEQ[] = "[P]";

Param::Param() : Param("", false, false, nullptr, 0, nullptr, nullptr) {}

Param::Param(const char *name, const bool broadcast, const bool writable, uint8_t *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize)
    : name("")
    , broadcast(broadcast)
    , writable(writable)
    , buf(buf)
    , size(size)
    , serialize(serialize)
    , deserialize(deserialize) {
    strncpy(const_cast<char*>(this->name), name, STR_MAX_LEN_PARAM_NAME);
}

bool ParamNameComparator::operator()(const char * const a, const char * const b) const {
    return strncmp(a, b, STR_MAX_LEN_PARAM_NAME) < 0;
}

bool ParamNameComparator::operator()(const Param& a, const Param& b) const {
    return (*this)(a.name, b.name);
}

bool ParamNameComparator::operator()(const Param& param, const char * const name) const {
    return (*this)(param.name, name);
}

bool ParamNameComparator::operator()(const char * const name, const Param& param) const {
    return (*this)(name, param.name);
}

Params& Params::instance() {
    static Params instance_;
    return instance_;
}

void Params::serializeAll(char * const str, uint32_t size) {

    std::lock_guard<mutex_t> lock(this->mutex_);

    uint32_t idx = strncpy_until(str, PARAMS_START_SEQ, size);
    str[idx++] = '{';

    for (values_t::const_iterator it = this->values_.begin(); it != this->values_.end(); ++it) {
        if (it->broadcast) {
            str[idx++] = '"';

            idx += strncpy_until(&str[idx], it->name, size - idx);

            str[idx++] = '"';
            str[idx++] = ':';

            idx += it->serialize(&str[idx], size - idx, it->buf);

            if (idx >= size) break;

            if (it != this->values_.back()) {
                str[idx++] = ',';
                if (idx >= size) break;
            }
        }
    }

    str[idx++] = '}';
    idx += strncpy_until(&str[idx], LOG_SEPARATOR_SEQ, size - idx);
}

void Params::deserializeAll(const char * const str, uint32_t size) {

    std::lock_guard<mutex_t> lock(this->mutex_);

    if (!strncmp(str, PARAMS_START_SEQ, strlen(PARAMS_START_SEQ))) {
        const char *msgEnd = std::find(str, str + size, LOG_SEPARATOR_CHAR);
        if (msgEnd != str + size) {
            uint32_t idx = strlen(PARAMS_START_SEQ);
            idx++; // '{'

            uint32_t numParams = 0;
            char name[STR_MAX_LEN_PARAM_NAME];

            while (idx < size) {
                idx = micro::indexOf('"', str, size, idx) + 1;
                idx += strncpy_until(name, &str[idx], min(size - idx, static_cast<uint32_t>(STR_MAX_LEN_PARAM_NAME)), '"');
                idx++; // '"'
                idx++; // ':'

                const values_t::iterator it = std::lower_bound(this->values_.begin(), this->values_.end(), name, ParamNameComparator{});
                if (it == this->values_.end() || strncmp(it->name, name, STR_MAX_LEN_PARAM_NAME) != 0) {
                    LOG_ERROR("Unknown parameter name: '%s'.", name);
                    skipParam(str, idx);
                } else if (it->writable) {
                    idx += it->deserialize(&str[idx], it->buf);
                    ++numParams;
                } else {
                    skipParam(str, idx);
                }

                skipWhiteSpaces(str, idx);

                if ('}' == str[idx]) {
                    idx++; // '$'
                    skipWhiteSpaces(str, idx);
                    break;
                } else {
                    idx++; // ','
                }
            }

            if (numParams >= 2) {
                LOG_INFO("Params updated from server");
            }
        }
    }
}

void Params::skipWhiteSpaces(const char * const str, uint32_t& idx) {
    while('\r' == str[idx] || '\n' == str[idx] || ' ' == str[idx]) { ++idx; }
}

void Params::skipParam(const char * const str, uint32_t& idx) {
    uint32_t depth = 0;
    bool isStr = false;

    do {
        const char c = str[idx];

        if (c == '"') {
            isStr = !isStr;
        }

        if (!isStr) {
            if (depth == 0 && (c == ',' || c == '}')) {
                break;
            } else if (c == '{') {
                ++depth;
            } else if (c == '}') {
                --depth;
            }
        }

        ++idx;
    } while (true);
}

} // namespace micro

#endif // OS_FREERTOS
