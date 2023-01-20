
#include "main.h"
#include "brisk.h"
#include "IRQ_Self_Settle.h"
/**
  ******************************************************************************
  * @file    IRQ_Self_Settle.c (IRQ Hardening demo)
  * @author  S.Ma
  * @brief   Shows how an external signal triggering interrupts should be hardened by dodging hackers injecting 48 MHz square wave on the external signal to interrupt saturate the application.
  *          One way is to have the interrupt self disable for a while, which is implemented using a Timer interrupt.... this way, main loop still operate in this "embedded DDOS style attack"
  *          The rock switch is used in this demo. MCO output is used to generate the square wave. Using the FanOut STMod+ board, user can use jumper wire to short MCO with EXTI IO source interrupt....
  *          The user led will freeze without the "patch" while remains unaffected with patch activated....
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
// _IRQ_SELF_SETTLE_H_

INT_Stats_Global_t gINT_Stats;

//int32_t GetTimestamp(void);
int32_t GetTimestamp(void) {

    uint32_t res;
    int32_t cnt;
//    uint32_t tmp = __get_PRIMASK(); // save the interrupt status (could already be disabled)
//    __set_PRIMASK(tmp | 1);
    cnt = TIM3->CNT;
    if(cnt<0) { // bit 31 is set if pending overflow
      res = ((gINT_Stats.tick_msb+1) <<16)|((uint16_t)cnt); 
    }else{
      res = ((gINT_Stats.tick_msb+0) <<16)|((uint16_t)cnt); 
    }
//    __set_PRIMASK(tmp); // restore the interrupt status
    return res;
}

int32_t INTs_AllStats_Init(void) {
  
  for(int32_t ndx = 0; ndx<INT_COUNT; ndx++) {
    INT_Stats_Init(ndx);
  }
  
//  gINT_Stats.highest_level = 0;
  //gINT_Stats.INT_us_temp;
//  gINT_Stats.lowest_level = 0;
//  gINT_Stats.bitpos = 0;
  gINT_Stats.tick_msb = 0;
  // now we assume here SYSTICK is already configured and we calibrate our stuff
  
  return 0;
}

int32_t INT_Stats_Init(INTs_Index_t ndx) {
                       
  if(ndx>=INT_COUNT)  while(1);
  INT_Stats_t* pis = &gINT_Stats.All_cy[ndx];
  
  pis->min_duration = 0x7FFFFFFF; // 
  pis->max_duration = 0x00000000; // 
  pis->min_period = 0x7FFFFFFF;
  pis->max_period = 0x00000000;  
  pis->nb_count = 0;
  pis->nb_countdown = -1; // interrupt overrun disabled
//  pis->nb_too_late = 0;
//  pis->enabled = 0;
  return 0;
}

//uint32_t ts_enter, ts_leave;

void INT_Enter(INTs_Index_t ndx) {

  if(ndx>=INT_COUNT)  while(1);
  INT_Stats_t* pis = &gINT_Stats.All_cy[ndx];
  
//  pis->was_too_late = 0;
  pis->last_enter = pis->enter; //
  pis->enter = GetTimestamp();
}


//=== coder's function to insert in the INT_Handler

void INT_Leave(INTs_Index_t ndx) {

  if(ndx>=INT_COUNT)  while(1);
  INT_Stats_t* pis = &gINT_Stats.All_cy[ndx];
  
  pis->leave = GetTimestamp(); // net
// most computation is done after interrupt latency critical timing
  int32_t period = pis->enter - pis->last_enter; // assume 32 bit value
  if(period>=0) {
    pis->min_period = min2(period, pis->min_period);
    pis->max_period = max2(period, pis->max_period);
  }
// interrupt ended  
  
  int32_t duration = pis->leave - pis->enter;
  if(pis->nb_count<0x7FFFFFFF) {
    pis->nb_count++; // clamped
  };
  
  if(duration<0) {
    NOPs(1);
    return;
  }
  // here we process if interrupt was too late (and might reenter in this interrupt 100% of Core time!)
    pis->min_duration = min2(pis->min_duration, duration);
    if(pis->nb_count>5) // we avoid the first values
      pis->max_duration = max2(pis->max_duration, duration);
    
}


//=========== Interrupt overload prevention ================

void IRQ_OVR_Set(INTs_Index_t ndx, int32_t count);
int32_t IRQ_OVR_Get(INTs_Index_t ndx);
int32_t IRQ_OVR_Fired(INTs_Index_t ndx);

void IRQ_OVR_Set(INTs_Index_t ndx, int32_t count) {
  
  int32_t previous_count = gINT_Stats.All_cy[ndx].nb_countdown;
  gINT_Stats.All_cy[ndx].nb_countdown = count;
  if((previous_count==0)&&(count>0)) { // reactivate the interrupt
    NOPs(1);
  };
  
}

int32_t IRQ_OVR_Get(INTs_Index_t ndx) {
  
  return gINT_Stats.All_cy[ndx].nb_countdown;
}

int32_t IRQ_OVR_Fired(INTs_Index_t ndx) {
  
  int32_t count = gINT_Stats.All_cy[ndx].nb_countdown;
  
  if(count==0)
    return 0;
  else
    if(count<0) {
      NOPs(1);
      return 0;
    }
  else {
    // count >0
    count--;
    gINT_Stats.All_cy[ndx].nb_countdown = count;
    if(count==0) {
      // interrupt disable
      return 1; // time to disable the interrupt!
    }
  }
  return 0;
}

//============ demonstration with simple EXTI interrupt this is the protection, main loop grants credits, interrupt consumes and self disable

void IRQ_OVR_DemoEXTI_MainLoop_100msec(void) {

  if(IRQ_OVR_Get(EXTI0_1_INT)==0) {
    IRQ_OVR_Set(EXTI0_1_INT, 10); // allow 10 interrupt events until next comeback here
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn); // reenable the interrupt
  }
}

void EXTI0_1_IRQHandler(void)
{
  INT_Enter(EXTI0_1_INT);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  if(IRQ_OVR_Fired(EXTI0_1_INT))
    HAL_NVIC_DisableIRQ(EXTI0_1_IRQn); // disable the interrupt
  INT_Leave(EXTI0_1_INT);
}

//================
void ConfigureClockTrimming(void);
void IRQ_OVR_DemoEnter(void) {

  // we use EXTI0 for the demo
  IRQ_OVR_ProtectionDisable();
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 3, 0);

  /* Configure PA1 pin as input floating */
