/**
  ******************************************************************************
  * @file    add_on_board.c (STMod+ plug and play add-on board manager)
  * @author  S.Ma
  * @brief   Top entry manager, polls at regular interval if a board is present (EEPROM ID), triggers the "plug/unplug" calls accordingly.
  *          There is a debug mode code to program new boards ID in this source using breakpoint and code manual rerouting.
  *          STM32C03 specific: Flash and RAM is very limited, we can't have the support of all add-on boards in one program (256kbyte), 
  *          so here in the H file, we decide which add-on to support (as long as the linker says there is enough memory...)
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
#include "add_on_board.h"

ADD_ON_BOARD_t STModADD_On = { &STModM24256D}; // ADD; // Manage the hot plug feature we manage only 1 add-on for most boards
void STModp_DeInit(void); // disable all pins connected to STMod+ except I2C lines
void JustPlugged(ADD_ON_BOARD_t* pA);
void JustUnPlugged(ADD_ON_BOARD_t* pA);

void Add_on_Board_Init(ADD_ON_BOARD_t* pA) {

  // these functions are called at MCU power on, not necessarily when the add-on board is connected... only for global variables initial values needs
  STModp_DeInit(); // 18/11/19
}

void STModp_DeInit(void) {// When unplugged, we shall deselect all the STMOD+ IOs except I2C.
  IO_PadDeInit(PA_0); //[1] PA0 CTS2
  IO_PadDeInit(PA_2); //[2] PA2 TX2
  IO_PadDeInit(PD_3); //[3] PA3 RX2
  IO_PadDeInit(PD_1); //[4] PA1 RTS2
  //----------- (I2C untouched)
  IO_PadDeInit(PA_7);// [8] PA7 ---
  IO_PadDeInit(PD_2);// [9] PD2
//=================  
  IO_PadDeInit(PC_14);//[11] PC14
  IO_PadDeInit(PC_15);//[12] PC15
#if 0 // used as disco user LED and user Analog Rock switch, so it may not need to be reinitialized at unplug event
  IO_PadDeInit(PA_4);// [13] PA4
  IO_PadDeInit(PA_5);// [14] PA5
#endif
  IO_PadDeInit(PB_0);// [17] PB0
  IO_PadDeInit(PA_6);// [18] PA6
  IO_PadDeInit(PD_1);// [19] PD1
  IO_PadDeInit(PD_0);// [20] PD0

}

// CALLED UNDER ISR, BE BRIEF!
void Add_on_board_LPTIM1_Tick(ADD_ON_BOARD_t* pA) { // this is called when LPTIM1 is enabled with periodic interrupt events, for example to generate the buzzer under interrupt

  if(pA->IsPlugged == 0) return;
  
  // a board is plugged
  // now we process the add-on specifics for 50 msec event
  switch(pA->ID_Code) {
//  case UNKNOWN_ADD_ON_BOARD: break; // too new add-on
//  case UNDETECTED_ADD_ON_BOARD: break;
//  case BLANK_ADD_ON_BOARD:              // same as LCD
//  case B_HMI_LCD16x2_BOARD:             break;//
//  case B_CELL_UG96_BOARD:               break;
//  case B_CELL_BG96_BOARD:               break;
//  case B_SDCARD_PMOD_BOARD:             SP_Tick(); break;
//  case B_MC_CONN34_BOARD:               MC_Tick(); break; // 7
//  case B_OPTO_INERT_BOARD:              OI_Tick(); break; // 8
//  case B_ANALOG_ENV_BOARD:              //break; // 9
//  case B_GP_ENV_BOARD:                  AE_Tick(); break; // 10
//DIS  case B_AUDIO_WM_BOARD:                AW_Tick();   break; // 11 used for piezzo buzzer half period toggle GPIO
//  case B_LSHIFT_GP_BOARD: break; // 12
//  case B_FAN_OUT2_BOARD: break;// 13
//  case B_RS485_DCDC_BOARD: break;//14
  // case STM32WL55C_DK1_BOARD: break;
  
  default: ;//TrapError();
  }

}



void Add_on_Board_50msec(ADD_ON_BOARD_t* pA) { // should be called every 100 msec

  // first, we try if AC/BC matches
  pA->pEEP->pDevice->SlaveAdr = 0xAC;
  pA->pEEP->pDeviceID->SlaveAdr = 0xBC;
  if(Is_EEP_Detected(pA->pEEP)) { // the EEPROM is I2C Acked
    pA->IsPresentCounter_50msec = min2(0xFFFFFFFE, pA->IsPresentCounter_50msec+1); // increase the match counter
    if(pA->IsPresentCounter_50msec == 3) // triple confirmed, let get the processing now
      JustPlugged(pA);
  }
  else
  { // Sticky legacy, discovered the EEPROM ID addresses are also used with HTS221 and RFID chipsets, which address is not changeable...
    pA->pEEP->pDevice->SlaveAdr = 0xAE;
    pA->pEEP->pDeviceID->SlaveAdr = 0xBE;
    if(Is_EEP_Detected(pA->pEEP)) { // the EEPROM is I2C Acked
      pA->IsPresentCounter_50msec = min2(0xFFFFFFFE, pA->IsPresentCounter_50msec+1); // increase the match counter
      if(pA->IsPresentCounter_50msec == 3) // triple confirmed, let get the processing now
        JustPlugged(pA);
    }
    else
    { // not present, a single NACK is considered unplugged.
      if(pA->IsPlugged)
        JustUnPlugged(pA);
      pA->IsPlugged = 0;    
    };
  }
  
  
  // now we process the add-on specifics for 50 msec event

  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD: break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD:         break;
  case BLANK_ADD_ON_BOARD:              // same as LCD
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             HL_50ms(); 
                                        break;//
#endif
//  case B_CELL_UG96_BOARD:               /*CU_50ms();*/ break;
//  case B_CELL_BG96_BOARD:               /*BU_50ms();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//deprecated SP_50ms(); break;
//  case B_MC_CONN34_BOARD:               break;//deprecated MC_50ms(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_50ms(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              // 9
  case B_ENV_GAS_V2_BOARD:              // 10
  case B_GP_ENV_BOARD:                  AE_50ms(); break; // 11
