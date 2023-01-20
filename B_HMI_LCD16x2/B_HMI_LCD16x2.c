/**
  ******************************************************************************
  * @file    B_HMI_LCD16x2.c (STMod+ add-on board with 2x16 char LCD display with backlight and 5 way JOYstock/Rock-switch)
  * @author  S.Ma
  * @brief   Due to STM32C0 limited pincount, the rock switch is not used, the STM32C0316-DK analog rock switch could be used instead.
  *          The navigation of a default menu is using the up/down for navigating, left/right adjust a value, select (push in) to validate  
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

#ifdef _B_HMI_LCD16x2_H_

uint8_t GUI_Manage_XBarSubMenu(HM_AddOn_t* pADD);
uint8_t GUI_Render_XBarMenu(HM_AddOn_t* pADD);
#if 0
//I2C_MasterIO_t gI2C_STMod;
// LCD related pinout (Brisk*)
IO_Pin_t LCD_RST_STMod = {  GPIOC,          {    GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_RS_STMod = {  GPIOA,          {    GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_CS_STMod = {  GPIOA,          {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_SCL_STMod = {  GPIOA,          {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_SI_STMod = {  GPIOA,          {    GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_BL_STMod = {  GPIOA,          {    GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_BLPWM_STMod = {  GPIOA,          {    GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, 0,   } }; // T2.1 if available

// patch for strange contrast when the signals are push pull...
IO_Pin_t LCD_RSToff_STMod = {  GPIOC,          {    GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_RSoff_STMod = {  GPIOA,          {    GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_CSoff_STMod = {  GPIOA,          {    GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_SCLoff_STMod = {  GPIOA,          {    GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t LCD_SIoff_STMod = {  GPIOA,          {    GPIO_PIN_2, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };

// Joystick related
IO_Pin_t JOY_UP_STMod = {  GPIOD,          {    GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t JOY_DOWN_STMod = {  GPIOA,          {    GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t JOY_LEFT_STMod = {  GPIOB,          {    GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t JOY_RIGHT_STMod = {  GPIOA,          {    GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t JOY_SEL_STMod = {  GPIOD,          {    GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
#endif


//I2C_MasterIO_t gI2C_STMod;
// LCD related pinout (Brisk*)
IO_Pad_t LCD_RST_STMod =    {  PC_15, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_PULLUP, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH}, 0 };
IO_Pad_t LCD_RS_STMod =     {  PA_7,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH}, 0 };
IO_Pad_t LCD_CS_STMod =     {  PA_0,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH}, 0 };
IO_Pad_t LCD_SCL_STMod =    {  PA_1,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH}, 0 };
IO_Pad_t LCD_SI_STMod =     {  PA_2,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH}, 0 };
IO_Pad_t LCD_BL_STMod =     {  PA_5,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH}, 0 };
IO_Pad_t LCD_BLPWM_STMod =  {  PA_5,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = GPIO_NOPULL, .Speed = IO_SPEED_02, .Odr = IO_ODR_HIGH}, 0 }; // T2.1 if available

// patch for strange contrast when the signals are push pull...
IO_Pad_t LCD_RSToff_STMod = {  PC_15, { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t LCD_RSoff_STMod =  {  PA_7,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t LCD_CSoff_STMod =  {  PA_0,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t LCD_SCLoff_STMod = {  PA_1,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t LCD_SIoff_STMod =  {  PA_2,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };

// Joystick related
IO_Pad_t JOY_UP_STMod =     {  PD_1,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t JOY_DOWN_STMod =   {  PA_4,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t JOY_LEFT_STMod =   {  PB_0,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t JOY_RIGHT_STMod =  {  PA_6,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };
IO_Pad_t JOY_SEL_STMod =    {  PD_0,  { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Odr = IO_ODR_HIGH }, 0 };



extern EEP_t STModM24256D; // this EEP is the one plugged as add-on: It may morph by hot plug, remains a single one on the bus

HM_AddOn_t HM_AddOn = {  // here is the constructor :-)  
  .LCD_16x2 = {&LCD_RST_STMod, &LCD_RS_STMod, &LCD_CS_STMod, &LCD_SCL_STMod, &LCD_SI_STMod, &LCD_BL_STMod, 
    &LCD_RSToff_STMod, &LCD_RSoff_STMod, &LCD_CSoff_STMod, &LCD_SCLoff_STMod, &LCD_SIoff_STMod, // SW patch from strange HW behaviour 
  20, 0}, // LCD
  .JOY = {&JOY_UP_STMod, &JOY_DOWN_STMod, &JOY_LEFT_STMod, &JOY_RIGHT_STMod, &JOY_SEL_STMod}, // JOY
  .pADD = {&STModADD_On},
};

int32_t HL_Init(void) {
  HM_AddOn.GUIs[0].IndexVerticalMax = 6; // top
  HM_AddOn.GUIs[1].IndexVerticalMax = 3; // service
  HM_AddOn.GUIs[2].IndexVerticalMax = 12; // stm32
  HM_AddOn.GUIs[3].IndexVerticalMax = 11; // brisk
  HM_AddOn.GUIs[4].IndexVerticalMax = 3; // SPIp
  HM_AddOn.GUIs[5].IndexVerticalMax = 3; // Arduino
  HM_AddOn.GUIs[6].IndexVerticalMax = 3; // Add-on
  HM_AddOn.GUIs[7].IndexVerticalMax = 0; // TBD
  HM_AddOn.MemView[0].SubAdrMax = 0xFF; // STM32 Vreg
  HM_AddOn.MemView[1].SubAdrMax = 0x1F; // Brisk EEP ID
  HM_AddOn.MemView[2].SubAdrMax = 0xFF; // Brisk EEP User
  HM_AddOn.MemView[3].SubAdrMax = 0xFF; // MEMs Regs
  HM_AddOn.MemView[4].SubAdrMax = 0xFF; 
  HM_AddOn.MemView[5].SubAdrMax = 0x1F; // Addon EEP ID
  HM_AddOn.MemView[6].SubAdrMax = 0xFF; // Addon EEP User
  HM_AddOn.MemView[7].SubAdrMax = 0xFF;
  return 0;
}


uint8_t GUI_Process_AddonMemView(HM_AddOn_t* pADD, HMI_MEMVIEW_EVENT_t ActionType);

uint8_t Add_LCD_Key_MemViewHandler(HM_AddOn_t* pADD){
  
  JOYSTICK_t* pJOY = &pADD->JOY;
  
  if(JOY_LeftLongPressed(pJOY)) { // exit memview by long press left key
    return HMI_MEMVIEW_EXIT;
  };

  if(JOY_UpRepeated(pJOY)) {
    if(pADD->pMemView->SubAdr==pADD->pMemView->SubAdrMin) return 0; // unchanged
    if(pADD->MemView->SubAdr<pADD->pMemView->SubAdrMin)  TrapError(); // error
    pADD->pMemView->SubAdr--;
    return HMI_MEMVIEW_READ; // update display
  }
  
  if(JOY_DownRepeated(pJOY)) {
    if(pADD->pMemView->SubAdr==pADD->pMemView->SubAdrMax) return 0; // unchanged
    if(pADD->pMemView->SubAdr>pADD->pMemView->SubAdrMax)  TrapError(); // error
    pADD->pMemView->SubAdr++;
    return HMI_MEMVIEW_READ; // update display
  }
  
  if(JOY_LeftRepeated(pJOY)) {
    if(pADD->pMemView->Bitpos==7) return 0;// unchanged
    if(pADD->pMemView->Bitpos>7) TrapError();// error
    pADD->pMemView->Bitpos++;
    return HMI_MEMVIEW_REDRAW;
  }
  
  if(JOY_RightRepeated(pJOY)) {
    if(pADD->pMemView->Bitpos==0) return 0;// unchanged
    pADD->pMemView->Bitpos--;
    return HMI_MEMVIEW_REDRAW;
  }
  
  if(JOY_SelectPressed(pJOY)) { // toggle the selected register bit
    return HMI_MEMVIEW_TOGGLE; // toggle the bit (read modify write)
  }
  
  return HMI_MEMVIEW_UNCHANGED;
}


void HL_Test(void) {
    
  uint8_t Mem[256];
  //uint8_t Reg;
  uint16_t i;
  HM_AddOn_t* pADD = &HM_AddOn;  
  
  for(i=0;i<sizeof(Mem);i++)
    Mem[i] = i;
                  
  LCD_16x2_Init(&pADD->LCD_16x2);
  //LCD_16x2_Test();
  JOY_Init(&pADD->JOY);
//  LCD_16x2_MemView_Init(pADD, 0x80);
  while(1) {
    HAL_Delay(50);    

    JOY_50ms(&pADD->JOY);
    switch(Add_LCD_Key_MemViewHandler(pADD)) {
    case HMI_MEMVIEW_EXIT: NOPs(1);break; // exit
    case HMI_MEMVIEW_UNCHANGED: break; // nothing to do
#if 0    
    case HMI_MEMVIEW_TOGGLE: Mem[pADD->MemView_SubAdr] ^= (1<< pADD->MemView_Bitpos); // and refresh next
    case HMI_MEMVIEW_READ:
    case HMI_MEMVIEW_REDRAW:
      LCD_16x2_ClearScreen_RAM(&pADD->LCD_16x2);
      LCD_16x2_MemView_Show(pADD, Mem[pADD->MemView_SubAdr]);
      LCD_16x2_RAM_to_Display(&pADD->LCD_16x2);
      break;
#endif      
    default: TrapError();
    };
  }
}

//=================================================================
// list of the memory views
// 0: STM32 Register  View 
// 1: Brisk EEPROM ID View
// 2: Brisk EEPROM    View
// 3: Brisk BCM       View
// 4: Brisk LSM       View
// 5: Add-on EEPROM ID View
// 6: Add-on EEPROM    View
// 7: Service View?

/////////////////////////////////////////////////////////////////
// special function to view some I2C registers and edit them

// to show bitfield with simple hexa printf viewer (%b not supported)
uint32_t LCD_16x2_MemView_u8_to_hexa_bitfield_as_u32(uint8_t byte)
{
	uint32_t answer = 0;
	if(byte&0x01) answer |= 0x00000001;
	if(byte&0x02) answer |= 0x00000010;
	if(byte&0x04) answer |= 0x00000100;
	if(byte&0x08) answer |= 0x00001000;
	if(byte&0x10) answer |= 0x00010000;
	if(byte&0x20) answer |= 0x00100000;
	if(byte&0x40) answer |= 0x01000000;
	if(byte&0x80) answer |= 0x10000000;
	return answer;
}

// Here we create a register navigator
//
// [FLIP     6      ]
// [20>$ 75 01110101]
void LCD_16x2_MemView_Show(HM_AddOn_t* pADD, uint8_t Reg)
{
  uint32_t bits;
  if(pADD->pMemView==0) TrapError();
  bits = LCD_16x2_MemView_u8_to_hexa_bitfield_as_u32(Reg);
  if((pADD->pMemView->Bitpos>=0)&&(pADD->pMemView->Bitpos<8))
  {
    //LCD_16x2_Printf(pL, "Flip\n@%X:%X->%4X", (uint32_t) pL->pMemView->SubAdr, (uint32_t) Reg, (uint32_t) bits);
    //LCD_16x2_Printf(pL, "Flip\n%X=%c\x7E%X\x7E%4X", (uint32_t) pL->pMemView->SubAdr, (uint32_t) Reg, (uint32_t) Reg, (uint32_t) bits);
    LCD_16x2_Printf(&pADD->LCD_16x2, "Flip\n%X\x7E%c %X %4X", (uint32_t) pADD->pMemView->SubAdr, (uint32_t) Reg, (uint32_t) Reg, (uint32_t) bits);
    LCD_16x2_RAM_GOTO_X_Y(&pADD->LCD_16x2, 15-pADD->pMemView->Bitpos,0);
    LCD_16x2_Writedata_to_RAM(&pADD->LCD_16x2, '0'+pADD->pMemView->Bitpos);
  }
}


//============================================================
// HIGH LEVEL ADD-ON BOARD MANAGEMENT AND SERVICES
int32_t HL_50ms(void) {
  HM_AddOn_t* pADD = &HM_AddOn;
//from:  Add_on_Board_50msec(pADD->pADD);
  // The board is initialized and present.
  JOY_50ms(&pADD->JOY);  
  GUI_50ms(pADD); // process LCD Menu (if enabled)
  return 0;
}

int32_t HL_1sec(void) {
  
  NOPs(1);
  return 0;
}

int32_t HL_IsPlugged(void) {
  HM_AddOn_t* pADD = &HM_AddOn;
  pADD->pADD->JustPlugged = 0; // clear the flag when processed
  STModp_DeInit();  
  do{
    // What needs to be initialized? Check members.
    HAL_Delay(100);
    LCD_16x2_Init(&pADD->LCD_16x2);
    HAL_Delay(100);
    JOY_Init(&pADD->JOY);
    // go to the default menu
    pADD->pGUI = 0; // not menu processing by default
    
    // Disable DAC on Backlight output
    //-HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
    
    // Let's initial a message
    LCD_16x2_ShowTemporaryMessage_0(&pADD->LCD_16x2, 1000, (uint8_t*)pADD->pADD->StringID );
  }while(0); // put 1 to loop (debug/board bringup)
   return 0;
}


int32_t HL_IsUnplugged(void) {
  HM_AddOn_t* pADD = &HM_AddOn;
  pADD->pADD->JustUnplugged = 0;
  // nothing much to do here.
  NOPs(1);
  return 0;
}

//===== Let's name all the pages
uint8_t GUI_Manage_UpDownKeys(HM_AddOn_t* pADD) {
  
  GUI_t* pGUI = pADD->pGUI;
  JOYSTICK_t* pJOY = &pADD->JOY;
  
  if(JOY_SelectLongPressed(pJOY)&&pGUI->IndexVertical==0) {
    pADD->pGUI->SpecialMenuEnabled ^= 1;// toggle service menu for this list only
    pADD->pGUI->LCD_UpdateRequested = 1;
  }
  
  if(JOY_UpPressed(pJOY)) {
    if(pADD->pGUI->IndexVertical>0) pADD->pGUI->IndexVertical--;
    pADD->pGUI->LCD_UpdateRequested = 1;
  }

  if(JOY_DownPressed(pJOY)) {
    if(pADD->pGUI->IndexVertical<pGUI->IndexVerticalMax) pADD->pGUI->IndexVertical++;
    pADD->pGUI->LCD_UpdateRequested = 1;
  }
  
  return 0;
}


//=============================================================
// Local default menu for demo purpose
// [Top Default Menu] 0
// [Use joystick   S] 1 => GUI[1]
// [STM32 Menu     >] 2 => GUI[2]
// [Brisk Menu     >] 3 => GUI[3]
// [Add-on Menu    >] 4 => GUI[4]
// [Arduino Menu   >] 5 => GUI[5]
// [SPI+ menu      >] 6 => GUI[6]
// [End Default Menu] 

int32_t GUI_50ms(HM_AddOn_t* pADD) {

  NOPs(1);
  //return;
  GUI_t* pGUI = pADD->pGUI;
// first let's process keyboard actions
  if((pGUI == 0)&&(JOY_SelectPressed(&pADD->JOY)==0))        return 0;
//  return;
  if((pGUI == 0)&&(JOY_SelectPressed(&pADD->JOY)==1))
  { // enter the menu structure
    pGUI = pADD-> pGUI = &pADD->GUIs[0];
    pGUI->LCD_UpdateRequested = 1; // render as it's a new menu listing
  }
  if(pGUI == &pADD->GUIs[0]) GUI_Manage_TopMenu(pADD);
  else if(pGUI == &pADD->GUIs[1]) GUI_Manage_ServiceMenu(pADD);  
  else if(pGUI == &pADD->GUIs[2]) GUI_Manage_STM32_Menu(pADD);
  else if(pGUI == &pADD->GUIs[3]) GUI_Manage_BriskMenu(pADD);
  else if(pGUI == &pADD->GUIs[4]) GUI_Manage_AddonMenu(pADD);
  else if(pGUI == &pADD->GUIs[5]) GUI_Manage_ArduinoMenu(pADD);
  else if(pGUI == &pADD->GUIs[6]) GUI_Manage_SPIpMenu(pADD);
  // submenus
//  else if(pGUI == &pADD->GUIs[7]) GUI_Manage_XBarSubMenu(pADD);
  
  if(pGUI != pADD->pGUI) { // if page changed
    pGUI = pADD->pGUI; // update from possible changes
    if(pGUI==0) return 0; // leave is no valid pointer
    pADD->pGUI->LCD_UpdateRequested = 1; // render as it's a new menu listing
  }
  
  if(pADD->pGUI->LCD_UpdateRequested == 0) return 0; // nothing to update
  
  LCD_16x2_ClearScreen_RAM(&pADD->LCD_16x2);
  if(pGUI == &pADD->GUIs[0]) GUI_Render_TopMenu(pADD);
  else if(pGUI == &pADD->GUIs[1]) GUI_Render_ServiceMenu(pADD);
  else if(pGUI == &pADD->GUIs[2]) GUI_Render_STM32_Menu(pADD);
  else if(pGUI == &pADD->GUIs[3]) GUI_Render_BriskMenu(pADD);
  else if(pGUI == &pADD->GUIs[4]) GUI_Render_AddonMenu(pADD);
  else if(pGUI == &pADD->GUIs[5]) GUI_Render_ArduinoMenu(pADD);
  else if(pGUI == &pADD->GUIs[6]) GUI_Render_SPIpMenu(pADD);
  // submenus
//  else if(pGUI == &pADD->GUIs[7]) GUI_Render_XBarSubMenu(pADD);

   // let's display the page  
  LCD_16x2_RAM_to_Display(&pADD->LCD_16x2);  // common to all pages
  pADD->pGUI->LCD_UpdateRequested = 0; // job done
  return 0;
}


//==== Pages Description
// Specific to the Add-on LCD Key
  
uint8_t GUI_Manage_TopMenu(HM_AddOn_t* pADD) {
  
//  LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;
  pADD->pGUI->IndexVerticalMax=6;

  GUI_Manage_UpDownKeys(pADD);
  
  // Menu Exit check
  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    LCD_16x2_ClearScreen_RAM(&pADD->LCD_16x2); // just clear the screen
    LCD_16x2_RAM_to_Display(&pADD->LCD_16x2);  // common to all pages
    pADD->pGUI = 0; // leave the menu (back to user control)&pADD->GUIs[0]; // go back to main page
    return 0; // finished
  }
  
  if(JOY_SelectLongPressed(pJOY)&&(pADD->pGUI->IndexVertical==0)) { // program the Add-on B-HMI-16x2 as string ID to Add-on board's EEPROM 
    EEP_SetID_Page(pADD->pADD->pEEP, "B-HMI-LCD16x2"); // set the string
    EEP_WriteID(pADD->pADD->pEEP); // program the string
    LCD_16x2_ShowTemporaryMessage_0(&pADD->LCD_16x2, 2000, "Add-on ID Set to\nB-HMI-LCD16x2");
  }
  
  if(JOY_RightPressed(pJOY)) {
    switch(pADD->pGUI->IndexVertical) {
    case 1: if(pADD->pGUI->SpecialMenuEnabled==1) 
              pADD->pGUI = &pADD->GUIs[1];
            break; // service menu
    case 2: pADD->pGUI = &pADD->GUIs[2];break; // STM32 Menu
    case 3: pADD->pGUI = &pADD->GUIs[3];break; // Brisk Menu
    case 4: pADD->pGUI = &pADD->GUIs[4];break; // Add-on Menu
    case 5: pADD->pGUI = &pADD->GUIs[5];break; // Arduino Menu
    case 6: pADD->pGUI = &pADD->GUIs[6];break; // SPI+ Menu
    default: break; // nothing to do
    }
  }
  
  return 0;
}

//==========================
// Top_Menu (GUI[0])
// [Top Default Menu]      GUI[0] Top Menu
// [Use joystick   S] 1 => GUI[1] service menu
// [STM32 Menu     >] 2 => GUI[2] STM32 menu
// [Brisk Menu     >] 3 => GUI[3] Brisk menu
// [Add-on Menu    >] 4 => GUI[4] Addon menu
// [Arduino Menu   >] 5 => GUI[5] Arduino menu
// [SPI+ menu      >] 6 => GUI[6] SPI Plus menu
// [End Default Menu] 
//==========================

uint8_t GUI_Render_TopMenu(HM_AddOn_t* pADD) {

  LCD_16x2_t* pL = &pADD->LCD_16x2;
  uint16_t Lines = 1;

  switch(pADD->pGUI->IndexVertical) {
  case 0: LCD_16x2_Printf(pL, "Top Default Menu\n"); Lines--;
  case 1: LCD_16x2_Printf(pL, "Use Joystick %c %c\n", pADD->pGUI->SpecialMenuEnabled?'S':' ',((Lines)&&(pADD->pGUI->SpecialMenuEnabled))? '>' : ' ' ); if(--Lines) break;
  case 2: LCD_16x2_Printf(pL, "STM32 Menu     %c\n", (Lines)?'>':' '); if(--Lines) break;
  case 3: LCD_16x2_Printf(pL, "Brisk Menu     %c\n", (Lines)?'>':' '); if(--Lines) break;
  case 4: LCD_16x2_Printf(pL, "Add-on Menu    %c\n", (Lines)?'>':' '); if(--Lines) break;
  case 5: LCD_16x2_Printf(pL, "Arduino Menu   %c\n", (Lines)?'>':' '); if(--Lines) break;
  case 6: LCD_16x2_Printf(pL, "SPI+ Menu      %c\n", (Lines)?'>':' '); if(--Lines) break;
  case 7: LCD_16x2_Printf(pL, "Menu End");break;
  default:TrapError();
  }

  return 0;
}


//--------------------------------------------
// STM32 menu [2]
uint8_t GUI_Manage_STM32_Menu(HM_AddOn_t* pADD) {
  
//  LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;

  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    pADD->pGUI = &pADD->GUIs[0]; // go back to main page
    return 0; // finished
  }
  
  GUI_Manage_UpDownKeys(pADD);  
  
  if(JOY_SelectPressed(pJOY)) {
    switch(pADD->pGUI->IndexVertical) {
    case 12: NVIC_SystemReset();// reset MCU...
      break;
    default: break; // nothing to do
    }
  }
  
  return 0;
}

// STM32 menu (2)
//
// [SW Date/Time:   ] 0
// [__DATE__        ] 1
// [__TIME__        ] 2
// [Serial ID:      ] 3
// [ 0:  xxxx xxxx  ] 4 32 bit
// [ 8:  xxxx xxxx  ] 5
// [10:  xxxx xxxx  ] 6
// [Tj:      123.5'C] 7
// [Vdd:       5.1 V] 8
// [Vbat:      3.6 V] 9
// [SysClk:  100 MHz] 10
// [Clock Stop 5 sec] 11
// [Press for Reset ] 12
// [Menu end        ] 13

uint8_t GUI_Render_STM32_Menu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
  uint16_t Lines = 1;

  switch(pADD->pGUI->IndexVertical) {
    
  case 0: LCD_16x2_Printf(pL, "SW Date/Time:\n" );Lines--;
  case 1: LCD_16x2_Printf(pL, __DATE__);if(--Lines) break;
          LCD_16x2_RAM_GOTO_X_Y(pL, 0,1);
  case 2: LCD_16x2_Printf(pL, __TIME__);if(--Lines) break;
          LCD_16x2_RAM_GOTO_X_Y(pL, 0,1);
  case 3: LCD_16x2_Printf(pL, "Serial ID:\n");if(--Lines) break;
  case 4: LCD_16x2_Printf(pL, "ID + 0: %4X\n", (uint32_t) HAL_GetUIDw0()); if(--Lines) break;
  case 5: LCD_16x2_Printf(pL, "ID + 4: %4X\n", (uint32_t) HAL_GetUIDw1()); if(--Lines) break;
  case 6: LCD_16x2_Printf(pL, "ID + 8: %4X\n", (uint32_t) HAL_GetUIDw2()); if(--Lines) break;
  case 7: LCD_16x2_Printf(pL, "Tj:  %D\n", 0); if(--Lines) break;
  case 8: LCD_16x2_Printf(pL, "Vdd: %D\n", 0); if(--Lines) break;
  case 9: LCD_16x2_Printf(pL, "Vbat: %D\n",0); if(--Lines) break;
  case 10: LCD_16x2_Printf(pL, "Sysclk: %d MHz\n", (uint32_t) (HAL_RCC_GetSysClockFreq()/1000000)); if(--Lines) break;
  case 11: LCD_16x2_Printf(pL, "[Clock Stop 5 s]\n"); if(--Lines) break;
  case 12: LCD_16x2_Printf(pL, "[SW Reset]\n"); if(--Lines) break;
  case 13: LCD_16x2_Printf(pL, "Menu End" );break;
  
  default:TrapError();
  }

  return 0;
}

//===========================================================
// Brisk Menu
uint8_t GUI_Manage_BriskMenu(HM_AddOn_t* pADD) {
  
//  LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;

  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    pADD->pGUI = &pADD->GUIs[0]; // go back to main page
    return 0; // finished
  }
  GUI_Manage_UpDownKeys(pADD);  
  if(JOY_SelectPressed(pJOY)) {
    switch(pADD->pGUI->IndexVertical) {
    case 10: // reset MCU...
      break;
    default: break; // nothing to do
    }
  }
  return 0;
}
//
// [Control LEDs:   ] 0
// [LED <1> ON      ] 1     
// [EEPROM ID      >] 2
// [EEPROM         >] 3
// [MEMs: BMC156    ] 4
// [ACC     x=10.2  ] 5
// [y=12.6  z=11.4  ] 6
// [MAG     x=10.2  ] 7
// [y=12.6  z=11.4  ] 8
// [GYR     x=10.2  ] 9
// [y=12.6  z=11.4  ] 10
// [MEMs Reg Edit  >] 11
// [End of Menu     ] 
uint8_t GUI_Render_BriskMenu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
//  uint16_t Lines = 1;
  
  switch(pADD->pGUI->IndexVertical) {
  case 0: LCD_16x2_Printf(pL, "Control LEDs:\nLED%D [   ]", 0 );     break;
  case 1: LCD_16x2_Printf(pL, "LED <%D> [   ]\nEEPROM ID >", 0 );       break;
  case 2: LCD_16x2_Printf(pL, "EEPROM ID >\nEEPROM >" );                break;
  case 3: LCD_16x2_Printf(pL, "EEPROM >\nMEMs IC: xxxx", 0 ); break;
  case 4: LCD_16x2_Printf(pL, "MEMs: \nACC x=%D g", 0/*Motion3D.gravity_x100*/ ); break;
  case 5: LCD_16x2_Printf(pL, "ACC %D g\nr=%D p=%D", 0,0,0/*Motion3D.gravity_x100, Motion3D.roll_deg, Motion3D.pitch_deg*/ ); break;
  case 6: LCD_16x2_Printf(pL, "r=%D p=%D\nMAG %D uT", 0,0,0/*Motion3D.roll_deg, Motion3D.pitch_deg, Magneto3D.uT*/); break;
  case 7: LCD_16x2_Printf(pL, "MAG %D uT\nr=%D p=%D", 0,0,0/*Magneto3D.uT,Magneto3D.roll_deg,Magneto3D.pitch_deg*/); break;
  case 8: LCD_16x2_Printf(pL, "r=%D p=%D\nGYR x=%D", 0,0,0/*Magneto3D.roll_deg, Magneto3D.pitch_deg,0*/); break;
  case 9: LCD_16x2_Printf(pL, "GYR x=%D\ny=%D z=%D", 0,0); break;
  case 10: LCD_16x2_Printf(pL, "y=%D z=%D\nMEMs Reg Edit >"); break;
  case 11: LCD_16x2_Printf(pL, "MEMs Reg Edit >\nMenu end"); break;

  default:TrapError();
  }

  return 0;
}

