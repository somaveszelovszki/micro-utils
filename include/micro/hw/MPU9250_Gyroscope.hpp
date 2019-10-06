#pragma once

#include <micro/bsp/i2c.hpp>
#include <micro/bsp/tim.hpp>
#include <micro/utils/point3.hpp>


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

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

class MPU9250 {
public:

    MPU9250(I2C_HandleTypeDef *hi2c, Ascale aScale, Gscale gScale, Mscale mScale, uint8_t Mmode)
        : hi2c(hi2c)
        , aScale(aScale)
        , gScale(gScale)
        , mScale(mScale)
        , aRes(getAres(aScale))
        , gRes(getGres(gScale))
        , mRes(getMres(mScale))
        , Mmode(Mmode)
        , magBias({ -150.0f, 225.0f, 0.0f }) {

    }

    point3<m_per_sec2_t> readAccelData(void);
    point3<rad_per_sec_t> readGyroData(void);
    point3<gauss_t> readMagData(void);
    celsius_t readTempData(void);

    void initialize(void);

private:
    void writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
    char readByte(uint8_t address, uint8_t subAddress);
    void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest);

    float getMres(Mscale scale);
    float getGres(Gscale scale);
    float getAres(Ascale scale);

    void reset(void);
    void initMPU9250(void);
    void initAK8963(void);

    void calibrate(void);

    I2C_HandleTypeDef *hi2c;
    const Ascale aScale;
    const Gscale gScale;
    const Mscale mScale;
    const float aRes;
    const float gRes;
    const float mRes;
    const uint8_t Mmode;

    float magCalibration[3];
    float gyroBias[3];
    float accelBias[3];
    float magBias[3];
};

} // namespace hw
} // namespace micro
