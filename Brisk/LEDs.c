/**
  ******************************************************************************
  * @file    LED.c (LED[s] manager)
  * @author  S.Ma
  * @brief   LED on/off/dimming with PWM using percentage / Blinking pulses mode.
  *          The dimming mode requires TIMER compare HW support, can be disabled at this level.
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

// implements digital LED with ON/OFF, blinking (period programmable, nb of pulses or infinite), and if TIMER resources is used, DIMMING in % with duty cycle. (like SLEEP LED for laptops)

void LEDsConfigureTimers(TIM_TypeDef* Timer);
void LEDsConfigureChannel(TIM_TypeDef* Timer, uint32_t TIM_CHANNEL);
void LEDsSetDutyCycle(LED_t* pLED, uint32_t duty);

const uint16_t PercentToDuty[] = {
  
0, // 0
1, // 1
2, // 2
3, // 
4, // 4
5, // 
6, // 6
7, // 
8, // 8
9, // 
10, // 10
11, // 
12, // 
13, // 
14, // 
15, // 
16, // 
17, // 
18, // 
19, // 
20, // 20
21, // 
22, // 
23, // 
24, // 
25, // 
26, // 
27, // 
28, // 
29, // 
30, // 30
31, // 
32, // 
33, // 
34, // 
35, // 
36, // 
37, // 
38, // 
39, // 
40, // 40
41, // 
42, // 
43, // 
44, // 
45, // 
46, // 
47, // 
48, // 
49, // 
50, // 50
51, // 
52, // 
53, // 
54, // 
55, // 
56, // 
57, // 
58, // 
59, // 
60, // 60
61, // 
62, // 
63, // 
64, // 
65, // 
66, // 
67, // 
68, // 
69, // 
70, // 70
71, // 
72, // 
73, // 73
//======
75, // 74
83, // 75
92, // 
102, // 
112, // 
124, // 
137, // 80
152, // 
167, // 
185, // 
205, // 
226, // 
250, // 
276, // 
305, // 
337, // 
373, // 90
412, // 
455, // 
503, // 
556, // 
615, // 
679, // 
751, // 
830, // 
917, // 
1000, // 100
};

//========== LED Definitions as Static Digital Output or Timer PWM based

const IO_Pad_t LED1_AsOutput = {  PA_5, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED1 = PC7 Digital Output
const IO_Pad_t LED1_AsPWM =    {  PA_5, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH, .Signal = TIM1_CH1/*5*/ } }; // LED3 = PE5 Digital WPM T1.1 AF5
// we also need to manage the timers.
// we need 100 log levels, so 1000 steps
// if dimming period is 1kHz, we run the timer at 1 MHz
// TIMER 3,4,16 are tied to LEDs

  LED_t BriskLEDs[LED_COUNT] = {
    {    (IO_Pad_t*)0,                  0,     0,     0 },
//    {    (IO_Pad_t*)&LED1_AsOutput,     0,     0,     0 },
    {    (IO_Pad_t*)&LED1_AsPWM,     TIM1,     TIM_CHANNEL_1,     0 },    
  };
              
static uint8_t BriskLEDsDisabled = 0;
  
//=================================

void BriskTimedLED_Disable(void) { BriskLEDsDisabled=1;}
void BriskTimedLED_Enable(void) { BriskLEDsDisabled=0;}


void BriskLEDsInit(void);
void BriskLEDsInit(void) { // Initialize all LEDs as output

  // we assume all GPIO clock enabled have been done already
  // first, all LEDs are put as digital output low (off)
  LED_t * pLED;
  uint8_t index;

  for(index=1;index<LED_COUNT;index++) {
    pLED = &BriskLEDs[index]; // get handle to the pointed LED element
//    IO_PadSetHigh((IO_Pad_t*)pLED->pPad->PadName);
    IO_PadInit((IO_Pad_t*)pLED->pPad);
    // if it has PWM option, enable the Timer for it
#if 1 // support timer demo     
    if(pLED->TIM) { // if not null
      // not nice, easy to read, compact code
      __HAL_RCC_TIM1_CLK_ENABLE();
      LEDsConfigureTimers(pLED->TIM);
      LEDsConfigureChannel(pLED->TIM, pLED->TIM_CH);
    };
#endif
  };
             
  BriskTestLEDs();
}

