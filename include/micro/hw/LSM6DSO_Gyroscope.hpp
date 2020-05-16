#pragma once

#include <micro/hw/lsm6dso_reg.h>
#include <micro/port/hal.h>
#include <micro/utils/units.hpp>
#include <micro/utils/point3.hpp>

#if defined STM32F0
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_i2c.h>
#elif defined STM32F4
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_spi.h>
#endif

namespace micro {
namespace hw {
class LSM6DSO_Gyroscope {

public:
    struct handle_t {
        I2C_HandleTypeDef *hi2c;
#ifdef STM32F4
        SPI_HandleTypeDef *hspi;
#else
        void *hspi;
#endif
        GPIO_TypeDef* csGpio;
        uint16_t csGpioPin;
    };

    LSM6DSO_Gyroscope(I2C_HandleTypeDef *hi2c);

#ifdef STM32F4
    LSM6DSO_Gyroscope(SPI_HandleTypeDef *hspi, GPIO_TypeDef *csGpio, uint16_t csGpioPin);
#endif

    Status initialize();

    point3<rad_per_sec_t> readGyroData();

private:

    handle_t handle;
    stmdev_ctx_t dev_ctx;
};
} // namespace hw
} // namespace micro
