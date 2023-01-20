/**
  ******************************************************************************
  * @file    SPI_MasterIO.c (SPI by IO with SW, bit bang method)
  * @author  S.Ma
  * @brief   SPI master emulation by IO and SW, assuming C0 at 48 MHz core speed
  *          POL/PHASE/LSB-MSB are fixed. Grep on the function to check where it is used in the application code.
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
//#include "SPI_MasterIO.h"

// WARNING: Arduino and STMod+ SPI share SCK. Make sure only one NSS is active at any time (which should be as these are blocking functions)
// Careful when activating the HW SPI on DIP28 Arduino

// this is the SPI 3 wire communication by S/W GPIO manipulation on the iNemo/FROG connector
#if 0
// keep same order as it is in a struct
IO_Pin_t MISO_STMod = {  GPIOA,          {    GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, 0,   } }; // STMod pin 3
IO_Pin_t MOSI_STMod = {  GPIOA,          {    GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, 0,   } }; // STMod pin 2
//IO_Pin_t MOSI_as_MISO_3W_STMod = { GPIOA,          {    GPIO_PIN_2, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, 0,   } };
IO_Pin_t SCK_STMod =  {  GPIOA,          {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0,   } }; // STMod pin 4
IO_Pin_t NSS_STMod =  {  GPIOA,          {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0,   } }; // STMod pin 1

SPI_MasterIO_t SPI_IO_STMod = { &MISO_STMod, &MOSI_STMod, 0/*&MOSI_as_MISO_3W_STMod*/, &SCK_STMod, {&NSS_STMod, } };

// should be filled up with right GPIOs (only for Brisk V2)
IO_Pin_t MISO_ARD = {  GPIOA,          {    GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t MOSI_ARD = {  GPIOA,          {    GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
//IO_Pin_t MOSI_as_MISO_3W_ARD = { GPIOA,          {    GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t SCK_ARD =  {  GPIOA,          {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pin_t NSS_ARD =  {  GPIOB,          {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,   } };

SPI_MasterIO_t SPI_IO_ARD = { &MISO_ARD, &MOSI_ARD, 0/*&MOSI_as_MISO_3W_ARD*/, &SCK_ARD, {&NSS_ARD, } };
#endif

//=========================================
// keep same order as it is in a struct
IO_Pad_t MISO_STMod = {  PA_3, { .Mode = IO_INPUT,                        .Pull = IO_PULLUP} }; // STMod pin 3
IO_Pad_t MOSI_STMod = {  PA_2, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03 } }; // STMod pin 2
//IO_Pin_t MOSI_as_MISO_3W_STMod = { GPIOA,          {    GPIO_PIN_2, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, 0,   } };
IO_Pad_t SCK_STMod =  {  PA_1, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 } }; // STMod pin 4
IO_Pad_t NSS_STMod =  {  PA_0, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 } }; // STMod pin 1

SPI_MasterIO_t SPI_IO_STMod = { &MISO_STMod, &MOSI_STMod, 0/*&MOSI_as_MISO_3W_STMod*/, &SCK_STMod, {&NSS_STMod, } };

// should be filled up with right GPIOs (only for Brisk V2)
IO_Pad_t MISO_ARD = {  PA_6, { .Mode = IO_INPUT,                          .Pull = IO_PULLUP} };
IO_Pad_t MOSI_ARD = {  PA_7, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL,   .Pull = IO_PULLUP, .Speed = IO_SPEED_01 } };
//IO_Pin_t MOSI_as_MISO_3W_ARD = { GPIOA,         {    GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 0,   } };
IO_Pad_t SCK_ARD =  {  PA_1, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL,   .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } };
IO_Pad_t NSS_ARD =  {  PB_0, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL,   .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } };

SPI_MasterIO_t SPI_IO_ARD = { &MISO_ARD, &MOSI_ARD, 0/*&MOSI_as_MISO_3W_ARD*/, &SCK_ARD, {&NSS_ARD, } };

//======================------------------------>
//uint32_t SPI_MIO_Start(SPI_MasterIO_t* S, uint32_t BitMask);
uint32_t SPI_MIO_Move(SPI_MasterIO_t* M, uint32_t Param1, uint32_t Param2, uint32_t Param3);
uint32_t SPI_MIO_SendByte(SPI_MasterIO_t* M, uint8_t byte);
//uint32_t SPI_MIO_ReadByte(SPI_MasterIO_t* M);
//uint32_t SPI_MIO_Stop(SPI_MasterIO_t* S, uint32_t BitMask);
//uint32_t TimerCountdownWait(SPI_MasterIO_t* M);
static uint32_t NopsWait(uint32_t u);
static uint32_t WaitHere(SPI_MasterIO_t* M, uint32_t delay);


