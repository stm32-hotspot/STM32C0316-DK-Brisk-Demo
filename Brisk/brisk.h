/**
  ******************************************************************************
  * @file    brisk.h
  * @author  S.Ma
  * @brief   VERY IMPORTANT AS IT IS A CONFIGURATION HEADER FILE FOR THE WHOLE APP
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

// configuration file for the project
// enable global application features
// converted from L4R5 to C031
// due to small flash/ram, activate only needed features that fits memory

//- : removed code
//+ : added code
//> : to be activated when needed by coder

// This is the board application functionality description as a built global structure

#ifndef _BRISK_H_
#define _BRISK_H_

#include "commons.h"

#define TRIM_DBG 32768 // frequency in Hz

#define SPL_ENABLED 0

#ifdef SPL_ENABLED
#include "ds_c031_qfp48.h"
#include "io_driver.h"
#endif

typedef struct {
  int32_t gravity_x100;
  int32_t roll_deg;
  int32_t pitch_deg;
} Motion3D_t; // gravity vector from Accelerometer (hand tilt the board)

typedef struct {
  int32_t uT;
  int32_t roll_deg;
  int32_t pitch_deg;
} Magneto3D_t; // magnetic vector from Magnetometer (use of fridge magnet)

// extern Motion3D_t Motion3D;   // not populated = no global
// extern Magneto3D_t Magneto3D; // not populated = no global

// the order of includes is IMPORTANT
// it is not a simple TREE order

// when you migrate this code to a new application, import files in the below order.

#include "u8fifo.h" // sw fifo to exchange bytes between interrupts and/or main

#include "sif.h"

//#include "io_pins.h"  // define io_pin_t and helper functions (groups GPIO with pin properties as single pointable entity)
#include "leds.h"     // uses io_pins.h

#include "i2c_masterIO.h" // SW bit bang I2C, uses io_pins.h
#include "spi_masterIO.h" // SW bit bang SPI, uses io_pins.h

#include "M24256D.h" // I2C EEPROM from STMod+ add-on board to perform hot plug and play and detection (OTP page, 64 bytes), uses i2c_master.io
#include "add_on_board.h" // plug and play STMod+ add-on board monitoring and hot IO configuration, uses M24256D.h

#include "analog.h" // ADC voltage and temperature monitoring, scan channels

void Brisk_PostInit(void); // this is to override any CubeMX initialisation in main()
void Brisk_Polling(void); // inserted in main's while loop (bare metal)


// Simple timebase flags, set by ISR, cleared by main loop.
extern uint8_t Brisk_5ms_Flag; // set by ISR, cleared by main
extern uint8_t Brisk_10ms_Flag; 
extern uint8_t Brisk_50ms_Flag;
extern uint8_t Brisk_100ms_Flag;
extern uint8_t Brisk_1sec_Flag;

void BriskI2C_10msecTick(void); // process 10msec
void Brisk_1ms_ISR(void); // plugs in 1ms ISR

// LPTIM1 used for SW period event in us units
void LPTIM1_Init(void);
void LPTIM1_SetTick_us(uint32_t delay_us); // if delay is 0 then timer will be disabled.

extern int32_t UserKeyPressed;
int32_t UserKeyInit(void);
int32_t UserKey_50ms(void);

// built-in buzzer
void LPTIM1_SetPWM_us(uint32_t delay_us); // LPTIM1_OUT being used


//===== HERE IS THE HW TREE

typedef struct {
  IO_Pad_t* pPin;
  char codebefore;
  char codenow;
  char /*int32_t*/ Pressed;
  char /*int32_t*/ Pressed_5sec;
  int32_t PressedCount_50ms;
} UserKey_t;

typedef struct { // regroup all the HW elements here, easier to open this structure on watch debug window
  int32_t version;
  UserKey_t* pUserKey;
//  buzzer_t* pLocalBuzzer;
 TimedLED_t* pClockLEDs[LED_COUNT];

  Analog_t* pAnalog;
#if 0  
  Motion3D_t* pMotion3D; // this is the accelerometer application data
  Magneto3D_t* pMagneto3D; // this is the magnetometer
#endif  
ADD_ON_BOARD_t* pSTModpADD_On;
#ifdef _B_OPTO_INERT_BOARD_   
OI_AddOn_t* pOI_AddOn; // may not be plugged right now, first check what add-on is plugged before accessing a live data, otherwise, it's last use.
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  AE_AddOn_t* pAE_AddOn;
#endif  
  
} Brisk_t;

extern Brisk_t Brisk;
extern Brisk_t* pBrisk;

/*
typedef struct {
  
  int32_t value;
  int32_t errorcode;
} i32_err_t;

i32_err_t sebtestcompiler(int32_t a, int32_t b);
*/
#endif