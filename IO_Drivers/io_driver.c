/**
  ******************************************************************************
  * @file    io_driver.c (an alternate way to manage MCU GPIOs)
  * @author  S.Ma
  * @brief   Can coexist with HAL/LL. How this driver deviate from HAL and LL?
  * 
  *  This is a different way to implement an alternative to HAL. 
  *
  *  It is assumed the pin configuration is not time critical.
  *  IO Driver uses a pin name enumerated in a package file (ds_c031_qfp48.h): PadName_t
  *  Compile fails if using a non existant pin for the selected package.
  *
  *  One API call, one pin only (one passing argument).  
  *
  *  The pin init structure becomes very small and reusable, one pin configuration, one code line...
  *  The pin default output level can be included in the init structure.
  *  There is no Alternate Function Number to memorize, it's coded in a C table:
  *  Use a Signal Name and the proper alternate function will be programmed.
  *  GPIO clock enable is auto activated when configuring the port.
  * 
  *  RawPortPads_t type uses RAM when a pin must be speedy (bit banging)
  *
  *  EXTI is also handled here.
  *  Check the application code, was HAL_GPIO before, and became IO_Pad now.
  *
  *  LL_GPIO can still be used for bitmask multiple IO of same port management. Conversion function is provided.
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
#include "main.h"
#include "brisk.h"
#include "ds_c031_qfp48.h"
#include "io_driver.h"

//ApplicationSignalName ==> Pin(s) ==> Port/bitmask


//const RawPortPad_t LEDx_AsOutput = { PadName2Port(PA5), PadName2PadMask(PA5) };
//,   { GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,    },}; // LED1 = PC7 Digital Output
//const IO_Pin_t LEDx_AsPWM = {  GPIOA,          {    GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 5,   },}; // LED3 = PE5 Digital WPM T1.1 AF5
#if 1
// use PCB signal name in the naming is allowed
const IO_Pad_t LEDtest1_AsPWM = { PA_5, {.Mode = IO_SIGNAL, .Pull = IO_NOPULL, .Drive = IO_PUSHPULL, .Speed = IO_SPEED_00, .Odr = IO_ODR_NO_INIT, .Signal = IO_AF_05 } /*TIM1_1 signal name to seach in dbase*/ };
//const IO_Pad_t LEDtest2_AsPWM = { PA5, {IO_NOPULL, IO_SIGNAL, IO_PUSHPULL, IO_SPEED_00}, IO_AF_05 /*TIM1_1 signal name to seach in dbase*/ }; // this generates a warning! better than #define version
#endif

int32_t IO_AddPadToRawPortPads(PadName_t PadName, RawPortPads_t* pRPP ) {

  uint8_t gpio_index = PadName>>4;
  // first is it a valid gpio?
  if(gpio_index>=countof(IO_GPIOx))
    return -1;
  
  uint8_t pad_position = PadName & 0x000F;
  pRPP->bfPin = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pRPP->bfPin)==0)
    return -1;
  
  // valid port, valid pin
  pRPP->pPort = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  return 0;
}

int32_t IO_ClearRawPortPads(RawPortPads_t* pRPP) {
  
  pRPP->bfPin = 0;
  pRPP->pPort = NULL;
  return 0;
}

