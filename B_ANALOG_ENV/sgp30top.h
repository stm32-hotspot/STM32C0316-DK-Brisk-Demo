
#ifndef _SGP30TOP_H_
#define _SGP30TOP_H_

/********************************************************************************
  * @file    SGP30TOP.h
  * @author  MCD Application Team
  * @brief   This file provides the standard USB requests following chapter 9.
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

//#include "sgp30.h"

int32_t SGP30_Report(u8fifo_t* pL);

int32_t SGP30_IsPlugged(void);
int32_t SGP30_1sec(void);
int32_t SGP30_IsPolling(void);

extern uint16_t tvoc_ppb, co2_eq_ppm;
#endif
