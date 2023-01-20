
#include "main.h"
#include "brisk.h"

/**
  ******************************************************************************
  * @file    VL53L1X.c (Time Of Fligth adaptation layer to drive the chip)
  * @author  S.Ma
  * @brief   Implement MCU side BTEL protocol and Arduino/STMod+ Serial Interfaces (USART) and SW FIFO (BSP)
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
#ifdef _B_OPTO_INERT_BOARD_
// inside:
// 
#include "I2C_MasterIO.h"


I2C_SlaveDevice_t VL53L1SD = { &gI2C_STMod, 0x52, 0 };
I2C_SlaveDevice_t * pVL = &VL53L1SD;


VL53L1_Dev_t                   dev; // this is the structure
VL53L1_DEV                     Dev = &dev; // this is the pointer to the structure
VL53L1_Dev_t*                  pVL53L1 = &dev; // the cleaner version to use from now
uint8_t byteData;
uint16_t wordData;



int32_t VL53L1_Init(void) { NOPs(1); return 0;}

int32_t VL53L1_IsPlugged(void) {
  
  int status=0;
  
  pVL53L1->pSD = &VL53L1SD; 
  
/* Those basic I2C read functions can be used to check your own I2C functions */
  status = VL53L1_RdByte(&dev, 0x010F, &pVL53L1->ModelID); // printf("VL53L1X Model_ID: %X\n", byteData);
  status = VL53L1_RdByte(&dev, 0x0110, &pVL53L1->ModuleType); // printf("VL53L1X Module_Type: %X\n", byteData);
  status = VL53L1_RdWord(&dev, 0x010F, &wordData); // printf("VL53L1X: %X\n", wordData);
  while(pVL53L1->sensorState==0){
	status = VL53L1X_BootState(dev, &pVL53L1->sensorState);
	HAL_Delay(2);
  }
  
  /* This function must to be called */
  status = VL53L1X_SensorInit(dev);

  /* Optional functions to set specific parameters here before start ranging */
//  status = VL53L1X_SetDistanceMode(dev, 1); /* 1=short, 2=long */
//  status = VL53L1X_SetTimingBudgetInMs(dev, 100); /* in ms possible values [20, 50, 100, 200, 500] */
//  status = VL53L1X_SetInterMeasurementInMs(dev, 200); /* in ms, IM must be >= TB+ 5ms, otherwise IM*2 */
//  status = VL53L1X_SetOffset(dev,20); /* offset compensation in mm */
//  status = VL53L1X_SetROI(dev, 16, 16); /* minimum ROI 4,4 */
//	status = VL53L1X_CalibrateOffset(dev, 140, &offset); /* may take few second to perform the offset cal*/
//	status = VL53L1X_CalibrateXtalk(dev, 1000, &xtalk); /* may take few second to perform the xtalk cal */
//  printf("VL53L1X Ultra Lite Driver Example running ...\n");
  status = VL53L1X_StartRanging(dev);   /* This function has to be called to enable the ranging */

  return status;
}


int32_t VL53L1_50ms(void) {

  int status;
  uint8_t dataReady;
  
  status = VL53L1X_CheckForDataReady(dev, &dataReady);  
  if(dataReady == 0)
    return 0;

  HAL_Delay(2);
  status = VL53L1X_GetRangeStatus(dev, &pVL53L1->RangeStatus);
  status = VL53L1X_GetDistance(dev, &pVL53L1->Distance);
  status = VL53L1X_GetSignalRate(dev, &pVL53L1->SignalRate);
  status = VL53L1X_GetAmbientRate(dev, &pVL53L1->AmbientRate);
  status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
  NOPs(1); // breakpoint hook point
  return status;
}

int32_t VL53L1_Report(u8fifo_t* pL) {
  
  BTEL_Printf(pL,"*A%d", pVL53L1->Distance); // distance in mm
  
  return 0;
}

#endif
