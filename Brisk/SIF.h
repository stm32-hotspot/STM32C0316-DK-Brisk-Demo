/**
  ******************************************************************************
  * @file    sif.h
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
#ifndef _SIF_H_
#define _SIF_H_

#include "stm32c0xx_ll_usart.h"

#include "BTEL_Drivers.h" // Android HC-06 Bluetooth Electronics Dashboard Application 
#include "brisk_reports.h"


// import from SPIP_SIF references
typedef enum {
//  APP_SERIAL, // 0
//  USB_SERIAL, // 1 (to consider with G0 512k QFP48 version on Disco28)
  ARD_STM_SERIAL, // 0 Arduino xor STMod+
  BT_STL_SERIAL, //++ 4 this is bluetooth HC-05 or STLink Serial, using USART2
  MAX_SERIAL, // 2
} XXX_Serial_t;

extern u8fifo_t u8fifo_to_SPIP[MAX_SERIAL], u8fifo_from_SPIP[MAX_SERIAL]; // These FIFO are re-routable to any peripheral. 4 TX FIFO channels, 4 RX FIFO channels

uint32_t SIF_to_USARTn_NoLongerEmpty(uint32_t ct);
uint32_t SIF_to_USARTn_Emptied(uint32_t ct);
void SPIP_SIF_Init(void);

#endif
