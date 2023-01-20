
#ifndef _VL53L1X_H_
#define _VL53L1H_H_
/**
  ******************************************************************************
  * @file    vl51l1x.h
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


#include "vl53l1x_api.h"
#include "vl53l1x_calibration.h"

int32_t VL53L1_Init(void);
int32_t VL53L1_DeInit(void);
int32_t VL53L1_IsPlugged(void);
int32_t VL53L1_IsUnplugged(void);
int32_t VL53L1_Polling(void);
int32_t VL53L1_50ms(void);

int32_t VL53L1_Report(u8fifo_t* pL);

extern VL53L1_Dev_t* pVL53L1;

extern VL53L1_Dev_t                   dev;
#endif