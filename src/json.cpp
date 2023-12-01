#include <micro/json/json.hpp>

#include <iterator>

namespace micro {

bool JSONValue::exists() const {
    return !!delegate_;
}

const char* JSONValue::key() const {
    return exists() ? json_getName(delegate_) : nullptr;
}

bool JSONValue::isObject() const {
    return exists() && json_getType(delegate_) == JSON_OBJ;
}

bool JSONValue::isArray() const {
    return exists() && json_getType(delegate_) == JSON_ARRAY;
}

JSONValue JSONValue::operator[](const size_t index) const {
    if (!isArray()) {
        return JSONValue(nullptr);
    }

    size_t i = 0;
    return *std::find_if(begin(), end(), [&i, &index](const auto& child){
        return i++ == index;
    });
}

JSONValue JSONValue::operator[](const char * const key) const {
    if (!isObject()) {
        return JSONValue(nullptr);
    }

    return *std::find_if(begin(), end(), [&key](const auto& child){
        const auto k = child.key();
        return k && !etl::strcmp(k, key);
    });
}

auto JSONValue::begin() const -> const_iterator {
    return const_iterator(JSONValue(json_getChild(delegate_)));
}

auto JSONValue::end() const -> const_iterator {
    return const_iterator(JSONValue(nullptr));
}

size_t JSONValue::size() const {
    return std::distance(begin(), end());
}

bool JSONValue::empty() const {
    return size() == 0;
}

JSONParser::JSONParser(char * const str, const size_t length)
    : root_{json_create(str, pool_, ETL_ARRAY_SIZE(pool_))} {}

JSONParser::JSONParser(char * const str) : JSONParser(str, etl::strlen(str)) {}

} // namespace micro
