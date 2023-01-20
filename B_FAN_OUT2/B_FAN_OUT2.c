/**
  ******************************************************************************
  * @file    B_FAN_OUT2.C (STMod+ add-on board)
  * @author  S.Ma
  * @brief   STMod+ add-on board services for STM32C0316-DK (hot plug and play supported)
  * Future_improvements: Split the pinout specific part to make most of the code frozen. (reduce maintenance, speedup reuse and customization)
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
#include "B_FAN_OUT2.h"

FO_AddOn_t FO_AddOn ={ // This is the SW FIFO to "printf" onto UART to HC-06 SPP Bluetooth to Blueooth Electronics Android App (remote dashboard display onto a smartphone)
  
  .pReport = &u8fifo_from_SPIP[BT_STL_SERIAL],
};

#if 0 // old HAL way
// STMod+ Pin configuration through STMod+, assume by default all GPIOs are input floating
// pin 1
IO_Pin_t FO_CTS_STMod =         {  GPIOA, {    GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,      		GPIO_AF1_USART2,   } };
IO_Pin_t FO_NSS_STMod =         {  GPIOA, {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,      0,   } };
// pin2
IO_Pin_t FO_TX_STMod =          {  GPIOA, {    GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          GPIO_AF1_USART2,   } };
IO_Pin_t FO_MOSI_STMod =        {  GPIOA, {    GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,          0,   } };
// pin3
IO_Pin_t FO_RX_STMod =          {  GPIOA, {    GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,         GPIO_AF1_USART2,   } };
IO_Pin_t FO_MISO_STMod =        {  GPIOA, {    GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         0,   } };
// pin4
IO_Pin_t FO_RTS_STMod =         {  GPIOA, {    GPIO_PIN_1, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          GPIO_AF1_USART2,   } };
IO_Pin_t FO_SCK_STMod =         {  GPIOA, {    GPIO_PIN_1, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,          0,   } };
// pin5 = GND
// pin6 = 5V
// pin7 = SCL
// pin8 = gpio
IO_Pin_t FO_Pin8_STMod =    {  GPIOA, {    GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
// pin9 = gpio
IO_Pin_t FO_Pin9_STMod =    {  GPIOA, {    GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
// pin10 = SDA
// pin11 gpio
IO_Pin_t FO_Pin11_STMod =    {  GPIOC, {    GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
// pin12 gpio
IO_Pin_t FO_Pin12_STMod =    {  GPIOC, {    GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
// pin13 DAC
IO_Pin_t FO_DAC1_STMod =    {  GPIOA, {    GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
// pin14 DAC
IO_Pin_t FO_DAC2_STMod =    {  GPIOA, {    GPIO_PIN_5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
// pin15 5V
// pin16 GND
// pin17 gpio
IO_Pin_t FO_Pin17_STMod =    {  GPIOB, {    GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };
// pin18 gpio
IO_Pin_t FO_Pin18_STMod =    {  GPIOA, {    GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };
// pin19 gpio
IO_Pin_t FO_ADC1_STMod =    {  GPIOD, {    GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };
// pin20 adc
IO_Pin_t FO_ADC2_STMod =    {  GPIOD, {    GPIO_PIN_0, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };
#endif


// STMod+ Pin configuration through STMod+, assume by default all GPIOs are input floating at plug in time (HW dynamic reconfiguration supported)
// FO = Fan Out (board)
// pin 1
IO_Pad_t FO_CTS_STMod =   {  PA_0, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },	GPIO_AF1_USART2 };
IO_Pad_t FO_NSS_STMod =   {  PA_0, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },      0 };
// pin2
IO_Pad_t FO_TX_STMod =    {  PA_2, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },  GPIO_AF1_USART2 };
IO_Pad_t FO_MOSI_STMod =  {  PA_2, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },          0 };
// pin3
IO_Pad_t FO_RX_STMod =    {  PA_3, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },  GPIO_AF1_USART2 };
IO_Pad_t FO_MISO_STMod =  {  PA_3, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },         0 };
// pin4
IO_Pad_t FO_RTS_STMod =   {  PA_1, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },  GPIO_AF1_USART2 };
IO_Pad_t FO_SCK_STMod =   {  PA_1, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },          0 };
// pin5 = GND
// pin6 = 5V
// pin7 = SCL
// pin8 = gpio
IO_Pad_t FO_Pin8_STMod =  {  PA_7, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };
// pin9 = gpio
IO_Pad_t FO_Pin9_STMod =  {  PA_12, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };
// pin10 = SDA
// pin11 gpio
IO_Pad_t FO_Pin11_STMod = {  PC_14, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };
// pin12 gpio
IO_Pad_t FO_Pin12_STMod = {  PC_15, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };
// pin13 DAC
IO_Pad_t FO_DAC1_STMod =  {  PA_4, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };
// pin14 DAC
IO_Pad_t FO_DAC2_STMod =  {  PA_5, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 },       0 };
// pin15 5V
// pin16 GND
// pin17 gpio
IO_Pad_t FO_Pin17_STMod = {  PB_0, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },       0 };
// pin18 gpio
IO_Pad_t FO_Pin18_STMod = {  PA_6, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },       0 };
// pin19 gpio
IO_Pad_t FO_ADC1_STMod =  {  PD_1, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },       0 };
// pin20 adc
IO_Pad_t FO_ADC2_STMod =  {  PD_0, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_03 },       0 };

//======= 8>< ------------------
// These are the functions always present to all types of STMod+ Add-on boards(.c)
int32_t FO_Init(void) { return 0;};
int32_t FO_DeInit(void) { return 0;};
void FO_Test(void){};
int32_t FO_Polling(void) { return 0;};
int32_t FO_10ms(void) { return 0;};
int32_t FO_50ms(void) { // called every 50 msec The click board for Ozone testing has been disabled.
//  O2C_Test(); // to check if the click board Ozone 2 Click is working fine.
  return 0;
};
int32_t FO_100ms(void) {   return 0;};
int32_t FO_1sec(void) { return 0;};

int32_t FO_IsPlugged(void) { // When the board was plugged in and detected through the EEPROM ID : Configure the GPIO and peripherals for this board.
// default pinout configuration (RS232 and Analog activated)

//    IO_PadInit(&FO_CTS_STMod);
    IO_PadInit(&FO_TX_STMod);// BT HC-06 by default
    IO_PadInit(&FO_RX_STMod);// BT HC-06 by default
//    IO_PadInit(&FO_RTS_STMod);
    
    IO_PadSetHigh(FO_Pin8_STMod.PadName); // This can be buried within the Init structure when using new io drivers.
    IO_PadInit(&FO_Pin8_STMod);
    IO_PadSetLow(FO_Pin9_STMod.PadName); // This can be buried within the Init stucture when using new io drivers.
    IO_PadInit(&FO_Pin9_STMod);
    
    IO_PadInit(&FO_Pin11_STMod);
    IO_PadSetHigh(FO_Pin12_STMod.PadName); // same
    IO_PadInit(&FO_Pin12_STMod);
    IO_PadInit(&FO_DAC1_STMod);
    IO_PadInit(&FO_DAC2_STMod);
    
    IO_PadInit(&FO_Pin17_STMod);
    IO_PadInit(&FO_Pin18_STMod);
    IO_PadInit(&FO_ADC1_STMod);
    IO_PadInit(&FO_ADC2_STMod);
    
    // configurer l'USART 9600bps par defaut
    
    // Bluetooth Electronics remote dashboard pages count
    pBTEL->PanelSelectorMax = BRISK_PANELS + 2; // 1+(0,1,2)
    
  return 0;
};

int32_t FO_IsUnplugged(void) { // the deinit of the IO pins is done in add_on_board.c
  
  return 0;
};

//================
// Bluetooth Electronics (BTLE) Android App HMI implementation, simplified for STM32C0316
 
static  int32_t BTEL_IO_Mode_Printf(u8fifo_t* pReport, IO_Pin_t* pPin); // Reporting

//static uint8_t FO_AddOn.Pinmode[20];

//ByteVein_t* FO_AddOn.pReport = &BV_to_SPIP[APP_SERIAL];
int32_t FO_Report(void) {
  // we can use BTEL as global context here
  // this report is triggered from brisk.c
  switch(BTEL.PanelSelector) {
  case (0+BRISK_PANELS): // pin 0..10
    // show the pin level
    BTEL_Printf(FO_AddOn.pReport, "*A%d", IO_PadGet(FO_CTS_STMod.PadName));    // pin 1
    BTEL_Printf(FO_AddOn.pReport, "*B%d", IO_PadGet(FO_TX_STMod.PadName));     // pin 2
    BTEL_Printf(FO_AddOn.pReport, "*C%d", IO_PadGet(FO_RX_STMod.PadName));     // pin 3
    BTEL_Printf(FO_AddOn.pReport, "*D%d", IO_PadGet(FO_RTS_STMod.PadName));    // pin 4
    
    //BTEL_Printf(FO_AddOn.pReport, "*E%d", IO_PadGet(PC_0); //HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)); //SCL3 IO_PinGet(&FO_TX_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*F%d", IO_PadGet(FO_Pin8_STMod.PadName));
    BTEL_Printf(FO_AddOn.pReport, "*G%d", IO_PadGet(FO_Pin9_STMod.PadName));
    //BTEL_Printf(FO_AddOn.pReport, "*H%d", IO_PadGet(PC_1);//HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)); //SDA3 IO_PinGet(&FO_TX_STMod));

    // show the pin configuration
    BTEL_Printf(FO_AddOn.pReport, "*I"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_CTS_STMod);    // pin 1
    BTEL_Printf(FO_AddOn.pReport, "*J"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_TX_STMod);     // pin 2
    BTEL_Printf(FO_AddOn.pReport, "*K"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_RX_STMod);     // pin 3
    BTEL_Printf(FO_AddOn.pReport, "*L"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_RTS_STMod);    // pin 4
    
    BTEL_Printf(FO_AddOn.pReport, "*MSCL3"); // pin 7
    BTEL_Printf(FO_AddOn.pReport, "*N"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_Pin8_STMod);// pin 8
    BTEL_Printf(FO_AddOn.pReport, "*O"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_Pin9_STMod);// pin 9
    BTEL_Printf(FO_AddOn.pReport, "*PSDA3"); // pin 10
  break;
  
  case (1+BRISK_PANELS): // pin 11..20
#if 0
    BTEL_Printf(FO_AddOn.pReport, "*A%d", IO_PadGet(FO_Pin11_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*B%d", IO_PadGet(FO_Pin12_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*C%d", IO_PadGet(FO_DAC1_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*D%d", IO_PadGet(FO_DAC2_STMod));
    
    BTEL_Printf(FO_AddOn.pReport, "*E%d", IO_PadGet(&FO_Pin17_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*F%d", IO_PadGet(&FO_Pin18_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*G%d", IO_PadGet(&FO_ADC1_STMod));
    BTEL_Printf(FO_AddOn.pReport, "*H%d", IO_PadGet(&FO_ADC2_STMod));
#endif
    // show the pin configuration
    BTEL_Printf(FO_AddOn.pReport, "*I"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_Pin11_STMod);
    BTEL_Printf(FO_AddOn.pReport, "*J"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_Pin12_STMod);
    BTEL_Printf(FO_AddOn.pReport, "*K"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_DAC1_STMod);
    BTEL_Printf(FO_AddOn.pReport, "*L"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_DAC2_STMod);
    
    BTEL_Printf(FO_AddOn.pReport, "*M"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_Pin17_STMod);
    BTEL_Printf(FO_AddOn.pReport, "*N"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_Pin18_STMod);
    BTEL_Printf(FO_AddOn.pReport, "*O"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_ADC1_STMod);
    BTEL_Printf(FO_AddOn.pReport, "*P"); BTEL_IO_Mode_Printf(FO_AddOn.pReport, &FO_ADC2_STMod);
  break;
  
  case (2+BRISK_PANELS): // analog pane
    if(Analog.ADCs.Normal_Lsb[5]>4095)
      NOPs(1);

    BTEL_Printf(FO_AddOn.pReport, "*G%d,%d,%d,%d,%d,%d,%d,%d", 
                Analog.ADCs.Normal_Lsb[12],
                Analog.ADCs.Normal_Lsb[11],
                Analog.ADCs.Normal_Lsb[9],
                Analog.ADCs.Normal_Lsb[10],
                Analog.ADCs.Normal_Lsb[8],
                Analog.ADCs.Normal_Lsb[7],
                Analog.ADCs.Normal_Lsb[6],
                Analog.ADCs.Normal_Lsb[5] );

  return 0;
  break;
  default: TrapError();
  };
  return 0;
}
                
                
static int32_t BTEL_IO_Mode_Printf(u8fifo_t* pReport, IO_Pin_t* pPin) {

  switch(pPin->Init.Mode & 3) {
  case 0: BTEL_Printf(pReport, "In"); break;
  case 1: BTEL_Printf(pReport, (pPin->GPIO->ODR & pPin->Init.Pin)?"1":"0"); break;
  case 2: BTEL_Printf(pReport, "AF"); break;
  case 3: BTEL_Printf(pReport, "AD"); break;
  };
  
  return 0;
}
                
int32_t FO_ChangePinmoder(IO_Pin_t* pPin);

int32_t FO_ChangePinmoder(IO_Pin_t* pPin) {

  switch(pPin->Init.Mode) {
  case GPIO_MODE_INPUT: pPin->Init.Mode = GPIO_MODE_OUTPUT_PP; IO_PinConfigure(pPin); break;
  case GPIO_MODE_OUTPUT_PP: pPin->Init.Mode = GPIO_MODE_ANALOG; IO_PinConfigure(pPin); break;
  case GPIO_MODE_ANALOG: pPin->Init.Mode = GPIO_MODE_AF_PP; IO_PinConfigure(pPin); break;
  case GPIO_MODE_AF_PP: pPin->Init.Mode = GPIO_MODE_INPUT; IO_PinConfigure(pPin); break;
  }
  
  return 0;
}
//int32_t FO_AddOn.SliderValue;
int32_t FO_PaneProcess(void) {
  
  char c = BTEL.incoming[0];
  
  // called when this is the add-on board of this STM32
  // for now, we assume it is master brisk.
  if(pBTEL->PanelSelector==(0+BRISK_PANELS)) { // this is the GPIO panel pin 1..10
    
    switch(c) {
    case 'a': IO_PinToggle(&FO_CTS_STMod);       break;
    //
    //
    case 'd': IO_PinToggle(&FO_RTS_STMod);     break;
    //
    case 'f': IO_PinToggle(&FO_Pin8_STMod);    break;
    case 'g': IO_PinToggle(&FO_Pin9_STMod);    break;
    //
      // single character pull button
    case 'i': FO_ChangePinmoder(&FO_CTS_STMod);      break;
    //forbidden in master mode case 'j': FO_ChangeFO_AddOn.Pinmoder(&);      break;
    //forbidden in master mode case 'k': FO_ChangeFO_AddOn.Pinmoder(&);       break;
    case 'l': FO_ChangePinmoder(&FO_RTS_STMod);       break;
    //case 'm': FO_ChangeFO_AddOn.Pinmoder(&);       break;
    case 'n': FO_ChangePinmoder(&FO_Pin8_STMod);       break;
    case 'o': FO_ChangePinmoder(&FO_Pin9_STMod);       break;
    //case 'p': FO_ChangeFO_AddOn.Pinmoder(&);       break;
    };
  };

  if(pBTEL->PanelSelector==(1+BRISK_PANELS)) { // this is the GPIO panel pin 11..20 + DAC control
    
    switch(c) {
    case 'a': IO_PinToggle(&FO_Pin11_STMod);       break;
    case 'b': IO_PinToggle(&FO_Pin12_STMod); break;
    case 'c': IO_PinToggle(&FO_DAC1_STMod); break;
    case 'd': IO_PinToggle(&FO_DAC2_STMod);     break;
    case 'e': IO_PinToggle(&FO_Pin17_STMod); break;
    case 'f': IO_PinToggle(&FO_Pin18_STMod);    break;
    case 'g': IO_PinToggle(&FO_ADC1_STMod);    break;
    case 'h': IO_PinToggle(&FO_ADC2_STMod); break;
      // single character pull button
    case 'i': FO_ChangePinmoder(&FO_Pin11_STMod);      break;
    case 'j': FO_ChangePinmoder(&FO_Pin12_STMod);      break;
    case 'k': FO_ChangePinmoder(&FO_DAC1_STMod);       break;
    case 'l': FO_ChangePinmoder(&FO_DAC2_STMod);       break;
    case 'm': FO_ChangePinmoder(&FO_Pin17_STMod);       break;
    case 'n': FO_ChangePinmoder(&FO_Pin18_STMod);       break;
    case 'o': FO_ChangePinmoder(&FO_ADC1_STMod);       break;
    case 'p': FO_ChangePinmoder(&FO_ADC2_STMod);       break;
    
    // DAC related, on/off and slider
    case 'T': /*HAL_DAC_Start(&hdac1, DAC_CHANNEL_1)*/; break; // turn on DAC1
    case 't': /*HAL_DAC_Stop(&hdac1,DAC_CHANNEL_1)*/; break; // turn off DAC1
    case 'U': /*HAL_DAC_Start(&hdac1, DAC_CHANNEL_2)*/; break; // turn on DAC2
    case 'u': /*HAL_DAC_Stop(&hdac1,DAC_CHANNEL_2)*/; break; // turn off DAC2
    case 'V': 
      if(StringToFrac(&BTEL.incoming[1], &FO_AddOn.SliderValue)<0) return 0; // failed
      if((FO_AddOn.SliderValue<0)||(FO_AddOn.SliderValue>100)) return 0; // out of range
      //-HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, FO_AddOn.SliderValue * 40); // 100*40 = 2000 / 4095
      break; // slider DAC1
    case 'W': 
      if(StringToFrac(&BTEL.incoming[1], &FO_AddOn.SliderValue)<0) return 0;
      if((FO_AddOn.SliderValue<0)||(FO_AddOn.SliderValue>100)) return 0; // out of range
      //-HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, FO_AddOn.SliderValue * 40); // 100*40 = 2000 / 4095
      break; // slider DAC2
    };
  };

  if(pBTEL->PanelSelector==(2+BRISK_PANELS)) { // this is the GPIO panel pin 11..20 + DAC control
    
    switch(c) {
    // DAC related, on/off and slider
    case 'T': /*HAL_DAC_Start(&hdac1, DAC_CHANNEL_1)*/; break; // turn on DAC1
    case 't': /*HAL_DAC_Stop(&hdac1,DAC_CHANNEL_1)*/; break; // turn off DAC1
    case 'U': /*HAL_DAC_Start(&hdac1, DAC_CHANNEL_2)*/; break; // turn on DAC2
    case 'u': /*HAL_DAC_Stop(&hdac1,DAC_CHANNEL_2)*/; break; // turn off DAC2
    case 'V': 
      if(StringToFrac(&BTEL.incoming[1], &FO_AddOn.SliderValue)<0) return 0; // failed
      if((FO_AddOn.SliderValue<0)||(FO_AddOn.SliderValue>100)) return 0; // out of range
      //-HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, FO_AddOn.SliderValue * 40); // 100*40 = 2000 / 4095
      break; // slider DAC1
    case 'W': 
      if(StringToFrac(&BTEL.incoming[1], &FO_AddOn.SliderValue)<0) return 0;
      if((FO_AddOn.SliderValue<0)||(FO_AddOn.SliderValue>100)) return 0; // out of range
      //-HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, FO_AddOn.SliderValue * 40); // 100*40 = 2000 / 4095
      break; // slider DAC2
    };
  };
  
  
  return 0;
}


