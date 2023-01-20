/**
  ******************************************************************************
  * @file    brisk.c (This democode one layer below main() for easy add/remove/disable. )
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

#include "main.h"
#include "brisk.h"

/*
uint32_t CountOnes(uint32_t v);

uint32_t CountOnes(uint32_t v) {
  uint32_t c; // if optimized, 12 instructions to detect 1 bit or more (one pin or multiple pins)
  v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
  c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
  return c;
};
*/

#include "IRQ_Self_Settle.h"

#if 0
Motion3D_t Motion3D;
Magneto3D_t Magneto3D;
#endif

#if 0
const IO_Pin_t UserKeyPin =    {  GPIOA, {    GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } }; // as digital push-in button, can also be used as analog keyboard...
const IO_Pin_t PA11_Default = {  GPIOA, {    GPIO_PIN_11, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW,          0,   } };
#endif
const IO_Pad_t UserKeyPin =   { PA_4, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } }; // as digital push-in button, can also be used as analog keyboard...
const IO_Pad_t PA11_Default = { PA_11,{ .Mode = IO_INPUT, .Pull = IO_PULLUP, .Speed = IO_SPEED_01 } };


UserKey_t UserKey = {
  .pPin = (IO_Pad_t*)&UserKeyPin,
  .Pressed = 0,
  .PressedCount_50ms = 0,
};

// 1 msec Systick interrupt handler for Brisk application purpose
uint32_t Brisk_1ms_Counter;
uint8_t Brisk_5ms_Flag, Brisk_10ms_Flag, Brisk_50ms_Flag, Brisk_100ms_Flag, Brisk_1sec_Flag;
uint32_t Brisk_1sec_Counter;


void Brisk_1ms_ISR(void) {

  Brisk_1ms_Counter++;
  if((Brisk_1ms_Counter % 8/*5*/)==0) { // optimisation for speed and size avoiding division on CortexM0
    Brisk_5ms_Flag = 1;
    if((Brisk_1ms_Counter % 16 /*10*/)==0) {
      Brisk_10ms_Flag = 1;
      if((Brisk_1ms_Counter % 64 /*50*/)==0) {
        Brisk_50ms_Flag = 1;
        if((Brisk_1ms_Counter % 128 /*100*/)==0) {
          Brisk_100ms_Flag = 1;
          if((Brisk_1ms_Counter % 1024 /*1000*/)==0) {
            Brisk_1sec_Flag = 1;
            Brisk_1sec_Counter++; 
          }
        }
      }
    }
  }
}

void Brisk_PostInit(void) { // hook from main() after HAL initialisation (override here)

  uint8_t StartAdr;
  uint8_t i;

//-  IO_PinInit();

  Add_on_Board_Init(&STModADD_On); // 18/11/19

  BriskTimedLEDsInit();
  for(i=1;i<=1;i++, MinDelay_ms(15))
    BriskDimLED(i, BRISK_LED_OFF);
  BriskDimTimedLED(1, 100);
  BriskBlinkTimedLED(1, 1000, 500, BRISK_LED_BLINK_FOREVER );
  Brisk_I2C_MasterIO_Init();
#ifdef _B_HMI_LCD16x2_H_
//-  HL_Init(); // reset the LCD Screen in case it's already plugged in
#endif  
//>  ArduinoInit(&Arduino); // initialize local Arduino context // to check AF I2C/USART/SPI vs Arduino IO control functions
//return;  
  // test I2C_Brisk to know if it is a V1 or V2
//  while(1) {
  StartAdr = 0;
  while(StartAdr = I2C_MasterIO_FindNextSlaveByAddress(&gI2C_STMod, StartAdr)) {

      NOPs(1); // Breakpoint here to sweep and find all I2C slaves on the bus (handy for board bringup, alive test)
  }; // when StartAdr == 0, it means the sweep is over
    
  NOPs(1); // breakpoint hook

  
// Nucleo I2C Arduino test here
  StartAdr = 0;
  while(StartAdr = I2C_MasterIO_FindNextSlaveByAddress(&gI2C_Arduino, StartAdr)) {

      NOPs(1); // Breakpoint here to sweep and find all I2C slaves on the bus (handy for board bringup, alive test)
  }; // when StartAdr == 0, it means the sweep is over
    
  NOPs(1); // breakpoint hook
  
// Test TMOS PIR MEMs
  
  
  
//  };

  // CubeMX may enable EXTI which are unnecessary at present time. Should be disabled at boot
#if 0  
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  HAL_NVIC_DisableIRQ(EXTI1_IRQn);
  HAL_NVIC_DisableIRQ(EXTI2_IRQn);
  HAL_NVIC_DisableIRQ(EXTI3_IRQn);
  HAL_NVIC_DisableIRQ(EXTI4_IRQn);
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
#endif

  SPIP_SIF_Init();
  UserKeyInit(); // mae it analog input
  AnalogInit();
  
  BTEL_Init(); // Bluetooth Electronics Android AP using BT HC-06 Serial link over STMod+ by default
  
  UserKeyInit();
  
  IO_PadInit((IO_Pad_t*)&PA11_Default);
 
  // interrupt statistic, use it only in debug mode, it slightly degrade performance, doesn't replace a simulator or WTEC SW tool, however, it's free
  INTs_AllStats_Init();
  
  // IRQ_OVR demo prep
  //IRQ_OVR_DemoEnter();
  
  // put PA11 with pull up or pull down after reset
  
  // debug HSI48 trimming
#ifdef TRIM_DBG
IRQ_OVR_DemoEnter(); // activate MCO (until a plug and play STMod+ addon board is plugged in
#endif
}


