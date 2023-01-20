/**
  ******************************************************************************
  * @file    LV104CS.c (optical sensor drivers home made as no source was vendor provided)
  * @author  S.Ma
  * @brief   Built from datasheet reading, using I2C Master IO drivers
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

#include "main.h"
#include "brisk.h"
//#include "lv0104cs.h"
#ifdef _B_OPTO_INERT_BOARD_
int32_t LV0104CS_WriteReg(LV0104CS_t* pLV, uint8_t w1);
int32_t VEML6070_ReadReg(VEML6070_t* pLV);

I2C_SlaveDevice_t gSTModLV0104CS  = {   &gI2C_STMod, 0x26, 0 };
LV0104CS_t LV0104CS = { &gSTModLV0104CS, 0, 0, 0 };
LV0104CS_t* pLV0104CS = &LV0104CS;


int32_t LV0104CS_WriteReg(LV0104CS_t* pLV, uint8_t w1) {
  
  int32_t result = 0;
  
  pLV->pD->pWriteByte[0] = w1;
  pLV->pD->WriteByteCount = 1;
  pLV->pD->ReadByteCount = 0;
  
  result = I2C_MasterIO_AccessSlave(pLV->pD);
  
  return result;
}

int32_t LV0104CS_ReadReg(LV0104CS_t* pLV);
int32_t LV0104CS_ReadReg(LV0104CS_t* pLV) {
  
  int32_t result;
  
  //pLV->pD->pWriteByte[0] = w1;
  pLV->pD->WriteByteCount = 0;
  pLV->pD->ReadByteCount = 2;
  
  result = I2C_MasterIO_AccessSlave(pLV->pD);
  return result;
}




//==== LV0104CS no sub address

int32_t LV0104CS_Init(LV0104CS_t* pLV) {

  int32_t result = 0;
  
  pLV->pD->pWriteByte = &pLV->pD->SubAdrBytes[0]; // first unused 2 bytes of sub address to write up to 2 data bytes registers
  pLV->pD->pReadByte = &pLV->pD->SubAdrBytes[2];  // last unused 2 bytes of sub address to read up to 2 data bytes registers

  pLV->CR = 0xEC;
  pLV->Sensitivity = 0x80;
  
  return result;
}

int32_t LV0104CS_ReadData(LV0104CS_t* pLV) {
  
  int32_t result = 0;
  int32_t read;
  int32_t gain_x1000 = 1000; // 1000 / 1000 = 100% 
  uint8_t sens = pLV->Sensitivity & 0x3F;
 
  result |= LV0104CS_ReadReg(pLV);
  
  read = pLV->Readout = MAKEWORD(pLV->pD->SubAdrBytes[2],pLV->pD->SubAdrBytes[3]);
  
  // convert to lux with gain adjustment
  if(sens) {
    if(sens & 0x20) { // sign = 1
      sens &= 0x1F;
      gain_x1000 = (2*sens*1000)/(2*sens+1);
    }else{
      sens&= 0x1F;
      gain_x1000 = ((2*sens+1)*1000)/(2*sens);     
    }
  }

  switch((pLV->CR >>3)&0x03) { // factor out the gain
  case 0:gain_x1000 *= 4;break; // gain x0.25, real value x4
  case 1:;break; // gain 1 = no change
  case 2:gain_x1000 /= 2;break;// gain x2, real value /2
  case 3:gain_x1000 /= 8;break;
  }
  
  pLV->Lux_x100 = (100 * read * gain_x1000)/1000;
  
  return result;
}

int32_t LV0104CS_RunOneShotBlocking(LV0104CS_t* pLV) {

  int32_t result;
  result = LV0104CS_StartOneShotConversion(pLV);
  HAL_Delay(300);
  pLV->Countdown_50ms = 0;
  pLV->ContinuousEnable = 0;
  result |= LV0104CS_ReadData(pLV);
  pLV->ReadoutUpdated = 1;
  return result;
}

int32_t LV0104CS_StartOneShotConversion(LV0104CS_t* pLV) {
  
  int32_t result;
  int8_t counts;

  switch((pLV->CR >>1)&0x03) {
  case 0:counts = 2;break;
  case 1:counts = 3;break;
  case 2:counts = 5;break;
  default:TrapError(); break; // unsupported
  }
  pLV->Countdown_50msInit = counts;
  pLV->Countdown_50ms = counts;
  pLV->ContinuousEnable = 0;
  
  result = LV0104CS_WriteReg(pLV, pLV->CR);          // 11xG GTTS
  result = LV0104CS_WriteReg(pLV, pLV->Sensitivity); // 10Ss ssss

  return result;
}

int32_t LV0104CS_StartContinuousConversion(LV0104CS_t* pLV) {
  
  int32_t result;
  
  result = LV0104CS_StartOneShotConversion(pLV);
  pLV->ContinuousEnable = 1;
  
  return result;
}


int32_t LV0104CS_50msec(LV0104CS_t* pLV) { // the device is free run by SW, based on CR and sensitivity levels

  int32_t result = 0;
// manually update all values
  if(pLV->Countdown_50ms==0) return result;
  
  pLV->Countdown_50ms--;
  if(pLV->Countdown_50ms!=0) return result;
  
  // time elapsed!
  result |= LV0104CS_ReadData(pLV);
  pLV->ReadoutUpdated = 1;
  
  if(pLV->ContinuousEnable) {
    result |= LV0104CS_StartContinuousConversion(pLV); // at this point, the CR and Sensitivity fields will be updated to the sensor registers no change during conversion
  }

  return result;
}

int32_t LV0104CS_Report(u8fifo_t* pL) {
  
  BTEL_Printf(pL,"*E%C", LV0104CS.Lux_x100); // 
  return 0;
}

#endif

