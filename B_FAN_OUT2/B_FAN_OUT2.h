#ifndef _B_FAN_OUT2_BOARD_
#define _B_FAN_OUT2_BOARD_
/**
  ******************************************************************************
  * @file    B_FAN_OUT2.h (STMod+ add-on board)
  * @author  S.Ma
  * @brief   STMod+ add-on board services for STM32C0316-DK (hot plug and play supported)
  * Future_improvements: Split the pinout specific part to make most of the code frozen. (reduce maintenance, speedup reuse and customization)
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

typedef struct {
  
  int32_t BoardVersion;
  
  uint8_t PinMode[20];
  int32_t SliderValue;
  u8fifo_t* pReport;
  
} FO_AddOn_t;

extern FO_AddOn_t FO_AddOn;

int32_t FO_Init(void);
int32_t FO_DeInit(void);
void FO_Test(void);
int32_t FO_Polling(void);
int32_t FO_10ms(void);
int32_t FO_50ms(void);
int32_t FO_100ms(void);
int32_t FO_1sec(void);
int32_t FO_IsPlugged(void);
int32_t FO_IsUnplugged(void);

//extern ByteVein_t* pFO_Report;
int32_t FO_Report(void);
int32_t FO_PaneProcess(void);
int32_t FO_SetPanel(void);
#endif
