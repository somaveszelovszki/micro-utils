#include <micro/hw/VL53L1X_DistanceSensor.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/port/task.hpp>
#include <micro/utils/log.hpp>
#include <micro/utils/timer.hpp>

#define SOFT_RESET                                                          0x0000
#define VL53L1_I2C_SLAVE__DEVICE_ADDRESS                                    0x0001
#define VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND                        0x0008
#define ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS                      0x0016
#define ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS                  0x0018
#define ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS                  0x001A
#define ALGO__PART_TO_PART_RANGE_OFFSET_MM                                  0x001E
#define MM_CONFIG__INNER_OFFSET_MM                                          0x0020
#define MM_CONFIG__OUTER_OFFSET_MM                                          0x0022
#define GPIO_HV_MUX__CTRL                                                   0x0030
#define GPIO__TIO_HV_STATUS                                                 0x0031
#define SYSTEM__INTERRUPT_CONFIG_GPIO                                       0x0046
#define PHASECAL_CONFIG__TIMEOUT_MACROP                                     0x004B
#define RANGE_CONFIG__TIMEOUT_MACROP_A_HI                                   0x005E
#define RANGE_CONFIG__VCSEL_PERIOD_A                                        0x0060
#define RANGE_CONFIG__VCSEL_PERIOD_B                                        0x0063
#define RANGE_CONFIG__TIMEOUT_MACROP_B_HI                                   0x0061
#define RANGE_CONFIG__TIMEOUT_MACROP_B_LO                                   0x0062
#define RANGE_CONFIG__SIGMA_THRESH                                          0x0064
#define RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS                         0x0066
#define RANGE_CONFIG__VALID_PHASE_HIGH                                      0x0069
#define VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD                              0x006C
#define SYSTEM__THRESH_HIGH                                                 0x0072
#define SYSTEM__THRESH_LOW                                                  0x0074
#define SD_CONFIG__WOI_SD0                                                  0x0078
#define SD_CONFIG__INITIAL_PHASE_SD0                                        0x007A
#define ROI_CONFIG__USER_ROI_CENTRE_SPAD                                    0x007F
#define ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE                       0x0080
#define SYSTEM__SEQUENCE_CONFIG                                             0x0081
#define VL53L1_SYSTEM__GROUPED_PARAMETER_HOLD                               0x0082
#define SYSTEM__INTERRUPT_CLEAR                                             0x0086
#define SYSTEM__MODE_START                                                  0x0087
#define VL53L1_RESULT__RANGE_STATUS                                         0x0089
#define VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0                       0x008C
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD                                  0x0090
#define VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0               0x0096
#define VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0  0x0098
#define VL53L1_RESULT__OSC_CALIBRATE_VAL                                    0x00DE
#define VL53L1_FIRMWARE__SYSTEM_STATUS                                      0x00E5
#define VL53L1_IDENTIFICATION__MODEL_ID                                     0x010F
#define VL53L1_ROI_CONFIG__MODE_ROI_CENTRE_SPAD                             0x013E

