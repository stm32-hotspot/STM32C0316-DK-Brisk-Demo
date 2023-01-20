#ifndef _VEML6070_H_
#define _VEML6070_H_

/**
  ******************************************************************************
  * @file    veml6070.h
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

#include "I2C_MasterIO.h"

typedef struct {
  I2C_SlaveDevice_t* pD ;
  uint8_t CR;
  
  uint8_t ContinuousEnable;
  uint8_t ReadoutUpdated;
  uint16_t Readout;
  uint32_t UV_Index;
  uint8_t Countdown_50msInit;
  uint8_t Countdown_50ms;
  
} VEML6070_t;

extern VEML6070_t VEML6070;

int32_t VEML6070_Init(VEML6070_t* pVE);

int32_t VEML6070_RunOneShotBlocking(VEML6070_t* pVE); // for bringup HW, perform a single measurement, takes 300ms

int32_t VEML6070_ReadData(VEML6070_t* pVE);

int32_t VEML6070_StartOneShotConversion(VEML6070_t* pVE);
int32_t VEML6070_StartContinuousConversion(VEML6070_t* pVE);
int32_t VEML6070_50msec(VEML6070_t* pVE);

int32_t VEML6070_Report(u8fifo_t* pL);

#endif