//===================================================================
// Addon Menu for LCD 16x2 + 5 way joystick + EEPROM + backlight control
uint8_t GUI_Manage_AddonMenu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;
  MemView_t* pMemView = pADD->pMemView;
  HMI_MEMVIEW_EVENT_t ActionType;
  
  // Memory View Manage first
  if(pMemView) { // memory view activated
    
    ActionType =  Add_LCD_Key_MemViewHandler(pADD); // process all user events
    GUI_Process_AddonMemView(pADD, ActionType);
    return 0;
  }
  
  //=== menu view activated
  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    pADD->pGUI = &pADD->GUIs[0]; // go back to main page
    return 0; // finished
  }
  GUI_Manage_UpDownKeys(pADD);
  if(JOY_SelectPressed(pJOY)&&(pADD->pGUI->IndexVertical==1)) {
      if(IO_PadGet(pL->BL->PadName)==1) IO_PadSetLow(pL->BL->PadName); else IO_PadSetHigh(pL->BL->PadName);
      pADD->pGUI->LCD_UpdateRequested = 1;
  }
  
  if(JOY_RightPressed(pJOY)&&(pADD->pGUI->IndexVertical==2)) { // enter memory view of EEPROM ID (32 bytes)
    pADD->pMemView = &pADD->MemView[5]; // activate the link. Range pre-initialized, only to read the value
    GUI_Process_AddonMemView(pADD, HMI_MEMVIEW_READ);
    return 0;
  }

  if(JOY_RightPressed(pJOY)&&(pADD->pGUI->IndexVertical==3)) { // enter memory view of user EEPROM (256 bytes from 0000..00FF)
    pADD->pMemView = &pADD->MemView[6]; // activate the link. Range pre-initialized, only to read the value
    GUI_Process_AddonMemView(pADD, HMI_MEMVIEW_READ);
    return 0;
  }
  
  return 0;
}
//
// [LCD KEY Add-on  ] 0
// [Backlight: [ON] ] 1     
// [EEPROM ID      >] 2
// [EEPROM         >] 3
// [Menu End        ] 
uint8_t GUI_Render_AddonMenu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
  uint16_t Lines = 1;

  // Render Memory view mode?
  if(pADD->pMemView) { 
    LCD_16x2_MemView_Show(pADD, pADD->pMemView->Register);
    return 0;
    }
  
  switch(pADD->pGUI->IndexVertical) {
  case 0: LCD_16x2_Printf(pL, "LCD KEY Add-on\n");Lines--;
  case 1: ((IO_PadGet(pL->BL->PadName)==1))?LCD_16x2_Printf(pL, "Backlight [ON]\n" ):LCD_16x2_Printf(pL, "Backlight [OFF]\n" ); if(--Lines) break;
  case 2: LCD_16x2_Printf(pL, "EEPROM ID %c\n", (Lines)?'>':' ' );if(--Lines) break;
  case 3: LCD_16x2_Printf(pL, "EEPROM    %c\n", (Lines)?'>':' ' );if(--Lines) break;
  case 4: LCD_16x2_Printf(pL, "Menu End" ); break;
  default:TrapError();
  }
  return 0;
}