namespace micro {
namespace hw {

const uint8_t DEFAULT_CONFIG[] = {
    0x00, /* 0x2d : set bit 2 and 5 to 1 for fast plus mode (1MHz I2C), else don't touch */
    0x00, /* 0x2e : bit 0 if I2C pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
    0x00, /* 0x2f : bit 0 if GPIO pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
    0x01, /* 0x30 : set bit 4 to 0 for active high interrupt and 1 for active low (bits 3:0 must be 0x1), use SetInterruptPolarity() */
    0x02, /* 0x31 : bit 1 = interrupt depending on the polarity, use CheckForDataReady() */
    0x00, /* 0x32 : not user-modifiable */
    0x02, /* 0x33 : not user-modifiable */
    0x08, /* 0x34 : not user-modifiable */
    0x00, /* 0x35 : not user-modifiable */
    0x08, /* 0x36 : not user-modifiable */
    0x10, /* 0x37 : not user-modifiable */
    0x01, /* 0x38 : not user-modifiable */
    0x01, /* 0x39 : not user-modifiable */
    0x00, /* 0x3a : not user-modifiable */
    0x00, /* 0x3b : not user-modifiable */
    0x00, /* 0x3c : not user-modifiable */
    0x00, /* 0x3d : not user-modifiable */
    0xff, /* 0x3e : not user-modifiable */
    0x00, /* 0x3f : not user-modifiable */
    0x0F, /* 0x40 : not user-modifiable */
    0x00, /* 0x41 : not user-modifiable */
    0x00, /* 0x42 : not user-modifiable */
    0x00, /* 0x43 : not user-modifiable */
    0x00, /* 0x44 : not user-modifiable */
    0x00, /* 0x45 : not user-modifiable */
    0x20, /* 0x46 : interrupt configuration 0->level low detection, 1-> level high, 2-> Out of window, 3->In window, 0x20-> New sample ready , TBC */
    0x0b, /* 0x47 : not user-modifiable */
    0x00, /* 0x48 : not user-modifiable */
    0x00, /* 0x49 : not user-modifiable */
    0x02, /* 0x4a : not user-modifiable */
    0x0a, /* 0x4b : not user-modifiable */
    0x21, /* 0x4c : not user-modifiable */
    0x00, /* 0x4d : not user-modifiable */
    0x00, /* 0x4e : not user-modifiable */
    0x05, /* 0x4f : not user-modifiable */
    0x00, /* 0x50 : not user-modifiable */
    0x00, /* 0x51 : not user-modifiable */
    0x00, /* 0x52 : not user-modifiable */
    0x00, /* 0x53 : not user-modifiable */
    0xc8, /* 0x54 : not user-modifiable */
    0x00, /* 0x55 : not user-modifiable */
    0x00, /* 0x56 : not user-modifiable */
    0x38, /* 0x57 : not user-modifiable */
    0xff, /* 0x58 : not user-modifiable */
    0x01, /* 0x59 : not user-modifiable */
    0x00, /* 0x5a : not user-modifiable */
    0x08, /* 0x5b : not user-modifiable */
    0x00, /* 0x5c : not user-modifiable */
    0x00, /* 0x5d : not user-modifiable */
    0x01, /* 0x5e : not user-modifiable */
    0xcc, /* 0x5f : not user-modifiable */
    0x0f, /* 0x60 : not user-modifiable */
    0x01, /* 0x61 : not user-modifiable */
    0xf1, /* 0x62 : not user-modifiable */
    0x0d, /* 0x63 : not user-modifiable */
    0x01, /* 0x64 : Sigma threshold MSB (mm in 14.2 format for MSB+LSB), use SetSigmaThreshold(), default value 90 mm  */
    0x68, /* 0x65 : Sigma threshold LSB */
    0x00, /* 0x66 : Min count Rate MSB (MCPS in 9.7 format for MSB+LSB), use SetSignalThreshold() */
    0x80, /* 0x67 : Min count Rate LSB */
    0x08, /* 0x68 : not user-modifiable */
    0xb8, /* 0x69 : not user-modifiable */
    0x00, /* 0x6a : not user-modifiable */
    0x00, /* 0x6b : not user-modifiable */
    0x00, /* 0x6c : Intermeasurement period MSB, 32 bits register, use SetIntermeasurementInMs() */
    0x00, /* 0x6d : Intermeasurement period */
    0x0f, /* 0x6e : Intermeasurement period */
    0x89, /* 0x6f : Intermeasurement period LSB */
    0x00, /* 0x70 : not user-modifiable */
    0x00, /* 0x71 : not user-modifiable */
    0x00, /* 0x72 : distance threshold high MSB (in mm, MSB+LSB), use SetD:tanceThreshold() */
    0x00, /* 0x73 : distance threshold high LSB */
    0x00, /* 0x74 : distance threshold low MSB ( in mm, MSB+LSB), use SetD:tanceThreshold() */
    0x00, /* 0x75 : distance threshold low LSB */
    0x00, /* 0x76 : not user-modifiable */
    0x01, /* 0x77 : not user-modifiable */
    0x0f, /* 0x78 : not user-modifiable */
    0x0d, /* 0x79 : not user-modifiable */
    0x0e, /* 0x7a : not user-modifiable */
    0x0e, /* 0x7b : not user-modifiable */
    0x00, /* 0x7c : not user-modifiable */
    0x00, /* 0x7d : not user-modifiable */
    0x02, /* 0x7e : not user-modifiable */
    0xc7, /* 0x7f : ROI center, use SetROI() */
    0xff, /* 0x80 : XY ROI (X=Width, Y=Height), use SetROI() */
    0x9B, /* 0x81 : not user-modifiable */
    0x00, /* 0x82 : not user-modifiable */
    0x00, /* 0x83 : not user-modifiable */
    0x00, /* 0x84 : not user-modifiable */
    0x01, /* 0x85 : not user-modifiable */
    0x00, /* 0x86 : clear interrupt, use ClearInterrupt() */
    0x00  /* 0x87 : start ranging, use StartRanging() or StopRanging(), If you want an automatic start after VL53L1X_init() call, put 0x40 in location 0x87 */
};

VL53L1X_DistanceSensor::VL53L1X_DistanceSensor(const i2c_t& i2c, uint16_t deviceId)
    : i2c_(i2c)
    , deviceId_(deviceId) {}

void VL53L1X_DistanceSensor::initialize() {

    static constexpr uint16_t TIMING_BUDGET_MS = 20;

    uint8_t byteData = 0;
    uint16_t wordData = 0;

    this->readByte(0x010f, byteData);
    this->readByte(0x0110, byteData);
    this->readWord(0x010f, wordData);

    uint8_t sensorState = 0;
    while(!sensorState){
        os_sleep(millisecond_t(10));
        this->readByte(VL53L1_FIRMWARE__SYSTEM_STATUS, sensorState); // reads boot state
    }

    for (uint16_t i = 0; i < ARRAY_SIZE(DEFAULT_CONFIG); ++i){
        this->writeByte(0x2d + i, DEFAULT_CONFIG[i]);
    }

    this->writeByte(SYSTEM__MODE_START, 0x40); // enables VL53L1X

    while(!this->checkDataReady()) {
        os_sleep(millisecond_t(1));
    }

    this->writeByte(SYSTEM__INTERRUPT_CLEAR, 0x01); // clears interrupt
    this->writeByte(SYSTEM__MODE_START, 0x00);      // disables VL53L1X
    this->writeByte(VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND, 0x09); // two bounds VHV
    this->writeByte(0x0B, 0); // starts VHV from the previous temperature

    this->setDistanceMode(distanceMode_t::Long);
    this->setTimingBudgetMs(TIMING_BUDGET_MS);

    uint16_t clockPLL = 0;
    this->readWord(VL53L1_RESULT__OSC_CALIBRATE_VAL, clockPLL);
    clockPLL &= 0x03ff;
    this->writeDWord(VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD, static_cast<uint32_t>(1.075f * clockPLL * TIMING_BUDGET_MS)); // sets inter-measurement time

    this->writeByte(SYSTEM__MODE_START, 0x40); // enables VL53L1X
}

meter_t VL53L1X_DistanceSensor::readDistance() {

    meter_t distance = micro::numeric_limits<meter_t>::infinity();
    if (this->checkDataReady()) {
        uint16_t dist_mm = 0;
        this->readWord(VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0, dist_mm);
        this->writeByte(SYSTEM__INTERRUPT_CLEAR, 0x01); // clears interrupt
        distance = millimeter_t(dist_mm);
    }

    return distance;
}

uint16_t VL53L1X_DistanceSensor::readTimingBudgetMs() {
    uint16_t value = 0;
    uint16_t budget = 0;
    this->readWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, value);
    switch (value) {
        case 0x001D: budget = 15;  break;
        case 0x0051:
        case 0x001E: budget = 20;  break;
        case 0x00D6:
        case 0x0060: budget = 33;  break;
        case 0x1AE:
        case 0x00AD: budget = 50;  break;
        case 0x02E1:
        case 0x01CC: budget = 100; break;
        case 0x03E1:
        case 0x02D9: budget = 200; break;
        case 0x0591:
        case 0x048F: budget = 500; break;
        default:     budget = 0;   break;
    }
    return budget;
}

void VL53L1X_DistanceSensor::setTimingBudgetMs(const uint16_t budget) {

    switch (this->readDistanceMode()) {
    case distanceMode_t::Short:
        switch (budget) {
        case 15: /* only available in short distance mode */
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x001D);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0027);
            break;
        case 20:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0051);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
            break;
        case 33:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x00D6);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
            break;
        case 50:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x01AE);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x01E8);
            break;
        case 100:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x02E1);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0388);
            break;
        case 200:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x03E1);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0496);
            break;
        case 500:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0591);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x05C1);
            break;
        default:
            break;
        }
        break;

    case distanceMode_t::Long:
        switch (budget) {
        case 20:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x001E);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0022);
            break;
        case 33:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0060);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
            break;
        case 50:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x00AD);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x00C6);
            break;
        case 100:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x01CC);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x01EA);
            break;
        case 200:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x02D9);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x02F8);
            break;
        case 500:
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x048F);
            this->writeWord(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x04A4);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

