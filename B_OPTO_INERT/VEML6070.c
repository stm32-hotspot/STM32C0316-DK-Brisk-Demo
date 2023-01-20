
#include "main.h"
#include "brisk.h"
//#include "lv0104cs.h"

/**
  ******************************************************************************
  * @file    VEML6070.c (optical sensor driver, home built)
  * @author  S.Ma
  * @brief   Built on I2C Master IO and sensor datasheet
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

// 3 hours coding and debug with magnifying glass with White and UV LED
// https://www.google.com/url?sa=i&source=images&cd=&cad=rja&uact=8&ved=2ahUKEwiCwqqgw5DiAhUR6OAKHfTuCpgQjRx6BAgBEAU&url=https%3A%2F%2Fwww.amazon.fr%2FLoupe-Bijoutier-avec-LED-%25C3%25A9clairage%2Fdp%2FB00MMYO8PK&psig=AOvVaw3Zy8IxA9lhPieBH5PMpBJU&ust=1557562661476090

#ifdef _B_OPTO_INERT_BOARD_
int32_t VEML6070_WriteReg(VEML6070_t* pVE, uint8_t w1);
int32_t VEML6070_ReadReg(VEML6070_t* pVE);

I2C_SlaveDevice_t gSTModVEML6070  = {   &gI2C_STMod, 0x70, 0 };
VEML6070_t VEML6070 = { &gSTModVEML6070, 0, 0, 0 };
VEML6070_t* pVEML6070 = &VEML6070;

int32_t VEML6070_Init(VEML6070_t* pVE) {

  int32_t result = 0;
  
  VEML6070_WriteReg(pVE, 0x02); // no INT, no ACK
 
  return result;
}


int32_t VEML6070_WriteReg(VEML6070_t* pVE, uint8_t w1) {
  
  int32_t result = 0;
  /*
  // clear stupid ACK at 0x19 slave address
  pVE->pD->SlaveAdr = 0x18;
  pVE->pD->WriteByteCount = 0;
  pVE->pD->pReadByte = &pVE->pD->SubAdrBytes[2];  // read LSB
  pVE->pD->ReadByteCount = 1;
  result = I2C_MasterIO_AccessDevice(pVE->pD);
  //== ACK ======== 8>< ------------------
  */
  pVE->pD->SlaveAdr = 0x70;
  pVE->pD->pWriteByte[0] = w1;
  pVE->pD->pWriteByte = &pVE->pD->SubAdrBytes[0]; // first unused 2 bytes of sub address to write up to 2 data bytes registers
  pVE->pD->WriteByteCount = 1;

  pVE->pD->ReadByteCount = 0;
  
  result |= I2C_MasterIO_AccessSlave(pVE->pD);
  pVE->CR = w1; // remember what was written
    
  return result;
}

int32_t VEML6070_ReadReg(VEML6070_t* pVE) {
  
  int32_t result;
/*
  // clear stupid ACK at 0x19 slave address
  pVE->pD->SlaveAdr = 0x18;
  pVE->pD->WriteByteCount = 0;
  pVE->pD->pReadByte = &pVE->pD->SubAdrBytes[2];  // read LSB
  pVE->pD->ReadByteCount = 1;
  result = I2C_MasterIO_AccessDevice(pVE->pD);
  //== ACK ======== 8>< ------------------
*/  
  pVE->pD->SlaveAdr = 0x70;
  pVE->pD->WriteByteCount = 0;
  pVE->pD->pReadByte = &pVE->pD->SubAdrBytes[2];  // read LSB
  pVE->pD->ReadByteCount = 1;
  result = I2C_MasterIO_AccessSlave(pVE->pD);

  pVE->pD->SlaveAdr = 0x72;  
  pVE->pD->pReadByte = &pVE->pD->SubAdrBytes[3];  // read MSB
  result = I2C_MasterIO_AccessSlave(pVE->pD);
  
  return result;
}

//==== VEML6070 no sub address


int32_t VEML6070_ReadData(VEML6070_t* pVE) {
  
  int32_t result = 0;
 
  result |= VEML6070_ReadReg(pVE);
  pVE->UV_Index = pVE->Readout = MAKEWORD(pVE->pD->SubAdrBytes[3],pVE->pD->SubAdrBytes[2]);
  
  return result;
}

int32_t VEML6070_RunOneShotBlocking(VEML6070_t* pVE) {

  int32_t result;
  result = VEML6070_StartOneShotConversion(pVE);
  HAL_Delay(300);
  pVE->Countdown_50ms = 0;
  pVE->ContinuousEnable = 0;
  result |= VEML6070_ReadData(pVE);
  pVE->ReadoutUpdated = 1;
  return result;
}

int32_t VEML6070_StartOneShotConversion(VEML6070_t* pVE) {
  
  int32_t result;
  int8_t counts = 0;

  switch((pVE->CR >>2)&0x03) {
  case 0:counts = 3;break;  // 62.5ms ==> 150ms = 3x50
  case 1:counts = 4;break;  // 125+50=175=200 = 4x50
  case 2:counts = 6;break;  // 250ms + 50 = 300 = 6x30
  case 3:counts = 11;break; // 500+50 = 11
  }
  pVE->Countdown_50msInit = counts;
  pVE->Countdown_50ms = counts;
  pVE->ContinuousEnable = 0;
  
  result = VEML6070_WriteReg(pVE, pVE->CR);          // write command register

  return result;
}

int32_t VEML6070_StartContinuousConversion(VEML6070_t* pVE) {
  
  int32_t result;
  
  result = VEML6070_StartOneShotConversion(pVE);
  pVE->ContinuousEnable = 1;
  
  return result;
}


int32_t VEML6070_50msec(VEML6070_t* pVE) { // the device is free run by SW, based on CR and sensitivity levels

  int32_t result = 0;
// manually update all values
  if(pVE->Countdown_50ms==0) return result;
  
  pVE->Countdown_50ms--;
  if(pVE->Countdown_50ms!=0) return result;
  
  // time elapsed!
  result |= VEML6070_ReadData(pVE);
  pVE->ReadoutUpdated = 1;
  
  if(pVE->ContinuousEnable) {
    result |= VEML6070_StartContinuousConversion(pVE); // at this point, the CR and Sensitivity fields will be updated to the sensor registers no change during conversion
  }

  return result;
}

int32_t VEML6070_Report(u8fifo_t* pL) {
  
  BTEL_Printf(pL,"*C%d", VEML6070.UV_Index); // 
  return 0;
}
#endif