/*
  GPIO_InitTypeDef   GPIO_InitStructure;  
#ifdef TRIM_DBG
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; // this will enable interrupts inside and fires interrupt unfortunately  
#else // normal  
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING; // this will enable interrupts inside and fires interrupt unfortunately
#endif  
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Pin = GPIO_PIN_0;
  
  __disable_irq(); // workaround
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
  IO_Pad_t IO_Pad = { PA_0, { .Mode = IO_INPUT, .Pull = IO_PULLUP, .Speed = IO_SPEED_03 } };
  IO_PadInit(&IO_Pad);
#ifdef TRIM_DBG  
  EXTI_Config_t EXTI_Config = { .Edge = EXTI_RISING_EDGE, .Trigger = EXTI_CHANNEL_TRIGGER_INTERRUPT };
#else
  EXTI_Config_t EXTI_Config = { .Edge = EXTI_BOTH_EDGES, .Trigger = EXTI_CHANNEL_TRIGGER_INTERRUPT };
#endif

  IO_PadInit(&IO_Pad);//+  
  __disable_irq(); // workaround
  IO_EXTI_Config( IO_Pad.PadName, EXTI_Config );
//------------------------  
  HAL_NVIC_DisableIRQ(EXTI0_1_IRQn); // disable the interrupt, it's activated later by user push button!

  

  // activer le MCO2 en PB2.3 = MCO2 = PB1 = PB0
#if 0 // compare!  
  GPIO_InitTypeDef   GPIO_InitStructure;    
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Alternate = 3; // MCO2
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
  IO_Pad_t Pad = { PB_2, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Signal = MCO2 }};
  IO_PadInit(&Pad);
    
  RCC->CFGR |= (3<<20) | (3<<16); // activate MCO on HSI48 with prescaler of 8

  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
  
  __enable_irq();
  
  ConfigureClockTrimming();
  
  
}

void IRQ_OVR_ProtectionEnable(void) {
  
  IRQ_OVR_Set(EXTI0_1_INT, 10);
}

void IRQ_OVR_ProtectionDisable(void) {

  IRQ_OVR_Set(EXTI0_1_INT, -1);  
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);  
}

void IRQ_OVR_DemoLeave(void) {
  
  HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
/*  // disable clock generator MCO
  GPIO_InitTypeDef   GPIO_InitStructure;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Alternate = 3; // MCO2
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);*/
  IO_Pad_t Pad = { PB_2, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Signal = MCO2 } };
  IO_PadInit(&Pad);
  
}

