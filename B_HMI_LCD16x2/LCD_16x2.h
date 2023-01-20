#ifndef _LCD_16x2_H_
#define _LCD_16x2_H_

/**
  ******************************************************************************
  * @file    LCD_16x2.h (Character based 2x16 matrix display, low memory footprint MCU compatible)
  * @author  S.Ma
  * @brief   Use local memory to draw the 2 lines of text, then flush and push the frame to the LCD screen. Avoid some artifacts when updated too fast. LCD pseudo Printf function provided.
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

#include "io_pins.h"    
    
#define LCD_NB_COL_X	18
#define LCD_NB_ROW_Y	2
// NHD-C0216CZ-NSW-BBW-3V3 www.digikey.com (blue with backlight)
// NHD-C0216CZ-FSW-FBW-3V3-ND www.digikey.com (tranflextive, optional backlight)

typedef struct {
  IO_Pad_t* RST; // 
  IO_Pad_t* RS; // 
  IO_Pad_t* CS; // 
  IO_Pad_t* SCL; // 
  IO_Pad_t* SI; // 
  IO_Pad_t* BL; // 

  IO_Pad_t* RSToff; // 
  IO_Pad_t* RSoff; // 
  IO_Pad_t* CSoff; // 
  IO_Pad_t* SCLoff; // 
  IO_Pad_t* SIoff; // 

  uint8_t Cursor_X, Cursor_Y;
  uint8_t Cursor_X0, Cursor_Y0;

  uint8_t RAM[LCD_NB_COL_X][LCD_NB_ROW_Y];
} LCD_16x2_t;

// initialize the display
void LCD_16x2_Init(LCD_16x2_t* pL);

// manipulate the display through the RAM buffer
void LCD_16x2_RAM_GOTO_X_Y(LCD_16x2_t* pL, uint8_t col_x, uint8_t row_y);
void LCD_16x2_FillRAM(LCD_16x2_t* pL, uint8_t c);
void LCD_16x2_ClearScreen_RAM(LCD_16x2_t* pL);
void LCD_16x2_Writedata_to_RAM(LCD_16x2_t* pL, uint8_t d);
void LCD_16x2_GOTO_X_Y(LCD_16x2_t* pL, uint8_t col_x, uint8_t row_y);
void LCD_16x2_RAM_GOTO_Next_Line(LCD_16x2_t* pL);
void LCD_16x2_RAM_GOTO_HorizontalCenter(LCD_16x2_t* pL);
void LCD_16x2_putint_to_RAM(LCD_16x2_t* pL, uint32_t i);

// finally update really the display from RAM buffer to HW LCD 
void LCD_16x2_RAM_to_Display(LCD_16x2_t* pL);
void LCD_16x2_ShowTemporaryMessage_0(LCD_16x2_t* pL, uint16_t time_ms, uint8_t* string );
void LCD_16x2_ShowTemporaryMessage_1(LCD_16x2_t* pL, uint16_t time_ms, uint8_t* string, uint32_t param_1);
void LCD_16x2_ShowTemporaryMessage_2(LCD_16x2_t* pL, uint16_t time_ms, uint8_t* string, uint32_t param_1, uint32_t param_2);
void LCD_16x2_Printf(LCD_16x2_t* pL, const char *str,...);

void LCD_16x2_Test(void);

#endif