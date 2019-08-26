#pragma once

#include <micro/utils/types.hpp>

namespace micro {

struct gpio_handle_t { void *ptr; };   // GPIO handle type - underlying instance type is bsp library dependent.
struct gpio_pin_t { uint16_t value; }; // GPIO pin identifier type.

/* @brief Structure storing GPIO handle and pin.
 **/
struct gpio_pin_struct {
    gpio_handle_t handle;      // Pointer to the GPIO handle.
    gpio_pin_t pin;            // The GPIO pin.
};

/* @brief Writes GPIO pin.
 * @param gpio Structure containing the GPIO instance and pin.
 * @param pinState Pin state to write.
 **/
void GPIO_WritePin(const gpio_pin_struct& gpio, PinState pinState);

/* @brief Reads GPIO pin.
 * @param gpio Structure containing the GPIO instance and pin.
 * @returns The read pin state.
 **/
PinState GPIO_ReadPin(const gpio_pin_struct& gpio);

/* @brief Toggles GPIO pin.
 * @param gpio Structure containing the GPIO instance and pin.
 **/
void GPIO_TogglePin(const gpio_pin_struct& gpio);
} // namespace micro
