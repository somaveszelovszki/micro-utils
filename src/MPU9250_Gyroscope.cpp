#include <micro/hw/MPU9250_Gyroscope.hpp>
#include <micro/utils/log.hpp>
#include <micro/utils/arrays.hpp>
#include <micro/math/constants.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace micro {
namespace hw {

// See also MPU-9250 Register Map and Descriptions, Revision 4.0, RM-MPU-9250A-00, Rev. 1.4, 9/9/2013 for registers not listed in
// above document; the MPU9250 and MPU9150 are virtually identical but the latter has a different register map
//
//Magnetometer Registers
#define AK8963_ADDRESS   0x0C<<1
#define AK8963_WHO_AM_I  0x00 // should return 0x48
#define AK8963_INFO      0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

/*#define X_FINE_GAIN      0x03 // [7:0] fine gain
#define Y_FINE_GAIN      0x04
#define Z_FINE_GAIN      0x05
#define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
#define XA_OFFSET_L_TC   0x07
#define YA_OFFSET_H      0x08
#define YA_OFFSET_L_TC   0x09
#define ZA_OFFSET_H      0x0A
#define ZA_OFFSET_L_TC   0x0B */

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define SELF_TEST_A      0x10

#define XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L      0x14
#define YG_OFFSET_H      0x15
#define YG_OFFSET_L      0x16
#define ZG_OFFSET_H      0x17
#define ZG_OFFSET_L      0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E
#define WOM_THR          0x1F

#define MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Check DMP interrupt
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL  0x69
#define USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define XA_OFFSET_H      0x77
#define XA_OFFSET_L      0x78
#define YA_OFFSET_H      0x7A
#define YA_OFFSET_L      0x7B
#define ZA_OFFSET_H      0x7D
#define ZA_OFFSET_L      0x7E

// Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1
#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69<<1  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68<<1  // Device address when ADO = 0
#endif

void MPU9250::writeByte(uint8_t address, uint8_t subAddress, uint8_t data) {
    uint8_t data_write[2];
    data_write[0] = subAddress;
    data_write[1] = data;
    HAL_I2C_Master_Transmit(this->hi2c, address, data_write, 2, 20);
}

char MPU9250::readByte(uint8_t address, uint8_t subAddress)
{
    uint8_t data = 0;
    HAL_I2C_Mem_Read(this->hi2c, address, subAddress, 1, &data, 1, 20);
    return data;
}

bool MPU9250::readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{
    return HAL_OK == HAL_I2C_Mem_Read(this->hi2c, address, subAddress, 1, dest, count, 20);
}

float MPU9250::getMres(Mscale scale) {
    float mRes = 0.0f;
    switch (scale)
    {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
    case Mscale::MFS_14BITS:
        mRes = 10.0*4912.0/8190.0; // Proper scale to return milliGauss
        break;
    case Mscale::MFS_16BITS:
        mRes = 10.0*4912.0/32760.0; // Proper scale to return milliGauss
        break;
    }
    return mRes;
}

float MPU9250::getGres(Gscale scale) {
    float gRes = 0.0f;
    switch (scale)
    {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
    case Gscale::GFS_250DPS:
        gRes = 250.0/32768.0;
        break;
    case Gscale::GFS_500DPS:
        gRes = 500.0/32768.0;
        break;
    case Gscale::GFS_1000DPS:
        gRes = 1000.0/32768.0;
        break;
    case Gscale::GFS_2000DPS:
        gRes = 2000.0/32768.0;
        break;
    }
    return gRes;
}

float MPU9250::getAres(Ascale scale) {
    float aRes = 0.0f;
    switch (scale)
    {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    case Ascale::AFS_2G:
        aRes = 2.0/32768.0;
        break;
    case Ascale::AFS_4G:
        aRes = 4.0/32768.0;
        break;
    case Ascale::AFS_8G:
        aRes = 8.0/32768.0;
        break;
    case Ascale::AFS_16G:
        aRes = 16.0/32768.0;
        break;
    }
    return aRes;
}

point3<m_per_sec2_t> MPU9250::readAccelData(void) {

    uint8_t rawData[6];
    this->readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);

    return {
        ((((int16_t)rawData[0] << 8) | rawData[1]) - this->accelBias[0]) * this->aRes * micro::G,
        ((((int16_t)rawData[2] << 8) | rawData[3]) - this->accelBias[0]) * this->aRes * micro::G,
        ((((int16_t)rawData[4] << 8) | rawData[5]) - this->accelBias[0]) * this->aRes * micro::G
    };
}

