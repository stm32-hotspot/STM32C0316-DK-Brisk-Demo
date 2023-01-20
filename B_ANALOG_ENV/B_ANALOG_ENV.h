#ifndef _B_ANALOG_ENV_BOARD_
#define _B_ANALOG_ENV_BOARD_

/**
  ******************************************************************************
  * @file    B_ANALOG_ENV.h (STMod+ Addon board which includes environmental sensors and analog specifics transceivers, analog gas sensing being an option with DAC preffered drive)
  * @author  S.Ma
  * @brief   The analog gas sensor usually requires a DAC not present in STM32C0. Sensors drivers using float math are disabled due to the small memory footprint of STM32C0.
  *          This layer is below the Add_on_board.c
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

//#include "hts221.h"
//#include "lps22hh.h"
//#include "sgp30top.h"
//#include "bme280.h"

int32_t AE_Init(void);
int32_t AE_DeInit(void);
int32_t AE_Test(void);
int32_t AE_Polling(void);
int32_t AE_10ms(void);
int32_t AE_50ms(void);
int32_t AE_100ms(void);
int32_t AE_1sec(void);
int32_t AE_IsPlugged(int32_t ADS_FootprintVersion); // 0: with ADS, 1: without ADS 
int32_t AE_IsUnplugged(void);
/*
typedef struct { // audio board context and remote commands
} AnalogEnv_t;
*/
//extern ByteVein_t* pAE_Report;
int32_t AE_Report(void);
int32_t AE_PaneProcess(void);
int32_t AE_SetPanel(void);

typedef struct {
  
  int32_t BoardVersion; // 1=version with ADS, 2=version without ADS, 3=Board_V2 with new gas sensor
  int32_t ADS_Available;
  int32_t LPS_Available;
  int32_t HTS_Available;
  int32_t BME_Available;
  int32_t SGP_Available;
  int32_t STTS_Available;
  // we should aggregate the sensors structure here to be clean and portable. TODO

int32_t AE_SubSample;

  int32_t* pbme280_pressure;
  int32_t* pbme280_temperature;
  int32_t* pbme280_humidity;// export

  int32_t SubSample;
  
  int32_t Altitute1_cm;
  int32_t Delta_ZeroRef1_cm;
  int32_t Delta_Altitude1_cm;
  int32_t Altitute2_cm;
  int32_t Delta_ZeroRef2_cm;
  int32_t Delta_Altitude2_cm;
  int32_t DewPoint_degC_x10;

  int32_t adc_mV;  
  
  int32_t* pLPS_hPa_x100;
  int32_t* pLPS_DegC_x10;
  
  int32_t* pHTS_RH_x10;
  int32_t* pHTS_DegC_x10;  
  
  int32_t* pSTTS_DegC_x10;
  
  uint16_t* ptvoc_ppb;
  uint16_t* pco2_eq_ppm;
  
  u8fifo_t* pReport;
  
} AE_AddOn_t;

extern AE_AddOn_t AE_AddOn;

#endif
