/**
  ******************************************************************************
  * @file    analog.c (STM32 ADC related functions, internal and external channel)
  * @author  S.Ma
  * @brief   ADC management, Rock Switch Analog Keyboard management (convert voltage to key index), external and internal channels, mV conversion, loop channels continuous conversion mode.
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

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

__ALIGN_BEGIN
Analog_t Analog;
__ALIGN_END

//const IO_Pin_t UserRockSwitchAnalog = {  GPIOA,          {    GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0,    },}; // PA4 = analog keyboard if demo switch activated

const IO_Pad_t UserRockSwitchAnalog = {  PA_4, { .Mode = IO_ANALOG, .Pull = IO_NOPULL} }; // PA4 = analog keyboard if demo switch activated

// the voltages of the rock switch are tracking Vdd so we should use LSB to be Vdd independent (1.8~3.6V)
// still at rest 3.3V vdd, the keyboard is read at 2.6V... there is still something fishy to debug
//const uint16_t rock_lsbs[] = { 0, 577, 1305, 2066, 2288, 3249, 4095 }; // ADC 12 bit modes right aligned
const uint16_t rock_lsbs[] = { 0, 0x33d, 0x65e, 0x9c0, 0xcdd, 4095 }; // ADC 12 bit modes right aligned
const uint16_t rock_limits[] = { (0 + 0x33d)/2, (0x33d + 0x65e)/2, (0x65e + 0xcdd)/2, (0x9c0 + 0xfbe)/2, (0xccd + 0xfff)/2};
//old enum { ROCK_SEL, ROCK_UP, ROCK_LEFT, ROCK_RIGHT, ROCK_DOWN, ROCK_UNPRESSED,  } rock_keys_t;
//typedef enum { ROCK_SEL, ROCK_LEFT, ROCK_DOWN, ROCK_RIGHT, ROCK_UP, ROCK_UNPRESSED } rock_keys_t; // keysequence in increasing voltage

void ActivateRockSwitch(void) {

	IO_PadInit((IO_Pad_t*)&UserRockSwitchAnalog); // this should be done after any PA4 GPIO configuration, or in the main loop
}

uint8_t RockGetKey(void) {

	uint16_t lsb = Analog.ADCs.Normal_Lsb[2];
	uint8_t i;
	for(i = 0; i<COUNTOF(rock_limits); i++) {
		if(lsb<=rock_limits[i])
			return i;
	}

	return ROCK_UNPRESSED;
}

#if 0 // with DMA
const ADC_ChannelConfTypeDef sConfigs[] = {
  { ADC_CHANNEL_VREFINT,    ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 1 Internal
  { ADC_CHANNEL_TEMPSENSOR, ADC_REGULAR_RANK_2,    ADC_SAMPLINGTIME_COMMON_1       }, // 2 Internal
  { ADC_CHANNEL_4,          ADC_REGULAR_RANK_3,    ADC_SAMPLINGTIME_COMMON_1       }, // 3 DAC User Analog Keyboard
  { ADC_CHANNEL_5,          ADC_REGULAR_RANK_4,    ADC_SAMPLINGTIME_COMMON_1       }, // 4 DAC User LED / DAC if enabled and LED disabled
  { ADC_CHANNEL_15,          ADC_REGULAR_RANK_3,    ADC_SAMPLINGTIME_COMMON_1       }, // 3 DAC User Analog Keyboard
  { ADC_CHANNEL_16,          ADC_REGULAR_RANK_4,    ADC_SAMPLINGTIME_COMMON_1       }, // 4 DAC User LED / DAC if enabled and LED disabled
//  { ADC_CHANNEL_16,          ADC_REGULAR_RANK_5,    ADC_SAMPLINGTIME_COMMON_1       }, // 5 PB12 (no connect, available)
};
#else // one shot successive
const ADC_ChannelConfTypeDef sConfigs[] = {
  {/* LL_ADC_CHANNEL_10 | ADC_CHANNEL_ID_INTERNAL_CH*/ADC_CHANNEL_VREFINT,    ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 1 Internal
  {/* LL_ADC_CHANNEL_9 | ADC_CHANNEL_ID_INTERNAL_CH*/ADC_CHANNEL_TEMPSENSOR, ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 2 Internal
  { ADC_CHANNEL_4,          ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 3 DAC User Analog Keyboard
  { ADC_CHANNEL_5,          ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 4 DAC User LED / DAC if enabled and LED disabled
  { ADC_CHANNEL_15,          ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 3 DAC User Analog Keyboard
  { ADC_CHANNEL_16,          ADC_REGULAR_RANK_1,    ADC_SAMPLINGTIME_COMMON_1       }, // 4 DAC User LED / DAC if enabled and LED disabled
//  { ADC_CHANNEL_16,          ADC_REGULAR_RANK_5,    ADC_SAMPLINGTIME_COMMON_1       }, // 5 PB12 (no connect, available)
};
#endif


#ifndef hadc1
  ADC_HandleTypeDef hadc1;
#endif
void AnalogInit(void) {
  
  //uint16_t i;
  
  // Bringup too long if we use DMA and especially DMAMUX to sweep through all channels
  // we will use manual single channel conversion as simple 8 bit MCU. Non optimal.

//  ADC_ChannelConfTypeDef sConfig;
//  ADC_InjectionConfTypeDef sConfigInjected;

  __HAL_RCC_ADC_CLK_ENABLE();;//ClockGateEnable_PPP((uint32_t) ADCx, ENABLE); // Enable ADC Clocks
 //because it's void without MX! HAL_ADC_DeInit(&hadc1);
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;//ADC_CLOCK_ASYNC_DIV2;//ADC_CLOCK_SYNC_PCLK_DIV2; // 48/2 /2 = 12 MHz
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;//- ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;//ENABLE;//DISABLE;
  hadc1.Init.NbrOfConversion = 1;//COUNTOF(sConfigs);
  hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;//ENABLE;// DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;//ADC_SAMPLETIME_12CYCLES_5;//ADC_SAMPLETIME_1CYCLE_5;//ADC_SAMPLETIME_160CYCLES_5;//ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_39CYCLES_5;//ADC_SAMPLETIME_12CYCLES_5;//ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;

  if (HAL_ADC_Init(&hadc1) != HAL_OK)  {
    Error_Handler();
  }
  
//- sept 20  ADC1_COMMON->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN ; // hack Aug 30
  
#if 0 // no injected channels
  /**Configure Injected Channel     */
  for(i=0;i<COUNTOF(sConfigs); i++) {
    if (HAL_ADC_ConfigChannel(&hadc1, (ADC_ChannelConfTypeDef*)&sConfigs[i]) != HAL_OK)  {
      Error_Handler();
    }
  }
#endif
#ifndef hdma_adc1
DMA_HandleTypeDef hdma_adc1;
#endif
#if 0  
  HAL_DMA_DeInit(&hdma_adc1);
  // pins being analog or not, the ADC will be kicked with DMA to run
  /* ADC1 DMA Init *///DMAContinuousRequests
  hdma_adc1.Instance = DMA1_Channel1;
  hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;//DMA_NORMAL;
  hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
    Error_Handler();
  }

  __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);
#endif
  HAL_ADCEx_Calibration_Start(&hadc1/*, ADC_SINGLE_ENDED*/);
#if 0
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)Analog.ADCs.Normal_Lsb,COUNTOF(sConfigs));
  
  // patch for wrong HAL enabling unwanted IT
  __HAL_DMA_DISABLE_IT(hadc1.DMA_Handle, DMA_IT_HT);
  __HAL_DMA_DISABLE_IT(hadc1.DMA_Handle, (DMA_IT_TC | DMA_IT_TE));