VL53L1X_DistanceSensor::distanceMode_t VL53L1X_DistanceSensor::readDistanceMode() {
    uint8_t value = 0;
    this->readByte(PHASECAL_CONFIG__TIMEOUT_MACROP, value);
    return value == 0x14 ? distanceMode_t::Short : value == 0x0A ? distanceMode_t::Long : distanceMode_t::Unknown;
}

void VL53L1X_DistanceSensor::setDistanceMode(const distanceMode_t mode) {
    const uint16_t budget = this->readTimingBudgetMs();

    switch (mode) {
    case distanceMode_t::Short:
        this->writeByte(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x14);
        this->writeByte(RANGE_CONFIG__VCSEL_PERIOD_A,    0x07);
        this->writeByte(RANGE_CONFIG__VCSEL_PERIOD_B,    0x05);
        this->writeByte(RANGE_CONFIG__VALID_PHASE_HIGH,  0x38);
        this->writeWord(SD_CONFIG__WOI_SD0,              0x0705);
        this->writeWord(SD_CONFIG__INITIAL_PHASE_SD0,    0x0606);
        break;

    case distanceMode_t::Long:
        this->writeByte(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x0A);
        this->writeByte(RANGE_CONFIG__VCSEL_PERIOD_A,    0x0F);
        this->writeByte(RANGE_CONFIG__VCSEL_PERIOD_B,    0x0D);
        this->writeByte(RANGE_CONFIG__VALID_PHASE_HIGH,  0xB8);
        this->writeWord(SD_CONFIG__WOI_SD0,              0x0F0D);
        this->writeWord(SD_CONFIG__INITIAL_PHASE_SD0,    0x0E0E);
        break;
    default:
        break;
    }

    this->setTimingBudgetMs(budget);
}

