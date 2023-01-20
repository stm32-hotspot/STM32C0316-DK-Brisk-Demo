/**
  ******************************************************************************
  * @file    brisk_reports.h
  * @author  S.Ma
  * @brief   ...
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

#ifndef _BRISK_REPORTS_H_
#define _BRISK_REPORTS_H_

#define BRISK_PANELS 1

extern u8fifo_t* pBrisk_Report;
int32_t Brisk_Report(void);

int32_t RightPaneProcess(void);
int32_t LeftPaneProcess(void);

void BTEL_SetDelayedPanel(int32_t PanelSelector, int32_t Delay_100ms);
void BTEL_SetPanel(void);

void BTEL_100ms(void);

void BTEL_SetVerticalNavigation(int32_t BriskPanes, int32_t AddOnPanes, int32_t DefaultPane, int32_t DelayedDefault_100ms); // if DefaultPane = -1 = none, Delayed... = 0 = immediate


void BTEL_NavigateDown(void);
void BTEL_NavigateUp(void);
void BTEL_ChangeAddOnPanes(int32_t Panes);
void BTEL_ChangeNbOfSlaves(int32_t Slaves);

#endif
