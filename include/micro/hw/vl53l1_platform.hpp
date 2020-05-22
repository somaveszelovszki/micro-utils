#pragma once

#include <micro/port/i2c.hpp>

namespace micro {
namespace hw {

void VL53L1_setI2C(const i2c_t& i2c);

} // namespace hw
} // namespace micro
