#pragma once

#include <micro/bsp/tim.hpp>

namespace micro {
namespace hw {

class RotaryEncoder {
public:
    /* @brief Constructor - sets timer handle and counter resolution, initializes previous value.
     * @param _htim The handle for the timer used in encoder mode.
     **/
    RotaryEncoder(const tim_handle_t _htim);

    /* @brief Initializes encoder - sets offset.
     * @returns Status indicating operation success.
     **/
    Status initialize();

    /* @brief Gets position difference from the previous measurement.
     * @returns The position difference from the previous measurement.
     **/
    int32_t getDiff();

private:
    const tim_handle_t htim; // The handle for the timer used in encoder mode.
    int32_t prevPos;         // The counter's previous value - needed to calculate difference.
    int32_t offset;          // The counter's offset.
};
} // namespace hw
} // namespace micro
