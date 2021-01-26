#pragma once

#include <cstdint>
#include <type_traits>

namespace micro {

#define OUT

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

template <typename T, typename std::enable_if<std::is_enum<T>::value, void>::type* = nullptr>
typename std::underlying_type<T>::type enum_cast(const T& value) {
    return static_cast<typename std::underlying_type<T>::type>(value);
}

/**
 * @brief Status for operations
 */
enum class Status : uint32_t {
    OK = 0,         // OK.
    ERROR,          // Unknown error.
    BUSY,           // Resource busy.
    TIMEOUT,        // Waiting has reached timeout.
    INVALID_ID,     // Invalid identifier detected.
    INVALID_DATA,   // Invalid data detected.
    NO_NEW_DATA,    // No new data is available.
    BUFFER_FULL,    // Buffer is full.
    BUFFER_EMPTY    // Buffer is empty.
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
enum class Direction : int8_t {
    LEFT   = 1,
    CENTER = 0,
    RIGHT  = -1
};

const char* to_string(const Direction& dir);

inline Direction operator-(Direction dir) {
    return static_cast<Direction>(-static_cast<int8_t>(dir));
}

/**
 * @brief Defines bit order.
 */
enum class BitOrder : uint8_t {
	ENDIAN_LITTLE = 0,
	ENDIAN_BIG
};

/**
 * @brief Defines sign of a number;
 */
enum class Sign : int8_t {
    POSITIVE = 1,
    NEUTRAL  = 0,
    NEGATIVE = -1
};

inline Sign operator-(Sign sign) {
    return static_cast<Sign>(-static_cast<int8_t>(sign));
}

template <typename T>
inline T operator*(const T& value, Sign sign) {
    return value * static_cast<int8_t>(sign);
}

template <typename T>
inline T operator*(Sign sign, const T& value) {
    return value * sign;
}

const char* to_string(const Sign& sign);

template <typename Iter>
auto to_raw_pointer(const Iter& it) -> decltype(&*it) {
    return &*it;
}

} // namespace micro
