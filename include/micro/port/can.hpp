#pragma once

#include <micro/utils/types.hpp>

#if defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_can.h>
#endif

namespace micro {

using canFrameId_t = uint32_t;

#if defined STM32F4

struct can_t {
    CAN_HandleTypeDef *handle = nullptr;
};

using rxHeader_t = CAN_RxHeaderTypeDef;
using txHeader_t = CAN_TxHeaderTypeDef;

#else // !STM32F4

struct can_t {};

struct rxHeader_t {};
struct txHeader_t {};

#endif // !STM32F4

struct canFrame_t {
    union {
        rxHeader_t rx;
        txHeader_t tx;
    } header;
    uint8_t data[8];
};

canFrameId_t can_getId(const canFrame_t& frame);
canFrame_t can_buildFrame(const canFrameId_t id, const uint8_t * const data, const uint32_t size);
Status can_transmit(const can_t& can, const canFrame_t& frame);
Status can_receive(const can_t& can, canFrame_t& OUT frame);

} // namespace micro
