/**
  ******************************************************************************
  * @file    add_on_board.h
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
#ifndef _ADD_ON_BOARD_H_
#define _ADD_ON_BOARD_H_


#include "main.h"
#include "brisk.h"

typedef enum { // list of the supported (precompiled) add-on boards
  
  UNDETECTED_ADD_ON_BOARD, // 0
  BLANK_ADD_ON_BOARD, // 1
  UNKNOWN_ADD_ON_BOARD, // 2
  B_HMI_LCD16x2_BOARD, // 3
  B_CELL_UG96_BOARD, // 4
  B_CELL_BG96_BOARD, // 5
  B_SDCARD_PMOD_BOARD, // 6
  B_MC_CONN34_BOARD, // 7
  B_OPTO_INERT_BOARD, // 8
  B_ANALOG_ENV_BOARD, // 9
  B_GP_ENV_BOARD, // 10
  B_AUDIO_WM_BOARD, // 11
  B_LSHIFT_GP_BOARD, // 12
  B_FAN_OUT2_BOARD, // 13
  B_WIFI_EMW3080_BOARD, // 14
  B_ENV_GAS_V2_BOARD, // 15+
  B_RS485_DCDC_BOARD, // 16
  STM32WL55C_DK1_BOARD, // 17
  ADD_ON_ID_COUNT // 18
} Add_on_ID_t;


typedef struct {
  
  EEP_t* pEEP;
  uint32_t IsPresentCounter_50msec;
  uint8_t JustPlugged;
  uint8_t IsPlugged; // set when confirmed plugged and valid
  uint8_t JustUnplugged; // set as soon as unplugged
  
  char StringID[32]; // This contains the name of the ADD-on for this add-on (match/mismatch/confirm/infirm)
/*  
  uint32_t (*fnPlugged)(uint32_t);
  uint32_t ctPlugged;
  
  uint32_t (*fnUnplugged)(uint32_t);
  uint32_t ctUnplugged;

  uint32_t (*fnProcess_50msec)(uint32_t);
  uint32_t ctProcess_50msec;

  uint32_t (*fnProcess_Polling)(uint32_t);
  uint32_t ctProcess_Polling;
*/  
  Add_on_ID_t ID_Code;
  
} ADD_ON_BOARD_t;

void Add_on_Board_Init(ADD_ON_BOARD_t* pA);

void Add_on_Board_50msec(ADD_ON_BOARD_t* pA);
void Add_on_Board_1sec(ADD_ON_BOARD_t* pA);
uint8_t Add_on_BoardConfigure(ADD_ON_BOARD_t* pA);

void Add_on_board_LPTIM1_Tick(ADD_ON_BOARD_t* pA);

extern ADD_ON_BOARD_t STModADD_On; // Manage the hot plug feature

//void STModDeactivateSignals(void);

void STModp_DeInit(void); // disable all pins connected to STMod+ except I2C lines

int32_t Add_on_BoardReport(ADD_ON_BOARD_t* pA);
int32_t AddOnPaneProcess(ADD_ON_BOARD_t* pA);
int32_t Add_on_Board_SetPanel(ADD_ON_BOARD_t* pA);

// MEMS COMMON TYPES DECLARED HERE ONCE FOR ALL
#define MEMS_SHARED_TYPES

/**
  * @defgroup axisXbitXX_t
  * @brief    These unions are useful to represent different sensors data type.
  *           These unions are not need by the driver.
  *
  *           REMOVING the unions you are compliant with:
  *           MISRA-C 2012 [Rule 19.2] -> " Union are not allowed "
  *
  * @{
  *
  */

typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

typedef union{
  int16_t i16bit;
  uint8_t u8bit[2];
} axis1bit16_t;

typedef union{
  int32_t i32bit[3];
  uint8_t u8bit[12];
} axis3bit32_t;

typedef union{
  int32_t i32bit;
  uint8_t u8bit[4];
} axis1bit32_t;

/**
  * @}
  *
  */

typedef struct{
  uint8_t bit0       : 1;
  uint8_t bit1       : 1;
  uint8_t bit2       : 1;
  uint8_t bit3       : 1;
  uint8_t bit4       : 1;
  uint8_t bit5       : 1;
  uint8_t bit6       : 1;
  uint8_t bit7       : 1;
} bitwise_t;

#define PROPERTY_DISABLE                (0U)
#define PROPERTY_ENABLE                 (1U)

// END OF MEMS COMMON DECLARATION




//#include "B_HMI_LCD16x2.h"
//#include "B_SDCARD_PMOD.h"
//#include "B_MC_CONN34.h"
#include "B_OPTO_INERT.h"
//#include "B_ANALOG_ENV.h"
//#include "B_AUDIO_WM.h"
//#include "B_LSHIFT_GP.h"
//#include "B_FAN_OUT2.h"
//#include "B_RS485_DCDC.h"
//#include "STM32WL55C_DK1.h"


#endif