//static uint32_t tmp, bfmask, bfval; // for debug watch purpose
int32_t IO_PadInit(IO_Pad_t* pIO ) { // this is for a single pin

  uint32_t tmp, bfmask, bfval; // for debug watch purpose
  // transformed HAL
  //uint32_t tmp, bfmask, bfval;
/* TODO use this function and adjust the code    
  RawPortPads_t RPP;
  if(IO_MakeRawPortPadsFromPin(&RPP, pIO->Name)<0)
    return -1; // wrong padname
  */
  //--- original code for checking here --- to use RRP instead
  uint8_t gpio_index = pIO->PadName>>4;
  // first is it a valid gpio?
  if(gpio_index>=countof(IO_GPIOx))
    return -1;
  
  uint8_t pad_position = pIO->PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pin_mask)==0)
    return -1;
  
  // valid port, valid pin
  GPIO_TypeDef *GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];

  uint32_t primask  = __get_PRIMASK(); // save current interrupt context
  
  // if needed, enable GPIO Clock
  while(READ_BIT(RCC->IOPENR, RCC_GPIOx_ClockEnBitMask[gpio_index])==0) {
    
    __set_PRIMASK(1);// atomic start
    SET_BIT(RCC->IOPENR, RCC_GPIOx_ClockEnBitMask[gpio_index]); 
    __set_PRIMASK(primask);// atomic end
    
    NOPs(4);
  };

  /* Configure the IO Speed */
  if(pIO->Config.Speed) {
    bfmask = ~(0x3<<(pad_position * 2));
    bfval = (pIO->Config.Speed-1) << (pad_position * 2); 
    __set_PRIMASK(1);// atomic start
    tmp = (GPIOx->OSPEEDR & bfmask) | bfval;
    GPIOx->OSPEEDR = tmp;
    __set_PRIMASK(primask);// atomic end
  }

  /* Configure the IO Output Type */  
  if(pIO->Config.Pull) {
    
    bfmask = ~(0x3<<(pad_position * 2));
    bfval = (pIO->Config.Pull-1) << (pad_position * 2);
    __set_PRIMASK(1);// atomic start
    tmp = (GPIOx->OTYPER & bfmask) | bfval;
    GPIOx->PUPDR = tmp;
    __set_PRIMASK(primask);    // atomic end
  }
  
  /* Configure the output type */
  if(pIO->Config.Drive) {

    bfmask = ~(1<<pad_position);
    bfval = (pIO->Config.Drive-1) << (pad_position);
    __set_PRIMASK(1);// atomic start
    tmp = (GPIOx->OTYPER & bfmask)|bfval;
    GPIOx->OTYPER = tmp;
    __set_PRIMASK(primask);    // atomic end
  }

  // to improve, output should be set BEFORE being an output and AFTER being an input ? to avoid glitch if mode is changing.
  // for now, refrain to set the output when asking the pin to become an input
  // also refrain to use this function to change only the ouput level, there are dedicated speedy functions for it
  if(pIO->Config.Odr) {

    bfmask = ~(1<<pad_position);
    bfval = (pIO->Config.Odr-1) << pad_position;
    __set_PRIMASK(1);// atomic start
    tmp = (GPIOx->ODR & bfmask)|bfval;
    GPIOx->ODR = tmp;
    __set_PRIMASK(primask);    // atomic end
  }
  
  /*--------------------- GPIO Mode Configuration ------------------------*/

  // analog: nothing to do here
  /* In case of Alternate function mode selection */
  if( pIO->Config.Mode == IO_SIGNAL )
  {
    int8_t Alternate = pIO->Config.Signal;
    if(Alternate > 0x0F) // if it's not a direct value [0..16] (legacy)
    {
      /* Configure Alternate function mapped with the current IO */
      // This will use a database using pin and signal to find out the AF value to program.
      DS_PAD_t const* pDS_PAD = pDS_PAD_MatchByPinAndSignal(pIO->PadName, pIO->Config.Signal); // function freeze if no solution
      if( pDS_PAD == NULL ) while(1); // freeze. issue.
      Alternate = pDS_PAD->AF;
    }
    
    bfmask = ~(0xF << ((pad_position & 0x07U) * 4U));
    bfval = (Alternate & 0x0FUL) << ((pad_position & 0x07U) * 4U);
    __set_PRIMASK(1);// atomic start
    tmp = (GPIOx->AFR[pad_position >> 3U] & bfmask)|bfval;
    GPIOx->AFR[pad_position >> 3U] = tmp;
    __set_PRIMASK(primask);// atomic end
  }
  
  if(pIO->Config.Mode) {

    bfmask = ~(0x3<<(pad_position * 2));
    bfval = (pIO->Config.Mode-1) << (pad_position * 2);
    __set_PRIMASK(1); // atomic start
    tmp = (GPIOx->MODER & bfmask )|bfval;
    GPIOx->MODER = tmp;
    __set_PRIMASK(primask);//atomic end
    // atomic end
  }
 
  return 0;
}

int32_t IO_PadDeInit(PadName_t  PadName) {
  
  IO_Pad_t DeInitPad = { PadName, {.Mode=IO_ANALOG, .Pull=IO_NOPULL, .Drive=IO_PUSHPULL, .Speed=IO_SPEED_00, .Signal = IO_AF_00 } }; // stack based local
  return IO_PadInit( &DeInitPad ); // deinit register sequence could be inverted. To check should be mode dependent?
  
  /*------------------------- EXTI Mode Configuration --------------------*/
  /*------------------------- GPIO Mode Configuration --------------------*/
  /* Configure IO in Analog Mode */
  /* Configure the default Alternate Function in current IO */
  /* Configure the default value for IO Speed */
  /* Configure the default value IO Output Type */
  /* Deactivate the Pull-up and Pull-down resistor for the current IO */
}

