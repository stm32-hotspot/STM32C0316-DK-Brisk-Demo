/**
  ******************************************************************************
  * @file    B_HMI_LCD16x2.h ()
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

#ifndef _B_HMI_LCD16x2_H_
#define _B_HMI_LCD16x2_H_

#include "lcd_16x2.h"
#include "joystick.h"

// This is the STMod+ LCD 16x2 char + 5 way digital joystick board element
//================= global structure

typedef struct {

//  uint8_t MenuDepth;
//  uint8_t PageLast;
//  uint8_t PageNow;
  uint8_t SpecialMenuEnabled;
  uint8_t LCD_UpdateRequested;
  int32_t IndexVertical; // from 0 to Max
  int32_t IndexVerticalMax;
  int32_t IndexHorizontal;
  int32_t IndexHorizontalMax;
//  uint8_t GUI_ExitIndex; // Points to the previous menu, if it exist (non null) for convenience when menus are relayouted, back non-hardcoded
} GUI_t;

typedef enum {
  HMI_MEMVIEW_UNCHANGED = 0,
  HMI_MEMVIEW_REDRAW, // display change without register update
  HMI_MEMVIEW_READ, // write
  HMI_MEMVIEW_TOGGLE, // read
  HMI_MEMVIEW_EXIT, // exit (if needed)
} HMI_MEMVIEW_EVENT_t;

typedef struct {
  uint32_t SubAdr;
  uint32_t SubAdrMin;
  uint32_t SubAdrMax;
  uint8_t Register;
  signed char Bitpos;  
} MemView_t;

typedef struct {
  
  int32_t BoardVersion;
  
  LCD_16x2_t LCD_16x2;  
  JOYSTICK_t JOY;
  ADD_ON_BOARD_t* pADD; // Manage the hot plug feature
  GUI_t* pGUI;
  GUI_t GUIs[16]; // we have up to 8 menus to manage (including sub menus)
  MemView_t* pMemView; // if defined, replaces pGUI
  MemView_t MemView[8]; // 256 bytes array view and edit
  
} HM_AddOn_t;

extern HM_AddOn_t HM_AddOn; // this is the default item

int32_t HL_Init(void);
void HL_Test(void);
int32_t HL_50ms(void);
int32_t HL_1sec(void);
int32_t HL_IsPlugged(void);
int32_t HL_IsUnplugged(void);


/////////////////////////////////////////////////////////////////
// special function to view some group of registers and edit them
// Facilities to view/edit a memory of up to 256 bytes


uint32_t LCD_16x2_MemView_u8_to_hexa_bitfield_as_u32(uint8_t byte);
void LCD_16x2_MemView_Show(HM_AddOn_t* pADD, uint8_t Reg);
uint8_t Add_LCD_Key_MemViewHandler(HM_AddOn_t* pADD);

int32_t GUI_50ms(HM_AddOn_t* pADD);

uint8_t GUI_Manage_UpDownKeys(HM_AddOn_t* pADD);

uint8_t GUI_Manage_TopMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_TopMenu(HM_AddOn_t* pADD);
uint8_t GUI_Manage_STM32_Menu(HM_AddOn_t* pADD);
uint8_t GUI_Render_STM32_Menu(HM_AddOn_t* pADD);
uint8_t GUI_Manage_BriskMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_BriskMenu(HM_AddOn_t* pADD);
uint8_t GUI_Manage_AddonMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_AddonMenu(HM_AddOn_t* pADD);
uint8_t GUI_Manage_ArduinoMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_ArduinoMenu(HM_AddOn_t* pADD);
uint8_t GUI_Manage_SPIpMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_SPIpMenu(HM_AddOn_t* pADD);
uint8_t GUI_Manage_ServiceMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_ServiceMenu(HM_AddOn_t* pADD);

extern u8fifo_t * pHL_Report;
int32_t HL_Report(void);

int32_t HL_SendDashboard(void);


#endif