//==== use timer as cycle counter for timestamp generation?
// == timer demo will input capture

// We activate 32768 Hz oscillator injected in PC14 = TIM3.2 = TIM17.1
// we configure TIM3 to free run and interrupt when there is a capture on rising edge
// we measure the time between 2 successive interrupts and check the spread.
void ConfigureClockTrimming(void);

// Now we need the corresponding interrupt IRQ
uint32_t TIM3_IC_Before;
uint32_t TIM3_IC_Now;
volatile uint32_t TIM3_Period;
uint32_t TIM3_Period_Max = 0;
uint32_t TIM3_Period_Min = 0x0000FFFF;

TIM_HandleTypeDef    TRIM_TimHandle;

void HSI48_UserTrim(void);
volatile uint8_t runtrim = 0;

void ConfigureClockTrimming(void) {
/*
  GPIO_InitTypeDef   GPIO_InitStructure;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;//GPIO_MODE_INPUT;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_14;
  GPIO_InitStructure.Alternate = 11; // 
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure); // PC14 = OSCIN32
*/
  IO_Pad_t Pad = { PC_14, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Signal = TIM3_CH2/*11*/ } };
  IO_PadInit(&Pad);
  __HAL_RCC_TIM3_CLK_ENABLE();
//  __HAL_RCC_TIM17_CLK_ENABLE();
  
/* Timer Output Compare Configuration Structure declaration */
 
  TRIM_TimHandle.Instance = TIM3;
  TRIM_TimHandle.Init.Prescaler         = 0; // cycle speed BASE_TIMER_CLOCK_MHZ-1; // SYSCLK = 48 MHz, timer will run at 1MHz
  TRIM_TimHandle.Init.Period            = 0xFFFF;//999; // counter period is 1000 steps
  TRIM_TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  TRIM_TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TRIM_TimHandle.Init.RepetitionCounter = 0;
  if (HAL_TIM_IC_Init(&TRIM_TimHandle) != HAL_OK)
    // HAL_StatusTypeDef HAL_TIM_Base_Start_IT(&TRIM_TimHandle);
  { // Initialization Error 
    TrapError();
  }

  TIM_IC_InitTypeDef sTRIMConfig = {
    
  .ICPolarity = TIM_ICPOLARITY_RISING,
  .ICSelection = TIM_ICSELECTION_DIRECTTI,
  .ICPrescaler =  TIM_ICPSC_DIV1,
  .ICFilter = 0,
  };
    
  if( HAL_TIM_IC_ConfigChannel(&TRIM_TimHandle, &sTRIMConfig, TIM_CHANNEL_2) != HAL_OK)  {    
    TrapError();
  }
  
  //HAL_TIM_Base_Start_IT() ??
  
  if (HAL_TIM_IC_Start_IT (&TRIM_TimHandle, TIM_CHANNEL_2)!= HAL_OK)    {     
    TrapError();
  }

  HAL_NVIC_DisableIRQ(TIM3_IRQn);  

    /* Enable the TIM Update interrupt */ // this is to make 32 bit counter in core cycles (48MHz here)
  __HAL_TIM_ENABLE_IT(&TRIM_TimHandle, TIM_IT_UPDATE);
  __HAL_TIM_UIFREMAP_ENABLE(&TRIM_TimHandle); // this helps making 32 bit emulated counters
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);    
  
