#include <cstring>
#include <micro/hw/LSM6DSO_Gyroscope.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/port/task.hpp>

namespace micro {
namespace hw {

#if defined STM32F4
#define I2C_DEFAULT {nullptr}
#define SPI_DEFAULT {nullptr}
#define GPIO_DEFAULT {nullptr, 0}
#else // !STM32F4
#define I2C_DEFAULT                                                                                \
    {                                                                                              \
    }
#define SPI_DEFAULT                                                                                \
    {                                                                                              \
    }
#define GPIO_DEFAULT                                                                               \
    {                                                                                              \
    }
#endif // !STM32F4

constexpr millisecond_t MAX_COMM_TIMEOUT = millisecond_t(10);

int32_t write(void* handle, uint8_t reg, uint8_t* buf, uint16_t len) {
#if defined STM32F4
    LSM6DSO_Gyroscope::handle_t* gyroHandle = static_cast<LSM6DSO_Gyroscope::handle_t*>(handle);
    if (gyroHandle->i2c.handle) {
        i2c_memoryWrite(gyroHandle->i2c, LSM6DSO_I2C_ADD_L, reg, 1, buf, len);
        gyroHandle->commSemaphore.take(MAX_COMM_TIMEOUT);
    } else if (gyroHandle->spi.handle) {
        gpio_write(gyroHandle->cs, gpioPinState_t::RESET);
        spi_exchange(gyroHandle->spi, &reg, nullptr, 1);
        gyroHandle->commSemaphore.take(MAX_COMM_TIMEOUT);
        spi_exchange(gyroHandle->spi, buf, nullptr, len);
        gyroHandle->commSemaphore.take(MAX_COMM_TIMEOUT);
        gpio_write(gyroHandle->cs, gpioPinState_t::SET);
    }
#endif // STM32F4

    return 0;
}

int32_t read(void* handle, uint8_t reg, uint8_t* buf, uint16_t len) {
#if defined STM32F4
    LSM6DSO_Gyroscope::handle_t* gyroHandle = static_cast<LSM6DSO_Gyroscope::handle_t*>(handle);
    if (gyroHandle->i2c.handle) {
        i2c_memoryRead(gyroHandle->i2c, LSM6DSO_I2C_ADD_L, reg, 1, buf, len);
        gyroHandle->commSemaphore.take(MAX_COMM_TIMEOUT);
    } else if (gyroHandle->spi.handle) {
        reg |= 0x80;
        gpio_write(gyroHandle->cs, gpioPinState_t::RESET);
        os_sleep(millisecond_t(2));
        spi_exchange(gyroHandle->spi, &reg, nullptr, 1);
        gyroHandle->commSemaphore.take(MAX_COMM_TIMEOUT);
        spi_exchange(gyroHandle->spi, nullptr, buf, len);
        gyroHandle->commSemaphore.take(MAX_COMM_TIMEOUT);
        gpio_write(gyroHandle->cs, gpioPinState_t::SET);
    }
#endif // STM32F4

    return 0;
}

LSM6DSO_Gyroscope::LSM6DSO_Gyroscope(const i2c_t& i2c)
    : LSM6DSO_Gyroscope(i2c, SPI_DEFAULT, GPIO_DEFAULT) {
}

LSM6DSO_Gyroscope::LSM6DSO_Gyroscope(const spi_t& spi, const gpio_t& cs)
    : LSM6DSO_Gyroscope(I2C_DEFAULT, spi, cs) {
}

LSM6DSO_Gyroscope::LSM6DSO_Gyroscope(const i2c_t& i2c, const spi_t& spi, const gpio_t& cs)
    : handle_{i2c, spi, cs}, dev_ctx_{write, read, &this->handle_} {
}

Status LSM6DSO_Gyroscope::initialize() {
    Status status = Status::ERROR;
    uint8_t whoAmI, rst;

    os_sleep(millisecond_t(10));

    lsm6dso_device_id_get(&this->dev_ctx_, &whoAmI);
    if (whoAmI == LSM6DSO_ID) {
        /* Restore default configuration */
        lsm6dso_reset_set(&this->dev_ctx_, PROPERTY_ENABLE);
        do {
            lsm6dso_reset_get(&this->dev_ctx_, &rst);
        } while (rst);

        lsm6dso_spi_mode_set(&this->dev_ctx_, LSM6DSO_SPI_4_WIRE);
        lsm6dso_i3c_disable_set(&this->dev_ctx_, LSM6DSO_I3C_DISABLE);

        lsm6dso_auto_increment_set(&this->dev_ctx_, PROPERTY_ENABLE);
        lsm6dso_block_data_update_set(&this->dev_ctx_, PROPERTY_DISABLE);
        // lsm6dso_fifo_mode_set(&this->dev_ctx_, LSM6DSO_BYPASS_MODE);

        lsm6dso_xl_data_rate_set(&this->dev_ctx_, LSM6DSO_XL_ODR_OFF);

        lsm6dso_gy_power_mode_set(&this->dev_ctx_, LSM6DSO_GY_HIGH_PERFORMANCE);
        lsm6dso_gy_data_rate_set(&this->dev_ctx_, LSM6DSO_GY_ODR_6667Hz);
        lsm6dso_gy_full_scale_set(&this->dev_ctx_, LSM6DSO_2000dps);

        lsm6dso_xl_hp_path_on_out_set(&this->dev_ctx_, LSM6DSO_LP_ODR_DIV_100);
        lsm6dso_xl_filter_lp2_set(&this->dev_ctx_, PROPERTY_ENABLE);

        status = Status::OK;
    } else {
        status = Status::INVALID_ID;
    }
    return status;
}

point3<rad_per_sec_t> LSM6DSO_Gyroscope::readGyroData() {
    point3<rad_per_sec_t> result;

    uint8_t reg;
    lsm6dso_gy_flag_data_ready_get(&this->dev_ctx_, &reg);
    if (reg) {
        union {
            int16_t i16bit[3];
            uint8_t u8bit[6];
        } data;

        memset(data.u8bit, 0x05, 3 * sizeof(int16_t));
        lsm6dso_angular_rate_raw_get(&this->dev_ctx_, data.u8bit);

        result.X = deg_per_sec_t(lsm6dso_from_fs2000_to_mdps(data.i16bit[0]) / 1000.0f);
        result.Y = deg_per_sec_t(lsm6dso_from_fs2000_to_mdps(data.i16bit[1]) / 1000.0f);
        result.Z = deg_per_sec_t(lsm6dso_from_fs2000_to_mdps(data.i16bit[2]) / 1000.0f);

    } else {
        result.X = result.Y = result.Z = micro::numeric_limits<rad_per_sec_t>::infinity();
    }
    return result;
}

void LSM6DSO_Gyroscope::onCommFinished() {
    this->handle_.commSemaphore.give();
}

} // namespace hw
} // namespace micro