void BriskDimLED(uint8_t index, int8_t percentage) {// a negative value will turn the LED OFF (toggling the sign is used for blinking implementation)

  LED_t * pLED;
  
  pLED = &BriskLEDs[index]; // get handle to the pointed LED element
  
  if(pLED->TIM) { // if not null, is Timer PWM dimmable
    if(percentage>100) percentage = 100; // or TrapError, clip to max 100
    if(percentage<0) LEDsSetDutyCycle(pLED, PercentToDuty[0]); // 0 to 100 becomes 0 to 1000 as log, later use a lookup table
      else LEDsSetDutyCycle(pLED, PercentToDuty[percentage]); // 0 to 100 becomes 0 to 1000 as log, later use a lookup table
  }else{
    if(percentage<50){ // Turn LED OFF
      IO_PadSetHigh(pLED->pPad->PadName);
    }else{ // Turn LED ON
      IO_PadSetLow(pLED->pPad->PadName);
    };
  };
  
  pLED->Dim_perc = percentage; // save the used percentage dimming value
}

// Timer HW peripheral handling to generate PWM 

/* Timer handler declaration */
#if 1 // PWM LED Dimmig demo enabled
TIM_HandleTypeDef    LED_TimHandle;
/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfigLED;

void LEDsConfigureTimers(TIM_TypeDef* Timer) {

  LED_TimHandle.Instance = Timer;
  LED_TimHandle.Init.Prescaler         = BASE_TIMER_CLOCK_MHZ-1; // SYSCLK = 48 MHz, timer will run at 1MHz
  LED_TimHandle.Init.Period            = 999; // counter period is 1000 steps
  LED_TimHandle.Init.ClockDivision     = 0;
  LED_TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  LED_TimHandle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&LED_TimHandle) != HAL_OK)
  { /* Initialization Error */
    TrapError();
  }
}

void LEDsConfigureChannel(TIM_TypeDef* Timer, uint32_t TIM_CHANNEL) {
  
  LED_TimHandle.Instance = Timer;

  sConfigLED.OCMode       = TIM_OCMODE_PWM1;
  sConfigLED.OCPolarity   = TIM_OCPOLARITY_LOW;
  sConfigLED.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfigLED.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfigLED.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfigLED.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfigLED.Pulse = 1;  
  if (HAL_TIM_PWM_ConfigChannel(&LED_TimHandle, &sConfigLED, TIM_CHANNEL ) != HAL_OK)
  {
    TrapError();
  }
  
  if (HAL_TIM_PWM_Start(&LED_TimHandle, TIM_CHANNEL) != HAL_OK)
  { /* PWM Generation Error */
    TrapError();
  }
  
}
#endif 

void LEDsSetDutyCycle(LED_t* pLED, uint32_t duty) {
#if 1   // PWM dimming enabled
  uint32_t TIM_CHANNEL;
  
  if(pLED->TIM==0) TrapError();
  

  LED_TimHandle.Instance = pLED->TIM;
  TIM_CHANNEL = pLED->TIM_CH;

  sConfigLED.Pulse = duty;
#if 0 // this code doesn't work with HAL C0 version    
  if (HAL_TIM_PWM_ConfigChannel(&LED_TimHandle, &sConfigLED, TIM_CHANNEL) != HAL_OK)
  {
    TrapError();
  }  

  // not compatible with newer HAL

  if (HAL_TIM_PWM_Start(&LED_TimHandle, TIM_CHANNEL) != HAL_OK)
  { /* PWM Generation Error */
    TrapError();
  }
#else // TO REWORK !
  // 4hours rework cost adaptation
  // monster hack which destroys the user LED code entirely: // not scalable, not portable, not efficient
  TIM1->CCR1 = duty; // for a single hardcoded LED....
  
  

#endif

  
#endif  
}

//============================
// Timed LED
  TimedLED_t BriskTimedLEDs[LED_COUNT] = {
    { &BriskLEDs[0], 0, 0, 0,  0, 0,   },
    { &BriskLEDs[1], 0, 0, 0,  0, 0,   },
  };


