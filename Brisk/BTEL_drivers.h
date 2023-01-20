/**
  ******************************************************************************
  * @file    btel_drivers.h
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
#ifndef _BTEL_DRIVERS_H_
#define _BTEL_DRIVERS_H_
// These are dedicated functions for Bluetooth Electronics Dashboard APK Android App
// http://www.keuwl.com/apps/bluetoothelectronics/ for more information

typedef struct { // all this works in the main loop, either by polling or by minimal time interval
  
  uint8_t       Enabled;
  uint8_t       DeviceSelector;
  uint8_t       DeviceSelectorMax;
  uint8_t       PanelSelector; // 0 = STM32 brisk (always exist), 1+ = add-on extra pannels (if present and dynamic)
  uint8_t       PanelSelectorMax;
  uint8_t       Delayed_PanelSelector;
  uint16_t      Delayed_PanelCountdown_100ms;
  uint8_t       SubSampleReporting_100ms;
  uint32_t      TimeSlice; // used as sequencer to output all parameters sequentially with refresh. One step = 50 msec = 48 bytes at 9600bps, 576 bytes at 115200 bps
  char          incoming[512];
  int16_t       in_ndx;
  int16_t       in_size;
  uint8_t       in_start; // first '*' received
  uint8_t       in_stop; // second '*' received (message to process)
//  char          outgoing[512];
//  int16_t       out_ndx;
//  int16_t       out_size;
} BTEL_t;

extern BTEL_t BTEL;
extern BTEL_t* pBTEL;


// manages add-on board on master and slaves
// master should have a MC or Shifter or FanoutV2 board with USART enabled

void BTEL_Init(void);
void BTEL_Deinit(void);
void BTEL_Polling(void);
void BTEL_50msec(void);
void BTEL_100msec(void);
void BTEL_1sec(void);
void BTEL_IsPlugged(void);
void BTEL_IsUnplugged(void);

void BTEL_Enable(void);
void BTEL_Disable(void);

void BTEL_Printf(u8fifo_t* pL, const char *str,...); // send formatted text to the serial FIFO (application one by default)
int32_t StringToFrac(char s[], int32_t* pVal);

int32_t TransmitPanel(u8fifo_t* pL, char* pS);

void BTEL_SelectDevice(void);

#endif
/* these are examples of formatting

Text: *THello, world!* Text string
Button:
Switch:
http://www.keuwl.com/apps/bluetoothelectronics/userguide/ct.html
*/
