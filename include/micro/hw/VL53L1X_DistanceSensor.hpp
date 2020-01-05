#pragma once

#include <micro/utils/units.hpp>

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f4xx_hal_i2c.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_i2c.h>
#endif

namespace micro {
namespace hw {
class VL53L1X_DistanceSensor {

public:
    VL53L1X_DistanceSensor(I2C_HandleTypeDef *hi2c, uint16_t deviceId)
        : hi2c(hi2c)
        , deviceId(deviceId) {}

    void initialize();

    /**
     * Reads measured distance.
     * @param distance [out] The measured distance
     * @return Status::OK          if the distance has been read successfully
     *         Status::NO_NEW_DATA if the measurement has not finished yet
     */
    Status readDistance(meter_t& distance);

private:
    I2C_HandleTypeDef *hi2c;
    const uint16_t deviceId;
    millisecond_t lastUpdateTime;
};
} // namespace hw
} // namespace micro
