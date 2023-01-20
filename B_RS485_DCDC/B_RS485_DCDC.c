#include "main.h"
#include "brisk.h"
#include "B_RS485_DCDC.h"
/**
  ******************************************************************************
  * @file    B_RS485_DCDC.c (STMod+ add-on board for RS485 support)
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
// note that we use TIM5 for Sense output (ESC outputs)
// we can optionally use TIM2 for IC (ESC input) with longer time periods.


// These are the pin configurations for this add-on board.
// When unplugged all GPIO connected to STMod+ revert to Input HiZ
#if 0
IO_Pin_t RS_DE = {  GPIOA,    {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF1_USART2,   } }; // PD11 CTS3 = DE (SW GPIO)
IO_Pin_t RS_TXD = {  GPIOD,    {    GPIO_PIN_8, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW,    GPIO_AF1_USART2,   } };   // PD8 TX3 = TXD
IO_Pin_t RS_RXD = {  GPIOD,    {    GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW,    GPIO_AF1_USART2,   } };   // PD9 RX3 = RXR
IO_Pin_t RS_RE = {  GPIOD,    {    GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW,GPIO_AF1_USART2,   } };  // PD12 RTS3 = RE (SW GPIO)

IO_Pin_t RS_SLR = {  GPIOA,    {    GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF1_TIM1*/,   } }; // PA7 = SLR
IO_Pin_t RS_LED1 = {  GPIOA,   {    GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF1_TIM1*/,   } }; // PA6 = LED1

