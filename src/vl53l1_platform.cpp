
/*
* This file is part of VL53L1 Platform
*
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


#include <micro/hw/vl53l1_platform.h>
#include <micro/hw/vl53l1_platform.hpp>
#include <micro/hw/vl53l1_error_codes.h>
#include <micro/port/task.hpp>
#include <micro/port/timer.hpp>

#include <cstring>

using namespace micro;

namespace {

#define VL53L1_GetI2cBus() ((void)0)
#define VL53L1_PutI2cBus() ((void)0)

#if defined STM32
i2c_t i2c_dist = { nullptr };
#else // !STM32
i2c_t i2c_dist = {};
#endif // !STM32

uint8_t _I2CBuffer[256];

int _waitI2C(void) {
    static constexpr millisecond_t MAX_TIMEOUT = millisecond_t(10);

    const millisecond_t waitStartTime = getTime();
    bool timeout = false;
    while (i2c_dist.handle->State != HAL_I2C_STATE_READY && !(timeout = getTime() - waitStartTime >= MAX_TIMEOUT)) {
        os_sleep(1);
    }
    return timeout ? -1 : 0;
}

int _I2CWrite(uint16_t Dev, uint8_t *pdata, uint32_t count) {
    int status;

    status = _waitI2C();
    if (HAL_OK == status) {
        status = isOk(i2c_masterTransmit(i2c_dist, Dev, pdata, count)) ? 0 : -1;
        if (!status) {
            status = _waitI2C();
        }
    }
    return status;
}

int _I2CRead(uint16_t Dev, uint8_t *pdata, uint32_t count) {
    int status;

    status = _waitI2C();
    if (HAL_OK == status) {
        status = isOk(i2c_masterReceive(i2c_dist, Dev | 0x0001, pdata, count)) ? 0 : -1;
        if (!status) {
            status = _waitI2C();
        }
    }
    return status;
}

} // namespace

namespace micro {
namespace hw {

void VL53L1_setI2C(const i2c_t& i2c) {
    i2c_dist = i2c;
}

} // namespace hw
} // namespace micro

extern "C" int8_t VL53L1_WriteMulti(uint16_t Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
    int status_int;
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    if (count > sizeof(_I2CBuffer) - 1) {
        return VL53L1_ERROR_INVALID_PARAMS;
    }
    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    memcpy(&_I2CBuffer[2], pdata, count);
    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, count + 2);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
    }
    VL53L1_PutI2cBus();
    return Status;
}

// the ranging_sensor_comms.dll will take care of the page selection
extern "C" int8_t VL53L1_ReadMulti(uint16_t Dev, uint16_t index, uint8_t *pdata, uint32_t count) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, pdata, count);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
    }
done:
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_WrByte(uint16_t Dev, uint16_t index, uint8_t data) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    _I2CBuffer[2] = data;

    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 3);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
    }
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_WrWord(uint16_t Dev, uint16_t index, uint16_t data) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    _I2CBuffer[2] = data >> 8;
    _I2CBuffer[3] = data & 0x00FF;

    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 4);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
    }
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_WrDWord(uint16_t Dev, uint16_t index, uint32_t data) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;
    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    _I2CBuffer[2] = (data >> 24) & 0xFF;
    _I2CBuffer[3] = (data >> 16) & 0xFF;
    _I2CBuffer[4] = (data >> 8)  & 0xFF;
    _I2CBuffer[5] = (data >> 0 ) & 0xFF;
    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 6);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
    }
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_RdByte(uint16_t Dev, uint16_t index, uint8_t *data) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;

	_I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;
    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if( status_int ){
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, data, 1);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
    }

done:
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_RdWord(uint16_t Dev, uint16_t index, uint16_t *data) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;
    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 2);

    if( status_int ){
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
        goto done;
    }

    *data = ((uint16_t)_I2CBuffer[0]<<8) + (uint16_t)_I2CBuffer[1];
done:
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_RdDWord(uint16_t Dev, uint16_t index, uint32_t *data) {
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;
    VL53L1_GetI2cBus();
    status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
        goto done;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 4);
    if (status_int != 0) {
        Status = VL53L1_ERROR_CONTROL_INTERFACE;
        goto done;
    }

    *data = ((uint32_t)_I2CBuffer[0]<<24) + ((uint32_t)_I2CBuffer[1]<<16) + ((uint32_t)_I2CBuffer[2]<<8) + (uint32_t)_I2CBuffer[3];

done:
    VL53L1_PutI2cBus();
    return Status;
}

extern "C" VL53L1_Error VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	(void)dev;
 	os_sleep(wait_ms);
    return VL53L1_ERROR_NONE;
}