//================== Panel 1 (this will be sent to the phone app to draw the dashboard of the corresponding pages)

const char* BTEL_FO_Panel1 =      
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text(5,0,large,C,1,245,240,245,B)\n\
add_text(6,0,large,C,1,245,240,245,C)\n\
add_text(7,0,large,C,1,245,240,245,D)\n\
add_text(9,0,large,C,1,245,240,245,E)\n\
add_text(10,0,large,C,1,245,240,245,F)\n\
add_text(11,0,large,C,1,245,240,245,G)\n\
add_text(12,0,large,C,1,245,240,245,H)\n\
add_text(9,4,large,C,7,245,240,245,)\n\
add_text(10,4,large,C,8,245,240,245,)\n\
add_text(11,4,large,C,9,245,240,245,)\n\
add_text(4,4,large,C,1,245,240,245,)\n\
add_text(5,4,large,C,2,245,240,245,)\n\
add_text(6,4,large,C,3,245,240,245,)\n\
add_text(7,4,large,C,4,245,240,245,)\n\
add_text(4,0,large,C,1,245,240,245,A)\n\
add_text(12,4,large,C,10,245,240,245,)\n\
add_text(4,2,medium,C,,245,240,245,I)\n\
add_text(5,2,medium,C,,245,240,245,J)\n\
add_text(6,2,medium,C,,245,240,245,K)\n\
add_text(7,2,medium,C,,245,240,245,L)\n\
add_text(9,2,medium,C,SCL,245,240,245,M)\n\
add_text(10,2,medium,C,,245,240,245,N)\n\
add_text(11,2,medium,C,,245,240,245,O)\n\
add_text(12,2,medium,C,SDA,245,240,245,P)\n\
add_button(10,1,9,,f)\n\
add_button(11,1,9,,g)\n\
add_button(5,1,9,,b)\n\
add_button(6,1,9,,c)\n\
add_button(7,1,9,,d)\n\
add_button(9,1,9,,e)\n\
add_button(12,1,9,,h)\n\
add_button(4,3,9,,i)\n\
add_button(5,3,9,,j)\n\
add_button(6,3,9,,k)\n\
add_button(7,3,9,,l)\n\
add_button(9,3,9,,m)\n\
add_button(10,3,9,,n)\n\
add_button(11,3,9,,o)\n\
add_button(12,3,9,,p)\n\
add_button(4,1,9,,a)\n\
run()\n\
*\n";

