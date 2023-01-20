/**
  ******************************************************************************
  * @file    B_OPTO_INERT.c (STMod+ addon board providing optical and inertial sensing)
  * @author  S.Ma
  * @brief   UV Index, Ambiant Lux, PIR, TOF, Accelero/Gyro/Magneto sensors. STM32C0 specific: float lib too big, sensors using it have been disabled in the code.
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
#ifdef _B_OPTO_INERT_BOARD_
OI_AddOn_t OI_AddOn = {
    .pVL53L1 = &dev,
    .TOF_Ref_mm = 0,
    
    .pVEML6070 = &VEML6070,
    .pLV0104CS = &LV0104CS,
    
    .pReport = &u8fifo_from_SPIP[BT_STL_SERIAL],
   // .pbmm050_mag_s32_data_t = &BMX_MAG_data_s32, // bosch source code driver makes it hard to declare here
   // .pbmg160_data_t = &data_gyro,
};
    
// components part of this extension board:
// LV0104CS-D : On Semi Ambiant Light Sensor, I2C address = 0x26/27 (WR)
// VEML6070 : Vishay UV Sensor I2C address = 0x70..73

// STMod+ Pin configuration through STMod+, assume by default all GPIOs are input floating
#if 0
  IO_Pin_t OI_ISM_CS_STMod =      {  GPIOA, {    GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,      0,   } };
  // SPI by GPIO bit bang
  IO_Pin_t OI_ISM_MOSI_STMod =    {  GPIOA, {    GPIO_PIN_2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,          0,   } };
  IO_Pin_t OI_ISM_MISO_STMod =    {  GPIOA, {    GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         0,   } };
  IO_Pin_t OI_ISM_SCK_STMod =     {  GPIOA, {    GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,          5,   } };
  // use real SPI HW
  //NOT SUPPORTED //IO_Pin_t OI_ISM_MOSI_STMod =    {  GPIOF, {    GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          5,   } };
  //NOT SUPPORTED IO_Pin_t OI_ISM_MISO_STMod =    {  GPIOF, {    GPIO_PIN_14, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         5,   } };
  //NOT SUPPORTED IO_Pin_t OI_ISM_SCK_STMod =     {  GPIOF, {    GPIO_PIN_13, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          5,   } };

  #ifdef BMX055_ENABLED
  IO_Pin_t OI_BMX_INT2_STMod =    {  GPIOA, {    GPIO_PIN_7, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };// GPIO_MODE_IT_FALLING
  IO_Pin_t OI_BMX_INT5_STMod =    {  GPIOA, {    GPIO_PIN_12, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };// GPIO_MODE_IT_FALLING
  #endif

  IO_Pin_t OI_ISM_INT1_STMod =    {  GPIOC, {    GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };// GPIO_MODE_IT_FALLING
  IO_Pin_t OI_VEML_RES_STMod =    {  GPIOC, {    GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };
  IO_Pin_t OI_PIR_ADC_STMod =     {  GPIOA, {    GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
  IO_Pin_t OI_VL53_XSHUT_STMod =  {  GPIOA, {    GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };

  IO_Pin_t OI_ISM_INTMAG_STMod =  {  GPIOB, {    GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          0,   } };// GPIO_MODE_IT_FALLING
  IO_Pin_t OI_VEML_ACK_STMod =    {  GPIOA, {    GPIO_PIN_6, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,       0,   } };
  #ifdef BMX055_ENABLED
  IO_Pin_t OI_BMX_DRDYM_STMod =   {  GPIOD, {    GPIO_PIN_1, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,       0,   } };// GPIO_MODE_IT_FALLING
  #endif
#endif  
  
IO_Pad_t OI_ISM_CS_STMod =      {  PA_0,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03} };
// SPI by GPIO bit bang
IO_Pad_t OI_ISM_MOSI_STMod =    {  PA_2,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03} };
IO_Pad_t OI_ISM_MISO_STMod =    {  PA_3,  { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };
IO_Pad_t OI_ISM_SCK_STMod =     {  PA_1,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03} } ;
// use real SPI HW
//NOT SUPPORTED //IO_Pin_t OI_ISM_MOSI_STMod =    {  GPIOF, {    GPIO_PIN_15, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          5,   } };
//NOT SUPPORTED IO_Pin_t OI_ISM_MISO_STMod =    {  GPIOF, {    GPIO_PIN_14, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH,         5,   } };
//NOT SUPPORTED IO_Pin_t OI_ISM_SCK_STMod =     {  GPIOF, {    GPIO_PIN_13, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW,          5,   } };

#ifdef BMX055_ENABLED
IO_Pad_t OI_BMX_INT2_STMod =    {  PA_7,  { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };// GPIO_MODE_IT_FALLING
IO_Pad_t OI_BMX_INT5_STMod =    {  PA_12, { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };// GPIO_MODE_IT_FALLING
#endif

IO_Pad_t OI_ISM_INT1_STMod =    {  PC_14, { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };// GPIO_MODE_IT_FALLING
IO_Pad_t OI_VEML_RES_STMod =    {  PC_15, { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };
IO_Pad_t OI_PIR_ADC_STMod =     {  PA_4,  { .Mode = IO_ANALOG,                       .Pull = IO_NOPULL } };
IO_Pad_t OI_VL53_XSHUT_STMod =  {  PA_5,  { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };

IO_Pad_t OI_ISM_INTMAG_STMod =  {  PB_0,  { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };// GPIO_MODE_IT_FALLING
IO_Pad_t OI_VEML_ACK_STMod =    {  PA_6,  { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };
#ifdef BMX055_ENABLED
IO_Pad_t OI_BMX_DRDYM_STMod =   {  PD_1,  { .Mode = IO_INPUT,                        .Pull = IO_NOPULL } };// GPIO_MODE_IT_FALLING
#endif
  
// no use of STMod+.pin10
// STMod+ pin 20 as no connect.



int32_t OI_Init(void) {
  // we use the 4 byte sub address buffer as data, as this slave has few registers
 return 0;
}

int32_t OI_IsPlugged(void) {
  BriskTimedLED_Disable(); // LED and Key used by add-on board

  IO_PadInit(&OI_ISM_CS_STMod);
  IO_PadInit(&OI_ISM_MOSI_STMod);
  IO_PadInit(&OI_ISM_MISO_STMod);
  IO_PadInit(&OI_ISM_SCK_STMod);
#ifdef BMX055_ENABLED  
  IO_PadInit(&OI_BMX_INT2_STMod);
  IO_PadInit(&OI_BMX_INT5_STMod);
#endif  
  IO_PadInit(&OI_ISM_INT1_STMod);
  IO_PadInit(&OI_VEML_RES_STMod);
  IO_PadInit(&OI_PIR_ADC_STMod);
  IO_PadSetHigh(OI_PIR_ADC_STMod.PadName);
  IO_PadInit(&OI_VL53_XSHUT_STMod);
  IO_PadInit(&OI_ISM_INTMAG_STMod);
  IO_PadInit(&OI_VEML_ACK_STMod);
#ifdef BMX055_ENABLED  
  IO_PadInit(&OI_BMX_DRDYM_STMod);
#endif  
  
  LV0104CS_Init(&LV0104CS);
  LV0104CS_StartContinuousConversion(&LV0104CS);
  
  VEML6070_Init(&VEML6070);
  VEML6070_StartContinuousConversion(&VEML6070);
  
  // PIR is analog input, so disable DAC outputs
  // PIR is PA4 = Analog.ADC.Normal_Lsb[9]
//-  HAL_DAC_Stop(&hdac1,DAC_CHANNEL_1);
//-  HAL_DAC_Stop(&hdac1,DAC_CHANNEL_2);
  
  // time of flight VL53L1
  //VL53L1_Init();
  VL53L1_IsPlugged(); // temp location
  
//  bmg160_data_readout_template(); // add-on board specific
//  bmm050_data_readout_template(); // add-on board specific
#ifdef BMX055_ENABLED
  BMX055_Init();
#endif  
#ifdef ISMDLC_ENABLED
  ISM330DLC_IsPlugged();
#endif  
  BTEL.PanelSelector = 0;
  BTEL.PanelSelectorMax = BRISK_PANELS + 3;
  return 0;
}

int32_t OI_IsUnplugged(void) {
  BriskTimedLED_Enable();
  BriskTimedLEDsInit(); // reinit LEDs when unplugged don't forget the DEMO SWITCH for good output voltage swing
  return 0;
}


int32_t OI_50ms(void) {
   
  int32_t result = 0;
  LV0104CS_50msec(&LV0104CS); // this will make continuous conversion (ODR-less sensor)
  VEML6070_50msec(&VEML6070); // implements non blocking continuous conversion (ODR-less sensor)
  VL53L1_50ms();
#ifdef BMX055_ENABLED
  BMX055_50ms();
#endif  
#ifdef ISMDLC_ENABLED  
  ISM330DLC_Polling();
#endif  
  return result;
}

int32_t OI_1sec(void) {
  
  return 0;  
}
#ifdef BMX055_ENABLED
#include "bmm050.h"
extern struct bmm050_mag_s32_data_t BMX_MAG_data_s32;//data_s32;         /* Structure used for read the mag xyz data with 32 bit output*/
extern int16_t	v_gyro_datax_s16, v_gyro_datay_s16, v_gyro_dataz_s16;/* variable used for read the sensor data*/
extern struct bmg160_data_t data_gyro;/* structure used for read the sensor data - xyz*/
#endif
//ByteVein_t* pReport = &BV_to_SPIP[STM_SERIAL];