//  HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);    
//  HAL_NVIC_DisableIRQ(SysTick_IRQn);    
  //while(1);
  
  HAL_Delay(100);
//  while(runtrim--)
//    HSI48_UserTrim(); // run trimming (32kHz external clock must be present!
  
}

uint32_t good,bad, total;

void TIM3_IRQHandler(void);
void TIM3_IRQHandler(void) {

  
  /* TIM Update event */
  if(__HAL_TIM_GET_FLAG(&TRIM_TimHandle, TIM_FLAG_UPDATE) != RESET)
  {
    if(__HAL_TIM_GET_IT_SOURCE(&TRIM_TimHandle, TIM_IT_UPDATE) != RESET)
    {
      __HAL_TIM_CLEAR_IT(&TRIM_TimHandle, TIM_IT_UPDATE);
      
      uint32_t tmp = __get_PRIMASK();
      __set_PRIMASK(tmp | 1);
      gINT_Stats.tick_msb++;
      __set_PRIMASK(tmp);
      return;
    }
  }
  
  INT_Enter(TIM3_INT); // if you place if after the UPDATE, you only measure the 32kHz
  // if we arrive here it's for the input captures
  TIM3_IC_Before = TIM3_IC_Now;
  TIM3_IC_Now = HAL_TIM_ReadCapturedValue(&TRIM_TimHandle, TIM_CHANNEL_2);//TIM3->CCR2;
//  if(TIM3_Period_Min>TIM3_Period_Max)
//    return; // this is the first valid entry, no relative calc can be done, wait for next come back...
  TIM3_Period = (uint16_t)((uint16_t) TIM3_IC_Now - (uint16_t) TIM3_IC_Before);
  TIM3_Period_Max = max2(TIM3_Period, TIM3_Period_Max);
  TIM3_Period_Min = min2(TIM3_Period, TIM3_Period_Min);
  if(   (TIM3_Period >1480)
     || (TIM3_Period <1449)  ) {
    bad++;}
  else{
    good++;
  };
    
  total = bad + good;
  
  INT_Leave(TIM3_INT); 
}
  


void HSI48_UserTrim(void) {

  // activate external 32.768kHz applied on PC14
  
  // TIM3 will be used with HSI as input clock and PC14 as Input capture
  
  // now we run the stats
  uint32_t ICSCR = RCC->ICSCR;
  int32_t TrimInit = (ICSCR >> 8) & 0x7F;
  int32_t TrimLow = max2(0, TrimInit - 10);
  int32_t TrimHigh = min2(0x7F, TrimInit + 10);
  int32_t TrimBest = -1;
  int32_t Delta;
  int32_t DeltaBest = 0x0FFFFFFF;
  int32_t PeriodTarget = 1465;
  int32_t Trim = TrimLow;
  
  while(Trim<=TrimHigh) {
    
    // set new clock trim value
    RCC->ICSCR = (ICSCR & 0x000000FF)|(Trim<<8);
    HAL_Delay(10); // wait 1 msec
    Delta = gap2(TIM3_Period, PeriodTarget);
    if(Delta<DeltaBest) {
        DeltaBest = Delta;
        TrimBest = Trim;
    }
    
    Trim++;
  };
   
  NOPs(1);
  if(TrimBest == -1) { // restore
    RCC->ICSCR = (ICSCR & 0x000000FF)|(TrimInit<<8);
  }
  else { // apply best solution
    RCC->ICSCR = (ICSCR & 0x000000FF)|(TrimBest<<8);
  }
  
}  
  
