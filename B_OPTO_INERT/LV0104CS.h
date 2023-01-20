/**
  ******************************************************************************
  * @file    lv0104cs.h
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
#ifndef _LV0104CS_H_
#define _LV0104CS_H_

#include "I2C_MasterIO.h"

typedef struct {
  I2C_SlaveDevice_t* pD ;
  uint8_t CR;
  uint8_t Sensitivity;
  
  uint8_t ContinuousEnable;
  uint8_t ReadoutUpdated;
  uint16_t Readout;
  uint32_t Lux_x100;//1 lx = 1 lm/m2 = 1 cd·sr/m2.
  uint8_t Countdown_50msInit;
  uint8_t Countdown_50ms;
  
} LV0104CS_t;

int32_t LV0104CS_Init(LV0104CS_t* pLV);

int32_t LV0104CS_RunOneShotBlocking(LV0104CS_t* pLV); // for bringup HW, perform a single measurement, takes 300ms

int32_t LV0104CS_ReadData(LV0104CS_t* pLV);

int32_t LV0104CS_StartOneShotConversion(LV0104CS_t* pLV);
int32_t LV0104CS_StartContinuousConversion(LV0104CS_t* pLV);
int32_t LV0104CS_50msec(LV0104CS_t* pLV);

extern LV0104CS_t LV0104CS;
extern LV0104CS_t* pLV0104CS;

int32_t LV0104CS_Report(u8fifo_t* pL);

#endif