extern float acceleration_mg[3];

int32_t OI_Report(void) {

  OI_AddOn.PIR_Raw = (Analog.ADCs.Normal_Lsb[2/*9*/]);
  OI_AddOn.SubSample = (OI_AddOn.SubSample+1)%3;// every 0.5 msec (2Hz)
  
    // bluetooth electronics
  switch(BTEL.PanelSelector) {
  case (0+BRISK_PANELS):
    // Time Of Flight, text [t] and gauge [T]
    BTEL_Printf(OI_AddOn.pReport, "*T%d", pVL53L1->Distance); // distance in mm
    // VEML UV index
    BTEL_Printf(OI_AddOn.pReport, "*U%d", pVEML6070->UV_Index); //
    // VEML Lux
    BTEL_Printf(OI_AddOn.pReport,"*L%C", pLV0104CS->Lux_x100); //
    // PIR analog
    BTEL_Printf(OI_AddOn.pReport, "*G%d.0", (int32_t) OI_AddOn.PIR_Raw ); //
    
    // slow down textual update on screen
    
    if(OI_AddOn.SubSample) return 0;
    
    BTEL_Printf(OI_AddOn.pReport, "*t%D cm", pVL53L1->Distance); // distance in mm
    BTEL_Printf(OI_AddOn.pReport, "*uUV %d", pVEML6070->UV_Index); //
    BTEL_Printf(OI_AddOn.pReport, "*l%d lux", pLV0104CS->Lux_x100/100); //
    BTEL_Printf(OI_AddOn.pReport, "*gPIR %d*", (int32_t)OI_AddOn.PIR_Raw ); //
    OI_AddOn.TOF_Delta_mm = ABS(OI_AddOn.TOF_Ref_mm-pVL53L1->Distance);
    BTEL_Printf(OI_AddOn.pReport, "*d<%d mm>*", OI_AddOn.TOF_Delta_mm);
    return 0;
  break;
  
  case (1+BRISK_PANELS): // magnetometer from Bosch
    #ifdef BMX055_ENABLED
    //BTEL_Printf(0,"*W%D", (int32_t)(BMX_MAG_data_s32.resistance * 10) ); // 
    BTEL_Printf(OI_AddOn.pReport, "*G%d,%d,%d*", (int32_t)(BMX_MAG_data_s32.datax), (int32_t)(BMX_MAG_data_s32.datay) , (int32_t)(BMX_MAG_data_s32.dataz) ); //
    //BTEL_Printf(pOI,"*W%D", (int32_t)(BMX_MAG_data_s32.resistance * 10) ); // 
    
    if(OI_AddOn.SubSample) return 0;
    BTEL_Printf(OI_AddOn.pReport,"*aMagX=%d", (int32_t)(BMX_MAG_data_s32.datax));
    BTEL_Printf(OI_AddOn.pReport,"*bMagY=%d", (int32_t)(BMX_MAG_data_s32.datay));
    BTEL_Printf(OI_AddOn.pReport,"*cMagZ=%d", (int32_t)(BMX_MAG_data_s32.dataz));
    BTEL_Printf(OI_AddOn.pReport,"*dMagR=%d", (int32_t)(BMX_MAG_data_s32.resistance));
    #endif
    return 0;
  break;

  case (2+BRISK_PANELS):
#ifdef BMX055_ENABLED    
//ISM330DLC_Report(LG_AddOn.pReport);    
    BTEL_Printf(OI_AddOn.pReport,"*G%d,%d,%d*", (int32_t)(v_gyro_datax_s16), (int32_t)(v_gyro_datay_s16), (int32_t)(v_gyro_dataz_s16) ); //
    if(OI_AddOn.SubSample) return 0;
    BTEL_Printf(OI_AddOn.pReport,"*aGyroX=%d", (int32_t)(v_gyro_datax_s16));
    BTEL_Printf(OI_AddOn.pReport,"*bGyroY=%d", (int32_t)(v_gyro_datay_s16));
    BTEL_Printf(OI_AddOn.pReport,"*cGyroZ=%d", (int32_t)(v_gyro_dataz_s16));
    BTEL_Printf(OI_AddOn.pReport,"*d0.0");
#endif    
    // ST Chip version? With Accelero?
    return 0;
  break;
  
  case (3+BRISK_PANELS):
    //ISM330DLC_Report(LG_AddOn.pReport);    
    BTEL_Printf(OI_AddOn.pReport,"*G%M,%M,%M*", (int32_t)(acceleration_mg[0]), (int32_t)(acceleration_mg[1]), (int32_t)(acceleration_mg[2]) ); //
    if(OI_AddOn.SubSample) return 0;
    BTEL_Printf(OI_AddOn.pReport,"*aAccX=%C g", (int32_t)(acceleration_mg[0]/10));
    BTEL_Printf(OI_AddOn.pReport,"*bAccY=%C g", (int32_t)(acceleration_mg[1]/10));
    BTEL_Printf(OI_AddOn.pReport,"*cAccZ=%C g", (int32_t)(acceleration_mg[2]/10));
    BTEL_Printf(OI_AddOn.pReport,"*d0.0*");
    // ST Chip version? With Accelero?
    return 0;
    break;
  default:
    TrapError();
  }

  //return 0;
}

