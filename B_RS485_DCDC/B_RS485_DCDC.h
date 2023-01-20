
#ifndef _B_RS485_DCDC_BOARD_
#define _B_RS485_DCDC_BOARD_
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
int32_t RS_Init(void);
int32_t RS_DeInit(void);
void RS_Test(void);
int32_t RS_Polling(void);
int32_t RS_10ms(void);
int32_t RS_50ms(void);
int32_t RS_100ms(void);
int32_t RS_1sec(void);
int32_t RS_IsPlugged(void);
int32_t RS_IsUnplugged(void);

typedef struct {
  // initialized in the same order
  int32_t BoardVersion;

  int32_t       dummy_Sense1; // Sense 1 : ESC mode 50Hz (20msec) PWM, pulse width 1msec = 0, 2msec = 100%
  int32_t       dummy_Sense2; // Sense 2 : ESC mode
  
  u8fifo_t* pReport; // Bluetooth electronics fifo
  
} RS_AddOn_t;

extern RS_AddOn_t RS_AddOn;

//extern ByteVein_t* pRS_Report;
int32_t RS_Report(void);

int32_t RS_SendDashboard(void);
int32_t RS_PaneProcess(void);

int32_t RS_SetPanel(void);
#endif
