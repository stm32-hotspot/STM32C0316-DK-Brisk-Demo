/**
  ******************************************************************************
  * @file    leds.h
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

#ifndef _LEDS_H_ // enables LED on/off, blinking ON/OFF (infinite or limited pulses with duty cycles), and if PWM mode is enabled, allow dimming in % for example to simulate most laptop "sleep" LEDs. 
#define _LEDS_H_

#include "io_pins.h"

typedef struct {
  IO_Pad_t* pPad;// contains the port and pin position as a single entity
  // LED Dimming
  TIM_TypeDef* TIM; // if PWM mode is available
  uint32_t TIM_CH;
  int8_t Dim_perc; // dimming as percentage
} LED_t;

// We put here time dependent parameters

typedef struct {
  LED_t* pLED;
  uint16_t Blink_Period_x10ms; // 123 = 1.23 second
  uint16_t Blink_OffTime_x10ms; // 097 = 0.97 seconds will be off
  uint16_t Blink_Countdown_x10ms; // this is the countdown from period down to zero. led is on until it reached the offtime.
  
  uint8_t Blink_Countdown;
//  uint8_t Blink_CountdownCompleted;
  
  uint8_t Dim_perc;
} TimedLED_t;

// There are fixed 12 clock LEDs which looks like a 12 hours clock (1 to 12).
// This is to match an index (integer) into an LED name. (LED 4 = LED_SB)
typedef enum { // Brisk LEDs example (static)
  NO_LED = 0,
  
  LED_DEMO, // 1
/*  LED_NR, // 2
  LED_NG, // 3
  
  LED_SB, // 4
  LED_SR, // 5
  LED_SG, // 6
  
  LED_EB, // 7
  LED_ER, // 8
  LED_EG, // 9
  
  LED_WB, // 10
  LED_WR, // 11
  LED_WG, // 12
*/  
  LED_COUNT
} BriskClockLEDs;

#define BRISK_LED_ON 100 // percentage 100 = full on
#define BRISK_LED_OFF 0 // percentage 0 = full off
#define BRISK_LED_BLINK_FOREVER 0xFF // special code

void BriskDimLED(uint8_t index, int8_t percentage);

// Time LED (Blinking, pulses)
void BriskTimedLED_10msecTick(void);
void BriskDimTimedLED(uint8_t index, int8_t percentage);
void BriskBlinkTimedLED(uint8_t index, uint16_t Blink_Period_ms, uint16_t Blink_OffTime_ms, uint8_t Blink_Countdown );

// system
//void BriskLEDsInit(void); // use this when not using blink timed function
void BriskTestLEDs(void);
void BriskTimedLEDsInit(void);
void BriskTimedLED_10msecTick(void);

void BriskTimedLED_Disable(void);
void BriskTimedLED_Enable(void);

extern TimedLED_t BriskTimedLEDs[LED_COUNT];

#endif
