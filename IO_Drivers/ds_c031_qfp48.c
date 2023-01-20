#include "main.h"
#include "brisk.h"
#include "ds_c031_qfp48.h"

/**
  ******************************************************************************
  * @file    ds_c031_qfp48.c (Mini Alternate IO Driver Package Pinout description code, Datasheet in C)
  * @author  S.Ma
  * @brief   Contains in C the DataSheet for STM32C031 in QFP48 pin package. Don't ever look at the Alternate function table anymore...
  *          To support a different package? Clone and rename this file, then edit it. Works along with IO_DRIVERS.C file. An alternate way to control the MCU GPIOs more intuitively.
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


// This is a translation of the datasheet into simple tables put within the microcontroller allowing dynamic HW reconfiguration when needed, and reduce reliancy from CubeMX

// list here the application signal names.

// slow pin(s) are flash described low footprint format with checks, interruptable
// fast pin(s) are RAM described in fast format without checks, atomic
const GPIO_TypeDef* IO_GPIOx[GPIO_COUNT] = {
  GPIOA, // 0
  GPIOB, // 1
  GPIOC, // 2
  GPIOD, // 3
  NULL,//GPIOE, // 4
  GPIOF, // 5
}; 

/* note: This replaces xx_hal_gpio_ex.h: (which shows a need for it) 
#define GPIO_GET_INDEX(__GPIOx__)    (((__GPIOx__) == (GPIOA))? 0uL :\
                                      ((__GPIOx__) == (GPIOB))? 1uL :\
                                      ((__GPIOx__) == (GPIOC))? 2uL :\
                                      ((__GPIOx__) == (GPIOF))? 3uL : 5uL)
*/

const uint16_t pGPIO_PadValidMask[GPIO_COUNT] = { // pin is related to a specific package, it's another story
  0xFFFF,
  0xFFFF,
  (1<<6)|(1<<7)|(1<<13)|(1<<14)|(1<<15),
  0x000F,
  0,
  0x000F,
};

const uint32_t RCC_GPIOx_ClockEnBitMask[GPIO_COUNT] = {
  
  RCC_IOPENR_GPIOAEN, 
  RCC_IOPENR_GPIOBEN,
  RCC_IOPENR_GPIOCEN,
  RCC_IOPENR_GPIODEN,
  0,
  RCC_IOPENR_GPIOFEN,
};

const uint32_t IO_Speed_Lows_2_7V_kHz[] = {
    350,
   2000,
  15000,
  30000,
}; // max speed, lowest voltage, high output load

const uint32_t IO_Speed_Highs_2_7V_kHz[] = {
   2000,
  10000,
  30000,
  60000,
}; // max speed, lowest voltage, high output load