int32_t SleepingLED = 0;

void Brisk_Polling(void) { // hook from main() loop


  if(Brisk_5ms_Flag) { // 8 ms
    Brisk_5ms_Flag = 0;

//==== 8>< -------------- adjust the LED slow dimming
    SleepingLED += 1;
    if(SleepingLED>200)
      SleepingLED = 0;

    if(SleepingLED>100)
      BriskDimTimedLED(1,200-SleepingLED);
    else
      BriskDimTimedLED(1,SleepingLED);
//==== 8>< --------------    
    
  }
  if(Brisk_10ms_Flag) { // 16ms things to do each 10+ msec
    BriskTimedLED_10msecTick(); // manage LED Blink function
//>    BriskI2C_10msecTick(); // eeprom, BMC and ST Mems //TODO TO WORK ON IT SEB THIS FUNCTION IS VERY SLOW, WHY?
    Brisk_10ms_Flag = 0; // 10ms++ (relaxed)
  }
  if(Brisk_50ms_Flag) { // 64ms things to do each 50+ msec
    // here we manage the initialization of the hot pluggable LCD 16x2 char with 5 way rock switch
      Add_on_Board_50msec(&STModADD_On);
      UserKey_50ms();
    Brisk_50ms_Flag = 0; // 50ms++
  }
  if(Brisk_100ms_Flag) { // things to do each 100 msec
    
    IRQ_OVR_DemoEXTI_MainLoop_100msec();
    BTEL_100ms();
    Brisk_100ms_Flag = 0; // 100ms++
  }
  if(Brisk_1sec_Flag) { // 1 second
    Add_on_Board_1sec(&STModADD_On);
    Brisk_1sec_Flag = 0;
  }
    
  
// here the things to do asap (polling mode)
//BriskTestLEDs(); // debug only
//Brisk_I2C_MasterIO_Test(); // debug only
//EEP_Test(); //debug only
//  HL_Test();

  if(   (UserKey.codenow==ROCK_SEL)
     && (UserKey.Pressed_5sec == 1)  ) { // push the BTEL dashboard
	  UserKey.Pressed_5sec = 0;
	  BTEL.Delayed_PanelCountdown_100ms = 2;
  }

// IRQ Overload demo
  if(   (UserKey.codenow==ROCK_DOWN)
     && (UserKey.Pressed_5sec == 1)  ) { // push the BTEL dashboard
	  UserKey.Pressed_5sec = 0;
	  IRQ_OVR_DemoEnter();
  }
  
  if(   (UserKey.codenow==ROCK_UP)
     && (UserKey.Pressed_5sec == 1)  ) { // push the BTEL dashboard
	  UserKey.Pressed_5sec = 0;
	  IRQ_OVR_DemoLeave();
  }

  if(   (UserKey.codenow==ROCK_RIGHT)
     && (UserKey.Pressed == 1)  ) { // push the BTEL dashboard
	  UserKey.Pressed = 0;
	  IRQ_OVR_ProtectionEnable();
  }

  if(   (UserKey.codenow==ROCK_LEFT)
     && (UserKey.Pressed == 1)  ) { // push the BTEL dashboard
	  UserKey.Pressed = 0;
	  IRQ_OVR_ProtectionDisable();
  }
  
  
  BTEL_Polling(); // monitor the FIFO from STmod+ USART and process any incoming/outgoing message.
}

//IO_Pin_t UserKeyPin =    {  GPIOA, {    GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW,          0,   } }; // as digital push-in button, can also be used as analog keyboard...




int32_t UserKeyInit(void) {
  
  IO_PadInit((IO_Pad_t*)&UserKeyPin); // input with pull up
  return 0;
}

int32_t UserKey_50ms(void) {
  
  UserKey.codebefore = UserKey.codenow;
  UserKey.codenow = RockGetKey();//==ROCK_SEL)
    
  //-------------------  
  if(UserKey.codebefore == UserKey.codenow) {
    UserKey.PressedCount_50ms = min2(UserKey.PressedCount_50ms + 1,200);
    if(UserKey.PressedCount_50ms == 4)
      UserKey.Pressed |= 1;
    else
      if(UserKey.PressedCount_50ms == 100)
        UserKey.Pressed_5sec = 1;
    
  }else{
    UserKey.PressedCount_50ms = 0;
  }
  
  return 0;
}


Brisk_t Brisk = {
  .version = 3,
  .pUserKey = &UserKey,
//  .pLocalBuzzer = &LocalBuzzer,
  
  .pAnalog = &Analog,
  
  .pSTModpADD_On = &STModADD_On, // one STMod+ add-on board
#ifdef _B_OPTO_INERT_BOARD_   
  .pOI_AddOn = &OI_AddOn, // context of the Optical and Motion add-on board
#endif
#ifdef _B_ANALOG_ENV_BOARD_  
  .pAE_AddOn = &AE_AddOn,
#endif  
  
  .pClockLEDs = &BriskTimedLEDs[0],
#if 0  
  .pMotion3D = &Motion3D,
  .pMagneto3D = &Magneto3D,
#endif  
};

Brisk_t* pBrisk = &Brisk; // in debugger mode, you can open this structure which describes neatly your application BSP and functionality, less searching with label names.

/*
i32_err_t sebtestcompiler(int32_t a, int32_t b) {
  
  i32_err_t result;
  result.value = a * b;
  result.errorcode = -3;
  return result;
}
*/