int32_t OI_PaneProcess(void) {
  
  char c = BTEL.incoming[0];
  
  // called when this is the add-on board of this STM32
  // for now, we assume it is master brisk.
  if(pBTEL->PanelSelector==(0+BRISK_PANELS)) { // no interaction
    if(c=='o') // time of flight autozero
      OI_AddOn.TOF_Ref_mm = pVL53L1->Distance;
  };

  if(pBTEL->PanelSelector==(1+BRISK_PANELS)) { // no interaction
  };

  if(pBTEL->PanelSelector==(2+BRISK_PANELS)) { // no interaction
  };
  
  return 0;
}


// here are the different panels
// Accelero (not shown)
// Magneto
// Gyro
// TOF
// PIR
// Ambiant
// UV

// optical panel
const char* BTEL_OI_Panel1 =
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text_box(10,6,3,C,0 mm,245,240,245,t)\n\
add_text_box(3,6,3,L,PIR raw,245,240,245,g)\n\
add_text_box(9,0,3,L,Lux,245,240,245,l)\n\
add_text_box(9,1,3,L,UV index,245,240,245,u)\n\
add_gauge(10,3,1,0,3000,0,T,0,3000,3,6)\n\
add_gauge(3,0,4,0,10000,0,L,,,10,5)\n\
add_gauge(3,1,5,0,12,0,U,,,12,1)\n\
add_text(11,2,medium,L,Delta,245,240,245,d)\n\
add_button(10,2,7,O,o)\n\
add_roll_graph(3,2,6,0.0,4095.0,100,G,,,,0,0,1,0,0,1,medium,none,1,1,42,97,222)\n\
run()\n\
*\n";