const DS_PAD_t DS_PADs[] = { //> didn't add timer complement outputs nor SYSCFG related signals fix
// Once you know what you need, you can shrking this flash table... or simply hardcode. Here the table is full size. (1kb)
  { PA_0, (uint32_t) USART2, USART2_CTS, 1 },//"USART2_CTS" },
{ PA_0, (uint32_t) TIM16, TIM16_CH1, 2 },//"TIM16_CH1" },
{ PA_0, (uint32_t) USART1, USART1_TX, 4 },//"USART1_TX" },
{ PA_0, (uint32_t) TIM1, TIM1_CH1, 4 },//"TIM1_CH1" },
{ PA_0, (uint32_t) ADC1, ADC1_IN0, 0 },//"ADC1_IN0" }, // *** ADC ch0 (AF contains the channel nb)

{ PA_1, (uint32_t) SPI1, SPI1_SCK, 0 },//"SPI1_SCK" },
{ PA_1, (uint32_t) USART2, USART2_RTS, 1 },//"USART2_RTS" },
{ PA_1, (uint32_t) TIM17, TIM17_CH1, 2 },//"TIM17_CH1" },
{ PA_1, (uint32_t) USART1, USART1_RX, 4 },//"UART1_RX" },
{ PA_1, (uint32_t) TIM1, TIM1_CH2, 5 },//"TIM1_CH2" },
{ PA_1, (uint32_t) I2C1, I2C1_SMBA, 6 },//"I2C1_SMBA" },
{ PA_1, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_1, (uint32_t) ADC1, ADC1_IN1, 1 },//"ADC1_IN1" }, // *** ADC

{ PA_2, (uint32_t) SPI1, SPI1_MOSI, 0 },//"SPI1_MOSI" },
{ PA_2, (uint32_t) USART2, USART2_TX, 1 },//"USART2_TX" },
{ PA_2, (uint32_t) TIM3, TIM3_ETR, 3 },//"TIM3_ETR" },
{ PA_2, (uint32_t) TIM1, TIM1_CH3, 5 },//"TIM1_CH3" },
{ PA_2, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_2, (uint32_t) ADC1, ADC1_IN2, 2 },//"ADC1_IN2" }, // *** ADC

{ PA_3, (uint32_t) USART2, USART2_RX, 1 },//"USART2_RX" },
{ PA_3, (uint32_t) TIM1, TIM1_CH4, 5 },//"TIM1_CH4" },
{ PA_3, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_3, (uint32_t) ADC1, ADC1_IN3, 3 },//"ADC1_IN3" }, // *** ADC

{ PA_4, (uint32_t) SPI1, SPI1_NSS, 0 },//"SPI1_NSS" },
{ PA_4, (uint32_t) USART2, USART2_TX, 1 },//"USART2_TX" },
{ PA_4, (uint32_t) TIM14, TIM14_CH1, 4  },//"TIM14_CH1" },
{ PA_4, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_4, (uint32_t) ADC1, ADC1_IN4, 4 },//"ADC1_IN4" }, // *** ADC

{ PA_5, (uint32_t) SPI1, SPI1_SCK, 0 },//"SPI1_SCK" },
{ PA_5, (uint32_t) USART2, USART2_RX, 1 },//"USART2_RX" },
{ PA_5, (uint32_t) TIM1, TIM1_CH1, 5 },//"TIM1_CH1" },
{ PA_5, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_5, (uint32_t) ADC1, ADC1_IN5, 5 },//"ADC1_IN5" }, // *** ADC

{ PA_6, (uint32_t) SPI1, SPI1_MISO, 0 },//"SPI1_MISO" },
{ PA_6, (uint32_t) TIM3, TIM3_CH1, 1 },//"TIM3_CH1" },
{ PA_6, (uint32_t) TIM1, TIM1_BKIN,0 },//"TIM1_BKIN" },
{ PA_6, (uint32_t) TIM16, TIM16_CH1, 5 },//"TIM16_CH1" },
{ PA_6, (uint32_t) ADC1, ADC1_IN6, 6 },//"ADC1_IN6" }, // *** ADC

{ PA_7, (uint32_t) SPI1, SPI1_MOSI, 0 },//"SPI1_MOSI" },
{ PA_7, (uint32_t) TIM3, TIM3_CH2, 1 },//"TIM3_CH2" },
{ PA_7, (uint32_t) TIM14, TIM14_CH1, 4 },//"TIM14_CH1" },
{ PA_7, (uint32_t) TIM17, TIM17_CH1, 5 },//"TIM17_CH1" },
{ PA_7, (uint32_t) ADC1, ADC1_IN7, 7 },//"ADC1_IN7" }, // *** ADC

{ PA_8, (uint32_t) RCC, MCO, 0 },//"MCO" }, // MCO2?
{ PA_8, (uint32_t) USART2, USART2_TX, 1 },//"USART2_TX" },
{ PA_8, (uint32_t) TIM1, TIM1_CH1, 2 },//"TIM1_CH1" },
{ PA_8, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_8, (uint32_t) SPI1, SPI1_NSS, 8 },//"SPI1_NSS" },
{ PA_8, (uint32_t) TIM3, TIM3_CH3, 11 },//"TIM3_CH3" },
{ PA_8, (uint32_t) TIM3, TIM3_CH4, 12 },//"TIM3_CH4" },
{ PA_8, (uint32_t) TIM14, TIM14_CH1, 13 },//"TIM14_CH1" },
{ PA_8, (uint32_t) USART1, USART1_TX, 14 },//"USART1_TX" },
{ PA_8, (uint32_t) ADC1, ADC1_IN8, 8 },//"ADC1_IN8" }, // *** ADC
//MCO2????

{ PA_9, (uint32_t) RCC, MCO, 0 },//"MCO" },
{ PA_9, (uint32_t) USART1, USART1_TX, 1 },//"USART1_TX" },
{ PA_9, (uint32_t) TIM1, TIM1_CH2, 2 },//"TIM1_CH2" },
{ PA_9, (uint32_t) TIM3, TIM3_ETR, 3 },//"TIM3_ETR" },
{ PA_9, (uint32_t) I2C1, I2C1_SCL, 6 },//"I2C1_SCL" },
{ PA_9, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PA_10, (uint32_t) USART1, USART1_RX, 1 },//"USART1_RX" },
{ PA_10, (uint32_t) TIM1, TIM1_CH3, 2 },//"TIM1_CH3" },
{ PA_10, (uint32_t) I2C1, I2C1_SDA, 6 },//"I2C1_SDA" },
{ PA_10, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PA_11, (uint32_t) SPI1, SPI1_MISO, 0 },//"SPI1_MISO" },
{ PA_11, (uint32_t) USART1, USART1_CTS, 1 },//"USART1_CTS" },
{ PA_11, (uint32_t) TIM1, TIM1_CH4, 2 },//"TIM1_CH4" },
{ PA_11, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_11, (uint32_t) ADC1, ADC1_IN11, 11 },//"ADC1_IN11" }, // *** ADC

{ PA_12, (uint32_t) SPI1, SPI1_MOSI, 0 },//"SPI1_MOSI" },
{ PA_12, (uint32_t) USART1, USART1_RTS, 1 },//"USART1_RTS" },
{ PA_12, (uint32_t) TIM1, TIM1_ETR, 2 },//"TIM1_ETR" },
{ PA_12, (uint32_t) ADC1, ADC1_IN12, 12 },//"ADC1_IN12" }, // *** ADC

{ PA_13, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_13, (uint32_t) ADC1, ADC1_IN13, 13 },//"ADC1_IN13" }, // *** ADC

{ PA_14, (uint32_t) USART2, USART2_TX, 1 },//"USART2_TX" },
{ PA_14, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_14, (uint32_t) SPI1, SPI1_NSS, 8 },//"SPI1_NSS" },
{ PA_14, (uint32_t) USART2, USART2_RX, 9 },//"USART2_RX" },
{ PA_14, (uint32_t) TIM1, TIM1_CH1, 10 },//"TIM1_CH1" },
{ PA_14, (uint32_t) RCC, MCO2, 11 },
{ PA_14, (uint32_t) USART1, USART1_RTS, 12 },//"USART2_RTS" },
{ PA_14, (uint32_t) ADC1, ADC1_IN14, 14 },//"ADC1_IN14" }, // *** ADC


{ PA_15, (uint32_t) SPI1, SPI1_NSS, 0 },//"SPI1_NSS" },
{ PA_15, (uint32_t) USART2, USART2_RX, 1 },//"USART2_RX" },
{ PA_15, (uint32_t) TIM1, TIM1_CH1, 2 },//"TIM1_CH1" },
{ PA_15, (uint32_t) RCC, MCO2, 3},// MCO2?
{ PA_15, (uint32_t) USART1, USART1_RTS, 4 },//"USART1_RTS" },
{ PA_15, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PA_15, (uint32_t) ADC1, ADC1_IN15, 15 },//"ADC1_IN15" }, // *** ADC


{ PB_0, (uint32_t) SPI1, SPI1_NSS, 0 },//"SPI1_NSS" },
{ PB_0, (uint32_t) TIM3, TIM3_CH3, 1 },//"TIM3_CH3" },
{ PB_0, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PB_0, (uint32_t) ADC1, ADC1_IN17, 17 },//"ADC1_IN17" }, // *** ADC

{ PB_1, (uint32_t) TIM14, TIM14_CH1, 0 },//"TIM1_CH3N" },
{ PB_1, (uint32_t) TIM3, TIM3_CH4, 1 },//"TIM3_CH4" },
{ PB_1, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PB_1, (uint32_t) ADC1, ADC1_IN18, 18 },//"ADC1_IN18" }, // *** ADC

{ PB_2, (uint32_t) USART1, USART1_RX, 0 },//"USART1_RX" },
{ PB_2, (uint32_t) RCC, MCO2, 3 },//MCO2?
{ PB_2, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PB_2, (uint32_t) ADC1, ADC1_IN19, 19 },//"ADC1_IN19" }, // *** ADC

{ PB_3, (uint32_t) SPI1, SPI1_SCK, 0 },//"SPI1_SCK" },
{ PB_3, (uint32_t) TIM1, TIM1_CH2, 1 },//"TIM1_CH2" },
{ PB_3, (uint32_t) TIM3, TIM3_CH2, 3 },//"TIM3_CH2" },
{ PB_3, (uint32_t) USART1, USART1_RTS, 4 },//"USART1_RTS" },
{ PB_3, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PB_4, (uint32_t) SPI1, SPI1_MISO, 0 },//"SPI1_MISO" },
{ PB_4, (uint32_t) TIM3, TIM3_CH1, 1 },//"TIM3_CH1" },
{ PB_4, (uint32_t) USART1, USART1_CTS, 4 },//"USART1_CTS" },
{ PB_4, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PB_5, (uint32_t) SPI1, SPI1_MOSI, 0 },//"SPI1_MOSI" },
{ PB_5, (uint32_t) TIM3, TIM3_CH2, 1 },//"TIM3_CH2" },
{ PB_5, (uint32_t) TIM3, TIM3_CH3, 3 },//"TIM3_CH3" },
{ PB_5, (uint32_t) I2C1, I2C1_SMBA, 6 },//"I2C1_SMBA" },

{ PB_6, (uint32_t) USART1, USART1_TX, 0 },//"USART1_TX" },
{ PB_6, (uint32_t) TIM1, TIM1_CH3, 1 },//"TIM1_CH3" },
{ PB_6, (uint32_t) TIM3, TIM3_CH3, 3 },//"TIM3_CH3" },
{ PB_6, (uint32_t) USART1, USART1_RTS, 4 },//"USART1_RTS" },
{ PB_6, (uint32_t) USART1, USART1_CTS, 5 },//"USART1_CTS" },
{ PB_6, (uint32_t) I2C1, I2C1_SDA, 6 },//"I2C1_SDA" },
{ PB_6, (uint32_t) I2C1, I2C1_SMBA, 7 },//"I2C1_SMBA" },
{ PB_6, (uint32_t) SPI1, SPI1_MOSI, 8 },//"SPI1_MOSI" },
{ PB_6, (uint32_t) SPI1, SPI1_MISO, 9 },//"SPI1_MISO" },
{ PB_6, (uint32_t) SPI1, SPI1_SCK, 10 },//"SPI1_SCK" },
{ PB_6, (uint32_t) TIM1, TIM1_CH2, 11 },//"TIM1_CH2" },
{ PB_6, (uint32_t) TIM3, TIM3_CH1, 12 },//"TIM3_CH1" },
{ PB_6, (uint32_t) TIM3, TIM3_CH2, 13 },//"TIM3_CH2" },
{ PB_6, (uint32_t) I2C1, I2C1_SCL, 14 },//"I2C1_SCL" },

{ PB_7, (uint32_t) USART1, USART1_RX, 0 },//"USART1_RX" },
{ PB_7, (uint32_t) TIM1, TIM1_CH4, 1 },//"TIM1_CH4" },
{ PB_7, (uint32_t) TIM3, TIM3_CH4, 3 },//"TIM3_CH4" },
{ PB_7, (uint32_t) I2C1, I2C1_SDA, 6 },//"I2C1_SDA" },
{ PB_7, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PB_7, (uint32_t) USART2, USART2_CTS, 9 },//"USART2_CTS" },
{ PB_7, (uint32_t) TIM16, TIM16_CH1, 10 },//"TIM16_CH1" },
{ PB_7, (uint32_t) TIM3, TIM3_CH1, 11 },//"TIM3_CH1" },
{ PB_7, (uint32_t) I2C1, I2C1_SCL, 14 },//"I2C1_SCL" },

{ PB_8, (uint32_t) USART2, USART2_CTS, 1 },//"USART2_CTS" },
{ PB_8, (uint32_t) TIM16, TIM16_CH1, 2 },//"TIM16_CH1" },
{ PB_8, (uint32_t) TIM3, TIM3_CH1, 3 },//"TIM3_CH1" },
{ PB_8, (uint32_t) I2C1, I2C1_SCL, 6 },//"I2C1_SCL" },
{ PB_8, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PB_9, (uint32_t) USART2, USART2_RTS, 1 },//"USART2_RTS" },
{ PB_9, (uint32_t) TIM17, TIM17_CH1, 2 },//"TIM17_CH1" },
{ PB_9, (uint32_t) TIM3, TIM3_CH2, 3 },//"TIM3_CH2" },
{ PB_9, (uint32_t) I2C1, I2C1_SDA, 6 },//"I2C1_SDA" },
{ PB_9, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PB_10, (uint32_t) ADC1, ADC1_IN20, 20 },//"ADC1_IN20" }, // *** ADC

{ PB_11, (uint32_t) ADC1, ADC1_IN21, 21 },//"ADC1_IN21" }, // *** ADC

{ PB_12, (uint32_t) TIM1, TIM1_BKIN, 1 },//"TIM1_BKIN" },
{ PB_12, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },
{ PB_12, (uint32_t) ADC1, ADC1_IN22, 22 },//"ADC1_IN22" }, // *** ADC

{ PB_13, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PB_14, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PB_15, (uint32_t) SYSCFG, EVENTOUT, 7 },//"EVENTOUT" },

{ PC_6, (uint32_t) TIM3, TIM3_CH1, 1 },//"TIM3_CH1" },

{ PC_7, (uint32_t) TIM3, TIM3_CH2, 1 },//"TIM3_CH2" },

{ PC_13, (uint32_t) TIM1, TIM1_ETR, 1 },//"TIM1_ETR" },//?

{ PC_14, (uint32_t) USART1, USART1_TX, 0 },//"USART1_TX" },
{ PC_14, (uint32_t) TIM1, TIM1_ETR, 1 },//"TIM1_ETR" },
{ PC_14, (uint32_t) USART2, USART2_RTS, 9 },//"USART2_RTS" },
{ PC_14, (uint32_t) TIM17, TIM17_CH1, 10 },//"TIM17_CH1" },
{ PC_14, (uint32_t) TIM3, TIM3_CH2, 11 },//"TIM3_CH2" },
{ PC_14, (uint32_t) I2C1, I2C1_SDA, 14 },//"I2C1_SDA" },
{ PC_14, (uint32_t) SYSCFG, EVENTOUT, 15 },//"EVENTOUT" },

{ PC_15, (uint32_t) TIM1, TIM1_ETR, 2 },//"TIM1_ETR" },
{ PC_15, (uint32_t) TIM3, TIM3_CH3, 3 },//"TIM3_CH3" },

{ PD_0, (uint32_t) TIM16, TIM16_CH1, 2 },//"TIM16_CH1" },
{ PD_0, (uint32_t) SYSCFG, EVENTOUT, 0 },//"EVENTOUT" },

{ PD_1, (uint32_t) TIM17, TIM17_CH1, 2 },//"TIM17_CH1" },
{ PD_1, (uint32_t) SYSCFG, EVENTOUT, 0 },//"EVENTOUT" },

{ PD_2, (uint32_t) TIM3, TIM3_ETR, 1 },//"TIM3_ETR" },

{ PD_3, (uint32_t) USART1, USART1_CTS, 0 },//"USART1_CTS" },

{ PF_0, (uint32_t) TIM14, TIM14_CH1, 2 },//"TIM14_CH1" },

{ PF_2, (uint32_t) TIM1, TIM1_CH4, 1 },//"TIM1_CH4" },
{ PF_2, (uint32_t) MCO, MCO, 0 },//"MCO" },

//== 8>< ----- internal signals fit in this table
{ INTERNAL_VREF,  (uint32_t) ADC1, ADC1_VREF,  ADC_CHANNEL_VREFINT },//"ADC1_VREF" }, // find the channel number
{ INTERNAL_VTEMP, (uint32_t) ADC1, ADC1_VTEMP, ADC_CHANNEL_TEMPSENSOR },//"ADC1_VTEMP" }, // find the channel number

};

