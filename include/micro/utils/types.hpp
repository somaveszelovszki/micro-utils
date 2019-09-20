#pragma once

#include "types.h"
#include "typeinfo.hpp"

#include <type_traits>

namespace micro {

DEFINE_TYPEINFO(bool);

DEFINE_TYPEINFO(uint8_t);
DEFINE_TYPEINFO(uint16_t);
DEFINE_TYPEINFO(uint32_t);
DEFINE_TYPEINFO(uint64_t);

DEFINE_TYPEINFO(int8_t);
DEFINE_TYPEINFO(int16_t);
DEFINE_TYPEINFO(int32_t);
DEFINE_TYPEINFO(int64_t);

DEFINE_TYPEINFO(float);
DEFINE_TYPEINFO(double);

/**
 * @brief Defines pin states.
 */
enum class PinState : uint8_t {
    RESET = 0,      // Reset state.
    SET             // Set state.
};
DEFINE_TYPEINFO(PinState);

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
    BUFFER_FULL     // Buffer is full.
};
DEFINE_TYPEINFO(Status);

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
const char* getStatusString(Status status);

/** @brief Demangles type name.
 * @param in The mangled type name.
 * @param out The result - the demangled type name.
 * @param size The size of the output buffer.
 */
void demangle(const char *in, char *out, uint32_t size);

typedef uint8_t logLevel_t;
#define LogLevel_Debug   0x01
#define LogLevel_Info    0x02
#define LogLevel_Warning 0x03
#define LogLevel_Error   0x04

/**
 * @brief Defines rotation direction.
 */
enum class Direction : int8_t {
    LEFT  = 1,
    CENTER = 0,
    RIGHT = -1
};
DEFINE_TYPEINFO(Direction);

/**
 * @brief Defines bit order.
 */
enum class BitOrder : uint8_t {
	ENDIAN_LITTLE = 0,
	ENDIAN_BIG
};
DEFINE_TYPEINFO(BitOrder);

/**
 * @brief Defines sign of a number;
 */
enum class Sign : int8_t {
    POSITIVE = 1,
    NEGATIVE = -1
};
DEFINE_TYPEINFO(Sign);

template < template <typename...> class base,typename derived>
struct is_base_of_template_impl
{
    template<typename... Ts>
    static constexpr std::true_type  test(const base<Ts...> *);
    static constexpr std::false_type test(...);
    using type = decltype(test(std::declval<derived*>()));
};

template < template <typename...> class base,typename derived>
using is_base_of_template = typename is_base_of_template_impl<base,derived>::type;

template <typename T>
T getValue(const T& value) {
    return value;
}

} // namespace micro