#endif
//  case B_AUDIO_WM_BOARD:                AW_50ms(); break; // 12
//  case B_LSHIFT_GP_BOARD:               LG_50ms(); break; // 13
#ifdef _B_FAN_OUT2_BOARD_   
  case B_FAN_OUT2_BOARD:                FO_50ms(); break; // 14
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_50ms(); break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_50ms(); break;
  default: ;//TrapError();
  }

}


void Add_on_Board_1sec(ADD_ON_BOARD_t* pA) { // should be called every 100 msec
  
  if(pA->IsPlugged == 0)
    return;

  // now we process the add-on specifics for 50 msec event
  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD:            break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD:         break;
  case BLANK_ADD_ON_BOARD:              //break; same as LCD
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             HL_1sec(); break;
#endif
//  case B_CELL_UG96_BOARD:               /*CU_1sec();*/ break;
//  case B_CELL_BG96_BOARD:               /*CB_1sec();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//deprecated SP_1sec(); break;
//  case B_MC_CONN34_BOARD:               break;//deprecated MC_1sec(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_1sec(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              // break; // 9
  case B_ENV_GAS_V2_BOARD:              //
  case B_GP_ENV_BOARD:                  AE_1sec(); break; //
#endif
//  case B_AUDIO_WM_BOARD:                AW_1sec(); break; //
//  case B_LSHIFT_GP_BOARD:               LG_1sec(); break; //
#ifdef _B_FAN_OUT2_BOARD_   
  case B_FAN_OUT2_BOARD:                FO_1sec(); break; //
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_1sec(); break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_1sec(); break;
  default: ;//TrapError();
  }

}



