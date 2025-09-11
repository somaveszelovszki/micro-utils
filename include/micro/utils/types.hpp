#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

namespace micro {

#define OUT

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

template <typename T, typename partial = void> struct underlying_type {};

template <typename T>
struct underlying_type<T, typename std::enable_if<std::is_arithmetic<T>::value, void>::type> {
    using type = T;
    static constexpr type value(const T& v) { return v; }
    static constexpr const type& ref(const T& v) { return v; }
    static constexpr type& ref(T& v) { return v; }
};

template <typename T>
struct underlying_type<T, typename std::enable_if<std::is_enum<T>::value, void>::type> {
    using type = std::underlying_type_t<T>;
    static constexpr type value(const T& v) { return static_cast<type>(v); }
    static constexpr const type& ref(const T& v) { return static_cast<const type&>(v); }
    static constexpr type& ref(T& v) { return static_cast<type&>(v); }
};

template <typename T> using underlying_type_t = typename underlying_type<T>::type;

template <typename T> auto underlying_value(const T& value) {
    return underlying_type<T>::value(value);
}

template <typename T> auto&& underlying_ref(T&& value) {
    return underlying_type<std::decay_t<T>>::ref(std::forward<T>(value));
};

template <typename T> struct remove_const_key {};

template <typename K, typename V> struct remove_const_key<std::pair<K, V>> {
    using type = std::pair<K, V>;
};

template <typename K, typename V> struct remove_const_key<std::pair<const K, V>> {
    using type = std::pair<K, V>;
};

template <typename T> using remove_const_key_t = typename remove_const_key<T>::type;

/**
 * @brief Status for operations
 */
enum class Status : uint32_t {
    OK = 0,       // OK.
    ERROR,        // Unknown error.
    BUSY,         // Resource busy.
    TIMEOUT,      // Waiting has reached timeout.
    INVALID_ID,   // Invalid identifier detected.
    INVALID_DATA, // Invalid data detected.
    NO_NEW_DATA,  // No new data is available.
    BUFFER_FULL,  // Buffer is full.
    BUFFER_EMPTY  // Buffer is empty.
};

/**
 * @brief Checks is status is ok.
 * @param status
 * @return True if status is ok.
 */
inline bool isOk(Status status) {
    return status == Status::OK;
}

/** @brief Gets status as string.
 * @param status The status to convert to string.
 * @returns The status as string.
 */
const char* to_string(const Status& status);

/**
 * @brief Defines rotation direction.
 */
enum class Direction : int8_t { LEFT = 1, CENTER = 0, RIGHT = -1 };

const char* to_string(const Direction& dir);

inline Direction operator-(Direction dir) {
    return static_cast<Direction>(-static_cast<int8_t>(dir));
}

/**
 * @brief Defines bit order.
 */
enum class BitOrder : uint8_t { ENDIAN_LITTLE = 0, ENDIAN_BIG };

/**
 * @brief Defines sign of a number;
 */
enum class Sign : int8_t { POSITIVE = 1, NEUTRAL = 0, NEGATIVE = -1 };

inline Sign operator-(Sign sign) {
    return static_cast<Sign>(-static_cast<int8_t>(sign));
}

template <typename T> inline T operator*(const T& value, Sign sign) {
    return value * static_cast<int8_t>(sign);
}

template <typename T> inline T operator*(Sign sign, const T& value) {
    return value * sign;
}

inline Sign operator*(const Sign s1, Sign s2) {
    return static_cast<Sign>(static_cast<int8_t>(s1) * static_cast<int8_t>(s2));
}

inline Direction operator*(const Direction value, Sign sign) {
    return static_cast<Direction>(underlying_value(value) * sign);
}

const char* to_string(const Sign& sign);

template <typename Iter> auto to_raw_pointer(const Iter& it) -> decltype(&*it) {
    return &*it;
}

} // namespace micro