int32_t IO_PadGet(PadName_t  PadName)
{
  return (IO_GPIOx[(PadName>>4)&7]->IDR)>>(PadName & 0x0F)&1;
}


int32_t IO_PadSetHigh(PadName_t PadName) {
  //GPIOx->BSRR = (uint32_t)GPIO_Pin;
  
  uint8_t gpio_index = PadName>>4;
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  
  GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  GPIOx->BSRR = pin_mask; // atomic
  return 0;
}

int32_t IO_PadSetLow(PadName_t PadName) {
  //GPIOx->BRR = (uint32_t)GPIO_Pin;
  uint8_t gpio_index = PadName>>4;
  uint8_t pad_position = PadName & 0x000F;
  
  GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  uint32_t pin_mask = 1<<pad_position;
  GPIOx->BRR = pin_mask; // atomic
  return 0;
}

int32_t IO_PadSet(PadName_t  PadName, GPIO_PinState PinState) {
  if (PinState != GPIO_PIN_RESET)
  {
    return IO_PadSetLow(PadName);
  }
  else
  {
    return IO_PadSetHigh(PadName);
  }
}

int32_t IO_PadToggle(PadName_t  PadName) {
  //uint32_t bfmask, bfvalue;
  uint8_t gpio_index = PadName>>4;
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  
  GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  GPIOx->BSRR = pin_mask; // atomic
  return 0;
}

#if 0
HAL_StatusTypeDef IO_PadLock(PadName_t  PadName) {
  
  __IO uint32_t tmp = GPIO_LCKR_LCKK;

  /* Check the parameters */
  assert_param(IS_GPIO_LOCK_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Apply lock key write sequence */
  tmp |= GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Reset LCKx bit(s): LCKK='0' + LCK[15-0] */
  GPIOx->LCKR = GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Read LCKK bit*/
  tmp = GPIOx->LCKR;

  /* read again in order to confirm lock is active */
  if ((GPIOx->LCKR & GPIO_LCKR_LCKK) != GPIO_LCKR_LCKK)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
}
#endif

int32_t IO_CreateRawPortPadsFromPads(RawPortPads_t* pRPP, PadName_t* pPadNames) {
  
  RawPortPads_t RPP = { NULL, 0 };
  uint8_t count=0;
   
  do{
    
    if(IO_AddPadToRawPortPads(*pPadNames, &RPP)<0)
      return -1; // invalid pins
    
    // valid pad here
    if(RPP.pPort == NULL) 
    { // first valid pad to set the variable
      pRPP->pPort = RPP.pPort;
      pRPP->bfPin = RPP.bfPin;
    }
    else
    {
      if(pRPP->pPort != RPP.pPort)
        return -1; // error, not all pins are from the same port, stop, skip?
      pRPP->bfPin |= RPP.bfPin; // add the pin to the port bitmask
    }
    
    pPadNames++; // next element
  }  
  while(count++<16); // max 16 PadNames.

  return 0;
}

#include "stm32c0xx_ll_gpio.h"
void IO_RawPadSetHigh(RawPortPads_t* RawPad) {
  
  if(RawPad==NULL)  while(1) {};
  LL_GPIO_SetOutputPin(RawPad->pPort, RawPad->bfPin);
}

void IO_RawPadSetLow(RawPortPads_t* RawPad) {
  
  if(RawPad==NULL)  while(1) {};
  LL_GPIO_ResetOutputPin(RawPad->pPort, RawPad->bfPin);
}

uint32_t IO_RawPadGet(RawPortPads_t* RawPad) {
  
  if(RawPad==NULL)  while(1) {};
  return LL_GPIO_IsInputPinSet(RawPad->pPort, RawPad->bfPin);
}


// EXTI is independent of the IO configuration, 16 input lines of EXTI can come from IO Port/pin.
// for legacy purpose, we put here the function which added to IO_INIT() will have equivalent functionality to HAL_GPIO_Init()
int32_t IO_EXTI_Config( PadName_t PadName, EXTI_Config_t EXTI_Config) {

  uint8_t gpio_index = PadName>>4;
  // first is it a valid gpio?
  if(gpio_index>=countof(IO_GPIOx))
    return -1;
  
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pin_mask)==0)
    return -1;
  
  uint8_t port_offset = PadName >> 4;
  // valid port, valid pin