// we assume at least 1 NACK is enough to detect unplugg, otherwise
// we would need to read ID and check if is has changed.
void JustPlugged(ADD_ON_BOARD_t* pA) {

  uint32_t i;
  
  pA->JustPlugged = 1;
  pA->IsPlugged = 1;
  pA->JustUnplugged = 0;
  
  STModp_DeInit();  
  // Let's initialize
  EEP_ReadID(pA->pEEP); // Read the ID Page for analysis

  // under construction... add-on dependent...
  // copy the ID Page in the string ID (clip it to 16 bytes RPN 16 chars)
  //mem_cpy(sn, dp->fn, 12);
  for(i=0;i<16;i++)
    pA->StringID[i] = pA->pEEP->ID_Page[i];
  // clip to 16 chars
  pA->StringID[16] = 0; // NULL CHAR
  
  Add_on_BoardConfigure(pA); // from string to ID_Code

  // now we process the add-on specifics for 50 msec event
  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD:            break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD:         break;
  case BLANK_ADD_ON_BOARD:              //break; same as LCD
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             HL_IsPlugged();    break;//
#endif
//  case B_CELL_UG96_BOARD:               /*CU_IsPlugged();*/ break;
//  case B_CELL_BG96_BOARD:               /*BU_IsPlugged();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//deprecated SP_IsPlugged(); break;
//  case B_MC_CONN34_BOARD:               break;//deprecated MC_IsPlugged(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_IsPlugged(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              AE_IsPlugged(0);break; // 9 with ADS
  case B_GP_ENV_BOARD:                  AE_IsPlugged(1); break; // 10 without ADS
  case B_ENV_GAS_V2_BOARD:              AE_IsPlugged(2); break; // 11
#endif
//  case B_AUDIO_WM_BOARD:                AW_IsPlugged(); break; // 12+
//  case B_LSHIFT_GP_BOARD:               LG_IsPlugged(); break; // 13+
#ifdef _B_FAN_OUT2_BOARD_   
  case B_FAN_OUT2_BOARD:                FO_IsPlugged(); break; // 14+
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_IsPlugged(); break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_IsPlugged(); break;
  default: break;//TrapError();
  };

}

void JustUnPlugged(ADD_ON_BOARD_t* pA) {

    pA->JustUnplugged = 1; // was plugged...    
    pA->IsPresentCounter_50msec = 0;
    
    // we can reinitialize all the GPIOs connected to STMod+ connector
    // and we can also notify (callback) in future implementation
  // now we process the add-on specifics for 50 msec event

  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD: break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD: break;
  case BLANK_ADD_ON_BOARD:              //break; same as LCD
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             HL_IsUnplugged();    break;//
#endif
//  case B_CELL_UG96_BOARD:               /*CU_IsUnplugged();*/ break;
//  case B_CELL_BG96_BOARD:               /*BU_IsUnplugged();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//deprecated SP_IsUnplugged(); break;
//  case B_MC_CONN34_BOARD:               break;//deprecated MC_IsUnplugged(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_IsUnplugged(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              //break; // 9
  case B_ENV_GAS_V2_BOARD:
  case B_GP_ENV_BOARD:                  AE_IsUnplugged(); break; // 10
#endif
//  case B_AUDIO_WM_BOARD:                AW_IsUnplugged(); break; // 11
//  case B_LSHIFT_GP_BOARD:               LG_IsUnplugged(); break; // 12
#ifdef _B_FAN_OUT2_BOARD_   
  case B_FAN_OUT2_BOARD:                FO_IsUnplugged(); break; // 13
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_IsUnplugged(); break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_IsUnplugged(); break;
  default: ;//TrapError();
  }

  pA->ID_Code = UNDETECTED_ADD_ON_BOARD;
  STModp_DeInit();
// valid for all  
//DIS  pBTEL->PanelSelectorMax = BRISK_PANELS;
//DIS  BTEL_SetDelayedPanel(0, 5);
//  pBTEL->PanelSelector = 0;
//  DelayedSetPanelCountdown_100ms = 5;
}

//================================== Now we declared all possible add-on boards (to be rearchitectured later

const char BlankStringID[] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";

volatile uint8_t debug_options = 0; // don't touch this

