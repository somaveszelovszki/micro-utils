#pragma once

#include <micro/port/i2c.hpp>
#include <micro/utils/units.hpp>

namespace micro {
namespace hw {
class VL53L1X_DistanceSensor {
  public:
    VL53L1X_DistanceSensor(const i2c_t& i2c, uint16_t deviceId);

    void initialize();

    meter_t readDistance();

  private:
    enum class distanceMode_t { Unknown = 0, Short = 1, Long = 2 };

    uint16_t readTimingBudgetMs();
    void setTimingBudgetMs(const uint16_t budget);

    distanceMode_t readDistanceMode();
    void setDistanceMode(const distanceMode_t mode);

    bool checkDataReady();
    Status waitComm();
    Status writeByte(const uint16_t reg, const uint8_t txData);
    Status writeWord(const uint16_t reg, const uint16_t txData);
    Status writeDWord(const uint16_t reg, const uint32_t txData);
    Status readByte(const uint16_t reg, uint8_t& rxData);
    Status readWord(const uint16_t reg, uint16_t& rxData);

    Status writeBytes(const uint16_t reg, const uint8_t* txData, const uint8_t size);
    Status readBytes(const uint16_t reg, uint8_t* rxData, const uint8_t size);

    const i2c_t i2c_;
    const uint16_t deviceId_;
};
} // namespace hw
} // namespace micro
