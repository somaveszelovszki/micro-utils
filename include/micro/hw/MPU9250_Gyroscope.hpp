#pragma once

#ifdef STM32F4

#include <micro/port/gpio.hpp>
#include <micro/port/i2c.hpp>
#include <micro/port/semaphore.hpp>
#include <micro/port/spi.hpp>
#include <micro/utils/point3.hpp>
#include <micro/utils/units.hpp>
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx_hal_spi.h>

namespace micro {
namespace hw {

enum class Ascale : uint8_t { AFS_2G = 0, AFS_4G, AFS_8G, AFS_16G };

enum class Gscale : uint8_t { GFS_250DPS = 0, GFS_500DPS, GFS_1000DPS, GFS_2000DPS };

enum class Mscale : uint8_t {
    MFS_14BITS = 0, // 0.6 mG per LSB
    MFS_16BITS      // 0.15 mG per LSB
};

#define MMODE_ODR_8Hz 0x02
#define MMODE_ODR_100Hz 0x06

#if defined STM32
#define I2C_DEFAULT {nullptr}
#define SPI_DEFAULT {nullptr}
#define GPIO_DEFAULT {nullptr, 0}
#else // !STM32
#define I2C_DEFAULT                                                                                \
    {                                                                                              \
    }
#define SPI_DEFAULT                                                                                \
    {                                                                                              \
    }
#define GPIO_DEFAULT                                                                               \
    {                                                                                              \
    }
#endif // !STM32

class MPU9250_Gyroscope {
  public:
    MPU9250_Gyroscope(const i2c_t& i2c, Ascale aScale, Gscale gScale, Mscale mScale, uint8_t Mmode);

    MPU9250_Gyroscope(const spi_t& spi, const gpio_t& gpio, Ascale aScale, Gscale gScale,
                      Mscale mScale, uint8_t Mmode);

    point3<rad_per_sec_t> gyroMeanError() const { return this->gyroMeanError_; }

    point3<m_per_sec2_t> readAccelData();
    point3<rad_per_sec_t> readGyroData();
    point3<gauss_t> readMagData();
    celsius_t readTempData();

    void initialize();

    void onCommFinished();

  private:
    MPU9250_Gyroscope(const i2c_t& i2c, const spi_t& spi, const gpio_t& cs, Ascale aScale,
                      Gscale gScale, Mscale mScale, uint8_t Mmode);

    bool waitComm();

    bool writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
    char readByte(uint8_t address, uint8_t subAddress);
    bool readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t* dest);

    float getMres(Mscale scale);
    float getGres(Gscale scale);
    float getAres(Ascale scale);

    void reset();
    void initMPU9250();
    void initAK8963();

    void calibrateGyro();

    bool readRawGyroData(point3f& result);

    static point3<int16_t> bufferToRaw(const uint8_t rawData[6]);

    struct handle_t {
        const i2c_t i2c;
        const spi_t spi;
        const gpio_t cs;
        semaphore_t commSemaphore;
    };

    handle_t handle;
    const Ascale aScale;
    const Gscale gScale;
    const Mscale mScale;
    const float aRes;
    const float gRes;
    const float mRes;
    const uint8_t Mmode;

    point3f magCalibration;
    point3f gyroBias;
    point3f gyroThreshold;
    point3f accelBias;
    point3<rad_per_sec_t> gyroMeanError_;
};

} // namespace hw
} // namespace micro

#endif // STM32F4
