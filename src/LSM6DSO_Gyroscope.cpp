#include <micro/hw/LSM6DSO_Gyroscope.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/port/task.hpp>
#include <cstring>

namespace micro {
namespace hw {

constexpr uint32_t COMM_TIMEOUT_MS = 10;

int32_t write(void *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    LSM6DSO_Gyroscope::handle_t *gyroHandle = static_cast<LSM6DSO_Gyroscope::handle_t*>(handle);
    if (gyroHandle->hi2c)
    {
        HAL_I2C_Mem_Write(gyroHandle->hi2c, LSM6DSO_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, buf, len, COMM_TIMEOUT_MS);
    }
#ifdef STM32F4
    else if (gyroHandle->hspi)
    {
        HAL_GPIO_WritePin(gyroHandle->csGpio, gyroHandle->csGpioPin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(gyroHandle->hspi, &reg, 1, COMM_TIMEOUT_MS);
        HAL_SPI_Transmit(gyroHandle->hspi, buf, len, COMM_TIMEOUT_MS);
        HAL_GPIO_WritePin(gyroHandle->csGpio, gyroHandle->csGpioPin, GPIO_PIN_SET);
    }
#endif // STM32F4
    return 0;
}

int32_t read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    LSM6DSO_Gyroscope::handle_t *gyroHandle = static_cast<LSM6DSO_Gyroscope::handle_t*>(handle);
    if (gyroHandle->hi2c)
    {
        HAL_I2C_Mem_Read(gyroHandle->hi2c, LSM6DSO_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, buf, len, COMM_TIMEOUT_MS);
    }
#ifdef STM32F4
    else if (gyroHandle->hspi)
    {
      reg |= 0x80;
      HAL_GPIO_WritePin(gyroHandle->csGpio, gyroHandle->csGpioPin, GPIO_PIN_RESET);
      os_delay(2);
      HAL_SPI_Transmit(gyroHandle->hspi, &reg, 1, COMM_TIMEOUT_MS);
      HAL_SPI_Receive(gyroHandle->hspi, buf, len, COMM_TIMEOUT_MS);
      HAL_GPIO_WritePin(gyroHandle->csGpio, gyroHandle->csGpioPin, GPIO_PIN_SET);
    }
#endif // STM32F4
  return 0;
}

LSM6DSO_Gyroscope::LSM6DSO_Gyroscope(I2C_HandleTypeDef *hi2c)
    : handle{ hi2c, nullptr, nullptr, 0 }
    , dev_ctx{ write, read, &this->handle } {}

#ifdef STM32F4
LSM6DSO_Gyroscope::LSM6DSO_Gyroscope(SPI_HandleTypeDef *hspi, GPIO_TypeDef* csGpio, uint16_t csGpioPin)
    : handle{ nullptr, hspi, csGpio, csGpioPin }
    , dev_ctx{ write, read, &this->handle } {}
#endif // STM32F4


Status LSM6DSO_Gyroscope::initialize() {

    Status status = Status::ERROR;
    uint8_t whoAmI, rst;

    os_delay(10);

    lsm6dso_device_id_get(&this->dev_ctx, &whoAmI);
    if (whoAmI == LSM6DSO_ID) {

        /* Restore default configuration */
        lsm6dso_reset_set(&this->dev_ctx, PROPERTY_ENABLE);
        do {
          lsm6dso_reset_get(&this->dev_ctx, &rst);
        } while (rst);

        lsm6dso_spi_mode_set(&this->dev_ctx, LSM6DSO_SPI_4_WIRE);
        lsm6dso_i3c_disable_set(&this->dev_ctx, LSM6DSO_I3C_DISABLE);

        lsm6dso_auto_increment_set(&this->dev_ctx, PROPERTY_ENABLE);
        lsm6dso_block_data_update_set(&this->dev_ctx, PROPERTY_DISABLE);
        //lsm6dso_fifo_mode_set(&this->dev_ctx, LSM6DSO_BYPASS_MODE);

        lsm6dso_xl_data_rate_set(&this->dev_ctx, LSM6DSO_XL_ODR_OFF);

        lsm6dso_gy_power_mode_set(&this->dev_ctx, LSM6DSO_GY_HIGH_PERFORMANCE);
        lsm6dso_gy_data_rate_set(&this->dev_ctx, LSM6DSO_GY_ODR_6667Hz);
        lsm6dso_gy_full_scale_set(&this->dev_ctx, LSM6DSO_2000dps);

        lsm6dso_xl_hp_path_on_out_set(&this->dev_ctx, LSM6DSO_LP_ODR_DIV_100);
        lsm6dso_xl_filter_lp2_set(&this->dev_ctx, PROPERTY_ENABLE);

        status = Status::OK;
    } else {
        status = Status::INVALID_ID;
    }
    return status;
}

point3<rad_per_sec_t> LSM6DSO_Gyroscope::readGyroData() {

    point3<rad_per_sec_t> result;

    uint8_t reg;
    lsm6dso_gy_flag_data_ready_get(&dev_ctx, &reg);
    if (reg)
    {
        union {
          int16_t i16bit[3];
          uint8_t u8bit[6];
        } data;

        memset(data.u8bit, 0x05, 3 * sizeof(int16_t));
        lsm6dso_angular_rate_raw_get(&dev_ctx, data.u8bit);

        result.X = deg_per_sec_t(lsm6dso_from_fs2000_to_mdps(data.i16bit[0]) / 1000.0f);
        result.Y = deg_per_sec_t(lsm6dso_from_fs2000_to_mdps(data.i16bit[1]) / 1000.0f);
        result.Z = deg_per_sec_t(lsm6dso_from_fs2000_to_mdps(data.i16bit[2]) / 1000.0f);

    } else {
        result.X = result.Y = result.Z = micro::numeric_limits<rad_per_sec_t>::infinity();
    }
    return result;
}

} // namespace hw
} // namespace micro