uint8_t GUI_Process_AddonMemView(HM_AddOn_t* pADD, HMI_MEMVIEW_EVENT_t ActionType) {

  if(ActionType==HMI_MEMVIEW_UNCHANGED) return 0; // nothing to do
  
  pADD->pGUI->LCD_UpdateRequested = 1; // update display
  
  if(ActionType==HMI_MEMVIEW_EXIT) {
    pADD->pMemView=0; // exit menu, redraw
    return 0;
  }
  if(ActionType==HMI_MEMVIEW_REDRAW) return 0;

  if(pADD->pGUI->IndexVertical==2) { // Add-on EEPROM ID, first thing is to read the real value
    // ACTION READ and TOGGLE
    pADD->pMemView->Register = pADD->pADD->pEEP->ID_Page[pADD->pMemView->SubAdr]; // read string ID
//    if(ActionType==HMI_MEMVIEW_TOGGLE) NOPs(1); // read-only, can't modify the value in user mode
    return 0;
  }
  
  if(pADD->pGUI->IndexVertical==3) { // Add-on EEPROM User
    // ACTION READ and TOGGLE
    EEP_ReadBytes (pADD->pADD->pEEP, pADD->pMemView->SubAdr, &pADD->pMemView->Register, 1); // read byte
    if(ActionType==HMI_MEMVIEW_TOGGLE) {
      pADD->pMemView->Register ^= 1<<pADD->pMemView->Bitpos;
      EEP_WriteBytes (pADD->pADD->pEEP, pADD->pMemView->SubAdr, &pADD->pMemView->Register, 1); // write now!
    }
  }
  return 0;

}


