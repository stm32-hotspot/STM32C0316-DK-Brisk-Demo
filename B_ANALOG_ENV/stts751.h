/**
  ******************************************************************************
  * @file    stts751.h (ST Temp sensor SW plumbing layer)
  * @author  S.Ma
  * @brief   
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
#ifndef _STTS751_H_
#define _STTS751_H_

#include "stts751_reg.h"


typedef struct {
  
  char IsAvailable;
  
  I2C_SlaveDevice_t* pI2C_SlaveDevice;
  stmdev_ctx_t dev_ctx;
  stts751_id_t whoamI;
  axis1bit16_t raw_temperature;
  int32_t DegC_x10;
  u8fifo_t* pReport;
  int32_t (*fReport) (u8fifo_t*);  
  
} STTS751_t; // this contains the data context, not the methods (function pointers)

extern STTS751_t STModSTTS751;
extern STTS751_t BriskSTTS751; // when there are more than one sensor of same type, the code evolves, here is the xtra sensor on brisk I2C bus.


int32_t STTS751_Report(u8fifo_t* pL);

int32_t STTS751_IsPlugged(void);
int32_t STTS751_Polling(void);

int32_t STTS751_CheckMeasurement(STTS751_t* pS);
int32_t STTS751_Configure(STTS751_t* pS);
//extern int32_t STTS_DegC_x10;

#endif
