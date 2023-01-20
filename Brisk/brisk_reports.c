/**
  ******************************************************************************
  * @file    brisk_reports.c (bluetooth electronics android app, STM32 Implementation side of it)
  * @author  S.Ma
  * @brief   Manage the app main dashboard and navigation through the sub pages (up and down arrows)
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

//============== reporting
int32_t BTEL_LeftPaneReport(u8fifo_t* pBrisk_Report);
int32_t BTEL_MasterRightPaneReport(u8fifo_t* pBrisk_Report);
u8fifo_t* pBrisk_Report = &u8fifo_from_SPIP[BT_STL_SERIAL];
uint32_t BR_SubSample = 0;
int32_t Brisk_Report(void) {
  
//return 0; // report disabled for programming HC-06 baud rate  
  
// for debug purpose
//BTEL_Printf(pBrisk_Report,"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,");
//return 0;

    ADC_UpdateConveredValues(&Analog.ADCs); // update ADC values in mV
    BR_SubSample = (BR_SubSample+1)%3;
    if(BR_SubSample==0)
      BTEL_LeftPaneReport(pBrisk_Report); // update the left side of the panel (common to all)
    
    if(BTEL.PanelSelector >= BRISK_PANELS) { // pages
      Add_on_BoardReport(&STModADD_On); // add-on board report through the serial interface
//    return 0;
    }else{
      BTEL_MasterRightPaneReport(pBrisk_Report); // main frame
    }
  
  return 0;
}



int32_t BTEL_LeftPaneReport(u8fifo_t* pBrisk_Report) {

  // main frame drawing
  BTEL_Printf(pBrisk_Report,"*1Demo-1");
  BTEL_Printf(pBrisk_Report,"*2");
  if(         (STModADD_On.IsPlugged) ) {
    STModADD_On.StringID[16] = 0; // stop at 16th char (crude here)
    BTEL_Printf(pBrisk_Report,STModADD_On.StringID); // hope the string exist!
  }
  else
    BTEL_Printf(pBrisk_Report,"No add-on");
  
    BTEL_Printf(pBrisk_Report, "*3STM32C0316-DK", 0);
  
  BTEL_Printf(pBrisk_Report, "*4 %d of %d*", BTEL.PanelSelector, BTEL.PanelSelectorMax);
  
  return 0;
}

int32_t BTEL_MasterRightPaneReport(u8fifo_t* pBrisk_Report) {
  
  // gravity accelerator vector (disabled)
  //BTEL_Printf(pBrisk_Report,"*G%d", (int32_t)() ); //
  //BTEL_Printf(pBrisk_Report,"*g%C g", (int32_t)(Motion3D.gravity_x100) ); //
  // roll and pitch
  BTEL_Printf(pBrisk_Report,"*A%D,%D", (int32_t)(Analog.ADCs.Normal_mV[2]/100), (int32_t)(Analog.ADCs.Normal_mV[3]/100) ); //
  // voltages as text
  BTEL_Printf(pBrisk_Report,"*V%C V", (int32_t)(Analog.ADCs.MeasuredVdd_mV/10) ); //
  //BTEL_Printf(pBrisk_Report,"*B%C V", (int32_t)(Analog.ADCs.MeasuredVBat_mV/10) ); //
  // temperature
  BTEL_Printf(pBrisk_Report,"*T%d", (int32_t)(Analog.ADCs.Temp_degC_x10/10) ); //
  BTEL_Printf(pBrisk_Report,"*t%D C", (int32_t)(Analog.ADCs.Temp_degC_x10) ); //
  // console log
  BTEL_Printf(pBrisk_Report, "*MTimestamp: %d\n", HAL_GetTick());
    
  return 0;
};

// BTEL dashboards and pages layout to be pushed to the phone app
const char* BTEL_MasterPanel0 =      
"*.kwl\n\
clear_panel()\n\
set_grid_size(13,7)\n\
add_4way_pad(0,4,5,6,7,8,,0,,)\n\
add_text_box(11,5,2,L,,245,240,245,t)\n\
add_text_box(0,0,3,C,,245,240,245,1)\n\
add_text_box(0,3,3,C,,245,240,245,4)\n\
add_text_box(9,5,2,C,,245,240,245,g)\n\
add_text_box(9,6,2,C,,245,240,245,V)\n\
add_text_box(11,6,2,C,,245,240,245,B)\n\
add_text(0,1,medium,L,,245,240,245,2)\n\
add_text(0,2,medium,L,,245,240,245,3)\n\
add_switch(9,0,4,D,d,0,0)\n\
add_slider(9,2,5,0,100,0,*X,*,0)\n\
add_led(11,2,2,L,129,128,126)\n\
add_send_box(8,4,5,Hello!,*T,*)\n\
add_roll_graph(3,0,6,0.0,4.0,100,A,ADC PA4 PA5,time,mV,0,0,1,1,1,1,medium,none,2,PA4,42,97,222,PA5,237,115,7)\n\
add_monitor(3,4,5,M,3)\n\
run()\n\
* \n";
//add_text(1,5,medium,L,t,245,240,245,9)\n\

extern const char* BTEL_MasterPanel0;
extern const char* BTEL_LG_Panel1;
extern const char* BTEL_LG_Panel2;
extern const char* BTEL_LG_Panel3;

int32_t LeftPaneProcess(void) {

  char c = BTEL.incoming[BTEL.in_ndx];
  
  switch(c) { // the common panel commands      
    // Up / Down
  case '5': // up : processed by selected device
    BTEL_SetDelayedPanel(BTEL.PanelSelector-1, 2);
    break;// up
  case '7': // down
    BTEL_SetDelayedPanel(BTEL.PanelSelector+1,2);
    break;// down
    // <= =>
  case '6': // next right device (something happen only for master control of serial interfaces pipes
    //BTEL.PanelSelector = 0; // reset to panel 0
      BTEL.DeviceSelector = 0;
      BTEL_SelectDevice(); // configure XBAR_SIF with range check
      BTEL_SetDelayedPanel(0,2);
      //DelayedSetPanelCountdown_100ms = 2;
    break;// right
  case '8': // LEFT: Device on the left // only processed by master
    //BTEL.PanelSelector = 0; // reset to panel 0    
      BTEL_SetDelayedPanel(0,5);
    break;// left
    // test pannel uploading
  default:
    NOPs(1); // breakpoint
    return 0;
  }
  
  return 0;
}
         
int32_t LED_SliderValue = 0;

int32_t RightPaneProcess(void) {

	int32_t i;

  // this one is panel number dependent
  // for now, brisk uses panel 0 only 1+ for add-on boards
  if(pBTEL->PanelSelector>=BRISK_PANELS) {
    AddOnPaneProcess(&STModADD_On);
    NOPs(1);
    return 0;
  }
  
  // Brisk pane has no interaction as of today
  // now we can interact..
  char c = BTEL.incoming[0];//BTEL.in_ndx];

  if((STModADD_On.IsPlugged) ) { // if an add-on board is plugged, there is not control available.
    return 0;
  }
  
  switch(c){
  case 'D': // power on push // control the Dimming LED PWM of C0
    BriskTimedLED_Disable();
	  NOPs(1);
	  break;
  case 'd': // power off push // disable the dimming LED PWM of C0
    BriskTimedLED_Enable();
	  NOPs(1);
	  break;
  case 'X': // rotating knob
      if(StringToFrac(&BTEL.incoming[1], &LED_SliderValue)<0) return 0;
      if((LED_SliderValue<0)||(LED_SliderValue>100)) return 0; // out of range
      // adjust the LED dimming as percentage
      BriskDimLED(1, LED_SliderValue);
//      BriskBlinkTimedLED(1, 1000, 500, BRISK_LED_BLINK_FOREVER );
	  break;
  case 'T': // send box: send the string directly to the other usart (arduino/stmod+)
	  for(i=1;i<32;i++) {
		  c = BTEL.incoming[i];
		  AddTou8fifo(&u8fifo_from_SPIP[ARD_STM_SERIAL],c);
		  if(c==0) break; // null char sent too
	  }
	  break;
  default: NOPs(1);

  }
  return 0;
}

// Initiate countdown to resent to the mobile app the page layout
void BTEL_SetDelayedPanel(int32_t PanelSelector, int32_t Delay_100ms) {
  
  BTEL.Delayed_PanelCountdown_100ms = Delay_100ms;
  BTEL.Delayed_PanelSelector = min2(PanelSelector, BTEL.PanelSelectorMax); // max limit
  BTEL.Delayed_PanelSelector = max2(PanelSelector, 0); // min limit
    
  if(BTEL.Delayed_PanelCountdown_100ms==0) BTEL_SetPanel(); // if no delay, draw immediately
/*  
  DelayedSetPanelCountdown_100ms = 10;
  if((PanelSelector==0)&&(BTEL.PanelSelector)) //if moving back to first panel
    NOPs(1);
  BTEL.PanelSelector = PanelSelector;
*/
}