//===================================================================
// Arduino Menu
uint8_t GUI_Manage_ArduinoMenu(HM_AddOn_t* pADD) {
  
  //LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;

  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    pADD->pGUI = &pADD->GUIs[0]; // go back to main page
    return 0; // finished
  }
  GUI_Manage_UpDownKeys(pADD);
  if(JOY_SelectPressed(pJOY)) {
    switch(pADD->pGUI->IndexVertical) {
    case 1: // Backlight control
      break;
    case 2: // View the EEPROM Page ID
      break;
    case 3: // View the first page 00-FF EEPROM 
      break;
    default: break; // nothing to do
    }
  }
  return 0;
}
//
// [Arduino Signals ] 0
// [Backlight: [ON] ] 1     
// [EEPROM ID      >] 2
// [EEPROM         >] 3
// [Menu End        ] 
uint8_t GUI_Render_ArduinoMenu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
//  uint16_t Lines = 1;
  
  switch(pADD->pGUI->IndexVertical) {
  case 0: LCD_16x2_Printf(pL, "LCD KEY Add-on\nBacklight [OFF]", 0 );     break;
  case 1: LCD_16x2_Printf(pL, "Backlight [OFF]\nEEPROM ID >", 0 );       break;
  case 2: LCD_16x2_Printf(pL, "EEPROM ID >\nEEPROM >" );                break;
  case 3: LCD_16x2_Printf(pL, "EEPROM >\nMenu End", 0 ); break;
  default:TrapError();
  }
  return 0;
}