const uint32_t DS_PADsCount = countof(DS_PADs);

//=============================
// const PinAlternateDescription_t PAD[]
// search functions from this const global structure (depends on dice and package)
/*
typedef struct {
  u8 PinName;
  u32 PPP_Base;
  SignalName_t SignalName;
  u8 AF;// ADC channel
  char* SignalNameString;
} PinAlternateDescription_t;
*/

// find the entry for the corresponding pinname and signal name (because one pin can have multiple signals from same peripheral...)
// we return a pointer to the entry so C code can directly -> its elements
// this is not itterative as there should be only one solution.
DS_PAD_t const* pDS_PAD_MatchByPinAndSignal(PadName_t PinName, STM32_SignalName_t SignalName) {
  
  uint32_t i;
  for(i=0;i<countof(DS_PADs);i++) {
    
    if(DS_PADs[i].PinName==PinName)
      if(DS_PADs[i].SignalName==SignalName) {
        // we found the pin!
        return &DS_PADs[i]; 
      };
  };
  
  while(1); // nothing found...
  //return NULL; // failed.
}



int32_t iDS_PAD_NextMatchBySignal(STM32_SignalName_t SignalName, int32_t startindex) {
  
  uint32_t i;
  for(i=0;i<countof(DS_PADs);i++) {
    
    if(DS_PADs[i].SignalName==SignalName) {
        // we found the pin!
        return i;
    };
  };
  
  return -1; // nothing found...
}


