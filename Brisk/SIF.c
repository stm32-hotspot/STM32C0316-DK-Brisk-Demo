/**
  ******************************************************************************
  * @file    SIF.c (Serial interfaces drivers)
  * @author  S.Ma
  * @brief   Implement MCU side BTEL protocol and Arduino/STMod+ Serial Interfaces (USART) and SW FIFO (BSP)
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


// protocol each 32 bytes every 5 msec = 64b/10ms = 64kbps


//  ARD_STM_SERIAL, // 0
//  BT_STL_SERIAL, // 1
// Total 2x2x2k= 8kb ram for buffers in serial

u8fifo_t u8fifo_to_SPIP[MAX_SERIAL], u8fifo_from_SPIP[MAX_SERIAL]; // These FIFO are re-routable to any peripheral. 4 TX FIFO channels, 4 RX FIFO channels
// these 4 RX TX channels
//uint8_t Bytes_to_SPIP[MAX_SERIAL][2048], Bytes_from_SPIP[MAX_SERIAL][2048]; // big buffer for simpler SW
uint8_t Bytes_to_SPIP[MAX_SERIAL][1500], Bytes_from_SPIP[MAX_SERIAL][1500]; // big buffer for simpler SW

uint32_t Configure_UART(uint32_t* CMD);

// Called when the fifo is no longer empty to usually enable peripheral interrupt to digest incoming.
uint32_t SIF_to_USARTn_NoLongerEmpty(uint32_t ct) { // the from_SPIP fifo is not longer empty: activate USART RX to digest them
  LL_USART_EnableIT_TXE((USART_TypeDef*) ct); // something to send will fill the BV if enabled
  return 0;
}

// Called to safely stop interrupts of destination peripheral, as the FIFO is now empty
uint32_t SIF_to_USARTn_Emptied(uint32_t ct) { // there is no data to feed usart anymore, so disable the interrupt
  LL_USART_DisableIT_TXE((USART_TypeDef*) ct);  // this will be enabled when the FIFO is not empty  
  return 0;
}

//const char sHelloFromUSB[] = " ";//Hello from USB port 0123456789ABCDEF!\n";
const char sHelloFromARD_STM[] = " ";//Hello from Arduino andSTMod+ add-on board port 0123456789ABCDEF!\n";
const char sHelloFromBT_STL[] = " ";//Hello from HC-06 and STLink serial debug port 0123456789ABCDEF!\n";
//const char sHelloFromAPP[] = " ";//Hello from Brisk Application 0123456789ABCDEF!\n";
//const char sHelloToUSB[] = " ";//Hello to USB port 0123456789ABCDEF!\n";
const char sHelloToARD_STM[] = " ";//Hello to Arduino and STMod+ add-on board port 0123456789ABCDEF!\n";
//const char sHelloToARD[] = " ";//Hello to Arduino or SWD debug port 0123456789ABCDEF!\n";
//const char sHelloToAPP[] = " ";//Hello to Brisk Application 0123456789ABCDEF!\n";
const char HC06_Command[] = "AT+BAUD8\n"; // Can be used to reprogram the default bluetooth dongle baud rate from 9600 to 115200 bps... (manual operation in debug mode)
volatile int32_t DetourHC06_115kProgramming = 0;

void USARTs_ClockEnable(void);
void USARTs_ClockEnable(void) {
  do { 
    __IO uint32_t tmpreg;
    SET_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN);
/* Delay after an RCC peripheral clock enabling */ 
    tmpreg = READ_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN);
   UNUSED(tmpreg);
  } while(0U);
  
  do {
  __IO uint32_t tmpreg;
  SET_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);
/* Delay after an RCC peripheral clock enabling */ \
  tmpreg = READ_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);
  UNUSED(tmpreg);
  } while(0U);
  
}