//===================================================================
// SPI+ Menu
uint8_t GUI_Slave, GUI_DemoMode, XBarMenuActivated;
uint8_t GUI_Manage_SPIpMenu(HM_AddOn_t* pADD) {
  
  //LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;

  if(XBarMenuActivated) {
    GUI_Manage_XBarSubMenu(pADD);
    return 0;
  }
  
  
  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    pADD->pGUI->LCD_UpdateRequested = 1;
    switch(pADD->pGUI->IndexVertical) {
    case 0: pADD->pGUI = &pADD->GUIs[0]; // go back to main page
            return 0; // finished
    case 2: GUI_Slave = 0;//(GUI_Slave + MAX_SLAVE - 1) % MAX_SLAVE;
            return 0;
    case 3: GUI_DemoMode = (GUI_DemoMode + 10 - 1) % 10;
            return 0;
    }
  }

  if(JOY_RightPressed(pJOY)) { // go back to main top menu
    pADD->pGUI->LCD_UpdateRequested = 1;
    switch(pADD->pGUI->IndexVertical) {
    case 2: GUI_Slave = 0;//(GUI_Slave + 1) % MAX_SLAVE;
            return 0;
    case 3: GUI_DemoMode = (GUI_DemoMode + 1) % 10;
            return 0;
    }
  }

  GUI_Manage_UpDownKeys(pADD);
  if(JOY_SelectPressed(pJOY)) {
    pADD->pGUI->LCD_UpdateRequested = 1;
    switch(pADD->pGUI->IndexVertical) {
    case 1:XBarMenuActivated = 1; // Enter SIF XBar Menu
      break;
    case 2: // Enter Slave menu
      break;
    case 3: // LED Demo On/Off
      break;
    default: break; // nothing to do
    }
  }
  return 0;
}
//
// [SPI+ Menu  12/12] 0
// [Edit SIF Xbar  *] 1     
// [Slave 12/12    *] 2
// [LED Demo     Off] 3
// [Menu End        ] 
uint8_t GUI_Render_SPIpMenu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
//  uint16_t Lines = 1;

  if(XBarMenuActivated) { // subpage activated
     GUI_Render_XBarMenu(pADD);
     return 0;
  }
  
  switch(pADD->pGUI->IndexVertical) {
  case 0: LCD_16x2_Printf(pL, "SPI+ Menu %d/%d\nEdit SIF Xbar", 0);break;//pSPIP->NbOfSlaves, MAX_SLAVE );     break;
  case 1: LCD_16x2_Printf(pL, "Edit SIF Xbar S\nSlave %d/%d", GUI_Slave, 0/*MAX_SLAVE*/ );       break;
  case 2: LCD_16x2_Printf(pL, "Slave %d/%d S\nLED Demo OFF" );                break;
  case 3: LCD_16x2_Printf(pL, "LED Demo OFF\nMenu End", 0 ); break;
  default:TrapError();
  }
  return 0;
}