point3<rad_per_sec_t> MPU9250::readGyroData(void) {

    point3<rad_per_sec_t> result;

    this->hi2c->State = HAL_I2C_STATE_READY;

    uint8_t rawData[6] = { 0, 0, 0, 0, 0, 0 };
    if (this->readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0])) {
        float x = (((int16_t)(int8_t)rawData[0] << 8) | rawData[1]) * this->gRes - this->gyroBias[0];
        float y = (((int16_t)(int8_t)rawData[2] << 8) | rawData[3]) * this->gRes - this->gyroBias[1];
        float z = (((int16_t)(int8_t)rawData[4] << 8) | rawData[5]) * this->gRes - this->gyroBias[2];

        if (abs(x) < this->gyroThreshold[0]) x = 0.0f;
        if (abs(y) < this->gyroThreshold[1]) y = 0.0f;
        if (abs(z) < this->gyroThreshold[2]) z = 0.0f;

        result.X = deg_per_sec_t(x);
        result.Y = deg_per_sec_t(y);
        result.Z = deg_per_sec_t(z);
    } else {
        result.X = result.Y = result.Z = rad_per_sec_t::infinity();
    }

    return result;
}

point3<gauss_t> MPU9250::readMagData(void) {
    point3<gauss_t> result;
    uint8_t rawData[7];
    if(this->readByte(AK8963_ADDRESS, AK8963_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
        this->readBytes(AK8963_ADDRESS, AK8963_XOUT_L, 7, &rawData[0]);  // Read the six raw data and ST2 registers sequentially into data array
        uint8_t c = rawData[6];
        if(!(c & 0x08)) { // Check if magnetic sensor overflow bit is set
            result.X = gauss_t((int16_t)(((int16_t)rawData[1] << 8) | rawData[0]) * this->mRes * this->magCalibration[0]);// - this->magBias[0]);
            result.Y = gauss_t((int16_t)(((int16_t)rawData[3] << 8) | rawData[2]) * this->mRes * this->magCalibration[1]);// - this->magBias[1]);
            result.Z = gauss_t((int16_t)(((int16_t)rawData[5] << 8) | rawData[4]) * this->mRes * this->magCalibration[2]);// - this->magBias[2]);
        }
    }
    return result;
}

celsius_t MPU9250::readTempData(void)
{
  uint8_t rawData[2];
  this->readBytes(MPU9250_ADDRESS, TEMP_OUT_H, 2, &rawData[0]);
  return celsius_t((((int16_t)rawData[0]) << 8 | rawData[1]) / 333.87f + 21.0f);
}

void MPU9250::reset(void) {
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit - toggle reset device
}

void MPU9250::initAK8963(void)
{
    this->writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
    vTaskDelay(10);

    this->writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
    vTaskDelay(10);

    uint8_t rawData[3];
    this->readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]); // Read the x-, y-, and z-axis calibration values
    this->magCalibration[0] = (float)(rawData[0] - 128)/256.0f + 1.0f;
    this->magCalibration[1] = (float)(rawData[1] - 128)/256.0f + 1.0f;
    this->magCalibration[2] = (float)(rawData[2] - 128)/256.0f + 1.0f;

    this->writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
    vTaskDelay(10);

    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
    // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
    this->writeByte(AK8963_ADDRESS, AK8963_CNTL, (((uint8_t)this->mScale << 4) | this->Mmode)); // Set magnetometer data resolution and sample ODR
    vTaskDelay(10);
}

void MPU9250::initMPU9250()
{
    // Initialize MPU9250 device
    // wake up device
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
    vTaskDelay(100); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt

    // get stable time source
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

    // Configure Gyro and Accelerometer
    // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
    // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
    // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
    this->writeByte(MPU9250_ADDRESS, CONFIG, 0x03);

    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
    this->writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
    uint8_t c = this->readByte(MPU9250_ADDRESS, GYRO_CONFIG); // get current GYRO_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5]
    c = c & ~0x02; // Clear Fchoice bits [1:0]
    c = c & ~0x18; // Clear AFS bits [4:3]
    c = c | ((uint8_t)this->gScale << 3); // Set full scale range for the gyro
    // c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
    this->writeByte(MPU9250_ADDRESS, GYRO_CONFIG, c ); // Write new GYRO_CONFIG value to register

    // Set accelerometer full-scale range configuration
    c = this->readByte(MPU9250_ADDRESS, ACCEL_CONFIG); // get current ACCEL_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5]
    c = c & ~0x18;  // Clear AFS bits [4:3]
    c = c | ((uint8_t)this->aScale << 3); // Set full scale range for the accelerometer
    this->writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

    // Set accelerometer sample rate configuration
    // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
    // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
    c = this->readByte(MPU9250_ADDRESS, ACCEL_CONFIG2); // get current ACCEL_CONFIG2 register value
    c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
    c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
    this->writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value

    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
    // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

    // Configure Interrupts and Bypass Enable
    // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips
    // can join the I2C bus and all can be controlled by the same master
    this->writeByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x22);
    this->writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}