void SetSPI_MasterIO_Timings(SPI_MasterIO_t* M, uint32_t MinBps, uint32_t MaxBps ) { // 1200000, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_FirstBit_MSB

  //uint32_t HalfClockPeriod_Hz;
  uint32_t HalfClockPeriod_us;
/*  
  if(MinBps>MHzToHz(10)) while(1); // over 10 MHz is not reasonable for bit banging
  if(MinBps==0) MinBps = 100000; // l00kHz default min
  M->Bps.Min = MinBps; // 400khz

  HalfClockPeriod_Hz = MinBps*2; // Timers runs at 1MHz max overflow speed. 500kHz = 2us
  
  if(M->Timer) {
    
    M->WaitParam = 1000000 / ( M->Timer->OverflowPeriod_us * HalfClockPeriod_Hz);
    if(M->WaitParam) {
      M->fnWaitMethod = TimerCountdownWait; // here we should take care of the timings, and choose the best scheme based on CPU MHz and bps of bus...    
      return; // found a tick period compatible with this Basic Timer
    };
    // otherwise, the strategy will be NOPs.
  }

  // Delay will use S/W NOPs because the incoming sys frequency is too low or the bit rate too high
  //!!! In IAR, 96MHz core STM32F437, No optimisation: WaitParam = 2. With max optimisation for speed: WaitParam = 20 (400kHz)
  // Later, we will use the help of the Timer with precise time to tune it dynamically.... more high tech and requires some time to tune.
  */
  M->fnWaitMethod = NopsWait;
  
//  if(M->Clocks->OutCoreClk_Hz.Value) {
    // clock is known, calculate the right NOPs delay
  HalfClockPeriod_us = (SYS_CLK_HZ)/(MaxBps*SYS_CLK_MHZ); // Here the feedclock is the core speed for IO emulated we assume 1 cycle per instruction which is not realistic.  
//    HalfClockPeriod_us = (M->Clocks->OutCoreClk_Hz.Value )/(MinBps*12); // Here the feedclock is the core speed for IO emulated we assume 1 cycle per instruction which is not realistic.
    // it should not be zero or the delays won't scale for the communication
    M->WaitParam = HalfClockPeriod_us;
//  }else{ // give estimation of min frequency to operate at desired min baud rate
//    M->Clocks->OutCoreClk_Hz.Min = MinBps * 12 * 20; // estimation only
//  };
}

void SetSPI_MasterIO_Format(SPI_MasterIO_t* M, uint32_t CPol, uint32_t CPha, uint32_t FirstBit ) {

//  if(CPol!=SPI_CPOL_Low) while(1); // not supported yet
//  if(CPha!=SPI_CPHA_1Edge) while(1); // not supported yet
  
};

void ConfigureSPI_MasterIO(SPI_MasterIO_t* M) {

  uint8_t n;
  // configure the GPIOs
  if((M->MISO)&&(M->MOSI!=M->MISO)) // If MISO=MOSI then we don't configure the input pin
    // configure MISO pin
    IO_PadInit(M->MISO);
 
  // configure MOSI pin
  if(M->MOSI)
    IO_PadInit(M->MOSI);
  
  // configure SCK pin
  IO_PadInit(M->SCK);
  
  // configure all the NSSs pins
  for(n=0;n<8;n++)
  if(M->NSSs[n]) {
      IO_PadSetHigh(M->NSSs[n]->PadName);
      IO_PadInit(M->NSSs[n]);
      
  }
    
}
  

void EnableSPI_MasterIO(SPI_MasterIO_t* M) {
  
  // nothing to do
}

/*
static uint32_t TimerCountdownWait(uint32_t u) {
  SPI_MasterIO_t* M = (SPI_MasterIO_t*) u;
  ArmTimerCountdown(M->Timer,M->Cn, M->WaitParam * M->ctWaitMethod);
  while(M->Timer->CountDownDone[M->Cn]==0) ;
  return 0;
}
*/
static uint32_t NopsWait(uint32_t u) {
  SPI_MasterIO_t* M = (SPI_MasterIO_t*) u;
  uint32_t n = M->ctWaitMethod * M->WaitParam;
  while(n--) asm("nop\n");
  return 0;
}

