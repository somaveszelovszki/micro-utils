#pragma once

#include <micro/utils/types.hpp>

namespace micro {

/* @brief Converts 16-bit signed integer value to bytes.
 * @param value The 16-bit signed integer value to convert to bytes.
 * @param bytes The destination byte array.
 * @param order Defines byte order (ENDIAN_LITTLE or BIG_ENDIAN_).
 **/
void toBytes(int16_t value, uint8_t bytes[], BitOrder order = BitOrder::ENDIAN_LITTLE);

/* @brief Converts 32-bit signed integer value to bytes.
 * @param value The 32-bit signed integer value to convert to bytes.
 * @param bytes The destination byte array.
 * @param order Defines byte order (ENDIAN_LITTLE or BIG_ENDIAN_).
 **/
void toBytes(int32_t value, uint8_t bytes[], BitOrder order = BitOrder::ENDIAN_LITTLE);

/* @brief Converts 32-bit float value to bytes.
 * @param value The 32-bit float value to convert to bytes.
 * @param bytes The destination byte array.
 * @param order Defines byte order (ENDIAN_LITTLE or BIG_ENDIAN_).
 **/
void toBytes(float32_t value, uint8_t bytes[], BitOrder order = BitOrder::ENDIAN_LITTLE);

/* @brief Converts byte array to a 32-bit signed integer value.
 * @param bytes The source byte array.
 * @returns The byte array array as a 32-bit signed integer value.
 * @param order Defines byte order (ENDIAN_LITTLE or BIG_ENDIAN_).
 **/
int16_t toInt16(const uint8_t bytes[], BitOrder order = BitOrder::ENDIAN_LITTLE);

/* @brief Converts byte array to a 32-bit signed integer value.
 * @param bytes The source byte array.
 * @returns The byte array array as a 32-bit signed integer value.
 * @param order Defines byte order (ENDIAN_LITTLE or BIG_ENDIAN_).
 **/
int32_t toInt32(const uint8_t bytes[], BitOrder order = BitOrder::ENDIAN_LITTLE);

/* @brief Converts byte array to a 32-bit float value.
 * @param bytes The source byte array.
 * @returns The byte array array as a 32-bit float value.
 * @param order Defines byte order (ENDIAN_LITTLE or BIG_ENDIAN_).
 **/
float32_t toFloat32(const uint8_t bytes[], BitOrder order = BitOrder::ENDIAN_LITTLE);

} // namespace micro