#else
  
#endif
//==== DAC initialization
  // only GPIOs need to be configured to finish the job
  // collect the first conversion

  // activate analog keyboard, disable it if you prefer the analog mode
  // you can detect which mode is enabled by looking at the GPIO MODER to be adaptive
return; // no need to run the bringup test below

  while(1) {
	ActivateRockSwitch();

    HAL_Delay(1);

    ADC_UpdateConveredValues(&Analog.ADCs);

    switch(RockGetKey()) {
    case ROCK_SEL:
    	NOPs(1);// breakpoint hook
    	break;
    case ROCK_UP:
    	NOPs(1);// breakpoint hook
    	break;
    case ROCK_LEFT:
    	NOPs(1);// breakpoint hook
    	break;
    case ROCK_RIGHT:
    	NOPs(1);// breakpoint hook
    	break;
    case ROCK_DOWN:
    	NOPs(1);// breakpoint hook
    	break;
    }
 // return; // go back to main...
  }
}


//============================== The public functions to build your system mecanics

uint32_t ADC_Normal_LsbTo_mV(ADC_t* A, uint32_t Lsb) {
  uint32_t mV = Interpolate_i32 (0, 0x0FFF, 0, A->VRef_mV, Lsb);  
  return mV;
}

uint32_t ADC_Injected_LsbTo_mV(ADC_t* A, uint32_t Lsb) {
  uint32_t mV = Interpolate_i32 (0, (int32_t)0x7FF8, 0, A->VRef_mV, (int32_t)(int16_t)Lsb);  
  return mV;
}
/* debug
  int32_t Deg30Lsb;
  int32_t Deg130Lsb;
  int32_t ADC_Num, ADC_Denum;
  int32_t DegC_x10;
*/
//===================== 8>< ----------------------------------