void SPIP_SIF_Init(void) {
// all the USART FIFO are being initialised here  
  //LL_USART_InitTypeDef USART_InitStruct;
//  ErrorStatus Error;

  uint32_t result,i;
  for(i=0;i<MAX_SERIAL;i++) {
    result = Newu8fifo(&u8fifo_to_SPIP[i], (uint32_t) &Bytes_to_SPIP[i][0], sizeof(Bytes_to_SPIP[0]));
    result = Newu8fifo(&u8fifo_from_SPIP[i], (uint32_t) &Bytes_from_SPIP[i][0], sizeof(Bytes_from_SPIP[0]));
  };

  // configure the callbacks
  Hooku8fifo_NoLongerEmpty(&u8fifo_from_SPIP[ARD_STM_SERIAL], SIF_to_USARTn_NoLongerEmpty, (uint32_t) USART2 );
  Hooku8fifo_NoLongerEmpty(&u8fifo_from_SPIP[BT_STL_SERIAL], SIF_to_USARTn_NoLongerEmpty, (uint32_t) USART1 );
  
  Hooku8fifo_Emptied(&u8fifo_from_SPIP[ARD_STM_SERIAL], SIF_to_USARTn_Emptied, (uint32_t) USART2 );
  Hooku8fifo_Emptied(&u8fifo_from_SPIP[BT_STL_SERIAL], SIF_to_USARTn_Emptied, (uint32_t) USART1 );

  
  // HERE THE USART SHALL BE CONFIGURED OR INITIALIZED WITH DEFAULT SETTINGS
    // all supported USART
/*  
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_USART2_CLK_ENABLE();
*/
USARTs_ClockEnable();  
  //__HAL_RCC_USART3_CLK_ENABLE();
  
  uint32_t CMD[3];
  //CMD[0] = ((9600L)<<12) | 1; // default SPIP going to SWD/BT port (you have to setup the serial interface on PC side for 115200bps like BT dongle
  CMD[0] = ((115200L)<<12) | 1; // default SPIP going to SWD port
  Configure_UART(CMD);

  // we don't configure the arduino as the GPIO can be used for many purpose.
  // the peripheral is enabled on need basis

  BTEL_Enable();
/*    CMD[0] = ((9600L)<<12) | 7; // activate STMod+ USART (by default, as most popular?) // when HC-06 is factory preset to program its speed to 115200bps
    // This should be done at add-on board plugging source code (TODO)
  
    Configure_UART(CMD);
  // here the usart is ready to kick off
  // here we configure BT serial
  CMD[0] = ((115200L)<<12 | 11); // enable USART2 with 115200 bps
  Configure_UART(CMD);
*/

//  for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_to_SPIP[ARD_STM_SERIAL], sHelloFromARD_STM[i]); if(sHelloFromARD_STM[i]==0) break; }
  for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_to_SPIP[BT_STL_SERIAL], sHelloFromBT_STL[i]); if(sHelloFromBT_STL[i]==0) break; }

//  for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_from_SPIP[ARD_STM_SERIAL], sHelloToARD_STM[i]); if(sHelloToARD_STM[i]==0) break; }
  //for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_from_SPIP[BT_STL_SERIAL], sHelloToBT_STL[i]); if(sHelloToAPP[i]==0) break; }
    
// by default, when master, local STMod+ RS232 goes to both ARD (SWD) and USB

//  CMD[0] = ((115200L)<<12) | 11; // activate STMod+ and Arduino serial port. By default, we will route STlink <=> Arduino as feedback loop
  // as soon as plug an STMod+ board, this functionality will stop
//  Configure_UART(CMD);

  
}



   //  CMD[0] = 
    //  [7:0] = USARTn:         0 => USART1 BT STL OFF (no GPIO connection)
    //                          1 => USART1 BT STL enabled
//                              2 => USART1 BT STL RX TX swapped
  //                          10 ==> USART2 ARD STM OFF
//                            11 ==> USART2 ARD STM ON
//                            12 ==> USART2 ARD STM RX TX Swapped
// there is also LPUART for ARDUINO (to be done later)
    //  [9:8] = USART parity: 0.None 1.Even 2.Odd (for now, only none)
    //  [11:10] = USART1 stop bits: 0.1 1.1,5 2.2 (for now, only one stop bit)
    //  [31:12] USART1 Baudrate

uint8_t SIF_ioconfig_now, SIF_ioconfig_before;

