/**
  ******************************************************************************
  * @file    joystick.h ()
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

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "io_pins.h"

// Keyboard scan, Buttons
typedef struct {
uint8_t bmLast;
uint8_t bmNow;
uint16_t PressedTime_50ms;// u8 = 4 sec max, u16 no limitation
uint8_t RepeatCountdown_50ms;
uint8_t Event;
uint8_t Code;
}TimedSwitch_t ;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Up/down/left/right 4 directions style keyboard
// Key ID for high level comparison
typedef enum {
NO_KEY =    (0x00),
KEY_RIGHT = (0x01),
KEY_LEFT =  (0x02),
KEY_DOWN =  (0x04),
KEY_UP =    (0x08),
KEY_SELECT =   (0x10),
}JOY_BITMASK_t;

typedef struct {

  IO_Pad_t* UP; // 
  IO_Pad_t* DOWN; // 
  IO_Pad_t* LEFT; // 
  IO_Pad_t* RIGHT; // 
  IO_Pad_t* SEL; //
  
  TimedSwitch_t Keys;
  
} JOYSTICK_t;

// Thanks to debounce, the key release will occur BEFORE next key pressed, no need to complexify the code
typedef enum {NO_KEY_EVENT = 0,KEY_PRESSED,KEY_REPEATED,KEY_RELEASED,KEY_LONG_PRESS,
// These are non-key events used by OSD GUI tool
//TIME_EVENT, IR_KEY_PRESSED, IR_KEY_REPEATED
}key_events_t;

uint8_t JOY_IsKeyMatch_50ms (JOYSTICK_t* pJOY, uint8_t Key, uint8_t Event);


uint8_t JOY_Init(JOYSTICK_t* pJOY);
uint8_t JOY_50ms(JOYSTICK_t* pJOY); // main loop scan. Key processing shall be done using the same 50msec event.

// cosmetic functions
uint8_t JOY_LeftPressed(JOYSTICK_t* pJOY);
uint8_t JOY_LeftRepeated(JOYSTICK_t* pJOY);
uint8_t JOY_LeftLongPressed(JOYSTICK_t* pJOY);

uint8_t JOY_RightPressed(JOYSTICK_t* pJOY);
uint8_t JOY_RightRepeated(JOYSTICK_t* pJOY);
uint8_t JOY_RightLongPressed(JOYSTICK_t* pJOY);

uint8_t JOY_UpPressed(JOYSTICK_t* pJOY);
uint8_t JOY_UpRepeated(JOYSTICK_t* pJOY);
uint8_t JOY_HotLongPressed(JOYSTICK_t* pJOY);

uint8_t JOY_DownPressed(JOYSTICK_t* pJOY);
uint8_t JOY_DownRepeated(JOYSTICK_t* pJOY);
uint8_t JOY_DownLongPressed(JOYSTICK_t* pJOY);

uint8_t JOY_SelectPressed(JOYSTICK_t* pJOY);
uint8_t JOY_SelectRepeated(JOYSTICK_t* pJOY);
uint8_t JOY_SelectLongPressed(JOYSTICK_t* pJOY);

uint8_t JOY_ScanSwitches(JOYSTICK_t* pJOY);
#endif