//===================================================================
// Service Menu
uint8_t GUI_Manage_ServiceMenu(HM_AddOn_t* pADD) {
  
  //LCD_16x2_t* pL = &pADD->LCD_16x2;
  JOYSTICK_t* pJOY = &pADD->JOY;
  
  if(JOY_LeftPressed(pJOY)) { // go back to main top menu
    pADD->pGUI = &pADD->GUIs[0]; // go back to main page
    return 0; // finished
  }
  GUI_Manage_UpDownKeys(pADD);
  if(JOY_SelectPressed(pJOY)) {
    switch(pADD->pGUI->IndexVertical) {
    case 1: // 
      break;
    case 2: // 
      break;
    case 3: //  
      break;
    default: break; // nothing to do
    }
  }
  return 0;
}
//
// [Service Menu    ] 0
// [AutoID [ ON]    ] 1     
// [Set EEPROM ID:  ] 2
// [B-HMI-LCD16x5  >] 3
// [MeXB XB nXBuXB XB 
uint8_t GUI_Render_ServiceMenu(HM_AddOn_t* pADD) {
  
  LCD_16x2_t* pL = &pADD->LCD_16x2;
  switch(pADD->pGUI->IndexVertical) {
  case 0: LCD_16x2_Printf(pL, "Service/nMenu", 0 );     break;
  case 1: LCD_16x2_Printf(pL, "Menu/nUnder", 0 );       break;
  case 2: LCD_16x2_Printf(pL, "Under/nconstruction" );                break;
  case 3: LCD_16x2_Printf(pL, "construction\nMenu End", 0 ); break;
  default:TrapError();
  }
  return 0;
}

