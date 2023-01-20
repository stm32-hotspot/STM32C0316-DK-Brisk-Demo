/**
  ******************************************************************************
  * @file    io_driver.h (Alternate way to implement IO APIs to make things more intuitive)
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
#ifndef IO_DRIVER_H
#define IO_DRIVER_H

/*
#define ATOMIC_SET_BIT(REG, BIT)                             \
  do {                                                       \
    uint32_t primask;                                        \
    primask = __get_PRIMASK();                               \
    __set_PRIMASK(1);                                        \
    SET_BIT((REG), (BIT));                                   \
    __set_PRIMASK(primask);                                  \
  } while(0)
*/

// Speed critical are Set/Reset/Read/Toggle and should be atomic
// Speed means pin and port must be set already
// If NULL, it will be set from its enum name which must be valid
// should we group all GPIOs are one?

typedef enum {
  IO_PULL_NO_INIT, // if the field is not initialized, it will be 0x00
  IO_NOPULL,
  IO_PULLUP,
  IO_PULLDOWN,
  IO_PULL_MAX,
} IO_Pull_t;

typedef enum {
  IO_MODE_NO_INIT,
  IO_INPUT,
  IO_OUTPUT,
  IO_SIGNAL,
  IO_ANALOG,
  IO_MODE_MAX,
} IO_Mode_t;

// speed is direct...
typedef enum {
  IO_DRIVE_NO_INIT,  
  IO_PUSHPULL,
  IO_OPENDRAIN,
  IO_DRIVE_MAX,
} IO_Drive_t;

typedef enum {
  IO_SPEED_NO_INIT,
  IO_SPEED_00,
  IO_SPEED_01,
  IO_SPEED_02,
  IO_SPEED_03,
  IO_SPEED_MAX,
} IO_Speed_t;

typedef enum {  
  IO_ODR_NO_INIT,
  IO_ODR_LOW,
  IO_ODR_HIGH,
} IO_Odr_t;

// This is the basic structure to describe port + pin(s) for LL when optimising
// some helper functions to convert pads to rawportpads
typedef struct {
   GPIO_TypeDef* pPort;
   uint16_t bfPin;
} RawPortPads_t; // only one pin

typedef enum {
  EXTI_NO_EDGE = 0,
  EXTI_RISING_EDGE = 1,
  EXTI_FALLING_EDGE = 2,
  EXTI_BOTH_EDGES = 3 
} EXTI_Edge_t;
    
typedef enum { 
  EXTI_CHANNEL_NOTRIGGER = 0,
  EXTI_CHANNEL_TRIGGER_INTERRUPT = 1, // bit field use
  EXTI_CHANNEL_TRIGGER_EVENT = 2, // bit field use
  EXTI_CHANNEL_TRIGGER_ALL = 3 // bit field use
} EXTI_Channel_Trigger_t;

typedef struct  { 
// in increasing bit position 0.... 31
  EXTI_Edge_t Edge : 2; // 
  EXTI_Channel_Trigger_t Trigger : 2; // static
} EXTI_Config_t; // should fit a single 32 bit core register.


int32_t IO_AddPadToRawPortPads(PadName_t PadName, RawPortPads_t* pRPP ); // convert HAL pin to pins for LL allowing multiple PortPads operations
int32_t IO_ClearRawPortPads(RawPortPads_t* pRPP); // we could create a "pad string" with termination character for simple passing parameter
int32_t IO_CreateRawPortPadsFromPads(RawPortPads_t* pRPP, PadName_t* pPadNames);
// daisy chain = string of portpads?
// To identify pad or pads, we need the port base address and a 16 bit mask
//#define PadName2PadMask(P) (((uint32_t) 1)<<(P & 0x0F)) // this can be replaced by a ROM LUT (wait state dependent except ROM)
//#define PadName2Port(P) Ports[(P>>4)&0x07]
//#define Pad2PortPin(P)  PadName2Port(P), PadName2PadMask(P)
// writing group of pins simultaneously is key
// application group of pin is important

typedef struct
  { // in increasing bit position 0.... 31
    IO_Mode_t Mode : 3; // 
    IO_Pull_t Pull : 3; // static
    IO_Drive_t Drive : 2; // static
    IO_Speed_t Speed : 3; // static
//    uint32_t Alternate : 5; // next byte
    IO_Odr_t Odr : 2; // default output level register
    STM32_SignalName_t Signal; // for alternate functions
//    uint8_t Remapped : 4; // swap pin position, swap RxTx swap MisoMosi (this requires special handling, not provided by default, refer to peripheral control bit affecting the pad configuration)
//    uint32_t MultiBondedGroup : 3; // up to 16 groups of multibonding functional IO (STM32C0)
}PadConfig_t; // less than 32 bits

/*
const IO_Pad_t MyBoardSignal = { PA_5, {.Mode = IO_, .Pull = IO_, .Drive = IO_, .Speed = IO_SPEED_, .Odr = IO_}, 0 };
*/

typedef struct { // This can be placed in RAM
  PadName_t PadName; // 
//  RawPortPad_t* pRPP;
  PadConfig_t Config; //[4] Same config for one or more pins
  //STM32_SignalName_t Signal; // for alternate functions
} IO_Pad_t;
/*
typedef struct {
  Pins_t Pins;
  char* sPadName;//[4] Optional
  char* sSignalName;// Optional application
} InfoPins_t;  

int32_t AddPadToPortPads(PadName_t PadName, PortPads_t* pPortPads);
*/

int32_t IO_PadInit(IO_Pad_t  *pIO_Pad);// intuitive
int32_t IO_PadDeInit(PadName_t PadName); // intuitive
int32_t IO_PadGet(PadName_t PadName); // thread safe, speed
int32_t IO_PadSetHigh(PadName_t PadName); // thread safe, speedy
int32_t IO_PadSetLow(PadName_t PadName); // thread safe, speedy

int32_t IO_PadSet(PadName_t PadName, GPIO_PinState PinState); // thread safe, speedy
int32_t IO_PadToggle(PadName_t PadName); // thread safe, speedy
HAL_StatusTypeDef IO_PadLock(PadName_t PadName);
//void HAL_IO_EXTI_IRQHandler(PadName_t PadName); // moved to EXTI driver
//void HAL_IO_EXTI_Rising_Callback(PadName_t PadName); 
//void HAL_IO_EXTI_Falling_Callback(PadName_t PadName);


void IO_RawPadSetHigh(RawPortPads_t* RawPad);
void IO_RawPadSetLow(RawPortPads_t* RawPad);
uint32_t IO_RawPadGet(RawPortPads_t* RawPad);

int32_t IO_EXTI_Config(PadName_t PadName, EXTI_Config_t EXTI_Config);
int32_t IO_EXTI_DeInit(PadName_t PadName);

#endif
