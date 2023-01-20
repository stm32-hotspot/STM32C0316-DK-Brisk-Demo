
#include "main.h"
//#include "io_pins.h"
#include "brisk.h"
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
//============ 8>< -----------------------------
// group in one structure the pin init and the port address as single entity which can then be added to higher level structure such as I2C, SPI, etc...


void IO_PinConfigure(IO_Pin_t *pPin) {
  
  if(pPin) // defined
    HAL_GPIO_Init(pPin->GPIO, &pPin->Init); 
}

void IO_PinSetHigh(IO_Pin_t *pPin) {
  if(pPin)
    HAL_GPIO_WritePin(pPin->GPIO, pPin->Init.Pin, GPIO_PIN_SET); 
}

void IO_PinSetLow(IO_Pin_t *pPin) {
  if(pPin)
    HAL_GPIO_WritePin(pPin->GPIO, pPin->Init.Pin, GPIO_PIN_RESET); 
}

void IO_PinSet(IO_Pin_t *pPin, uint32_t level) {
  if(level)
    IO_PinSetHigh(pPin);
  else
    IO_PinSetLow(pPin);
}

void IO_PinToggle(IO_Pin_t *pPin) {
  if(pPin)
    HAL_GPIO_TogglePin(pPin->GPIO, pPin->Init.Pin); 
}


int32_t IO_PinGet(IO_Pin_t* pPin) {
  if(pPin==0) return 0;
  return HAL_GPIO_ReadPin(pPin->GPIO, pPin->Init.Pin);
}
//============== All related GPIO configurations basic functions done here


void IO_PinInit(void) { // later we will pass a structure of pins here
  
// make sure all GPIO are clocked
// Activate all GPIOs clocks here for now // ADAPT_HERE
  __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
      __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
//          __HAL_RCC_GPIOE_CLK_ENABLE();
            __HAL_RCC_GPIOF_CLK_ENABLE();
//              __HAL_RCC_GPIOG_CLK_ENABLE();
//                __HAL_RCC_GPIOH_CLK_ENABLE();

// Set LeftMost Detection (static, no need function yet)
// prepare the pins East/West 9 to detect the most left element on the chain and tag it as a master
}
