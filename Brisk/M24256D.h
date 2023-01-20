/**
  ******************************************************************************
  * @file    m24256d.h
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

#ifndef _M24256D_H_ // I2C Slave EEPROM present in STMod+ add-on board for hot plug and play, a good I2C application example with I2C glitch and error recovery demonstration.
#define _M24256D_H_

// M24128DWMN default EEPROM on the bus
// Subadress is 16 bit, MSB first (?)
// 16 kbyte of data in pages of 64 bytes
// write in no longer than 5msec
// M24128D has additional locable page
// sub address is 16 bit MSB first
// The ID page at specific slave address is 64 byte wide 0000 to 003F

// To lock the ID page, write data byte value 0x02 at address 0x0400 using ID address

//=== the object and all its dependencies
#include "I2C_MasterIO.h"

#define EEPSIZE_KBIT 256
#define EEPPAGESIZE_BYTE 64

#define EEP_ADR_OOR (128 * EEPSIZE_KBIT ) // 0x8000
typedef struct {
  
  // I2C HAL Entity
  I2C_SlaveDevice_t* pDevice; // pointer to the I2C HW, quick init
  I2C_SlaveDevice_t* pDeviceID; // pointer to the I2C HW, quick init

  uint8_t IsAvailable; // 1 = available, 0 = not preset or not configured

  uint8_t ID_Page[EEPPAGESIZE_BYTE];
} EEP_t;

/* init the FIFO in stream mode with a given threshold */
uint8_t EEP_Init(void);

uint8_t Is_EEP_Detected(EEP_t* pEEP);
uint8_t EEP_ReadBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* pByte, uint16_t Count);
uint8_t EEP_WriteBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* ptr, uint16_t Count);

uint8_t EEP_ReadID (EEP_t* pEEP);
uint8_t EEP_WriteID (EEP_t* pEEP);

uint8_t EEP_SetID_Page(EEP_t* pEEP, char* pStringID);
/*
HAL_StatusTypeDef  EEP_LockPageID(void);
uint8_t EEP_IsPageID_Locked(void);
*/
void EEP_Test(void);

extern EEP_t STModM24256D;
extern EEP_t BriskM24256D;

#endif

