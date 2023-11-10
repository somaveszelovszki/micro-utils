#pragma once

#include <algorithm>
#include <iterator>
#include <optional>

#include <etl/char_traits.h>
#include <tiny-json.h>

namespace micro {

namespace detail {
class JSONValueConstIterator;
} // namespace detail

class JSONValue {
    friend class detail::JSONValueConstIterator;

public:
    using const_iterator = detail::JSONValueConstIterator;

    explicit JSONValue(const json_t* delegate) : delegate_{delegate} {}

    bool exists() const;
    const char* key() const;
    bool isObject() const;
    bool isArray() const;

    template <typename T>
    bool is() const {
        if (!delegate_) {
            return false;
        }

        const auto type = json_getType(delegate_);

        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return type == JSON_NULL;
        } else if constexpr (std::is_same_v<T, bool>) {
            return type == JSON_BOOLEAN;
        } else if constexpr (std::is_integral_v<T>) {
            return type == JSON_INTEGER;
        } else if constexpr (std::is_floating_point_v<T>) {
            return type == JSON_INTEGER || type == JSON_REAL;
        }

        return false;
    }

    template <typename T>
    std::optional<T> as() const {
        if (!is<T>()) {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return nullptr;
        } else if constexpr (std::is_same_v<T, bool>) {
            return json_getBoolean(delegate_);
        } else if constexpr (std::is_integral_v<T>) {
            return T(json_getInteger(delegate_));
        } else if constexpr (std::is_floating_point_v<T>) {
            return T(json_getReal(delegate_));
        } else if constexpr (std::is_same_v<T, JSONValue>) {
            return JSONValue(json_getChild(delegate_));
        }

        return std::nullopt;
    }

    JSONValue operator[](const size_t index) const;
    JSONValue operator[](const char * const key) const;

    template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, size_t>>* = nullptr>
    JSONValue operator[](const T index) const {
        return (*this)[static_cast<size_t>(index)];
    }


    const_iterator begin() const;
    const_iterator end() const;

private:
    const json_t* delegate_{nullptr};
};

namespace detail {
class JSONValueConstIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = JSONValue;
    using pointer           = const JSONValue*;
    using reference         = const JSONValue&;

    explicit JSONValueConstIterator(JSONValue value) : value_{std::move(value)} {}

    bool operator==(const JSONValueConstIterator& other) const {
        return value_.delegate_ == other.value_.delegate_;
    }

    bool operator!=(const JSONValueConstIterator& other) const {
        return !(*this == other);
    }

    reference operator*() const { return value_; }
    pointer operator->() { return &value_; }

    JSONValueConstIterator& operator++() {
        value_ = JSONValue(json_getSibling(value_.delegate_));
        return *this;
    }  

    JSONValueConstIterator operator++(int) {
        const auto tmp = *this;
        ++(*this);
        return tmp;
    }

private:
    JSONValue value_;
};

} // namesapce detail

class JSONParser {
public:
    explicit JSONParser(char * const str, const size_t length);
    explicit JSONParser(char * const str);

    JSONValue root() const { return root_; }

private:
    json_t pool_[512];
    JSONValue root_;
};

} // namespace micro