// for SPI+ slave devices taking control of the add-on board, let's call this SPIP_CMD_Remotexxxx()

// XBar view
// [0123456701234567]
// [XB XB  XB XB XB ]
// [12.SWD>99>00.ARD]
uint8_t XB_index, XB_Hpos;

uint8_t GUI_Manage_XBarSubMenu(HM_AddOn_t* pADD) {
/*
  JOYSTICK_t* pJOY = &pADD->JOY;
  Xbar_t* pXB = &SIF_XBarFromTo[XB_index];
  
  if(JOY_LeftPressed(pJOY)) { // move the cursor
    pADD->pGUI->LCD_UpdateRequested = 1;
    if(XB_Hpos==0) {XBarMenuActivated = 0; return 0;}
    XB_Hpos--;
  }
  
  if(JOY_RightPressed(pJOY)) { // move the cursor
    pADD->pGUI->LCD_UpdateRequested = 1;
    if(XB_Hpos==4) {XBarMenuActivated = 0; return 0;}
    XB_Hpos++;
  }
    
  if(JOY_SelectPressed(pJOY)) { // select = exit
    pADD->pGUI->LCD_UpdateRequested = 1;
    pXB->bf.Available ^= 1; // toggle on/off
  }
  
  if(JOY_UpPressed(pJOY)) {
     pADD->pGUI->LCD_UpdateRequested = 1;
    switch(XB_Hpos) {
    case 0:if(pXB->bf.FromSlaveNb) pXB->bf.FromSlaveNb--;
        break; // from slave
    case 1:if(pXB->bf.FromChannel) pXB->bf.FromChannel--;
        break; // channel
    case 2:XB_index = (XB_index + XBAR_ENTRIES_MAX - 1)%XBAR_ENTRIES_MAX;
        break; // slot
    case 3:if(pXB->bf.ToSlaveNb) pXB->bf.ToSlaveNb--;
        break; // to slave
    case 4:if(pXB->bf.ToChannel) pXB->bf.ToChannel--;
        break; // channel
    }
  }
  
  if(JOY_DownPressed(pJOY)) {
     pADD->pGUI->LCD_UpdateRequested = 1;
    switch(XB_Hpos) {
    case 0:if(pXB->bf.FromSlaveNb<MAX_SLAVE) pXB->bf.FromSlaveNb++;
        break; // from slave
    case 1:if(pXB->bf.FromChannel<(MAX_SERIAL-1)) pXB->bf.FromChannel++;
        break; // channel
    case 2:XB_index = (XB_index + 1)%XBAR_ENTRIES_MAX;
        break; // slot
    case 3:if(pXB->bf.ToSlaveNb<MAX_SLAVE) pXB->bf.ToSlaveNb++;
        break; // to slave
    case 4:if(pXB->bf.ToChannel<(MAX_SERIAL-1)) pXB->bf.ToChannel++;
        break; // channel
    }
  }
  */
  return 0;
}