uint8_t Add_on_BoardConfigure(ADD_ON_BOARD_t* pA) {
  
  if(StringsIdenticals (pA->StringID, BlankStringID, 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = BLANK_ADD_ON_BOARD;
    // blank ones will be assumed to be LCD
    //pA->ID_Code = B_HMI_LCD16x2_BOARD; //  Put a breakpoint here if you want to program the add-on blank EEPROM. Use debugger "Set Next Step" to directly execute the right case xxx : code. Then reset.
    switch(debug_options) {
#ifdef _B_HMI_LCD16x2_H_
    case B_HMI_LCD16x2_BOARD: // B-HMI-LCD16x2
          EEP_SetID_Page(pA->pEEP, "B-HMI-LCD16x2"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
#endif
/*    case B_SDCARD_PMOD_BOARD: // B-SDCARD-PMOD
          EEP_SetID_Page(pA->pEEP, "B-SDCARD-PMOD"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
    case B_CELL_UG96_BOARD: // 
          EEP_SetID_Page(pA->pEEP, "B-CELL-UG96"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
    case B_CELL_BG96_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B-CELL-BG96"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
*/    //-- add-on boards (ST Internal)
#ifdef _B_OPTO_INERT_BOARD_
    case B_OPTO_INERT_BOARD: // 
          EEP_SetID_Page(pA->pEEP, "B_OPTO_INERT"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
#endif
/*    case B_ANALOG_ENV_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B_ANALOG_ENV"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
    case B_AUDIO_WM_BOARD: // 
          EEP_SetID_Page(pA->pEEP, "B_AUDIO_WM"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
    case B_LSHIFT_GP_BOARD: // 
          EEP_SetID_Page(pA->pEEP, "B_LSHIFT_GP"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
    case B_MC_CONN34_BOARD: // 
          EEP_SetID_Page(pA->pEEP, "B_MC_CONN34"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
    case B_GP_ENV_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B_GP_ENV"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;*/
#ifdef _B_ANALOG_ENV_BOARD_
    case B_ENV_GAS_V2_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B_ENV_GAS_V2"); // set the string
          EEP_WriteID(pA->pEEP); // program the string
    break;
#endif
#ifdef _B_FAN_OUT2_BOARD_    
    case B_FAN_OUT2_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B_FAN_OUT2");
          EEP_WriteID(pA->pEEP);
          break;
#endif          
/*    case B_WIFI_EMW3080_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B_WIFI_EMW3080");
          EEP_WriteID(pA->pEEP);
    break;*/
    case B_RS485_DCDC_BOARD: //
          EEP_SetID_Page(pA->pEEP, "B_RS485_DCDC");
          EEP_WriteID(pA->pEEP);
    break;
/*    case STM32WL55C_DK1_BOARD: //
        EEP_SetID_Page(pA->pEEP, "STM32WL55C-DK1");
        EEP_WriteID(pA->pEEP);
    break;
*/
    }
    
    return 0;
  }
#ifdef _B_HMI_LCD16x2_H_
  if(StringsIdenticals (pA->StringID, "B-HMI-LCD16x2", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_HMI_LCD16x2_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
#endif
/*  if(StringsIdenticals (pA->StringID, "B-SDCARD-PMOD", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_SDCARD_PMOD_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
  
  if(StringsIdenticals (pA->StringID, "B-CELL-BG96", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_CELL_BG96_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }

  if(StringsIdenticals (pA->StringID, "B-CELL-UG96", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_CELL_UG96_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
*/
#ifdef _B_OPTO_INERT_BOARD_
  if(StringsIdenticals (pA->StringID, "B_OPTO_INERT", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_OPTO_INERT_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
#endif
#ifdef _B_ANALOG_ENV_BOARD_
/*
  if(StringsIdenticals (pA->StringID, "B_ANALOG_ENV", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_ANALOG_ENV_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }

  if(StringsIdenticals (pA->StringID, "B_GP_ENV", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_GP_ENV_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
*/
  if(StringsIdenticals (pA->StringID, "B_ENV_GAS_V2", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_ENV_GAS_V2_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
#endif
/*
  if(StringsIdenticals (pA->StringID, "B_AUDIO_WM", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_AUDIO_WM_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
  
  if(StringsIdenticals (pA->StringID, "B_LSHIFT_GP", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_LSHIFT_GP_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }

  if(StringsIdenticals (pA->StringID, "B_MC_CONN34", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_MC_CONN34_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
*/
#ifdef _B_FAN_OUT2_BOARD_  
  if(StringsIdenticals (pA->StringID, "B_FAN_OUT2", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_FAN_OUT2_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
#endif  
/*
  if(StringsIdenticals (pA->StringID, "B_WIFI_EMW3080", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_WIFI_EMW3080_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }
    */
  if(StringsIdenticals (pA->StringID, "B_RS485_DCDC", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = B_RS485_DCDC_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }  
/*
  if(StringsIdenticals (pA->StringID, "STM32WL55C-DK1", 16)) { // BLANK_ADD_ON_DETECTED
    pA->ID_Code = STM32WL55C_DK1_BOARD;
    BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
    return 1;
  }    
*/
  pA->ID_Code = UNKNOWN_ADD_ON_BOARD;
  BriskBlinkTimedLED(1, 1000, 500, pA->ID_Code ); // on all the time
  return 0;
}

// B_HMI_LCD16x2_t B_HMI_LCD16x2 with StringID = B-HMI-LCD16x2 (or FFFFFFFF)

//=== bluetooth electronics reporting specific functions for the add-on board, if any
int32_t Add_on_BoardReport(ADD_ON_BOARD_t* pA) {
 
  if(pA->IsPlugged == 0) return -1; // nothing to report, there is no add-on board

  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD:            break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD:         break;
  case BLANK_ADD_ON_BOARD:              break; 
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             HL_Report(); break;
#endif
//  case B_CELL_UG96_BOARD:               /*CU_1sec();*/ break;
//  case B_CELL_BG96_BOARD:               /*CB_1sec();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//deprecated SP_Report(); break;
//  case B_MC_CONN34_BOARD:               break;//deprecated MC_Report(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_Report(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              // break; // 9
  case B_ENV_GAS_V2_BOARD:
  case B_GP_ENV_BOARD:                  AE_Report(); break; // 10
#endif
//  case B_AUDIO_WM_BOARD:                AW_Report(); break; // 11
//  case B_LSHIFT_GP_BOARD:               LG_Report(); break; // 12
#ifdef _B_FAN_OUT2_BOARD_   
  case B_FAN_OUT2_BOARD:                FO_Report(); break;
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_Report(); break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_Report(); break;
  default: ;//TrapError();
  }

  return 0;
}

int32_t AddOnPaneProcess(ADD_ON_BOARD_t* pA) {
 
  if(pA->IsPlugged == 0) return -1; // nothing to report, there is no add-on board

  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD:            break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD:         break;
  case BLANK_ADD_ON_BOARD:              break;
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             break;//HL_PaneProcess(); break;
#endif
//  case B_CELL_UG96_BOARD:               /*CU_1sec();*/ break;
//  case B_CELL_BG96_BOARD:               /*CB_1sec();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//SP_PaneProcess(); break;
//  case B_MC_CONN34_BOARD:               break;//MC_PaneProcess(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_PaneProcess(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              //break; // 9
  case B_ENV_GAS_V2_BOARD:
  case B_GP_ENV_BOARD:                  AE_PaneProcess(); break; // 10
#endif
//  case B_AUDIO_WM_BOARD:                AW_PaneProcess(); break; // 11
//  case B_LSHIFT_GP_BOARD:               LG_PaneProcess(); break; // 12
#ifdef _B_FAN_OUT2_BOARD_   
  case B_FAN_OUT2_BOARD:                FO_PaneProcess(); break;
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_PaneProcess();break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_PaneProcess();break;
  default: ;//TrapError();
  }

  return 0;
}

int32_t Add_on_Board_SetPanel(ADD_ON_BOARD_t* pA) {

  if(pA->IsPlugged == 0) return -1; // nothing to report, there is no add-on board

  switch(pA->ID_Code) {
  case UNKNOWN_ADD_ON_BOARD:            break; // too new add-on
  case UNDETECTED_ADD_ON_BOARD:         break;
  case BLANK_ADD_ON_BOARD:              break;
#ifdef _B_HMI_LCD16x2_H_
  case B_HMI_LCD16x2_BOARD:             break;//HL_PaneProcess(); break;
#endif
//  case B_CELL_UG96_BOARD:               /*CU_1sec();*/ break;
//  case B_CELL_BG96_BOARD:               /*CB_1sec();*/ break;
//  case B_SDCARD_PMOD_BOARD:             break;//SP_PaneProcess(); break;
//  case B_MC_CONN34_BOARD:               break;//MC_PaneProcess(); break; // 7
#ifdef _B_OPTO_INERT_BOARD_
  case B_OPTO_INERT_BOARD:              OI_SetPanel(); break; // 8
#endif
#ifdef _B_ANALOG_ENV_BOARD_
  case B_ANALOG_ENV_BOARD:              ///break; // 9
  case B_ENV_GAS_V2_BOARD:
  case B_GP_ENV_BOARD:                  AE_SetPanel(); break; // 10
#endif
//  case B_AUDIO_WM_BOARD:                AW_SetPanel(); break; // 11
//  case B_LSHIFT_GP_BOARD:               LG_SetPanel(); break; // 12
#ifdef _B_FAN_OUT2_BOARD_  
  case B_FAN_OUT2_BOARD:                FO_SetPanel(); break;
#endif  
//  case B_WIFI_EMW3080_BOARD:            break;
#ifdef _B_RS485_DCDC_BOARD_  
  case B_RS485_DCDC_BOARD:              RS_SetPanel(); break;
#endif  
//  case STM32WL55C_DK1_BOARD:            WL_SetPanel(); break;
  default: ;//TrapError();
  }

  return 0;
}
