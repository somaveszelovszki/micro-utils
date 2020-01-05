#pragma once

#include "bit_array.hpp"

namespace micro {

/* @brief Grid (2-dimensional array) for storing binary values memory-efficiently. Values are stored in 32-bit containers to reduce memory need.
 * @tparam X Number of binary values stored in a row. Should be a multiple of 32 for the best efficiency.
 * @tparam Y Number of rows.
 **/
template <uint32_t X, uint32_t Y>
class bit_grid {
private:
    bit_array<X> data_[Y];   // Array of arrays storing the binary containers.

public:
    /* @brief Default constructor - does not initialize elements.
     **/
    bit_grid() {}

    /* @brief Sets given bit of the grid.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     **/
    void set(uint32_t x, uint32_t y) {
        this->data_[y].set(x);
    }

    /* @brief Resets given bit of the grid.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     **/
    void reset(uint32_t x, uint32_t y) {
        this->data_[y].reset(x);
    }

    /* @brief Toggles given bit of the grid.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     **/
    void toggle(uint32_t x, uint32_t y) {
        this->data_[y].toggle(x);
    }

    /* @brief Reads given bit of the grid.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @returns The given bit of the grid.
     **/
    bool read(uint32_t x, uint32_t y) const {
        return this->data_[y].read(x);
    }

    /* @brief Writes given bit of the grid.
     * @param pos The position.
     * @param bit The bit value to write.
     **/
    void write(uint32_t x, uint32_t y, bool bit) {
        this->data_[y].write(x, bit);
    }

    /* @brief Converts binary grid's given row to byte array.
     * @param y The row to convert.
     * @param The result byte array.
     **/
    void toBytes(uint32_t y, uint8_t result[]) {
        this->data_[y].toBytes(result);
    }

    /* @brief Resets all bits.
     **/
    void reset() {
        for (uint32_t y = 0; y < Y; ++y) {
            this->data_[y].reset();
        }
    }
};
} // namespace micro
