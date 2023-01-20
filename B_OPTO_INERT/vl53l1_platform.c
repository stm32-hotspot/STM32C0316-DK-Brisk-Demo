
/* 
* This file is part of VL53L1SD Platform 
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

#include "main.h"
#include "brisk.h"

#include "vl53l1_platform.h"
#include <string.h>
#include <time.h>
#include <math.h>


static uint8_t _I2CBuffer[256]; // only use locally

int _I2CWrite(VL53L1_DEV Dev, uint8_t *pdata, uint32_t count);
int _I2CWrite(VL53L1_DEV Dev, uint8_t *pdata, uint32_t count) {
    int status;
//    int i2c_time_out = I2C_TIME_OUT_BASE+ count* I2C_TIME_OUT_BYTE;

//    status = HAL_I2C_Master_Transmit(Dev->I2cHandle, Dev->I2cDevAddr, pdata, count, i2c_time_out);
    Dev->pSD->pWriteByte = pdata;
    Dev->pSD->WriteByteCount = count;
    Dev->pSD->ReadByteCount = 0;
    status =  I2C_MasterIO_AccessSlave(Dev->pSD); // I2C_SlaveDevice_t // I2C Generic transaction Write+Read cycle
    if (status) {
      NOPs(1);
    }
    return status;
}

int _I2CRead(VL53L1_DEV Dev, uint8_t *pdata, uint32_t count);
int _I2CRead(VL53L1_DEV Dev, uint8_t *pdata, uint32_t count) {
    int status;
//    int i2c_time_out = I2C_TIME_OUT_BASE+ count* I2C_TIME_OUT_BYTE;
    Dev->pSD->pReadByte = pdata;
    Dev->pSD->ReadByteCount = count;
    Dev->pSD->WriteByteCount = 0;
    status =  I2C_MasterIO_AccessSlave(Dev->pSD); // I2C_SlaveDevice_t // I2C Generic transaction Write+Read cycle
    if (status) {
      NOPs(1);
    }
    return status;
}

//==============

int8_t VL53L1_WrByte(VL53L1_DEV Dev, uint16_t index, uint8_t data) {
    int8_t Status = 0;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    _I2CBuffer[2] = data;

    status_int = _I2CWrite(Dev, _I2CBuffer, 3);
    if (status_int != 0) {
        Status = status_int;
    }

    return Status;
}

int8_t VL53L1_WrWord(VL53L1_DEV Dev, uint16_t index, uint16_t data) {
    int8_t Status = 0;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    _I2CBuffer[2] = data >> 8;
    _I2CBuffer[3] = data & 0x00FF;

    status_int = _I2CWrite(Dev, _I2CBuffer, 4);
    if (status_int != 0) {
        Status = status_int;
    }

    return Status;

}

int8_t VL53L1_WrDWord(VL53L1_DEV Dev, uint16_t index, uint32_t data) {
    int8_t Status = 0;
    int32_t status_int;
    _I2CBuffer[0] = index>>8;
    _I2CBuffer[1] = index&0xFF;
    _I2CBuffer[2] = (data >> 24) & 0xFF;
    _I2CBuffer[3] = (data >> 16) & 0xFF;
    _I2CBuffer[4] = (data >> 8)  & 0xFF;
    _I2CBuffer[5] = (data >> 0 ) & 0xFF;

    status_int = _I2CWrite(Dev, _I2CBuffer, 6);
    if (status_int != 0) {
        Status = status_int;
    }

    return Status;
}

int8_t VL53L1_RdByte(VL53L1_DEV Dev, uint16_t index, uint8_t *data) {
    int8_t Status = 0;
    int32_t status_int;

	_I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;

    status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if( status_int ){
        Status = status_int;
        goto done;
    }
    status_int = _I2CRead(Dev, data, 1);
    if (status_int != 0) {
        Status = status_int;
    }
done:
    return Status;

}

int8_t VL53L1_RdWord(VL53L1_DEV Dev, uint16_t index, uint16_t *data) {
    int8_t Status = 0;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;

    status_int = _I2CWrite(Dev, _I2CBuffer, 2);

    if( status_int ){
        Status = status_int;
        goto done;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = status_int;
        goto done;
    }

    *data = ((uint16_t)_I2CBuffer[0]<<8) + (uint16_t)_I2CBuffer[1];
done:

    return Status;

}

int8_t VL53L1_RdDWord(VL53L1_DEV Dev, uint16_t index, uint32_t *data) {
    int8_t Status = 0;
    int32_t status_int;

    _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;

    status_int = _I2CWrite(Dev, _I2CBuffer, 2);
    if (status_int != 0) {
        Status = status_int;
        goto done;
    }
    status_int = _I2CRead(Dev, _I2CBuffer, 4);
    if (status_int != 0) {
        Status = status_int;
        goto done;
    }

    *data = ((uint32_t)_I2CBuffer[0]<<24) + ((uint32_t)_I2CBuffer[1]<<16) + ((uint32_t)_I2CBuffer[2]<<8) + (uint32_t)_I2CBuffer[3];

done:

    return Status;

}

int8_t VL53L1_WaitMs(VL53L1_Dev_t *pdev, int32_t wait_ms){
  HAL_Delay(wait_ms);
	return 0; // to be implemented
}