//  GPIO_TypeDef *GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  uint32_t primask  = __get_PRIMASK(); // save current interrupt context
  
  // EXTI_IO_MUX_SETUP
  volatile uint8_t* pu8 = (uint8_t *)&EXTI->EXTICR[0]; // we switch to byte so we have an array of 16 bytes for bit 0..15, simple with byte access!
  pu8[pad_position] = port_offset; // no need to mask interrupt as it's a byte write, no read modify write here
// old scheme  
//  tmp = EXTI->EXTICR[position >> 2U];
//  tmp &= ~((0x0FUL) << (8U * (position & 0x03U)));
//  tmp |= (GPIO_GET_INDEX(GPIOx) << (8U * (position & 0x03U)));
//  EXTI->EXTICR[position >> 2U] = tmp;

  /* Clear EXTI line configuration */
  uint32_t tmp, bfmask, bfval; // for debug watch purpose
  
  bfmask = ~pin_mask;
  
  __set_PRIMASK(1);// atomic start
//+ here we clear all pending edge detection  
  EXTI->RTSR1 = bfmask;
  EXTI->FTSR1 = bfmask;
  __set_PRIMASK(primask);// atomic end
  // leave here interrupt opportunity to minimize jitter...
  
  __set_PRIMASK(primask);// atomic start
  
  bfval = (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_INTERRUPT) ? 1 : 0;
  __set_PRIMASK(1);// atomic start
  tmp = EXTI->IMR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->IMR1 = tmp;
  __set_PRIMASK(primask);// atomic start
//  tmp = EXTI->IMR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_INTERRUPT)
//    tmp |= pin_mask;
//  EXTI->IMR1 = tmp;
  
  bfval = (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_EVENT) ? 1 : 0;
  __set_PRIMASK(1);// atomic start
  tmp = EXTI->EMR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->EMR1 = tmp;
  __set_PRIMASK(primask);// atomic start
//  tmp = EXTI->EMR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_EVENT)
//    tmp |= pin_mask;
//  EXTI->EMR1 = tmp;

  /* Clear Rising Falling edge configuration */
  
  bfval = (EXTI_Config.Edge & EXTI_RISING_EDGE) ? 1 : 0;
  __set_PRIMASK(1);// atomic start
  tmp = EXTI->RTSR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->RTSR1 = tmp;
  __set_PRIMASK(primask);// atomic start
//  tmp = EXTI->RTSR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Edge & EXTI_RISING_EDGE)
//    tmp |= pin_mask;
//  EXTI->RTSR1 = tmp;

  bfval = (EXTI_Config.Edge & EXTI_FALLING_EDGE) ? 1 : 0;
  __set_PRIMASK(1);// atomic start
  tmp = EXTI->FTSR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->FTSR1 = tmp;
  __set_PRIMASK(primask);// atomic start
//  tmp = EXTI->FTSR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Edge & EXTI_FALLING_EDGE)
//    tmp |= pin_mask;
//  EXTI->FTSR1 = tmp;

  return 0;
}



int32_t IO_EXTI_DeInit(PadName_t PadName) {

  uint8_t gpio_index = PadName>>4;
  // first is it a valid gpio?
  if(gpio_index>=countof(IO_GPIOx))
    return -1;
  
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pin_mask)==0)
    return -1;
  
//  uint8_t port_offset = PadName >> 4;
  // valid port, valid pin
//  GPIO_TypeDef *GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  uint32_t primask  = __get_PRIMASK(); // save current interrupt context

  __set_PRIMASK(1);// atomic start
  /* Clear EXTI line configuration */
  EXTI->IMR1 &= ~pin_mask;
  EXTI->EMR1 &= ~pin_mask;

  /* Clear Rising Falling edge configuration */
  EXTI->RTSR1 &= ~pin_mask;
  EXTI->FTSR1 &= ~pin_mask;

  // clear the latch in case it's already armed.
  EXTI->RTSR1 |= pin_mask;
  EXTI->FTSR1 |= pin_mask;
  __set_PRIMASK(primask);// atomic start
  
  return 0;
}