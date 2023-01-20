/**
  ******************************************************************************
  * @file    ism330dlc.h
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
#ifndef _ISM330DLC_H_
#define _ISM330DLC_H_

int32_t ISM330DLC_IsPlugged(void);
int32_t ISM330DLC_Polling(void);

int32_t ISM330DLC_Report(u8fifo_t* pL);

#endif