IO_Pin_t RS_PTPB = {  GPIOC,   {    GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF1_TIM1*/,   } }; // PE9 = PTPB
IO_Pin_t RS_PTTA = {  GPIOC,   {    GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF1_TIM1*/,   } }; // PA8 = PTTA
IO_Pin_t RS_LVLB = {  GPIOA,   {    GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };             // LVLB
IO_Pin_t RS_LVLA = {  GPIOA,   {    GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };             // LVLA

IO_Pin_t RS_LED2 = {  GPIOB,   {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF2_TIM5*/,   } }; // PA3 LED2
IO_Pin_t RS_LED3 = {  GPIOA,   {    GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF2_TIM5*/,   } }; // PA2 LED3
IO_Pin_t RS_LED4 = {  GPIOD,   {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF1_TIM1*/,   } }; // PE9 LED4
IO_Pin_t RS_LED5 = {  GPIOD,   {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0/*GPIO_AF2_TIM5*/,   } }; // PA0 LED5
#endif

//const IO_Pad_t MyBoardSignal = { PA_5, {.Mode = IO_, .Pull = IO_, .Drive = IO_, .Speed = IO_SPEED_, .Odr = IO_}, 0 };
IO_Pad_t RS_DE =  {  PA_0,    {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01, .Odr = IO_ODR_LOW , .Signal = GPIO_AF1_USART2 } }; // PD11 CTS3 = DE (SW GPIO)
IO_Pad_t RS_TXD = {  PA_2,    {   .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01, .Signal = GPIO_AF1_USART2 } };   // PD8 TX3 = TXD
IO_Pad_t RS_RXD = {  PA_3,    {   .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01, .Signal = GPIO_AF1_USART2 } };   // PD9 RX3 = RXR
IO_Pad_t RS_RE =  {  PA_1,    {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH , .Signal = GPIO_AF1_USART2 } };  // PD12 RTS3 = RE (SW GPIO)

IO_Pad_t RS_SLR =  {  PA_7,    {  .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01, .Odr = IO_ODR_LOW }/*GPIO_AF1_TIM1*/ }; // PA7 = SLR
IO_Pad_t RS_LED1 = {  PA_12,   {  .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH }/*GPIO_AF1_TIM1*/ }; // PA6 = LED1

IO_Pad_t RS_PTPB = {  PC_14,   {  .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01}/*GPIO_AF1_TIM1*/ }; // PE9 = PTPB
IO_Pad_t RS_PTTA = {  PC_15,   {  .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01}/*GPIO_AF1_TIM1*/ }; // PA8 = PTTA
IO_Pad_t RS_LVLB = {  PA_4,   {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01} };             // LVLB
IO_Pad_t RS_LVLA = {  PA_5,   {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_01} };             // LVLA

IO_Pad_t RS_LED2 = {  PB_0,   {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH }/*GPIO_AF2_TIM5*/ }; // PA3 LED2
IO_Pad_t RS_LED3 = {  PA_6,   {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH }/*GPIO_AF2_TIM5*/ }; // PA2 LED3
IO_Pad_t RS_LED4 = {  PD_1,   {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH }/*GPIO_AF1_TIM1*/ }; // PE9 LED4
IO_Pad_t RS_LED5 = {  PD_0,   {   .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH }/*GPIO_AF2_TIM5*/ }; // PA0 LED5

extern EEP_t STModM24256D; // this EEP is the one plugged as add-on: It may morph by hot plug, remains a single one on the bus
RS_AddOn_t RS_AddOn = {
  .pReport = &u8fifo_from_SPIP[BT_STL_SERIAL],
};

int32_t RS_Init(void) { // called at initialization

  return 0;
}

int32_t RS_DeInit(void) { // undo all and reset globals
  return 0;
}

uint32_t Configure_UART(uint32_t* CMD); // lazy, reuse existing macro function

int32_t RS_TxMode(void);
int32_t RS_RxMode(void);

int32_t RS_TxMode(void) {

  IO_PadSetHigh(RS_RE.PadName); // enable transmit (if gpio is not alternate)  
  IO_PadSetLow( RS_DE.PadName); // enable receive
  return 0;
}

int32_t RS_RxMode(void) {

  IO_PadSetLow( RS_RE.PadName); // disable transmit (if gpio is not alternate)  
  IO_PadSetHigh(RS_DE.PadName); // enable receive
  return 0;
}

int32_t RS_IsPlugged(void) { // reinit HW keeping globals
  
  // here you have to configure the STMod+ GPIOs, the add-on board has just been plugged
  //
  
  IO_PadInit(&RS_RE); // mcu output
  IO_PadInit(&RS_TXD); // mcu transmit
  IO_PadInit(&RS_RXD); // mcu receive
  IO_PadInit(&RS_DE); // mcu input 
  
  IO_PadInit(&RS_SLR);
  IO_PadInit(&RS_LED1);
  
  IO_PadInit(&RS_PTPB);
  IO_PadInit(&RS_PTTA);
  IO_PadInit(&RS_LVLB);
  IO_PadInit(&RS_LVLA);
  
  IO_PadInit(&RS_LED2);
  IO_PadInit(&RS_LED3);
  IO_PadInit(&RS_LED4);
  IO_PadInit(&RS_LED5);

  //uint32_t CMD = ((115200L)<<7) | 1; // 115200bps on USART3 STMod+ (flow control not used here, this is only for test) // use 8 vs 7 to swap RX/TX (doesn' swap the flow control pins)
  //DIS Configure_UART(&CMD);
  // here you can reconfigure the USART3 for RS485 (activate the flow control)
  // you have to know that by default, the debug VCP is directly connected to the serial addon board with SW FIFOs
  // so you can use 2 teraterms with 2 STLinks to exchange data if the half duplex works.
  return 0;
}

int32_t RS_IsUnplugged(void) { // undo HW setting keeping globals
  
  return 0;
}

// Please note that the Addon board has a 32 kbyte EEPROM (non volatile memory) which you can use for your application.

void RS_Test(void) { // Self test when this module is tested alone
}


static int8_t counter;

int32_t RS_50ms(void) { // called every 50+ msec
  
  counter++;
  if(counter & 0x01)
    IO_PadSetHigh(RS_LED1.PadName);
  else
    IO_PadSetLow(RS_LED1.PadName);
  
  if(counter & 0x02)
    IO_PadSetHigh(RS_LED2.PadName);
  else
    IO_PadSetLow(RS_LED2.PadName);

  if(counter & 0x04)
    IO_PadSetHigh(RS_LED3.PadName);
  else
    IO_PadSetLow(RS_LED3.PadName);

  if(counter & 0x08)
    IO_PadSetHigh(RS_LED4.PadName);
  else
    IO_PadSetLow(RS_LED4.PadName);

  if(counter & 0x10)
    IO_PadSetHigh(RS_LED5.PadName);
  else
    IO_PadSetLow(RS_LED5.PadName);

  if(counter & 0x01)
    IO_PadSetHigh(RS_LED1.PadName);
  else
    IO_PadSetLow(RS_LED1.PadName);

  return 0;
}

char* sRS_HelloWorld = "Hello World!";

int32_t RS_1sec(void) {
  
  // you can for exemple try to send something out...
  // we need a handshake control signal to know when to send, when to receive....
  // here we will send every 1 second hello world, diff pair enabled or not
  int32_t i;
  for(i=0;i<200;i++) { 
    AddTou8fifo(&u8fifo_from_SPIP[ARD_STM_SERIAL], sRS_HelloWorld[i]);
    if(sRS_HelloWorld[i]==0) 
      break; 
  } // push all on the RS485 TX fifo
  
  return 0;
}

//-----------------------------------------------
// controls: As if it was remote, this data structure can be global and also remotely updated by SPI+ or other means
void Add_RS_Set(void);
void Add_RS_Set(void) { // structure telling what to change
}

void Add_RS_Get(void);
void Add_RS_Get(void) { // structure telling status and data (same one)
}

//==================

u8fifo_t* pRS_Report = &u8fifo_from_SPIP[BT_STL_SERIAL];
int32_t RS_Report(void) {
  return 0;
}
int32_t RS_SendDashboard(void) {
  return 0;
}

int32_t RS_PaneProcess(void) {
  return 0;
}

int32_t RS_SetPanel(void) {
  return 0;
}
