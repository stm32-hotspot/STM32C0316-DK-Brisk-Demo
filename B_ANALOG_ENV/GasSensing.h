#ifndef _GAS_SENSING_H_
#define _GAS_SENSING_H_

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

typedef struct {
  
  const char* sReference;
  const char* sPartNumber;
  const char* GasType;
  const char  NbOfTerminals;
  const char  DefaultGainSwitch;
  const int32_t  DefaultVref2_mV;
  
  // conversion from mV to PPM...
  int32_t mV2ppm;
  
} GasSensorSettings;


extern const GasSensorSettings SupportedGasSensors[];

typedef struct {
  
  int32_t GasSensorIndex;
  char GainSwitch;
//  int32_t VrefSwitch;
  
  int32_t DAC_ADC_Readback_mV;
  
  int32_t Vref2_mV; // controlled by MCU DAC
  int32_t Vref2_ADC_mV; // readback by ADC
//  int32_t Vref1_DAC_mV;
//  int32_t Vref1_ADC_mV;
  
  int32_t Gas_mV; // reading Gas sensing
  
  int32_t toto;
  
} GasSensing_t;

extern GasSensing_t GasSensing;

int32_t GasSensingInit(void);
int32_t GasSensingSetVRef2_Lsb(int32_t Vref2_Lsb);
int32_t GasSensingSetVRef2_mV(int32_t Vref2_mV);
int32_t GasSensingPolling(void);
#endif
