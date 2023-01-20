#ifndef _B_OPTO_INERT_BOARD_
#define _B_OPTO_INERT_BOARD_

/**
  ******************************************************************************
  * @file    b_opto_inert.h
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


// driver for the UV sensor
//#define ISMDLC_ENABLED 1
#include "ism330dlc_reg.h"
#include "ism330dlc.h"
#include "LV0104CS.h"
#include "VEML6070.h"
#include "vl53l1x.h"
//#define BMX055_ENABLED 1 // cost nearly 16kbyte code
#ifdef BMX055_ENABLED
#include "bmx055.h" // can be commented out to reduce code size
#endif
extern VEML6070_t VEML6070;
extern VEML6070_t* pVEML6070;

int32_t OI_Init(void);
int32_t OI_DeInit(void);
int32_t OI_Test(void);
int32_t OI_Polling(void);
int32_t OI_10ms(void);
int32_t OI_50ms(void);
int32_t OI_100ms(void);
int32_t OI_1sec(void);
int32_t OI_IsPlugged(void);
int32_t OI_IsUnplugged(void);

//extern ByteVein_t* pOI_Report;
int32_t OI_Report(void);
int32_t OI_PaneProcess(void);
int32_t OI_SetPanel(void);

typedef struct  {
	int32_t datax;/**<mag compensated X  data*/
	int32_t datay;/**<mag compensated Y  data*/
	int32_t dataz;/**<mag compensated Z  data*/
	uint16_t resistance;/**<mag R  data*/
	uint8_t data_ready;/**<mag data ready status*/
} bmm050_mag_s32_data_t;

typedef struct  {
	int16_t datax;/**<gyro X  data*/
	int16_t datay;/**<gyro Y  data*/
	int16_t dataz;/**<gyro Z  data*/
	char intstatus[5];/**<gyro interrupt status*/
} bmg160_data_t;


typedef struct {
  
  int32_t BoardVersion;
  
  VL53L1_Dev_t* pVL53L1; // Long range time of flight
  int32_t TOF_Ref_mm;
  int32_t TOF_Delta_mm;
  
  VEML6070_t* pVEML6070; // UV sensor
  
  LV0104CS_t* pLV0104CS; // Ambiant light sensor
#ifdef BMX055_ENABLED  
  bmm050_mag_s32_data_t* pBMX_MAG_data_s32; // Bosch Magneto Readings
  bmg160_data_t* pdata_gyro;
#endif  
  int16_t* pv_gyro_datax_s16;
  int16_t* pv_gyro_datay_s16;
  int16_t* pv_gyro_dataz_s16;
  
  uint32_t PIR_Raw; // Analog PIR
  
  int32_t SubSample;
  
  u8fifo_t* pReport;
  
} OI_AddOn_t; // contains pointers to all elements and data related to this add-on board

extern OI_AddOn_t OI_AddOn;

#endif
