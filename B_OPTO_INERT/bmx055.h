#ifndef _BMX055_H_
#define _BMX055_H_
/********************************************************************************
  * @file    BMX055.h
  * @author  MCD Application Team
  * @brief   This file provides the standard USB requests following chapter 9.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *******************************************************************************/

    
int32_t BMX055_Init(void);
int32_t BMX055_50ms(void);

int32_t BMX055_Report(u8fifo_t* pL);

#endif