#if 0 // DMAMUX related table


const DMA_StreamInfo_t DMA_StreamInfo[] = {

  {DMA1, DMA1_Stream0, 0, DMA1_Stream0_IRQn, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_FEIF0, DMA_FLAG_TCIF0 },
  {DMA1, DMA1_Stream1, 1, DMA1_Stream1_IRQn, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_FEIF1, DMA_FLAG_TCIF1 },
  {DMA1, DMA1_Stream2, 2, DMA1_Stream2_IRQn, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_FEIF2, DMA_FLAG_TCIF2 },
};

DMA_StreamInfo_t* Get_pDMA_Info(DMA_Stream_TypeDef* DMA_Stream) {
  
  u8 n;
  
  for(n=0;n<countof(DMA_StreamInfo);n++) {
    
      if(DMA_Stream==DMA_StreamInfo[n].Stream) // the stream is a memory location, it is unique
        return (DMA_StreamInfo_t*) &DMA_StreamInfo[n];
  }
  
  while(1);
  return 0; // failed to find it
}

const DMAMUX_Mapping_t DS_DMAMUX_Inputs[] = {
  
  {1, dmamux_req_gen0, "dmamux_req_gen0"},
  {2, dmamux_req_gen1, "dmamux_req_gen1"},
  {3, dmamux_req_gen2, "dmamux_req_gen2"},
  {4, dmamux_req_gen3, "dmamux_req_gen3"},
  {5, adc_dma, "adc_dma"},
  {10, i2c1_rx_dma, "i2c_rx_dma"},
  {11, i2c1_tx_dma, "i2c_tx_dma"},
  {16, spi1_rx_dma, "spi1_rx_dma"},
  {17, spi1_tx_dma, "spi1_tx_dma"},
  {20, TIM1_CH1, "TIM1_CH1" },
  {21, TIM1_CH2, "TIM1_CH2" },
  {22, TIM1_CH3, "TIM1_CH3" },
  {23, TIM1_CH4, "TIM1_CH4" },
  {24, TIM1_TRIG_COM, "TIM1_TRIG_COM"},
  {25, TIM1_UP, "TIM1_UP"},
  {32, TIM3_CH1, "TIM3_CH1"},
  {33, TIM3_CH2, "TIM3_CH2"},
  {34, TIM3_CH3, "TIM3_CH3"},
  {35, TIM3_CH4, "TIM3_CH4"},
  {36, TIM3_TRIG, "TIM3_TRIG"},
  {37, TIM3_UP, "TIM3_UP"},
  {44, TIM16_CH1, "TIM16_CH1"},
  {45, TIM16_TRIG_COM, "TIM16_TRIG_COM"},  
  {46, TIM16_UP, "TIM16_UP"},
  {47, TIM17_CH1, "TIM17_CH1"},
  {48, TIM17_TRIG_COM, "TIM17_TRIG_COM"},
  {49, TIM17_UP, "TIM17_UP"},
  {50, usart1_rx_dma, "usart1_rx_dma"},
  {51, usart1_tx_dma, "usart1_tx_dma"},
  {52, usart2_rx_dma, "usart2_x_dma"},
  {53, usart2_tx_dma, "usart2_tx_dma"},
}