const char* BTEL_OI_Panel2 = // magneto bosch
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text_box(3,5,4,L,,245,240,245,a)\n\
add_text_box(3,6,4,L,,245,240,245,c)\n\
add_text_box(9,5,4,L,,245,240,245,b)\n\
add_text_box(9,6,4,L,,245,240,245,d)\n\
add_roll_graph(3,0,8,-2048.0,2048.0,100,G,Magneto,,,0,0,1,0,0,1,medium,none,3,x,42,97,222,y,237,115,7,z,255,255,0)\n\
run()\n\
*\n";
//add_gauge(11,0,3,0,100,0,V,0,,10,2)\n
const char* BTEL_OI_Panel3 = // gyro bosch
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text_box(9,5,4,L,,245,240,245,b)\n\
add_text_box(9,6,4,L,,245,240,245,d)\n\
add_text_box(3,5,4,L,,245,240,245,a)\n\
add_text_box(3,6,4,L,,245,240,245,c)\n\
add_roll_graph(3,0,8,-2048.0,2048.0,100,G,Gyro,,,0,0,1,0,0,1,medium,none,3,x,42,97,222,y,237,115,7,z,255,255,0)\n\
run()\n\
*\n";
//add_gauge(11,0,3,0,100,0,V,0,,10,2)\n
const char* BTEL_OI_Panel4 =
"*.kwl\n\
clear_area(3,0,36,30)\n\
add_text_box(9,5,4,L,,245,240,245,b)\n\
add_text_box(9,6,4,L,,245,240,245,d)\n\
add_text_box(3,5,4,L,,245,240,245,a)\n\
add_text_box(3,6,4,L,,245,240,245,c)\n\
add_roll_graph(3,0,8,-2.0,2.0,100,G,Accelero,,,0,0,1,0,0,1,medium,none,3,x,42,97,222,y,237,115,7,z,255,255,0)\n\
run()\n\
*\n";
//add_gauge(11,0,3,0,100,0,V,0,,10,2)\n

int32_t OI_SetPanel(void) {

  switch(BTEL.PanelSelector) {
  case (0+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_OI_Panel1);
  break;
  case (1+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_OI_Panel2);
  break;
  case (2+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_OI_Panel3);
  break;
  case (3+BRISK_PANELS):
    TransmitPanel(pBrisk_Report, (char*)BTEL_OI_Panel4);
  break;
  default:
    TrapError();
  }
  
  return 0;
}


#endif
