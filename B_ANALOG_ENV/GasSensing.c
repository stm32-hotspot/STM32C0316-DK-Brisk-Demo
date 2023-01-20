/**
  ******************************************************************************
  * @file    GasSensing.c (Analog Gas Sensing)
  * @author  S.Ma
  * @brief   Works with DAC which is missing. Empty code shell. Could work playing with pull-up/down GPIO control. Untested.
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
#include "GasSensing.h"

extern Analog_t Analog; // transversal dependency
GasSensing_t GasSensing;

int32_t GasSensingInit(void) {
  
  return 0;
}

int32_t GasSensingSetVRef2_mV(int32_t Vref2_mV) {
  
//-  Analog.DACs.Use_mV[1] = Vref2_mV;
  return 0;
}

int32_t GasSengingSetGain(int32_t gain);
int32_t GasSengingSetGain(int32_t gain) {

  return 0;
}

int32_t GasSensingPolling(void) {

    //ADC_UpdateConveredValues(&Analog.ADC);
    GasSensing.Gas_mV = Analog.ADCs.Normal_mV[9]; // measured gas sensor output, gain dependent // wonder if STM32L4 gain control works here
    GasSensing.DAC_ADC_Readback_mV = Analog.ADCs.Normal_mV[10];
  
  return 0;
}
/*
const GasSensorSettings SupportedGasSensors[] = {
  { "unknown sensor",       "unknown",    "N/A", 2, 0, 1600 },
  { "CiTiceL 4CF+",         "2112B2005",  "CO", 2, 0, 1600 },
  { "CiTiceL 2CF-3",        "AB231-801",  "CO", 2, 0, 1600 },
  { "CiTiceL EcoSure (2E)", "2112B3000A", "CO", 2, 0, 1600 },
  { "CiTiceL 4LXH",         "2112B3008",  "CO", 2, 0, 1600 },
  { "CiTiceL 4HS-LM",       "AC226-800",  "CO", 2, 0, 1600 },
  { "CiTiceL 4HS+ H2S(M)",  "2112B2025",  "CO", 2, 0, 1600 },
  { "CiTiceL O3 3E 1",      "1531-031-30049", "O3", 2, 0, 1600 },
  { "CiTiceL 4H",           "AC200-800", "CO", 2, 0, 1600 },
  { "AlphaSense CO-AF",     "11813636", "CO", 2, 0, 1600 },
  { "AlphaSense H2S-AH",    "24810245", "H2S", 2, 0, 1600 },
  { "AlphaSense O2-A3",     "97330403", "O2", 2, 0, 1600 },
  { "", "", "", -1, -1, -1 }
};
*/
