#pragma once

#include <micro/hw/lsm6dso_reg.h>
#include <micro/port/gpio.hpp>
#include <micro/port/i2c.hpp>
#include <micro/port/semaphore.hpp>
#include <micro/port/spi.hpp>
#include <micro/utils/point3.hpp>
#include <micro/utils/units.hpp>

namespace micro {
namespace hw {
class LSM6DSO_Gyroscope {
  public:
    struct handle_t {
        const i2c_t i2c;
        const spi_t spi;
        const gpio_t cs;
        semaphore_t commSemaphore;
    };

    explicit LSM6DSO_Gyroscope(const i2c_t& i2c);

    LSM6DSO_Gyroscope(const spi_t& spi, const gpio_t& cs);

    Status initialize();

    point3<rad_per_sec_t> readGyroData();

    void onCommFinished();

  private:
    LSM6DSO_Gyroscope(const i2c_t& i2c, const spi_t& spi, const gpio_t& cs);

    handle_t handle_;
    stmdev_ctx_t dev_ctx_;
};
} // namespace hw
} // namespace micro