const DMAMUX_Mapping_t DS_DMAMUX_Triggers[] = {
  { 0, EXTI_LINE0, "EXTI_LINE0" }, 
  { 1, EXTI_LINE1, "EXTI_LINE1" }, 
  { 2, EXTI_LINE2, "EXTI_LINE2" }, 
  { 3, EXTI_LINE3, "EXTI_LINE3" }, 
  { 4, EXTI_LINE4, "EXTI_LINE4" }, 
  { 5, EXTI_LINE5, "EXTI_LINE5" }, 
  { 6, EXTI_LINE6, "EXTI_LINE6" }, 
  { 7, EXTI_LINE7, "EXTI_LINE7" }, 
  { 8, EXTI_LINE8, "EXTI_LINE8" }, 
  { 9, EXTI_LINE9, "EXTI_LINE9" }, 
  { 10, EXTI_LINE10, "EXTI_LINE10" }, 
  { 11, EXTI_LINE11, "EXTI_LINE11" }, 
  
  { 12, EXTI_LINE12 }, 
  { 13, EXTI_LINE13 }, 
  { 14, EXTI_LINE14 }, 
  { 15, EXTI_LINE15 }, 
  { 16, dmamux_evt0 }, 
  { 17, dmamux_evt1 }, 
  { 18, dmamux_evt2 }, 
  { 21, TIM14 }, 
}