static uint32_t WaitHere(SPI_MasterIO_t* M, uint32_t delay) {
  //SPI_MasterIO_t* M = (SPI_MasterIO_t*) u;
  M->ctWaitMethod = delay;
  if(M->fnWaitMethod) M->fnWaitMethod((uint32_t)M);
  return 0;
}
 
//=============-------------->

uint32_t SPI_MIO_Start(SPI_MasterIO_t* S, uint32_t BitMask) {

  uint8_t n;

// we don't make sure NSS are high at first.
  IO_PadSetLow(S->SCK->PadName);
  IO_PadSetLow(S->MOSI->PadName);
  IO_PadInit(S->MOSI);

  WaitHere(S, 2);//Wait_us(1);
  
  for(n=0;n<8;n++) { // put low all the selected pins
    if(S->NSSs[n]==0) break;
    
    if(BitMask & (1<<n)) // if no, continue
        IO_PadSetLow(S->NSSs[n]->PadName);
  };
  
  WaitHere(S, 2);//Wait_us(1); //+
  return 0; // no call back, next job right away
}


uint32_t SPI_MIO_Stop(SPI_MasterIO_t* S, uint32_t BitMask) {

  uint8_t n;
    
  WaitHere(S, 2);//Wait_us(1);
  
  for(n=0;n<8;n++) { // put low all the selected pins
    if(S->NSSs[n]==0) break;
    
    if(BitMask & (1<<n)) { // if no, continue
//        IO_PinSetOutput(S->NSSs[n]);
        IO_PadSetHigh(S->NSSs[n]->PadName);
    };
  };
  
  WaitHere(S, 2);//Wait_us(1);
  return 0; // no call back, next job right away
}


uint32_t SPI_MIO_Move(SPI_MasterIO_t* S, uint32_t Param1, uint32_t Param2, uint32_t Param3) {
  
  uint8_t* TX = (uint8_t*) Param1;
  uint8_t* RX = (uint8_t*) Param2;
  uint32_t bCount = Param3;
//  SPI_MasterIO_t* S = (SPI_MasterIO_t*) u;
  
  if((Param1)&&(Param2)) { // this means dummy read
    if(Param3==0) while(1); // nothing to do at all... problem
    
    while(bCount--) SPI_MIO_ReadByte(S);
  }
  
  // MASTER RECEIVE MODE
  // if the TX adr is null, no bytes to transmit, disable MOSI, and transmit dummy things instead
  if(Param1==0) {
    
    while(bCount--)
      *RX++ = (uint8_t) SPI_MIO_ReadByte(S);
    
    return 0;
  }

  // MASTER TRANSMIT MODE 
  // if the RX adr is null, no bytes to receive, TRANSMIT MODE
  if(Param2==0) {
    while(bCount--)
      SPI_MIO_SendByte(S, *TX++);
  };
  
  if(Param3==0) while(1); // no bytes to transfer? weird!

  return 0; // there is no interrupt generated in this implementation
}


//===== 8>< ~~~~ 

uint32_t SPI_MIO_SendByte(SPI_MasterIO_t* S, uint8_t byte)
{
//  uint8_t byte = 0;
  
  // bit 7
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x80)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);
  
  // bit 6
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x40)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

  // bit 5
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x20)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

  // bit 4
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x10)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

  // bit 3
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x08)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

  // bit 2
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x04)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

  // bit 1
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x02)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

  // bit 0
  IO_PadSetLow(S->SCK->PadName);
  if(byte & 0x01)
    IO_PadSetHigh(S->MOSI->PadName);
  else
    IO_PadSetLow(S->MOSI->PadName);
  WaitHere(S, 1);
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);
  
//  IO_PinSetLow(S->SCK);
  
  return 0;
}

uint32_t SPI_MIO_ReadByte(SPI_MasterIO_t* S)
{
  uint8_t byte = 0;
  IO_Pad_t* pIO_In = S->MISO;
//  IO_PinConfigure(S->MOSI_as_MISO_3W); //added by seb for devices which don't have dummy byte in SPI3W mode
  
  // bit 7
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x80;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);
  
  // bit 6
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x40;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

    // bit 5
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x20;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

    // bit 4
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x10;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

    // bit 3
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x08;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

    // bit 2
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x04;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

    // bit 1
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x02;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

    // bit 0
  IO_PadSetLow(S->SCK->PadName);
  WaitHere(S, 1);
  if(IO_PadGet(pIO_In->PadName))
    byte |= 0x01;
  
  IO_PadSetHigh(S->SCK->PadName);
  WaitHere(S, 1);

//  IO_PinSetLow(S->SCK);
  return byte;
}

//===========================================================