//==== Panel 2

const char* BTEL_FO_Panel2 =      
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text(4,0,large,C,,245,240,245,A)\n\
add_text(5,0,large,C,,245,240,245,B)\n\
add_text(6,0,large,C,,245,240,245,C)\n\
add_text(7,0,large,C,,245,240,245,D)\n\
add_text(9,0,large,C,,245,240,245,E)\n\
add_text(10,0,large,C,,245,240,245,F)\n\
add_text(11,0,large,C,,245,240,245,G)\n\
add_text(12,0,large,C,,245,240,245,H)\n\
add_text(5,2,medium,C,,245,240,245,J)\n\
add_text(6,2,medium,C,,245,240,245,K)\n\
add_text(7,2,medium,C,,245,240,245,L)\n\
add_text(9,2,medium,C,,245,240,245,M)\n\
add_text(10,2,medium,C,,245,240,245,N)\n\
add_text(11,2,medium,C,,245,240,245,O)\n\
add_text(12,2,medium,C,,245,240,245,P)\n\
add_text(4,4,large,C,11,245,240,245,)\n\
add_text(5,4,large,C,12,245,240,245,)\n\
add_text(6,4,large,C,13,245,240,245,)\n\
add_text(7,4,large,C,14,245,240,245,)\n\
add_text(9,4,large,C,17,245,240,245,)\n\
add_text(10,4,large,C,18,245,240,245,)\n\
add_text(11,4,large,C,19,245,240,245,)\n\
add_text(12,4,large,C,20,245,240,245,)\n\
add_text(4,2,medium,C,,245,240,245,I)\n\
add_button(5,1,9,,b)\n\
add_button(6,1,9,,c)\n\
add_button(7,1,9,,d)\n\
add_button(9,1,9,,e)\n\
add_button(10,1,9,,f)\n\
add_button(11,1,9,,g)\n\
add_button(12,1,9,,h)\n\
add_button(5,3,9,,j)\n\
add_button(6,3,9,,k)\n\
add_button(7,3,9,,l)\n\
add_button(9,3,9,,m)\n\
add_button(10,3,9,,n)\n\
add_button(11,3,9,,o)\n\
add_button(12,3,9,,p)\n\
add_button(4,3,9,,i)\n\
add_button(4,1,9,,a)\n\
run()\n\
*\n";

//===== Pannel 3

const char* BTEL_FO_Panel3 = 
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text(11,0,medium,C,Dac1,245,240,245,)\n\
add_text(12,0,medium,C,Dac2,245,240,245,)\n\
add_text(9,5,medium,L,Dac2.14,245,240,245,)\n\
add_text(4,5,medium,L,Dac1.13,245,240,245,)\n\
add_switch(6,5,2,T,t,0,0)\n\
add_switch(11,5,2,U,u,0,0)\n\
add_slider(11,1,3,0,100,70,*V,*,0)\n\
add_slider(12,1,3,0,100,59,*W,*,0)\n\
add_roll_graph(3,0,8,0.0,4095.0,100,G,,,Lsb,0,0,1,1,1,1,medium,none,8,#8,42,97,222,#9,237,115,7,#13*,51,215,155,#14*,215,51,205,#17,30,176,23,#18,246,227,65,#19,224,62,62,#20,62,170,205)\n\
run()\n\
*\n";

int32_t FO_SetPanel(void) {
  
  switch(BTEL.PanelSelector) {
  case (0+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_FO_Panel1);
    break;
  case (1+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_FO_Panel2);
    break;
  case (2+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_FO_Panel3);
    break;
  default:
    TrapError();
  }

  return 0;
}
