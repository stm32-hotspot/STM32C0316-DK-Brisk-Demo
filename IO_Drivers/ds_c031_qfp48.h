/**
  ******************************************************************************
  * @file    ds_c031_qfp48.h (Datasheet pinout for STM32C031 in QFP48 package)
  * @author  S.Ma
  * @brief   The datasheet pinout coded in C, enabling dynamic HW reconfiguration, using simpler APIs.
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
#ifndef _DS_C031_QFP48_H_
#define _DS_C031_QFP48_H_

#define GPIO_COUNT 6 // minimize its use outside this header file

typedef struct {

  int32_t (*fn)(int32_t);
  void* ct;
} fn_ct_t; // group a function and its passing parameter data context

extern const uint32_t IO_Speed_Lows_2_7V_kHz[4];
extern const uint32_t IO_Speed_Highs_2_7V_kHz[4];

// list the existing pins by simple enum (256 max covers most of products today)
// Here are the silicon padring enumeration as "PAD"
typedef enum // unsupported pins starts with _xx
{
  // This can be optimized, eg remove use of _Pxx. Pin & 0xF = pin position 0..15, Pin>>4 = Port A,B,C...
  PA_0,PA_1,PA_2,PA_3,PA_4,PA_5,PA_6,PA_7,PA_8,PA_9,PA_10,PA_11,PA_12,PA_13,PA_14,PA_15, // 0..15
  PB_0,PB_1,PB_2,PB_3,PB_4,PB_5,PB_6,PB_7,PB_8,PB_9,PB_10,PB_11,PB_12,PB_13,PB_14,PB_15, // 16..31
  _PC0,_PC1,_PC2,_PC3,_PC4,_PC5,PC_6,PC_7,_PC8,_PC9,_PC10,_PC11,_PC12,PC_13,PC_14,PC_15, // 32..47
  PD_0,PD_1,PD_2,PD_3,_PD4,_PD5,_PD6,_PD7,_PD8,_PD9,_PD10,_PD11,_PD12,_PD13,_PD14,_PD15, // 48..63
  _PE0,_PE1,_PE2,_PE3,_PE4,_PE5,_PE6,_PE7,_PE8,_PE9,_PE10,_PE11,_PE12,_PE13,_PE14,_PE15, // 64..95
  PF_0,PF_1,PF_2,PF_3,_PF4,_PF5,_PF6,_PF7,_PF8,_PF9,_PF10,_PF11,_PF12,_PF13,_PF14,_PF15, // 95..127
  DIE_PAD_MAX,
  INTERNAL_VREF, INTERNAL_VTEMP,
  NO_PAD = -1,
  // put here special group of pins or other signals like RX1, TX1, MOSI2, ADC1_12 etc...
} PadName_t;

//-------

// die and package related definitions
typedef enum {
  IO_AF_00, IO_AF_01, IO_AF_02, IO_AF_03, IO_AF_04, IO_AF_05, IO_AF_06, IO_AF_07, // speed and legacy
  IO_AF_08, IO_AF_09, IO_AF_10, IO_AF_11, IO_AF_12, IO_AF_13, IO_AF_14, IO_AF_15, // speed and legacy
  // the following enumeration are the signals which are user accessible, especially the ones going to pads
  AF_MAXVAL,
  USART1_RTS, USART1_CTS, USART1_TX, USART1_RX,
  USART2_RTS, USART2_CTS, USART2_TX, USART2_RX,
  TIM1_CH1, TIM1_CH2, TIM1_CH3, TIM1_CH4, TIM1_BKIN, TIM1_ETR, TIM1_CH1N, TIM1_CH2N, TIM1_CH3N,
  TIM3_CH1, TIM3_CH2, TIM3_CH3, TIM3_CH4, TIM3_ETR,
  TIM14_CH1,
  TIM16_CH1,
  TIM17_CH1,
  I2C1_SDA, I2C1_SCL, I2C1_SMBA,
  SPI1_MISO, SPI1_MOSI, SPI1_SCK, SPI1_NSS,
  ADC1_IN0, ADC1_IN1, ADC1_IN2, ADC1_IN3, ADC1_IN4, ADC1_IN5, ADC1_IN6, ADC1_IN7, ADC1_IN8, ADC1_IN9, ADC1_IN10, ADC1_IN11, ADC1_IN12, ADC1_IN13, ADC1_IN14, ADC1_IN15, 
  /*ADC1_IN16,*/ ADC1_IN17, ADC1_IN18, ADC1_IN19, ADC1_IN20, ADC1_IN21, ADC1_IN22, ADC1_VREF, ADC1_VTEMP,
  
  MCO, MCO2,
  RTC_REFIN, RTC_OUT, RTC_TS,
  EVENTOUT,
  
  // internal signals
  SPI1_TX, SPI1_RX, 
  I2C1_RX, I2C1_TX,
  TIM1_UP, TIM3_UP, TIM14_UP, TIM16_UP, TIM17_UP, TIM3_TRIG,
  ADC1_ANALOG,
  TIM1_TRIG,   TIM1_COM,
  RTC_WAKEUP, I2C1_WAKEUP, USART1_WAKEUP, LSE_CSS_WAKEUP,
  SIGNAL_MAX,
  NO_SIGNAL,  
  
} STM32_SignalName_t;


//extern u32 before,middle,after;
//#define ClearAndSetShiftedBits(reg, clr, set, shift) {before=(reg);middle = (reg) &= ~((clr)<<(shift)); (reg) |= ((set)<<(shift));after = (reg);}

// this could be in ROM but would take access time.
// better keep this in RAM for performance, and keep option to power down the flash.
// Function pointers are challenging for linkers...

extern const GPIO_TypeDef* IO_GPIOx[GPIO_COUNT];
extern const uint16_t pGPIO_PadValidMask[GPIO_COUNT];
extern const uint32_t RCC_GPIOx_ClockEnBitMask[GPIO_COUNT];

typedef struct {
  
  PadName_t PinName;
  uint32_t PPP_Base;
  STM32_SignalName_t SignalName;
  uint32_t AF;
//  char* SignalNameString;
  
} DS_PAD_t; // io_pin table and alternates in a single sweep through C database

extern const DS_PAD_t DS_PADs[]; // contains the pins and internal signals alternate functions/channels descriptions from datasheet
extern const uint32_t DS_PADsCount;

DS_PAD_t const* pDS_PAD_MatchByPinAndSignal(PadName_t PinName, STM32_SignalName_t SignalName);
int32_t iDS_PAD_NextMatchBySignal(STM32_SignalName_t SignalName, int32_t startindex);








#endif
