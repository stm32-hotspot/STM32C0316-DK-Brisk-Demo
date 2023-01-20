/**
  ******************************************************************************
  * @file    io_pins.h
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

#ifndef _IO_PINS_H_ // to point to a pin description, we need to group the GPIOx with the pin properties...
#define _IO_PINS_H_

typedef struct {
  GPIO_TypeDef* GPIO;
  GPIO_InitTypeDef Init;
}  IO_Pin_t; // group both init structure and port address in one element which can be inserted within higher level structures.

void IO_PinInit(void);                                // activate the pin structure
void IO_PinConfigure( IO_Pin_t *pPin);           // configure the pin
void IO_PinSetHigh( IO_Pin_t *pPin);             // set the pin high
void IO_PinSetLow( IO_Pin_t *pPin);              // set the pin low
void IO_PinSet( IO_Pin_t *pPin, uint32_t level); // set the pin level from parameter

void IO_PinToggle( IO_Pin_t *pPin);              // toggle the pin

//void IO_PinSet(IO_Pin_t* pPin, u32 value);
int32_t IO_PinGet( IO_Pin_t* pPin);              // read pin level
                                                 
#endif
