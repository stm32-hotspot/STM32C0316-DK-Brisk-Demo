


#ifndef _LPS22HH_H_
#define _LPS22HH_H_

/*
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
#include "lps22hh_reg.h"

int32_t LPS22HH_Report(u8fifo_t* pL);

int32_t LPS22HH_IsPlugged(void);
int32_t LPS22HH_Polling(void);

extern int32_t LPS_hPa_x100;
extern int32_t LPS_DegC_x10;

#endif