void MPU9250::calibrate(void)
{
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t i, packet_count, fifo_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

    // reset device, reset all registers, clear gyro and accelerometer bias registers
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    vTaskDelay(100);

    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_2, 0x00);
    vTaskDelay(200);

    // Configure device for bias calculation
    this->writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
    this->writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
    this->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
    this->writeByte(MPU9250_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
    this->writeByte(MPU9250_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    this->writeByte(MPU9250_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
    vTaskDelay(15);

    // Configure MPU9250 gyro and accelerometer for bias calculation
    this->writeByte(MPU9250_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    this->writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    this->writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    this->writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    this->writeByte(MPU9250_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO
    this->writeByte(MPU9250_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
    vTaskDelay(80);

    // At end of sample accumulation, turn off FIFO sensor read
    this->writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
    this->readBytes(MPU9250_ADDRESS, FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (i = 0; i < packet_count; i++) {
        int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
        this->readBytes(MPU9250_ADDRESS, FIFO_R_W, 12, &data[0]); // read data for averaging
        accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
        accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
        accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
        gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
        gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
        gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

        accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[1] += (int32_t) accel_temp[1];
        accel_bias[2] += (int32_t) accel_temp[2];
        gyro_bias[0]  += (int32_t) gyro_temp[0];
        gyro_bias[1]  += (int32_t) gyro_temp[1];
        gyro_bias[2]  += (int32_t) gyro_temp[2];
    }

    accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;

    // Remove gravity from the z-axis accelerometer bias calculation
    if(accel_bias[2] > 0L) {
        accel_bias[2] -= (int32_t) accelsensitivity;
    } else {
        accel_bias[2] += (int32_t) accelsensitivity;
    }

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    //this->gyroBias[0] = gyro_bias[0] * this->gRes; // construct gyro bias in deg/s for later manual subtraction
    //this->gyroBias[1] = gyro_bias[1] * this->gRes;
    //this->gyroBias[2] = gyro_bias[2] * this->gRes;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    this->readBytes(MPU9250_ADDRESS, XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    this->readBytes(MPU9250_ADDRESS, YA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    this->readBytes(MPU9250_ADDRESS, ZA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(i = 0; i < 3; i++) {
      if(accel_bias_reg[i] & mask) mask_bit[i] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Output scaled accelerometer biases for manual subtraction in the main program
    this->accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity;
    this->accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
    this->accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;
}

void MPU9250::calibrateGyro(void) {
    static constexpr uint32_t NUM_SAMPLES = 50;

    float bias[3] = { 0, 0, 0 }, sigma[3] = { 0, 0, 0 };

    for (uint32_t i = 0; i < NUM_SAMPLES; ++i) {
        uint8_t rawData[6];
        this->readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);

        const float x = (((int16_t)rawData[0] << 8) | rawData[1]) * this->gRes;
        const float y = (((int16_t)rawData[2] << 8) | rawData[3]) * this->gRes;
        const float z = (((int16_t)rawData[4] << 8) | rawData[5]) * this->gRes;

        bias[0] += x;
        bias[1] += y;
        bias[2] += z;

        sigma[0] += x * x;
        sigma[1] += y * y;
        sigma[2] += z * z;

        vTaskDelay(5);
    }

    this->gyroBias[0] = bias[0] / NUM_SAMPLES;
    this->gyroBias[1] = bias[1] / NUM_SAMPLES;
    this->gyroBias[2] = bias[2] / NUM_SAMPLES;


    this->gyroThreshold[0] = sqrt((sigma[0] / NUM_SAMPLES) - (this->gyroBias[0] * this->gyroBias[0])) * 2;
    this->gyroThreshold[1] = sqrt((sigma[1] / NUM_SAMPLES) - (this->gyroBias[1] * this->gyroBias[1])) * 2;
    this->gyroThreshold[2] = sqrt((sigma[2] / NUM_SAMPLES) - (this->gyroBias[2] * this->gyroBias[2])) * 2;
}

void MPU9250::initialize(void) {
    uint8_t whoAmI = this->readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);

    if (whoAmI == 0x73)
    {
        LOG_DEBUG("MPU9250 is online...");
        vTaskDelay(10);

        this->reset();
        vTaskDelay(50);

        this->calibrate();
        vTaskDelay(20);

        this->initMPU9250();

        vTaskDelay(150);
        this->calibrateGyro();
        LOG_DEBUG("gyro bias:  %f, %f, %f", this->gyroBias[0], this->gyroBias[1], this->gyroBias[2]);
        LOG_DEBUG("accel bias: %f, %f, %f", this->accelBias[0], this->accelBias[1], this->accelBias[2]);

        this->initAK8963();
        LOG_DEBUG("Gyro initialized");
        vTaskDelay(20);

    } else {
       LOG_ERROR("Could not connect to MPU9250: %u", (uint32_t)whoAmI);
       return;
    }
}

} // namespace hw
} // namespace micro