const DMAMUX_Mapping_t DS_DMAMUX_Syncs[] = {
  { 0, EXTI_LINE0, "EXTI_LINE0" }, 
  { 1, EXTI_LINE1, "EXTI_LINE1" }, 
  { 2, EXTI_LINE2, "EXTI_LINE2" }, 
  { 3, EXTI_LINE3, "EXTI_LINE3" }, 
  { 4, EXTI_LINE4, "EXTI_LINE4" }, 
  { 5, EXTI_LINE5, "EXTI_LINE5" }, 
  { 6, EXTI_LINE6, "EXTI_LINE6" }, 
  { 7, EXTI_LINE7, "EXTI_LINE7" }, 
  { 8, EXTI_LINE8, "EXTI_LINE8" }, 
  { 9, EXTI_LINE9, "EXTI_LINE9" }, 
  { 10, EXTI_LINE10, "EXTI_LINE10" }, 
  { 11, EXTI_LINE11, "EXTI_LINE11" }, 
  
  { 12, EXTI_LINE12 }, 
  { 13, EXTI_LINE13 }, 
  { 14, EXTI_LINE14 }, 
  { 15, EXTI_LINE15 }, 
  { 16, dmamux_evt0 }, 
  { 17, dmamux_evt1 }, 
  { 18, dmamux_evt2 }, 
  { 21, TIM14 }, 
}

