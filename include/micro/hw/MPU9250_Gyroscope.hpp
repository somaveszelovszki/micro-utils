#pragma once

#include <micro/utils/units.hpp>
#include <micro/utils/point3.hpp>

#if defined STM32F0
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_i2c.h>
#elif defined STM32F4
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_i2c.h>
#endif

namespace micro {
namespace hw {

enum class Ascale : uint8_t {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum class Gscale : uint8_t {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

enum class Mscale : uint8_t {
  MFS_14BITS = 0, // 0.6 mG per LSB
  MFS_16BITS      // 0.15 mG per LSB
};

#define MMODE_ODR_8Hz   0x02
#define MMODE_ODR_100Hz 0x06

class MPU9250_Gyroscope {
public:

    MPU9250_Gyroscope(I2C_HandleTypeDef *hi2c, Ascale aScale, Gscale gScale, Mscale mScale, uint8_t Mmode);

#ifdef STM32F4
    MPU9250_Gyroscope(SPI_HandleTypeDef *hspi, GPIO_TypeDef* csGpio, uint16_t csGpioPin, Ascale aScale, Gscale gScale, Mscale mScale, uint8_t Mmode);
#endif

    point3<m_per_sec2_t> readAccelData(void);
    point3<rad_per_sec_t> readGyroData(void);
    point3<gauss_t> readMagData(void);
    celsius_t readTempData(void);

    void initialize(void);

private:
    bool waitComm();

    bool writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
    char readByte(uint8_t address, uint8_t subAddress);
    bool readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest);

    float getMres(Mscale scale);
    float getGres(Gscale scale);
    float getAres(Ascale scale);

    void reset(void);
    void initMPU9250(void);
    void initAK8963(void);

    void calibrate(void);

    void calibrateGyro(void);

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

    handle_t handle;
    const Ascale aScale;
    const Gscale gScale;
    const Mscale mScale;
    const float aRes;
    const float gRes;
    const float mRes;
    const uint8_t Mmode;

    float magCalibration[3];
    float gyroBias[3];
    float gyroThreshold[3];
    float accelBias[3];

    volatile bool isCommActive;
};

} // namespace hw
} // namespace micro
