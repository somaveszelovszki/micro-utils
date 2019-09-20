#include <micro/utils/convert.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/arrays.hpp>

#include <cstring>
#include <cstdlib>

namespace micro {

void toBytes(int16_t value, uint8_t bytes[], BitOrder order) {
    if (order == BitOrder::ENDIAN_LITTLE) {
        bytes[0] = static_cast<uint8_t>(value);
        bytes[1] = static_cast<uint8_t>(value >> 8);
    } else {    // ENDIAN_BIG
        bytes[1] = static_cast<uint8_t>(value);
        bytes[0] = static_cast<uint8_t>(value >> 8);
    }
}

void toBytes(int32_t value, uint8_t bytes[], BitOrder order) {
    if (order == BitOrder::ENDIAN_LITTLE) {
        bytes[0] = static_cast<uint8_t>(value);
        bytes[1] = static_cast<uint8_t>(value >> 8);
        bytes[2] = static_cast<uint8_t>(value >> 16);
        bytes[3] = static_cast<uint8_t>(value >> 24);
    } else {    // ENDIAN_BIG
        bytes[3] = static_cast<uint8_t>(value);
        bytes[2] = static_cast<uint8_t>(value >> 8);
        bytes[1] = static_cast<uint8_t>(value >> 16);
        bytes[0] = static_cast<uint8_t>(value >> 24);
    }
}

void toBytes(float value, uint8_t bytes[], BitOrder order) {
    int32_t intVal;
    memcpy(&intVal, &value, 4);
    toBytes(intVal, bytes, order);
}

int16_t toInt16(const uint8_t bytes[], BitOrder order) {
    int16_t result;
    if (order == BitOrder::ENDIAN_LITTLE) {
        result = static_cast<int32_t>(bytes[0])
            | (static_cast<int32_t>(bytes[1]) << 8);
    } else {    // ENDIAN_BIG
        result = static_cast<int32_t>(bytes[1])
            | (static_cast<int32_t>(bytes[0]) << 8);
    }
    return result;
}

int32_t toInt32(const uint8_t bytes[], BitOrder order) {
    int32_t result;
    if (order == BitOrder::ENDIAN_LITTLE) {
        result = static_cast<int32_t>(bytes[0])
            | (static_cast<int32_t>(bytes[1]) << 8)
            | (static_cast<int32_t>(bytes[2]) << 16)
            | (static_cast<int32_t>(bytes[3]) << 24);
    } else {    // ENDIAN_BIG
        result = static_cast<int32_t>(bytes[3])
            | (static_cast<int32_t>(bytes[2]) << 8)
            | (static_cast<int32_t>(bytes[1]) << 16)
            | (static_cast<int32_t>(bytes[0]) << 24);
    }
    return result;
}

float toFloat32(const uint8_t bytes[], BitOrder order) {
    int32_t intVal = toInt32(bytes, order);
    float resultFloatVal;
    memcpy(&resultFloatVal, &intVal, 4);
    return resultFloatVal;
}

} // namespace micro