void BTEL_100ms(void) { // implement the countdown to push the layout to the phone

    if(BTEL.Delayed_PanelCountdown_100ms) {
      BTEL.Delayed_PanelCountdown_100ms--;
      if((BTEL.Delayed_PanelCountdown_100ms==0)/*&&(SPIP.Device==IS_MASTER)*/) {
        BTEL_SetPanel();
      }
    }
    else
    {
/*    BTEL.SubSampleReporting_100ms = (BTEL.SubSampleReporting_100ms+1)%2;
      if(BTEL.SubSampleReporting_100ms==0)*/ 
        Brisk_Report();        
  }
  
}

void BTEL_SetPanel(void) { // draw the page content
  // send the panel layout
  BTEL.Delayed_PanelCountdown_100ms = 0; // stop any delayed panel pop up
  BTEL.PanelSelector = min2(BTEL.Delayed_PanelSelector,BTEL.PanelSelectorMax); // back in valid range
  if(BTEL.PanelSelector==0)
    TransmitPanel(pBrisk_Report, (char*)BTEL_MasterPanel0);
  else
    Add_on_Board_SetPanel(&STModADD_On);
}

void BTEL_SetVerticalNavigation(int32_t BriskPanes, int32_t AddOnPanes, int32_t DefaultPane, int32_t DelayedDefault_100ms) {

}

void BTEL_NavigateDown(void) {
  
}

void BTEL_NavigateUp(void) {
  
}
  
void BTEL_ChangeAddOnPanes(int32_t Panes) {
  
}

void BTEL_ChangeNbOfSlaves(int32_t Slaves) {
  
}