int32_t ADC_Convert_mV_to_DegC_x10(ADC_t* A, uint32_t Lsb) {
  
  int32_t Deg30Lsb;
  int32_t Deg130Lsb;
  //int32_t ADC_Num, ADC_Denum;
  int32_t DegC_x10;
  
  Deg30Lsb = (int32_t)(*((uint16_t*) 0x1FFF7568));  // 3.0V +/- 10mV
  Deg130Lsb = Deg30Lsb + 343; // +100 degC at 2.53mV per degree at 3.0V Vdda result in an offset of 343 lsb
  // we first convert the LSB calibrated Flash values to mV (let's remove the sign issue between normal and injected channels)

  
  // convert ADC value to 3.0 Vdd vs measured value
  Lsb = (Lsb*A->MeasuredVdd_mV)/3000; // scale the LSB to 3.0V
  
  DegC_x10 = Interpolate_i32 (Deg30Lsb, Deg130Lsb, 300U, 1300U, (int32_t)(int16_t)Lsb);

  A->Temp_degC_x10 = DegC_x10; // capture the value for debugging or reference
  return DegC_x10;
}

//volatile int32_t monitor;
int32_t ADC_Convert_VRefByLsb(ADC_t* A, uint32_t Lsb) {
  // Vref = 1.212V
  int32_t VRefLsb3p3V_Lsb = (int32_t)(*((uint16_t*) 0x1FFF756A )); // this contains the ADC 12 bit right aligned injected raw data for Vref LSB at 30C and 3.0V
//  monitor = VRefLsb3p3V_Lsb;
  uint32_t Vdd_mV = (3000*VRefLsb3p3V_Lsb)/Lsb;
  A->MeasuredVdd_mV = Vdd_mV;
  return 0;
}

//===================== 8>< ----------------------------------
  
void ADC_MultiShotNoDMA(ADC_HandleTypeDef* A);  

void ADC_MultiShotNoDMA(ADC_HandleTypeDef* A) {
  uint32_t dr;
  /** Configure Regular Channel
  */
  
  for(int i = 0; i<COUNTOF(sConfigs); i++) {
    if (HAL_ADC_ConfigChannel(A, (ADC_ChannelConfTypeDef*)&sConfigs[i]) != HAL_OK) while(1);
    HAL_ADC_Start(A);
    if(HAL_ADC_PollForConversion(A, 2) != HAL_OK)     while(1);
    Analog.ADCs.Normal_Lsb[i] = dr = HAL_ADC_GetValue(A);
  }
  
}


void ADC_UpdateConveredValues(ADC_t* A) {
  
  uint8_t i;

#if 1
  // we do one shot mode, no DMA assist
  ADC_MultiShotNoDMA(&hadc1);
#endif
   
  
  A->VRef_mV = 3300;
  // mmeasure the real Vdd (3.3V?)
  ADC_Convert_VRefByLsb(A, A->Normal_Lsb[0]);
  // use the precise measurement as reference
  //A->VRef_mV = A->MeasuredVdd_mV; // hmm....  
  // convert all channels to their mV equivalent
  for(i=0; i<COUNTOF(sConfigs); i++)
    A->Normal_mV[i] = ADC_Normal_LsbTo_mV(A, A->Normal_Lsb[i]);
  // get the Vbat value
  //A->MeasuredVBat_mV = A->Normal_mV[0]*(5000/750); // external divider 100k over 200k, followed with divider which is also always on... CH18 is static!!!!
  // get the temperature
  ADC_Convert_mV_to_DegC_x10(A,A->Normal_Lsb[1]);
  
}


