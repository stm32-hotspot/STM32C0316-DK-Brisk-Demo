
#ifndef _IRQ_SELF_SETTLE_H_
#define _IRQ_SELF_SETTLE_H_
/*
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
/* from startup_stm32c031.h from CMSIS subfolder this is where the interrupt routine names are defined

        DCD     PendSV_Handler                 // PendSV Handler
        DCD     SysTick_Handler                // SysTick Handler

        // External Interrupts
        DCD     WWDG_INT                // Window Watchdog
        DCD     RTC_INT                 // RTC through EXTI Line
        DCD     FLASH_INT               // FLASH
        DCD     RCC_INT                 // RCC
        DCD     EXTI0_1_INT             // EXTI Line 0 and 1
        DCD     EXTI2_3_INT             // EXTI Line 2 and 3
        DCD     EXTI4_15_INT            // EXTI Line 4 to 15
        DCD     DMA1_Channel1_INT       // DMA1 Channel 1
        DCD     DMA1_Channel2_3_INT     // DMA1 Channel 2 and Channel 3
        DCD     ADC1_INT                // ADC1
        DCD     TIM1_BRK_UP_TRG_COM_INT // TIM1 Break, Update, Trigger and Commutation
        DCD     TIM1_CC_INT             // TIM1 Capture Compare
        DCD     TIM3_INT                // TIM3
        DCD     TIM14_INT               // TIM14
        DCD     TIM16_INT               // TIM16
        DCD     TIM17_INT               // TIM17
        DCD     I2C1_INT                // I2C1
        DCD     SPI1_INT                // SPI1
        DCD     USART1_INT              // USART1
        DCD     USART2_INT              // USART2

*/
typedef enum {
//PendSV_INT, //                 // PendSV Handler
//SysTick_INT, //                // SysTick Handler
        // External Interrupts
SYSTICK_INT,  // 0 added to the list
WWDG_INT,                // Window Watchdog
RTC_INT,                 // 2 RTC through EXTI Line
//FLASH_INT,               // FLASH
RCC_INT,                 // RCC
EXTI0_1_INT,             // 4 EXTI Line 0 and 1
EXTI2_3_INT,             //  EXTI Line 2 and 3
EXTI4_15_INT,            // 6 EXTI Line 4 to 15
DMA1_Channel1_INT,       //  DMA1 Channel 1
DMA1_Channel2_3_INT,     // 8 DMA1 Channel 2 and Channel 3
ADC1_INT,                //  ADC1
//TIM1_BRK_UP_TRG_COM_INT, // TIM1 Break, Update, Trigger and Commutation
TIM1_CC_INT,             // 10 TIM1 Capture Compare
TIM3_INT,                // TIM3
TIM14_INT,               // 12 TIM14
TIM16_INT,               // TIM16
TIM17_INT,               // 14 TIM17
I2C1_INT,                // I2C1
SPI1_INT,                // 16 SPI1
USART1_INT,              // USART1
USART2_INT,              // USART2
//MANUAL_1, // SW user triggered code segment measurement
//MANUAL_2,
INT_COUNT // the number of interrupts
} INTs_Index_t; //


typedef struct {
  int32_t last_enter;
  int32_t enter; // interrupt routine starts
//  int32_t release; // pending interrupt cleared
  int32_t leave; // interrupt routine ends
  int32_t min_duration;
  int32_t max_duration;
  int32_t min_period;
  int32_t max_period; // probably not a constrain
  int32_t nb_count;
//  int16_t nb_too_late;
  int16_t nb_countdown;
//  int8_t was_too_late;
//  int8_t level;
//  int8_t adjustable_level;
//  int8_t enabled;
  
} INT_Stats_t;

// now we need to reorganize the interrupt priorities 
// global structure

typedef struct {
  INT_Stats_t All_cy[INT_COUNT]; // the various interrupt sources in need of priority setting
  INT_Stats_t INT_cy_temp; // scratch for swaps
//  int8_t lowest_level;
//  int8_t highest_level;
//  uint32_t systick_bitpos;
  uint32_t tick_msb;
} INT_Stats_Global_t;

extern INT_Stats_Global_t gINT_Stats;

int32_t GetTimestamp(void);
void INT_Enter(INTs_Index_t ndx);
void INT_Leave(INTs_Index_t ndx);

int32_t INTs_AllStats_Init(void);
int32_t INT_Stats_Init(INTs_Index_t ndx);


void IRQ_OVR_DemoEnter(void);
void IRQ_OVR_DemoLeave(void);
void IRQ_OVR_ProtectionEnable(void);
void IRQ_OVR_ProtectionDisable(void);

void IRQ_OVR_DemoEXTI_MainLoop_100msec(void);
void EXTI0_1_IRQHandler(void);
#endif