uint32_t Configure_UART(uint32_t* CMD) {
    
    SIF_ioconfig_before = SIF_ioconfig_now;
    SIF_ioconfig_now = CMD[0]&0x0F;
    //uint8_t parity = (CMD[0]>>8)&0x03; // 2 bits
    //uint8_t stops = (CMD[0]>>10)&0x03; // 2 bits
    uint32_t bps = (CMD[0]>>12); // 20 bits
    uint8_t n;
    uint8_t swap_rx_tx = 0;

    // now let's configure all these things
    IO_Pad_t IO_Pad = { PB_6, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = GPIO_AF0_USART1 } };
    
    if(SIF_ioconfig_before != SIF_ioconfig_now)
      switch(SIF_ioconfig_now) { // for now, there will be glitches when changing GPIOs, and we don't care the FIFO are flushed, or USART is idle
      case 0: // BT&STL USART1 disconnected from GPIOs
        IO_PadDeInit(PB_6);
        IO_PadDeInit(PB_7);
        n = 1;
        break;
      case 2: // BT&STL USART1 swapped RX/TX
    	  swap_rx_tx = 1; // continue the rest is same
      case 1: // BT&STL USART1 normal
        IO_Pad.PadName = PB_6;
        IO_Pad.Config.Signal = GPIO_AF0_USART1;
        IO_PadInit(&IO_Pad);
        IO_Pad.PadName = PB_7;
        IO_PadInit(&IO_Pad);
        n = 1;
        break;
      case 10: // ARD&STM USART2 OFF
        IO_PadDeInit(PA_2);
        IO_PadDeInit(PA_3);
        n = 2;
        break;
      case 12: // ARD&STM USART2 Swapped RX/TX
        swap_rx_tx = 1; // continue as rest is same
      case 11: // ARD&STM USART2
        IO_PadDeInit(PD_2);
        IO_PadDeInit(PD_3);
        
        IO_Pad.PadName = PA_2;
        IO_Pad.Config.Signal = GPIO_AF1_USART2;
        IO_PadInit(&IO_Pad);
        IO_Pad.PadName = PA_3;
        IO_PadInit(&IO_Pad);
        n = 2;
        break;
      default: 
        return 0; // error
      }

    LL_USART_InitTypeDef USART_InitStruct;
    ErrorStatus Error;
    
    if(n==1) { // configure USART1
      LL_USART_DeInit(USART1);
      LL_USART_StructInit(&USART_InitStruct);
      USART_InitStruct.BaudRate = bps;
      Error = LL_USART_Init(USART1, &USART_InitStruct);
      LL_USART_SetTXRXSwap(USART1, swap_rx_tx ? LL_USART_TXRX_SWAPPED : LL_USART_TXRX_STANDARD); // 
      LL_USART_Enable(USART1);
      NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0F, 0x00));
      NVIC_EnableIRQ(USART1_IRQn);
      LL_USART_EnableIT_RXNE(USART1);
      if(u8fifo_from_SPIP[ARD_STM_SERIAL].bCount)// Dynamic case: we activate the TX if the u8fifo_from_SPIP is already not empty (or we flush it)
        LL_USART_EnableIT_TXE(USART1);
    }
    else
    if(n==2) { // configure USART2 now
      LL_USART_DeInit(USART2);      
      LL_USART_StructInit(&USART_InitStruct);
      USART_InitStruct.BaudRate = bps;
      Error = LL_USART_Init(USART2, &USART_InitStruct);
      LL_USART_SetTXRXSwap(USART2, swap_rx_tx ? LL_USART_TXRX_SWAPPED : LL_USART_TXRX_STANDARD); // 
      LL_USART_Enable(USART2);
      NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0F, 0x00));
      NVIC_EnableIRQ(USART2_IRQn);
      LL_USART_EnableIT_RXNE(USART2);
      if(u8fifo_from_SPIP[ARD_STM_SERIAL].bCount)// Dynamic case: we activate the TX if the u8fifo_from_SPIP is already not empty (or we flush it)
        LL_USART_EnableIT_TXE(USART2);
    }else
      NOPs(1);
      
  return 1;
}


void RS232_CloseLoopPolling(void);
void RS232_CloseLoopPolling(void) {
/*
	uint8_t c;
// this is the simple plumbing between STL FIFO and ARD FIFO
	  while(u8fifo_to_SPIP[ARD_STM_SERIAL].bCount) { // some characters to process

	    // read a char from incoming FIFO
	    c = (uint8_t) Clipu8fifo_Down(&u8fifo_to_SPIP[ARD_STM_SERIAL]);
	    AddToBV(&u8fifo_from_SPIP[BT_STL_SERIAL], c); // send it through stlink too
	    AddToBV(&u8fifo_from_SPIP[ARD_STM_SERIAL], '|'); // send it through stlink too
	    AddToBV(&u8fifo_from_SPIP[ARD_STM_SERIAL], c); // send it through stlink too
	  };

	  // you send from arduino and you get back an echo and also send it to the STLink/BT (only one way here)
*/
}

