/**
  ******************************************************************************
  * @file    B_ANALOG_ENV.c (STMod+ Addon board which includes environmental sensors and analog specifics transceivers, analog gas sensing being an option with DAC preffered drive)
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

#include "main.h"
#include "brisk.h"

#include "sgp30top.h"

#include "bme280.h"
//#define BME280_ENABLE
#include "lps22hh.h"
#include "hts221.h"
#include "stts751.h"
#include "gassensing.h"

#ifdef _B_ANALOG_ENV_BOARD_

AE_AddOn_t AE_AddOn = {
  .BoardVersion = -1, // no board version detected
  .pbme280_pressure = &bme280_pressure,
  .pbme280_temperature = &bme280_temperature,
  .pbme280_humidity = &bme280_humidity,
  .pLPS_hPa_x100 = &LPS_hPa_x100,
  .pLPS_DegC_x10 = &LPS_DegC_x10,
  .pHTS_RH_x10 = &HTS_RH_x10,
  .pHTS_DegC_x10 = &HTS_DegC_x10,
  .pSTTS_DegC_x10 = &STModSTTS751.DegC_x10, // Version 2 only
  .ptvoc_ppb = &tvoc_ppb,
  .pco2_eq_ppm = &co2_eq_ppm,
  .pReport = &u8fifo_from_SPIP[BT_STL_SERIAL],

};
    
void From_Pressure_mb_To_Altitude_US_Std_Atmosphere_1976_m(float Pressure_mb, float* Altitude_m);
void From_Pressure_mb_To_Altitude_US_Std_Atmosphere_1976_m(float Pressure_mb, float* Altitude_m) {
	
	//=(1-(A18/1013.25)^0.190284)*145366.45
	*Altitude_m = (1-pow(Pressure_mb/1013.25,0.190284))*145366.45/3.280839895;
}

void DewPointApproximation(float T, float RH, float* Td);
void DewPointApproximation(float T, float RH, float* Td) {
  
  *Td = T - ((100-RH)/5);
}

// Pin configuration through STMod+, assume by default all GPIOs are input floating
#if 0
// STMod+ Pin configuration through STMod+, assume by default all GPIOs are input floating
IO_Pin_t AE_ADS_CS_STMod =      /* 1 */{  GPIOA, {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,      0,   } };
#if 1 // SPI by GPIO bit bang
IO_Pin_t AE_ADS_MOSI_STMod =    /* 2 */{  GPIOA, {    GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };
IO_Pin_t AE_ADS_MISO_STMod =    /* 3 */{  GPIOA, {    GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         0,   } };
IO_Pin_t AE_ADS_SCK_STMod =     /* 4 */{  GPIOA, {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };
#else // use real SPI HW
NOT SUPPORTED IO_Pin_t AE_ADS_MOSI_STMod =    /* 2 */{  GPIOF, {    GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          5,   } };
NOT SUPPORTED IO_Pin_t AE_ADS_MISO_STMod =    /* 3 */{  GPIOF, {    GPIO_PIN_14, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         5,   } };
NOT SUPPORTED IO_Pin_t AE_ADS_SCK_STMod =     /* 4 */{  GPIOF, {    GPIO_PIN_13, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          5,   } };
#endif

IO_Pin_t AE_ADS_CLKSEL_STMod =  /* 8 */{  GPIOA, {    GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };// GPIO_MODE_IT_FALLING
IO_Pin_t AE_ANA_SWITCH_STMod =  /* 9 */{  GPIOA, {    GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };// select the gain of the analog gas sensor

IO_Pin_t AE_ADR_DRDY_STMod =    /* 11 */{  GPIOC, {    GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };// GPIO_MODE_IT_FALLING
IO_Pin_t AE_ADS_RESET_STMod =   /* 12 */{  GPIOC, {    GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };
IO_Pin_t AE_SENS_VAL_VREF_STMod = /* 13 */{  GPIOA, {    GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // analog gas sensor input
IO_Pin_t AE_ADS_CLK2_STMod =      /* 14 */{  GPIOA, {    GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };

IO_Pin_t AE_ADS_START_STMod =     /* 17 */{  GPIOB, {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };// GPIO_MODE_IT_FALLING
IO_Pin_t AE_ANA_VREF_SEL_STMod =  /* 18 */{  GPIOA, {    GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // select the analog gas channel input
IO_Pin_t AE_ADS_CS2_STMod =       /* 19 */{  GPIOD, {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };// GPIO_MODE_IT_FALLING
// STMod+ pin 20 as no connect.  

// MB1538B VERSION 2 
// LED pin won't work because LED are stuffed in the wrong side...

IO_Pin_t AE_VREF1 =               /* 7 */{  GPIOA, {    GPIO_PIN_7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // analog gas sensor input (floating by default)
IO_Pin_t AE_VREF2 =               /* 8 */{  GPIOA, {    GPIO_PIN_12, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // analog gas sensor input (floating by default)

IO_Pin_t AE_STTS751_EVENT =       /* 11 */{  GPIOC, {    GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         0,   } };
IO_Pin_t AE_STTS751_ADD_TH =      /* 12 */{  GPIOC, {    GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         0,   } };
IO_Pin_t AE_GAS_ADC =             /* 13 */{  GPIOA, {    GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // analog gas sensor input
IO_Pin_t AE_GAS_DAC =             /* 14 */{  GPIOA, {    GPIO_PIN_5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // analog gas sensor input

IO_Pin_t AE_GAS_GAIN_SEL =          /* 17 */{  GPIOB, {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };// GPIO_MODE_IT_FALLING
IO_Pin_t AE_GAS_VREF_SEL =             /* 18 */{  GPIOA, {    GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } }; // select the analog gas channel input
IO_Pin_t AE_LPS221_INT =          /* 19 */{  GPIOD, {    GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         0,   } }; // warning, this pin is used also to select I2C or I3C at power on...
/* 20 */ // unused pins in this addon board PA0
#endif


//=============== 8>< --------------------

// MB1538B VERSION 2 
// LED pin won't work because LED are stuffed in the wrong side...

IO_Pad_t AE_VREF1 =               /* 7 */{  PA_7, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 }; // analog gas sensor input (floating by default)
IO_Pad_t AE_VREF2 =               /* 8 */{  PA_12, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 }; // analog gas sensor input (floating by default)

IO_Pad_t AE_STTS751_EVENT =      /* 11 */{  PC_14, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },         0 };
IO_Pad_t AE_STTS751_ADD_TH =     /* 12 */{  PC_15, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },                            0 };
IO_Pad_t AE_GAS_ADC =            /* 13 */{  PA_4, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },                             0 }; // analog gas sensor input
IO_Pad_t AE_GAS_DAC =            /* 14 */{  PA_5, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },                             0 }; // analog gas sensor input

IO_Pad_t AE_GAS_GAIN_SEL =       /* 17 */{  PB_0, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };// GPIO_MODE_IT_FALLING
IO_Pad_t AE_GAS_VREF_SEL =       /* 18 */{  PA_6, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 }; // select the analog gas channel input
IO_Pad_t AE_LPS221_INT =         /* 19 */{  PD_0, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },         0 }; // warning, this pin is used also to select I2C or I3C at power on...
/* 20 */ // unused pins in this addon board PA0


//=====

// LPS22HH_example_main

int32_t AE_Init(void) {
  // we use the 4 byte sub address buffer as data, as this slave has few registers
 AE_AddOn.BoardVersion = 1;
 return 0;
}

int32_t AE_IsPlugged(int32_t ADS_FootprintVersion) {

  AE_AddOn.BoardVersion = ADS_FootprintVersion;
  AE_AddOn.STTS_Available = STTS751_IsPlugged(); // used to detect V2 board version only 
#ifdef BME280_ENABLE  
  AE_AddOn.BME_Available = BME280_IsPlugged();
#endif  
  AE_AddOn.HTS_Available = HTS221_IsPlugged();
  AE_AddOn.LPS_Available = LPS22HH_IsPlugged();
  if(AE_AddOn.LPS_Available==0)
      NOPs(1);//while(1) ; // cut LPS_INT pin (STMod+ connector pin 19) to boot in I2C mode vs I3C.
  AE_AddOn.SGP_Available = SGP30_IsPlugged();
  
  switch(AE_AddOn.BoardVersion) {
  case 0: // long board with ADE populated
#if 0
    AE_AddOn.ADS_Available = 1;

    IO_PadInit(&AE_ADS_CS_STMod);// pin1
    IO_PadInit(&AE_ADS_MOSI_STMod);
    IO_PadInit(&AE_ADS_MISO_STMod);
    IO_PadInit(&AE_ADS_SCK_STMod);

    IO_PadInit(&AE_ADS_CLKSEL_STMod);
    IO_PadInit(&AE_ADR_DRDY_STMod);// pin 10
    IO_PadInit(&AE_ADS_RESET_STMod);
    IO_PadSetHigh(AE_ADS_CLK2_STMod.PadName);
    IO_PadInit(&AE_ADS_START_STMod);
    IO_PadInit(&AE_ADS_CS2_STMod);
    // continue with common pins when ADS is missing break;
  case 1:
    IO_PadInit(&AE_ANA_SWITCH_STMod);
    IO_PadSetHigh(AE_ANA_SWITCH_STMod.PadName); // gain R13 mini by default

    IO_PadInit(&AE_SENS_VAL_VREF_STMod);
 
    IO_PadInit(&AE_ANA_VREF_SEL_STMod);
    IO_PadSetHigh(AE_ANA_VREF_SEL_STMod.PadName); // select gas analog signal to be measured
    break;
#endif
  case 2: //this is the V2 board with STTS Temp Sensor version and new gas sensing method with 4 LEDs
    
    IO_PadInit(&AE_VREF1); // not used by default
    IO_PadInit(&AE_VREF2); // not used by default
    
    IO_PadInit(&AE_GAS_GAIN_SEL);
    IO_PadSetHigh(AE_GAS_GAIN_SEL.PadName); // gain R13 mini by default

    IO_PadInit(&AE_GAS_ADC);
    IO_PadInit(&AE_GAS_DAC);
 
    IO_PadInit(&AE_GAS_VREF_SEL);
    IO_PadSetHigh(AE_GAS_VREF_SEL.PadName); // select gas analog signal to be measured

    // Gas sensing is analog input, so make sure DAC outputs are disabled
    // PIR is PA4 = Analog.ADC.Normal_Lsb[9]
//-    HAL_DAC_Stop(&hdac1,DAC_CHANNEL_1);
    // we use DAC as Vref generator when ADS1299 is not populated. HAL_DAC_Stop(&hdac1,DAC_CHANNEL_2);
//-    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 4096/2); // default at mid range
    
    break;
  default:
    while(1); // something is wrong here.
  };
 

  BTEL.PanelSelector = 0;
  BTEL.PanelSelectorMax = BRISK_PANELS + 3; // 4 panels
  
  if(AE_AddOn.BoardVersion == 2)
    GasSensingInit();
  // hack, let's try ADS1299 here
//-  ADS_TestSPI();
  
  return 0;
}

int32_t AE_IsUnplugged(void) { 
  NOPs(1); 
  AE_AddOn.BoardVersion = -1; // no board detected
  AE_AddOn.BME_Available = 0;
  AE_AddOn.ADS_Available = 0;
  AE_AddOn.HTS_Available = 0;
  AE_AddOn.LPS_Available = 0;
  AE_AddOn.SGP_Available = 0;
  return 0;
};

int32_t AE_50ms(void) {
   
  int32_t result = 0;
  if(AE_AddOn.HTS_Available)
    HTS221_Polling();
  if(AE_AddOn.LPS_Available)
    LPS22HH_Polling();
#ifdef BME280_ENABLE  
  if(AE_AddOn.BME_Available)
    BME280_Polling(); // 40+ msec
#endif  
  if(AE_AddOn.SGP_Available)
    SGP30_IsPolling();
  if(AE_AddOn.BoardVersion == 2)
    GasSensingPolling();
  return result;
}

int32_t AE_1sec(void) {
  
  int32_t result = 0;

  if(AE_AddOn.STTS_Available) // temp sensor updates every second...
    STTS751_Polling(); // for V2 only

  SGP30_1sec(); // this will kick a precise 1 sec measurement process which will take ~650msec and should be completed before next second...

  // bluetooth electronics
 
  return result;
}

//ByteVein_t* AE_AddOn.pReport = &BV_to_SPIP[STM_SERIAL];
//-int32_t AE_SubSample;

static float AE_float1, AE_float2, AE_float3;


int32_t AE_Report(void) {

  AE_AddOn.SubSample = (AE_AddOn.SubSample+1)%3;// every 0.5 msec (2Hz)
  switch(BTEL.PanelSelector) {
  case (0+BRISK_PANELS):
    BTEL_Printf(AE_AddOn.pReport,"*G%C,%C,", *AE_AddOn.pLPS_hPa_x100, (*AE_AddOn.pbme280_pressure/100) ); // 
    BTEL_Printf(AE_AddOn.pReport,"*H%D,%D", *AE_AddOn.pHTS_RH_x10, (*AE_AddOn.pbme280_humidity/100) ); // 
    BTEL_Printf(AE_AddOn.pReport,"*I%d", *AE_AddOn.pco2_eq_ppm ); // 
    BTEL_Printf(AE_AddOn.pReport,"*J%D,%D*", *AE_AddOn.pHTS_DegC_x10, (*AE_AddOn.pbme280_temperature/10) ); // 
        
    if(AE_AddOn.SubSample) return 0;
     //LPS_DegC_x10
    //(int32_t)(comp_data.pressure *100) , (int32_t)(comp_data.temperature *10), (int32_t)(comp_data.humidity *10)
    BTEL_Printf(AE_AddOn.pReport,"*g%C hPa", *AE_AddOn.pLPS_hPa_x100); // 
    BTEL_Printf(AE_AddOn.pReport,"*i%D RH", *AE_AddOn.pHTS_RH_x10 ); // 
    BTEL_Printf(AE_AddOn.pReport,"*h%d ppm", *AE_AddOn.pco2_eq_ppm ); // 
    BTEL_Printf(AE_AddOn.pReport,"*j%D C*", *AE_AddOn.pHTS_DegC_x10 ); // 
  break;

  case (1+BRISK_PANELS) :  // altitude and delta
#if 0 // no float as too code size costly    
    From_Pressure_mb_To_Altitude_US_Std_Atmosphere_1976_m((float)*AE_AddOn.pLPS_hPa_x100/100, &AE_float1);
    From_Pressure_mb_To_Altitude_US_Std_Atmosphere_1976_m((float)*AE_AddOn.pbme280_pressure/10000, &AE_float2);
    AE_AddOn.Altitute1_cm = (int32_t)(AE_float1 * 100); // m to cm
    AE_AddOn.Delta_Altitude1_cm = (int32_t)(AE_AddOn.Delta_ZeroRef1_cm - AE_AddOn.Altitute1_cm);
    AE_AddOn.Altitute2_cm = (int32_t) (AE_float2 * 100); // m to cm
    AE_AddOn.Delta_Altitude2_cm = AE_AddOn.Delta_ZeroRef2_cm - AE_AddOn.Altitute2_cm;
    BTEL_Printf(AE_AddOn.pReport,"*G%C,%C*", AE_AddOn.Altitute1_cm, AE_AddOn.Altitute2_cm );
    
    DewPointApproximation((float)bme280_temperature/100, (float)bme280_humidity/1000 , &AE_float3);
    AE_AddOn.DewPoint_degC_x10 = (int32_t)(AE_float3 * 10);
    BTEL_Printf(AE_AddOn.pReport,"*T%d", AE_AddOn.DewPoint_degC_x10/10);
#endif    
    if(AE_AddOn.SubSample) return 0;
#if 0    
    BTEL_Printf(AE_AddOn.pReport,"*aAlt1: %C m*", AE_AddOn.Altitute1_cm);
    BTEL_Printf(AE_AddOn.pReport,"*bAlt2: %C m*", AE_AddOn.Altitute2_cm);
    BTEL_Printf(AE_AddOn.pReport,"*rRV1: %C m*", AE_AddOn.Delta_Altitude1_cm);
    BTEL_Printf(AE_AddOn.pReport,"*sRV2: %C m*", AE_AddOn.Delta_Altitude2_cm);
    BTEL_Printf(AE_AddOn.pReport,"*t%D*", AE_AddOn.DewPoint_degC_x10);
    BTEL_Printf(AE_AddOn.pReport,"**");
#endif    
    break;
    
  case (2+BRISK_PANELS):  
    ADC_UpdateConveredValues(&Analog.ADCs);
    AE_AddOn.adc_mV = Analog.ADCs.Normal_mV[9];
    BTEL_Printf(AE_AddOn.pReport,"*G%d*", AE_AddOn.adc_mV );
    
    if(AE_AddOn.SubSample) return 0;
    
    NOPs(1);
  break;
  
  case (3+BRISK_PANELS):
    NOPs(1); 
  break;
  default: TrapError();
  };
  
  return 0;
}


//============= here we add the specifics for the analog gas sensor
// This is only for the V2 board for gas sensor setting
int32_t AE_VREF_SliderValue = 10;

int32_t AE_PaneProcess(void) {
  
  char c = BTEL.incoming[0];
  
  // called when this is the add-on board of this STM32
  // for now, we assume it is master brisk.
  if(pBTEL->PanelSelector==(0+BRISK_PANELS)) { // shows most environmental values
    // no interaction
  };

  if(pBTEL->PanelSelector==(1+BRISK_PANELS)) { // converted values, heat index, dew point, altitude with autozero
    // no interaction
    if(c=='o') {
      AE_AddOn.Delta_ZeroRef1_cm = AE_AddOn.Altitute1_cm;
      AE_AddOn.Delta_ZeroRef2_cm = AE_AddOn.Altitute2_cm;
    };
  };
  
  if(pBTEL->PanelSelector==(2+BRISK_PANELS)) { // Analog Gas sensor source mux and gain mux

    switch(c) {
    case 'X': // Slider Vref when available (otherwise, does not have any effect
      if(StringToFrac(&BTEL.incoming[1], &AE_VREF_SliderValue)<0) return 0;
      if((AE_VREF_SliderValue<0)||(AE_VREF_SliderValue>100)) return 0; // out of range
//-      HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, AE_VREF_SliderValue * 36 + 248); // 100*40 = 2000 / 4095
      break;
      
    case 'b': // Ch sel On
      if(AE_AddOn.BoardVersion==2)
        IO_PadSetHigh(AE_GAS_VREF_SEL.PadName);
#if 0
      else
        IO_PadSetHigh(AE_ANA_VREF_SEL_STMod.PadName);
#endif
      break;
    case 'B': // Ch sel Off
      if(AE_AddOn.BoardVersion==2)
        IO_PadSetLow(AE_GAS_VREF_SEL.PadName);
#if 0
      else
        IO_PadSetLow(AE_ANA_VREF_SEL_STMod.PadName);
#endif
      break;
      
    case 'c': // gain OFF
      if(AE_AddOn.BoardVersion==2)
        IO_PadSetHigh(AE_GAS_GAIN_SEL.PadName);
#if 0
        else
      IO_PadSetHigh(AE_ANA_SWITCH_STMod.PadName);
#endif
      break;
    case 'C': // gain ON
      if(AE_AddOn.BoardVersion==2)
        IO_PadSetLow(AE_GAS_GAIN_SEL.PadName);
#if 0
        else
      IO_PadSetLow(AE_ANA_SWITCH_STMod.PadName);
#endif
      break;
      
    // DAC control
    };
  };

  if(pBTEL->PanelSelector==(3+BRISK_PANELS)) { // this is the GPIO panel pin 11..20 + DAC control
    // no interaction
  };
  
  return 0;
}


// environmental sensors panel
const char* BTEL_AE_Panel1 = 
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text(3,3,medium,L,,245,240,245,g)\n\
add_text(6,3,medium,L,,245,240,245,h)\n\
add_text(8,3,medium,L,,245,240,245,i)\n\
add_text(11,3,medium,L,,245,240,245,j)\n\
add_roll_graph(3,0,5,950.0,1200.0,100,G,,,mb,1,0,1,0,0,1,medium,none,2,st,42,97,222,bmc,237,115,7)\n\
add_roll_graph(8,0,5,0.0,100.0,100,H,,,RH %,1,0,1,0,0,1,medium,none,2,st,42,97,222,bmc,237,115,7)\n\
add_roll_graph(3,4,5,200.0,800.0,100,I,,,CO2 ppm,1,0,1,0,0,1,medium,none,1,1,42,97,222)\n\
add_roll_graph(8,4,5,0.0,100.0,100,J,,,�C,1,0,1,0,0,1,medium,none,2,st,42,97,222,bosch,237,115,7)\n\
run()\n\
*\n";

//analog gas sensor
const char* BTEL_AE_Panel2 = 
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text(8,5,medium,L,Diff,245,240,245,r)\n\
add_text(8,6,medium,L,Diff2,245,240,245,s)\n\
add_text(11,6,medium,L,Dew Point,245,240,245,)\n\
add_text_box(11,5,2,L,dewval,245,240,245,t)\n\
add_text(3,5,medium,L,alt1,245,240,245,a)\n\
add_text(3,6,medium,L,alt2,245,240,245,b)\n\
add_button(7,5,7,O,o)\n\
add_gauge(11,0,2,0,100,0,T,0 C,100,10,5)\n\
add_roll_graph(3,0,8,0.0,100.0,100,G,Altitude,,m,1,0,1,0,0,1,medium,none,2,1,42,97,222,2,237,115,7)\n\
run()\n\
*\n";

const char* BTEL_AE_Panel3 = 
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text(10,6,medium,L,Gain,245,240,245,)\n\
add_text(7,6,medium,L,Ch sel,245,240,245,)\n\
add_switch(8,6,1,B,b,0,0)\n\
add_switch(11,6,1,C,c,0,0)\n\
add_text(12,1,small,L,3.1V,245,240,245,)\n\
add_text(12,4,small,L,0.2V,245,240,245,)\n\
add_text(11,0,medium,L,VRef,245,240,245,)\n\
add_slider(11,1,3,0,100,50,*X,*,0)\n\
add_roll_graph(3,0,8,0.0,3300.0,100,G,Electrochemical Gas Sensor,,mV,1,0,1,1,0,1,medium,none,1,ADC,42,97,222)\n\
run()\n\
*\n";

//TI ADS1299 interface if present
const char* BTEL_AE_Panel4 = 
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_roll_graph(3,0,10,-100.0,100.0,100,G,ADS1299,X-Axis,Y-Axis,1,0,1,0,0,1,medium,none,8,tr1,42,97,222,tr2,237,115,7,tr3,51,215,155,tr4,215,51,205,tr5,30,176,23,tr6,246,227,65,tr7,224,62,62,tr8,62,170,205)\n\
run()\n\
*\n";


int32_t AE_SetPanel(void) {
  
  switch(BTEL.PanelSelector) {
  case (0+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_AE_Panel1);
  break;
  case (1+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_AE_Panel2);
  break;
  case (2+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_AE_Panel3);
  break;
  case (3+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_AE_Panel4);
  break;
  default:
    TrapError();
  }

  return 0;
}

#endif