bool VL53L1X_DistanceSensor::checkDataReady() {

    uint8_t intPol = 0;
    this->readByte(GPIO_HV_MUX__CTRL, intPol);
    intPol = !((intPol & 0x10) >> 4);

    uint8_t dataReady = 0;
    this->readByte(GPIO__TIO_HV_STATUS, dataReady);
    dataReady &= 0x01;

    return dataReady == intPol;
}

Status VL53L1X_DistanceSensor::waitComm() {
    static constexpr millisecond_t MAX_TIMEOUT = millisecond_t(10);

    bool timeout = false;

#if defined STM32
    const millisecond_t waitStartTime = getTime();
    while (this->i2c_.handle->State != HAL_I2C_STATE_READY && !(timeout = getTime() - waitStartTime >= MAX_TIMEOUT)) {
        os_sleep(1);
    }
#endif // STM32

    return timeout ? Status::TIMEOUT : Status::OK;
}

Status VL53L1X_DistanceSensor::writeByte(const uint16_t reg, const uint8_t txData) {
    return this->writeBytes(reg, &txData, 1);
}

Status VL53L1X_DistanceSensor::readByte(const uint16_t reg, uint8_t& rxData) {
    return this->readBytes(reg, &rxData, 1);
}

Status VL53L1X_DistanceSensor::writeWord(const uint16_t reg, const uint16_t txData) {
    const uint8_t txBuf[2] = {
        static_cast<uint8_t>(txData >> 8),
        static_cast<uint8_t>(txData)
    };

    return this->writeBytes(reg, txBuf, 2);
}

Status VL53L1X_DistanceSensor::writeDWord(const uint16_t reg, const uint32_t txData) {
    const uint8_t txBuf[4] = {
        static_cast<uint8_t>(txData >> 24),
        static_cast<uint8_t>(txData >> 16),
        static_cast<uint8_t>(txData >> 8),
        static_cast<uint8_t>(txData)
    };

    return this->writeBytes(reg, txBuf, 2);
}

Status VL53L1X_DistanceSensor::readWord(const uint16_t reg, uint16_t& rxData) {
    uint8_t rxBuf[2] = { 0, 0 };
    const Status status = this->readBytes(reg, rxBuf, 2);
    if (isOk(status)) {
        rxData = (static_cast<uint16_t>(rxBuf[0]) << 8) + rxBuf[1];
    }
    return status;
}

Status VL53L1X_DistanceSensor::writeBytes(const uint16_t reg, const uint8_t *txData, const uint8_t size) {
    Status status = this->waitComm();
    if (isOk(status)) {
        uint8_t txBuf[6] = {
            static_cast<uint8_t>(reg >> 8),
            static_cast<uint8_t>(reg)
        };

        for (uint8_t i = 0; i < size; ++i) {
            txBuf[2 + i] = txData[i];
        }

        status = i2c_masterTransmit(this->i2c_, this->deviceId_, txBuf, 2 + size);
        if (isOk(status)) {
            status = this->waitComm();
        }
    }
    return status;
}

Status VL53L1X_DistanceSensor::readBytes(const uint16_t reg, uint8_t *rxData, const uint8_t size) {
    Status status = this->waitComm();
    if (isOk(status)) {
        uint8_t txBuf[2] = { static_cast<uint8_t>(reg >> 8), static_cast<uint8_t>(reg & 0x00ff) };
        status = i2c_masterTransmit(this->i2c_, this->deviceId_, txBuf, 2);
        if (isOk(status)) {
            status = this->waitComm();
            if (isOk(status)) {
                status = i2c_masterReceive(this->i2c_, this->deviceId_, rxData, size);
                if (isOk(status)) {
                    status = this->waitComm();
                }
            }
        }

    }
    return status;
}

} // namespace hw
} // namespace micro
