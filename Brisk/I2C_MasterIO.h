/**
  ******************************************************************************
  * @file    i2c_master_io.h
  * @author  S.Ma
  * @brief   ...
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef _I2C_MASTER_IO_H_ // I2C single master with pure GPIO, 8 bit address format, no clock stretch support for speed, bus recovery implemented.
#define _I2C_MASTER_IO_H_

#include "io_pins.h"

//****************************     I2C       **********************************
// This master mode is not using interrupts because it does not have any critical timings inside
// Single Master I2C bus done by 2 GPIO and SW only. No clock stretching, no timeouts needed. I2C Bus error recovery implemented.
// For specific slaves like Sensirion or MCU to MCU communication with clock stretching, you will have to edit the low level code to wait for the clock to actually go high... with a timeout.
// you could also create a copy of this code with clock stretch support. Code size is not so big anyway.

// This is the structure which defines a global physical I2C bus by bitbang IO

typedef struct {
//---- Physical part  
  PadName_t SDA; // pointer?
  PadName_t SCL; // pointer?
//---- Timing part
  int32_t bitrate_kHz;
  int16_t ClockStretchTimeout_ms;
//---- Report part
  uint8_t AckFail /*: 1*/; // actually could be a single bit. This is used by caller's function
//---- Internal part
  RawPortPads_t RawPad_SDA; // for speedy operations if needed
  RawPortPads_t RawPad_SCL; // for speedy operations if needed
  int32_t (*fnWaitMethod)(uint32_t);
  uint32_t ctWaitMethod;
  uint32_t WaitParam;
//----  
  
} I2C_MasterIO_t;


// This structure defines a Slave using an I2C Bus
// Most slaves can be described using this construct

typedef struct {

  I2C_MasterIO_t* M; // which bus?
  
  uint8_t SlaveAdr; // what slave address? If multiple, consider multiple slaves.

  uint8_t SubAdrByteCount;   // how many bytes for the sub address? 0,1,2,4...
  uint8_t SubAdrBytes[4]; // first byte is slave address, if any // up to 4 byte subaddress can be stored here. When not needed, could be reused as small data buffer.
  
  uint8_t *pWriteByte; // pointer to the data payload block to write (exclude sub-address)
  uint16_t WriteByteCount; // payload size. If zero, the write sequence is skipped
  
  uint8_t *pReadByte; // payload to read
  uint16_t ReadByteCount; // payload size. If zero, the read sequence is skipped

  uint8_t SlaveAdrWriteNack /*: 1*/; // 
  uint8_t SlaveAdrReadNack /*: 1*/; //  
  
} I2C_SlaveDevice_t;
  
//----
int32_t I2C_MasterIO_Init( I2C_MasterIO_t* pM ); // this version does not support clock stretch
int32_t I2C_MasterIO_ConfigHW( I2C_MasterIO_t* pM, PadName_t SDA, PadName_t SCL );
int32_t I2C_MasterIO_ConfigTimings( I2C_MasterIO_t* pM, uint32_t MinBps, uint32_t MaxBps, uint32_t ClockStretch_ms ); // here should be zero
int32_t I2C_MasterIO_Enable(I2C_MasterIO_t* pM);
//----
uint8_t I2C_MasterIO_IsSlavePresentByAddress(I2C_MasterIO_t* pM, uint8_t SlaveAdr);

uint8_t I2C_MasterIO_SweepAllSlaveAddresses(I2C_MasterIO_t* pM, uint8_t StartAdr);
// perform a write+read granular sequence.
uint8_t I2C_MasterIO_IsSlavePresent(I2C_SlaveDevice_t* pD);
uint8_t I2C_MasterIO_FindNextSlaveByAddress(I2C_MasterIO_t* pM, uint8_t StartAdr);

// Remote Slave
int32_t I2C_MasterIO_AccessSlave(I2C_SlaveDevice_t* pD); // Only function needed to interact with any slave device. Suggesting EEPROM example.
//uint8_t I2C_MasterIO_IsSlaveDevicePresent(I2C_SlaveDevice_t* pD);

//======== 8>< ---------------------------
// Brisk specific
void Brisk_I2C_MasterIO_Init(void);
void Brisk_I2C_MasterIO_Test(void);
extern I2C_MasterIO_t gI2C_STMod;
extern I2C_MasterIO_t gI2C_Brisk;
extern I2C_MasterIO_t gI2C_Arduino;

#endif
