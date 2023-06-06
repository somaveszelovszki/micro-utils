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

Param::Param() : Param("", nullptr, 0, nullptr, nullptr, nullptr) {}

Param::Param(const char *name, void *buf, uint8_t size, serialize_func serialize, deserialize_func deserialize, exchange_func exchange)
    : name("")
    , buf(buf)
    , size(size)
    , serializedCount(0u)
    , serialize(serialize)
    , deserialize(deserialize)
    , exchange(exchange) {
    strncpy(const_cast<char*>(this->name), name, STR_MAX_LEN_PARAM_NAME);
    memset(this->prev, 0, MAX_PARAM_SIZE_BYTES);
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

Params::Params(const char prefix) : prefix_(prefix) {}

uint32_t Params::serializeAll(char * const str, uint32_t size) {

    std::lock_guard<mutex_t> lock(this->mutex_);

    uint32_t idx = 0u;
    str[idx++] = this->prefix_;
    str[idx++] = ':';
    str[idx++] = '{';

    uint32_t count = 0u;

    for (values_t::iterator it = this->values_.begin(); it != this->values_.end(); ++it) {
        const bool changed = it->exchange(it->prev, it->buf);
        if (it->serializedCount == 0u || changed) {
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

            ++it->serializedCount;
            ++count;
        }
    }

    str[idx++] = '}';
    idx += strncpy_until(&str[idx], LOG_SEPARATOR_SEQ, size - idx);

    return count;
}

uint32_t Params::deserializeAll(const char * const str, uint32_t size) {

    std::lock_guard<mutex_t> lock(this->mutex_);

    uint32_t count = 0u;

    if (str[0] == this->prefix_) {
        const char *msgEnd = std::find(str, str + size, LOG_SEPARATOR_CHAR);
        if (msgEnd != str + size) {
            uint32_t idx = 3; // skips prefix, colon and opening bracket
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
                } else {
                    idx += it->deserialize(&str[idx], it->buf);
                    ++count;
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
        }
    }

    if (count > 0) {
        LOG_INFO("%u params updated in section '%c'", count, this->prefix_);
    }

    return count;
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