const DMAMUX_Mapping_t DS_DMAMUX_Triggers[] = {

const DMA_StreamChannelInfo_t DMA_StreamChannelInfo[] = {
//========================= DMA 1 ======================================================================================  
  {DMA1, DMA1_Stream0, 0, DMA_Channel_0, (uint32_t)SPI3, SPI3_RX, DMA_DIR_PeripheralToMemory}, // 0
  {DMA1, DMA1_Stream0, 0, DMA_Channel_1, (uint32_t)I2C1, I2C1_RX, DMA_DIR_PeripheralToMemory},
  {DMA1, DMA1_Stream0, 0, DMA_Channel_2, (uint32_t)TIM4, TIM4_CH1, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream0, 0, DMA_Channel_3, (uint32_t)I2S3ext, I2S3_EXT_RX, DMA_DIR_PeripheralToMemory}, // ???
  {DMA1, DMA1_Stream0, 0, DMA_Channel_4, (uint32_t)UART5, UART5_RX, DMA_DIR_PeripheralToMemory},
  {DMA1, DMA1_Stream0, 0, DMA_Channel_5, (uint32_t)0/*UART8*/, 0/*UART8_RX*/, DMA_DIR_PeripheralToMemory}, // ???
  {DMA1, DMA1_Stream0, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_CH3, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream0, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
//  {DMA1, DMA1_Stream0, 0, DMA_Channel_7, (uint32_t)UART5, 0, DMA_DIR_PeripheralToMemory},

//  {DMA, DMA2_Stream, 0, DMA_Channel_0, (uint32_t)0, 0, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
//  {DMA, DMA2_Stream, 0, DMA_Channel_1, (uint32_t)0, 0, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
//  {DMA, DMA2_Stream, 0, DMA_Channel_2, (uint32_t)0, 0, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream1, 0, DMA_Channel_3, (uint32_t)TIM2, TIM2_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral}, // ???
  {DMA1, DMA1_Stream1, 0, DMA_Channel_3, (uint32_t)TIM5, TIM5_CH4, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},  
  {DMA1, DMA1_Stream1, 0, DMA_Channel_4, (uint32_t)USART3, USART3_TX, DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream1, 0, DMA_Channel_5, (uint32_t)0/*UART7*/, 0/*UART7_TX*/, DMA_DIR_MemoryToPeripheral}, // ???
  {DMA1, DMA1_Stream1, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_CH4, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream1, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_TRIG, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},  
  {DMA1, DMA1_Stream1, 0, DMA_Channel_7, (uint32_t)TIM5, TIM5_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},

  {DMA1, DMA1_Stream2, 0, DMA_Channel_0, (uint32_t)SPI3, SPI3_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_1, (uint32_t)TIM7, TIM7_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_2, (uint32_t)I2S3ext, I2S3_EXT_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_3, (uint32_t)I2C3, I2C3_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_4, (uint32_t)UART4, UART4_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_5, (uint32_t)TIM3, TIM3_CH4, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_5, (uint32_t)TIM3, TIM3_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},  
  {DMA1, DMA1_Stream2, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_CH1, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream2, 0, DMA_Channel_7, (uint32_t)I2C2, I2C2_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},

  {DMA1, DMA1_Stream3, 0, DMA_Channel_0, (uint32_t)SPI2, SPI2_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
//  {DMA1, DMA1_Stream3, 0, DMA_Channel_1, (uint32_t), , DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_2, (uint32_t)TIM4, TIM4_CH2, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_3, (uint32_t)I2S2ext, I2S2_EXT_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_4, (uint32_t)USART3, USART3_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_5, (uint32_t)0/*UART7*/,0/*UART7_RX*/ , DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_CH4, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_TRIG, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream3, 0, DMA_Channel_7, (uint32_t)I2C2, I2C2_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  
  {DMA1, DMA1_Stream4, 0, DMA_Channel_0, (uint32_t)SPI2, SPI2_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_1, (uint32_t)TIM7, TIM7_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_2, (uint32_t)I2S2ext, I2S2_EXT_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_3, (uint32_t)I2C3, I2C3_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_4, (uint32_t)UART4, UART4_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_5, (uint32_t)TIM3, TIM3_CH1, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_5, (uint32_t)TIM3, TIM3_TRIG, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_CH2, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream4, 0, DMA_Channel_7, (uint32_t)USART3, USART3_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  
  {DMA1, DMA1_Stream5, 0, DMA_Channel_0, (uint32_t)SPI3, SPI3_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream5, 0, DMA_Channel_1, (uint32_t)I2C1, I2C1_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream5, 0, DMA_Channel_2, (uint32_t)I2S3ext, I2S3_EXT_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream5, 0, DMA_Channel_3, (uint32_t)TIM2, TIM2_CH1, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream5, 0, DMA_Channel_4, (uint32_t)USART2, USART2_RX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream5, 0, DMA_Channel_5, (uint32_t)TIM3, TIM3_CH2, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
//  {DMA1, DMA1_Stream5, 0, DMA_Channel_6, (uint32_t), , DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream5, 0, DMA_Channel_7, (uint32_t)DAC, DAC1_ANALOG, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  
//  {DMA1, DMA1_Stream6, 0, DMA_Channel_0, (uint32_t), , DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_1, (uint32_t)I2C1, I2C1_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_2, (uint32_t)TIM4, TIM4_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_3, (uint32_t)TIM2, TIM2_CH2, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_3, (uint32_t)TIM2, TIM2_CH4, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},  
  {DMA1, DMA1_Stream6, 0, DMA_Channel_4, (uint32_t)USART2, USART2_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_5, (uint32_t)0/*UART8*/,0/*UART8_RX*/ , DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_6, (uint32_t)TIM5, TIM5_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream6, 0, DMA_Channel_7, (uint32_t)DAC, DAC2_ANALOG, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  
  {DMA1, DMA1_Stream7, 0, DMA_Channel_0, (uint32_t)SPI3, SPI3_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream7, 0, DMA_Channel_1, (uint32_t)I2C1, I2C1_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream7, 0, DMA_Channel_2, (uint32_t)TIM4, TIM4_CH3, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream7, 0, DMA_Channel_3, (uint32_t)TIM2, TIM2_UP, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream7, 0, DMA_Channel_3, (uint32_t)TIM2, TIM2_CH4, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},  
  {DMA1, DMA1_Stream7, 0, DMA_Channel_4, (uint32_t)UART5, UART5_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream7, 0, DMA_Channel_5, (uint32_t)TIM3, TIM3_CH3, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
//  {DMA1, DMA1_Stream7, 0, DMA_Channel_6, (uint32_t), , DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
  {DMA1, DMA1_Stream7, 0, DMA_Channel_7, (uint32_t)I2C2, I2C2_TX, DMA_DIR_PeripheralToMemory | DMA_DIR_MemoryToPeripheral},
}  


DMA_StreamChannelInfo_t* Get_pStreamChannelForPPP_Signal(u32 PPP_Adr, u32 Signal, u32 Direction) {
  
  u32 n;
  
  for(n=0;n<countof(DMA_StreamChannelInfo);n++) {
    if(DMA_StreamChannelInfo[n].PPP_Adr == PPP_Adr)
      if(DMA_StreamChannelInfo[n].Signal == Signal) {
        if(IsDMA_StreamBooked(DMA_StreamChannelInfo[n].Stream)==FALSE) // if the stream is not booked, use this one
//        if(DMA_StreamChannelInfo[n].Direction & Direction) // because MemoryToPeripheral is 0x00, this does not work. It is only for XXX_TX and XXX_RX signals.
          return (DMA_StreamChannelInfo_t*) &DMA_StreamChannelInfo[n];
      }
  };
  
  while(1); // not found!
  return 0;
}

#endif
