/**
  ******************************************************************************
  * @file    Joystick.c (up/down/left/right/select 5 way rockswitch or joystick)
  * @author  S.Ma
  * @brief   Not really used for STM32C0. For reuse with user button, with debounce, auto repeat, long press event construction from a simple time interval calling function.
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
#include "joystick.h"
#ifdef _B_HMI_LCD16x2_H_
//====================================================================================
// Jostick related functions
// below definitions may be overriden in custom.h: They are default values
// Program here the debouncing time in 16msec unit
// each count is 50 msec
#define KEY_DEBOUNCE_TIME	2	// in loops 
// Program here the time to repeat a key 
#define KEY_REPEAT_START_DELAY		15	// in loops 
// Program here the normal speed repetition
#define KEY_REPEAT_SLOW_INTERVAL	4
// Program here the corresponding slow time (from Key Pressed)
#define KEY_REPEAT_SLOW_DURATION	80
// Program here the fast speed repetition
#define KEY_REPEAT_FAST_INTERVAL	2
// Time Key is setup for 4 seconds (must not be the maximum FF-1 value)
#define HOT_KEY_TIME	40 

uint8_t JOY_Init(JOYSTICK_t* pJOY) {
  
  // configure all pins as input
  IO_PadInit(pJOY->DOWN);
  IO_PadInit(pJOY->UP);
  IO_PadInit(pJOY->LEFT);
  IO_PadInit(pJOY->RIGHT);
  IO_PadInit(pJOY->SEL);
  
  // reset time counters
  pJOY->Keys.PressedTime_50ms = 0;
  pJOY->Keys.RepeatCountdown_50ms = 0;
  return 1;
}



uint8_t JOY_ScanSwitches(JOYSTICK_t* pJOY) {
  
  uint8_t KeysNow = NO_KEY;
    
  if(IO_PadGet(pJOY->DOWN->PadName)==0)
    KeysNow |= KEY_DOWN;
  if(IO_PadGet(pJOY->UP->PadName)==0)
    KeysNow |= KEY_UP;
  if(IO_PadGet(pJOY->LEFT->PadName)==0)
    KeysNow |= KEY_LEFT;
  if(IO_PadGet(pJOY->RIGHT->PadName)==0)
    KeysNow |= KEY_RIGHT;
  if(IO_PadGet(pJOY->SEL->PadName)==0)
    KeysNow |= KEY_SELECT;
  
    return KeysNow;
}

uint8_t JOY_50ms(JOYSTICK_t* pJOY) {
  
  pJOY->Keys.bmLast = pJOY->Keys.bmNow;
  pJOY->Keys.bmNow = JOY_ScanSwitches(pJOY);
    
    // at least one switch is pressed
    if(pJOY->Keys.bmLast != pJOY->Keys.bmNow) 
    {	// it is the same key as before
      pJOY->Keys.Event	= NO_KEY_EVENT;// (KBD[Ch].wKeyTime<KEY_DEBOUNCE_TIME)?NO_KEY_EVENT:KEY_RELEASED;
      pJOY->Keys.Code 	= NO_KEY;//KBD[Ch].bKeyLast; 
      pJOY->Keys.PressedTime_50ms	= 0;	// restart from scratch 
      return 0;
    }

    // increment the time it has been pressed (up to the max)
    if(pJOY->Keys.PressedTime_50ms<0xFFFF)
      pJOY->Keys.PressedTime_50ms++;	

    // Check if it has reached the debouncing delay
    if(pJOY->Keys.PressedTime_50ms==KEY_DEBOUNCE_TIME) 
    {
      pJOY->Keys.Event	= KEY_PRESSED;
      pJOY->Keys.Code	= pJOY->Keys.bmNow;
      pJOY->Keys.RepeatCountdown_50ms = KEY_REPEAT_START_DELAY;
      return 1;
    }

    // check for hot key (time key)
    if(pJOY->Keys.PressedTime_50ms==HOT_KEY_TIME) 
    {
      pJOY->Keys.Event	= KEY_LONG_PRESS;
      pJOY->Keys.Code	= pJOY->Keys.bmLast;//=KeyCur
      return 1;
    }

    // Key repeat events using countdown (can't use _32msec: key repeat will stop after 4 seconds = 0xFF)
    if(pJOY->Keys.RepeatCountdown_50ms) 
    {
      pJOY->Keys.RepeatCountdown_50ms--;

      if(pJOY->Keys.RepeatCountdown_50ms==0) 
      {	// repeat event reached
        pJOY->Keys.Event	= KEY_REPEATED;
        pJOY->Keys.Code	= pJOY->Keys.bmLast;//=KeyCur
// prepare for next repeat
        if(pJOY->Keys.PressedTime_50ms<KEY_REPEAT_SLOW_DURATION) 	// if key pressed for less than 200x16ms = 2.6 sec			
          pJOY->Keys.RepeatCountdown_50ms = KEY_REPEAT_SLOW_INTERVAL;
        else
          pJOY->Keys.RepeatCountdown_50ms = KEY_REPEAT_FAST_INTERVAL;

        return 1;
      }
    }

    // no caught event for now if we land here
    pJOY->Keys.Event	= NO_KEY_EVENT;

  return 0;
}

uint8_t JOY_IsKeyMatch_50ms (JOYSTICK_t* pJOY, uint8_t bmKey, uint8_t Event) {
  
  if((pJOY->Keys.Code == bmKey)&&(pJOY->Keys.Event == Event)) 
			return(1);
  return (0);
}

//=== cosmetic

uint8_t JOY_LeftPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_LEFT,KEY_PRESSED);
}
uint8_t JOY_LeftRepeated(JOYSTICK_t* pJOY) {                                        
  return (JOY_IsKeyMatch_50ms(pJOY,KEY_LEFT,KEY_PRESSED)|| JOY_IsKeyMatch_50ms(pJOY,KEY_LEFT,KEY_REPEATED));
}
uint8_t JOY_LeftLongPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_LEFT,KEY_LONG_PRESS);
}

uint8_t JOY_RightPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_RIGHT,KEY_PRESSED);
}

uint8_t JOY_RightRepeated(JOYSTICK_t* pJOY) {
  return (JOY_IsKeyMatch_50ms(pJOY,KEY_RIGHT,KEY_PRESSED)|| JOY_IsKeyMatch_50ms(pJOY,KEY_RIGHT,KEY_REPEATED));
}

uint8_t JOY_RightLongPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_RIGHT,KEY_LONG_PRESS);
}

uint8_t JOY_UpPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_UP,KEY_PRESSED);
}
uint8_t JOY_UpRepeated(JOYSTICK_t* pJOY) {
  return (JOY_IsKeyMatch_50ms(pJOY,KEY_UP,KEY_PRESSED)|| JOY_IsKeyMatch_50ms(pJOY,KEY_UP,KEY_REPEATED));
}
uint8_t JOY_UpLongPressed(JOYSTICK_t* pJOY);
uint8_t JOY_UpLongPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_UP,KEY_LONG_PRESS);
}

uint8_t JOY_DownPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_DOWN,KEY_PRESSED);
}
uint8_t JOY_DownRepeated(JOYSTICK_t* pJOY) {
  return (JOY_IsKeyMatch_50ms(pJOY,KEY_DOWN,KEY_PRESSED)|| JOY_IsKeyMatch_50ms(pJOY,KEY_DOWN,KEY_REPEATED));
}
uint8_t JOY_DownLongPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_DOWN,KEY_LONG_PRESS);
}

uint8_t JOY_SelectPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_SELECT,KEY_PRESSED);
}
uint8_t JOY_SelectRepeated(JOYSTICK_t* pJOY) {
  return (JOY_IsKeyMatch_50ms(pJOY,KEY_SELECT,KEY_PRESSED)|| JOY_IsKeyMatch_50ms(pJOY,KEY_SELECT,KEY_REPEATED));
}
uint8_t JOY_SelectLongPressed(JOYSTICK_t* pJOY) {
  return JOY_IsKeyMatch_50ms(pJOY,KEY_SELECT,KEY_LONG_PRESS);
}



#endif
