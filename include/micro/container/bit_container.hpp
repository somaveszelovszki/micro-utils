#pragma once

#include <micro/utils/types.hpp>

namespace micro {

/* @brief Selects mask for given container size.
 * @tparam B Container bit size.
 **/
template <uint32_t B>
struct container_selector;

/* @brief Selects mask array for bit length of 8 (uint8_t container).
 **/
template <>
struct container_selector<8> {
    typedef uint8_t type;
    static const type masks[8];     // Masks used for reading/writing elements.
};

/* @brief Selects mask array for bit length of 32 (uint32_t container).
 **/
template <>
struct container_selector<32> {
    typedef uint32_t type;
    static const type masks[32];    // Masks used for reading/writing elements.
};

template <uint32_t B>
class bit_container {
public:
    typedef typename container_selector<B>::type type;   // The underlying data type.

private:
    type value_;    // The container value.
    static const type * const masks;    // Masks used for reading/writing elements.

public:
    bit_container() {
        this->reset();
    }

    /* @brief Casts bit container to its container type.
     * @returns The container value.
     **/
    operator type() const {
        return this->value_;
    }

    /* @brief Gets given bit of the container.
     * @param pos The position.
     * @returns The bit value at the given position.
     **/
    bool get(uint32_t pos) const {
        return static_cast<bool>(this->value_ & container_selector<B>::masks[pos]);
    }

    /* @brief Sets given bit of the container.
     * @param pos The position.
     * @param value The bit value to write.
     **/
    void set(uint32_t pos, bool value) {
        if (value) {
            this->value_ |= container_selector<B>::masks[pos];
        } else {
            this->value_ &= ~container_selector<B>::masks[pos];
        }
    }

    /* @brief Resets all bits.
     **/
    void reset() {
        this->value_ = static_cast<type>(0);
    }
};

template <uint32_t B>
const typename bit_container<B>::type * const bit_container<B>::masks = container_selector<B>::masks;

} // namespace micro