void GUI_XBar_PrintfEntry(LCD_16x2_t* pL, uint8_t index, uint8_t ch) ;
uint8_t GUI_Render_XBarMenu(HM_AddOn_t* pADD) {
/*
  LCD_16x2_t* pL = &pADD->LCD_16x2;
  Xbar_t* pXB = &SIF_XBarFromTo[XB_index];
  
  switch(XB_Hpos) {        // [XB XB  XB XB XB ]
  case 0: LCD_16x2_Printf(pL, "from\n");break;
  case 1: LCD_16x2_Printf(pL, " from ch\n");break;
  case 2: LCD_16x2_Printf(pL, "     entry\n");break;
  case 3: LCD_16x2_Printf(pL, "          to\n");break;
  case 4: LCD_16x2_Printf(pL, "           to ch\n");break;
  };
  
  GUI_XBar_PrintfEntry(pL, pXB->bf.FromSlaveNb,pXB->bf.FromChannel);
  if(pXB->bf.Available)
    LCD_16x2_Printf(pL, ">%d>", XB_index);
  else
    LCD_16x2_Printf(pL, " %d ", XB_index);
  GUI_XBar_PrintfEntry(pL, pXB->bf.ToSlaveNb,pXB->bf.ToChannel);
*/
  return 0;
}

void GUI_XBar_PrintfEntry(LCD_16x2_t* pL, uint8_t index, uint8_t ch) {
/*
  if(index==MAX_SLAVE)
    LCD_16x2_Printf(pL, "MA.");
    else
      LCD_16x2_Printf(pL, "%d.", index);
  
  switch(ch) {//MAX_SERIAL, // 4
  case 0:LCD_16x2_Printf(pL,"APP");break;//APP_SERIAL, // 0
  case 1:LCD_16x2_Printf(pL,"USB");break;//USB_SERIAL, // 1
  case 2:LCD_16x2_Printf(pL,"STM");break;//STM_SERIAL, // 2
  case 3:LCD_16x2_Printf(pL,"ARD");break;//ARD_SERIAL, // 3
  case 4:LCD_16x2_Printf(pL,"BT");break;//BT_SERIAL, // 4
  }
*/
}

// 0ther sub menus to control slaves
// [0123456701234567]
// [U1: ARD         ] SWD OFF
// [U3:
// [
// [

//======================

/*
*.kwl
clear_panel()
set_grid_size(13,7)
add_button(5,5,27,R,r)
add_button(7,5,28,Y,y)
add_button(9,5,29,G,g)
add_button(11,5,30,B,b)
add_led(11,0,1,A,0,0,0)
add_led(12,0,1,B,0,0,0)
add_led(12,1,1,C,0,0,0)
add_led(12,2,1,D,0,0,0)
add_led(11,2,1,E,0,0,0)
add_led(10,2,1,F,0,0,0)
add_led(10,1,1,G,0,0,0)
add_led(10,0,1,H,0,0,0)
add_led(11,1,1,I,0,0,0)
add_monitor(5,0,5,c,3)
add_send_box(5,3,8,Line1,*a,*)
add_send_box(5,4,8,Line2,*c,*)
set_panel_notes(-,,,)
run()
*

*/


//DISByteVein_t* pHL_Report = &BV_to_SPIP[STM_SERIAL];
int32_t HL_Report(void) {
  return 0;
}
int32_t HL_SendDashboard(void) {
  return 0;
}

#endif
