#pragma once

#include <micro/utils/types.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>
#endif

namespace micro {

#if defined STM32F4

struct can_t {
    CAN_HandleTypeDef *handle;
};

struct canFrame_t {
    typedef uint16_t id_t;
    union {
        CAN_RxHeaderTypeDef rx;
        CAN_TxHeaderTypeDef tx;
    } header;
    alignas(8) uint8_t data[8];
};

#else // !STM32F4

struct can_t {};
struct canFrame_t {};

#endif // !STM32F4

Status can_transmit(const can_t& can, const canFrame_t& frame);
Status can_receive(const can_t& can, canFrame_t& OUT frame);

} // namespace micro