void BriskTimedLED_10msecTick(void) { // should be called from main loop. not timing critical.
  
  uint8_t index;
  TimedLED_t* pTimedLED;
  
  // coming here every 10msec 
  for(index=1;index<LED_COUNT;index++) {
    
    pTimedLED = &BriskTimedLEDs[index];
    if(pTimedLED->Blink_Countdown_x10ms==0) continue; // if countdown is zero, do nothing
    // countdown is non zero. decrement it.
    pTimedLED->Blink_Countdown_x10ms--;
    if(pTimedLED->Blink_Countdown_x10ms==0) { // cycle completed
      if(pTimedLED->Blink_Countdown==0) continue; // no more blink pulse needed
      // blink pulse needed
      if(pTimedLED->Blink_Countdown!=BRISK_LED_BLINK_FOREVER) pTimedLED->Blink_Countdown--; // if not infinite blinking, countdown, another pulse to run
      if(pTimedLED->Blink_Countdown) pTimedLED->Blink_Countdown_x10ms = pTimedLED->Blink_Period_x10ms;
    };
    
    // update the LED on/off dimming (refresh every 10 msec in case of dynamic/ISR change or ESD glitch)
    if(pTimedLED->Blink_Countdown_x10ms<=pTimedLED->Blink_OffTime_x10ms) { // Set LED OFF
      BriskDimLED(index, 0);
    } else { // Set LED ON
      BriskDimLED(index, pTimedLED->Dim_perc);
    };
    
  }; // for
  
}

// now here is the function to set the parameters including timings
void BriskDimTimedLED(uint8_t index, int8_t percentage) { // static, blinking disabled
  
  if(BriskLEDsDisabled==1) return;
    
  TimedLED_t* pTimedLED = &BriskTimedLEDs[index];
  if(percentage>100) TrapError();
  pTimedLED->Blink_Countdown_x10ms = 0; // disable all blinking
  pTimedLED->Dim_perc = percentage;
  BriskDimLED(index, percentage);
}

// use this function AFTER setting the dimming. Subject to change.
void BriskBlinkTimedLED(uint8_t index, uint16_t Blink_Period_ms, uint16_t Blink_OffTime_ms, uint8_t Blink_Countdown ) {
  
  TimedLED_t* pTimedLED = &BriskTimedLEDs[index];
  if(BriskLEDsDisabled==1) return;
  
  if(Blink_OffTime_ms>Blink_Period_ms) TrapError();
  
  pTimedLED->Blink_OffTime_x10ms = (Blink_OffTime_ms)/10;
  pTimedLED->Blink_Countdown = Blink_Countdown;
  //pTimedLED->Dim_perc = pTimedLED->pLED->Dim_perc; // we backup the chosen dimming
  pTimedLED->Blink_Period_x10ms = (Blink_Period_ms)/10;
  pTimedLED->Blink_Countdown_x10ms = pTimedLED->Blink_Period_x10ms; // this will trigger the ISR handling of the LED
  
}

void BriskTimedLEDsInit(void) {
  
  BriskLEDsInit();
}

uint32_t gLED_TestCounter = 0;

void BriskTestLEDs(void) {

    uint8_t i;
    uint32_t duty;
 // skip the test    
#if 1    
     for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);
 
     for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_ON);
      
      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);
//while(1) {     
      for(duty=0;duty<=100;duty++, HAL_Delay(5)) {
        for(i=1;i<LED_COUNT;i++)
          BriskDimTimedLED(i,duty);
      }
      

      for(duty=0;duty<=100;duty++, HAL_Delay(5)) {
        for(i=1;i<LED_COUNT;i++)
          BriskDimTimedLED(i,100-duty);
      }
//};        
    return;
    
    if(gLED_TestCounter==0) {
    
      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);
     
    }else 
    if(gLED_TestCounter<3) {

      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_ON);
      
      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);

    }else 
    if(gLED_TestCounter<5) {

      // Dimmable option
      for(duty=0;duty<=100;duty++, HAL_Delay(5)) {
        for(i=1;i<LED_COUNT;i++)
          BriskDimTimedLED(i,duty);
      }
      

        for(duty=0;duty<=100;duty++, HAL_Delay(5)) {
          for(i=1;i<LED_COUNT;i++)
            BriskDimTimedLED(i,100-duty);
        }
          
    }/*else
    
    */
#endif    
}

