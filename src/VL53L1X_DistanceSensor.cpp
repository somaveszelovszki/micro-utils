#include <micro/hw/VL53L1X_DistanceSensor.hpp>
#include <micro/utils/log.hpp>

#include "../Driver/VL53L1X/core/VL53L1X_api.h"

#include <FreeRTOS.h>
#include <task.h>

namespace micro {
namespace hw {

void VL53L1X_DistanceSensor::initialize() {
    uint8_t byteData = 0;
    uint16_t wordData = 0;
    uint8_t sensorState = 0;

    VL53L1_RdByte(this->deviceId, 0x010F, &byteData);
    LOG_DEBUG("VL53L1X Model_ID: %u\n", byteData);
    VL53L1_RdByte(this->deviceId, 0x0110, &byteData);
    LOG_DEBUG("VL53L1X Module_Type: %u\n", byteData);
    VL53L1_RdWord(this->deviceId, 0x010F, &wordData);
    LOG_DEBUG("VL53L1X: %u\n", wordData);

    while(sensorState == 0){
        VL53L1X_BootState(this->deviceId, &sensorState);
        vTaskDelay(2);
    }

    VL53L1X_SensorInit(this->deviceId);
    VL53L1X_SetDistanceMode(this->deviceId, 1);
    VL53L1X_SetTimingBudgetInMs(this->deviceId, 15);
    VL53L1X_SetInterMeasurementInMs(this->deviceId, 15);
    VL53L1X_StartRanging(this->deviceId);
}

Status VL53L1X_DistanceSensor::readDistance(meter_t& distance) {
    Status status = Status::NO_NEW_DATA;
    uint8_t dataReady = 0;
    VL53L1X_CheckForDataReady(this->deviceId, &dataReady);

    if (dataReady) {
        uint16_t dist_mm = 0;
        VL53L1X_GetDistance(this->deviceId, &dist_mm);
        VL53L1X_ClearInterrupt(this->deviceId);
        distance = millimeter_t(dist_mm);
        status = Status::OK;
    }
    return status;
}

} // namespace hw
} // namespace micro