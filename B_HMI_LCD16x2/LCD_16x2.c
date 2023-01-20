/**
  ******************************************************************************
  * @file    LCD_16x2.c (2 rows of 16 characters LCD display drivers using SPI SW emulation lower layer)
  * @author  S.Ma
  * @brief   Low memory footprint, STM32 RAM Frame to build the screen, then push it to the screen. Artifacts minimized. Check the application code using this display to find many use/case examples.
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
#include "lcd_16x2.h"

#ifdef _B_HMI_LCD16x2_H_

#define DELAY_BASE      1 // for the NOPs

// LCD related functions
static void LCD_16x2_sendByte(LCD_16x2_t* pL, uint8_t byte);
static void LCD_16x2_Writecom(LCD_16x2_t* pL, uint8_t d);
static void LCD_16x2_Writedata(LCD_16x2_t* pL, uint8_t d);
static void LCD_16x2_Clear(LCD_16x2_t* pL);

// patch for contrast loss depending on push pull output
static uint8_t LCD_16x2_ActivateSignals(LCD_16x2_t* pL);
static uint8_t LCD_16x2_DeactivateSignals(LCD_16x2_t* pL);


static uint8_t LCD_16x2_ActivateSignals(LCD_16x2_t* pL) {
  
    // configure RST pin
  IO_PadInit(pL->RST);
//  IO_PadSetHigh(pL->RST->PadName);

  // configure RS pin
  IO_PadInit(pL->RS);
//  IO_PadSetHigh(pL->RS->PadName);

  // configure CS pin
  IO_PadInit(pL->CS);
//  IO_PadSetHigh(pL->CS->PadName);

  // configure SCL pin
  IO_PadInit(pL->SCL);
//  IO_PadSetHigh(pL->SCL->PadName);

  // configure SI pin
  IO_PadInit(pL->SI);
//  IO_PadSetHigh(pL->SI->PadName);
  
  return 1;
}

static uint8_t LCD_16x2_DeactivateSignals(LCD_16x2_t* pL) {
  
      // configure RST pin
  IO_PadInit(pL->RSToff);
//  IO_PadSetHigh(pL->RSToff->PadName);

  // configure RS pin
  IO_PadInit(pL->RSoff);
//  IO_PadSetHigh(pL->RSoff->PadName);

  // configure CS pin
  IO_PadInit(pL->CSoff);
//  IO_PadSetHigh(pL->CSoff->PadName);

  // configure SCL pin
  IO_PadInit(pL->SCLoff);
//  IO_PadSetHigh(pL->SCL->PadName);

  // configure SI pin
  IO_PadInit(pL->SIoff);
//  IO_PadSetHigh(pL->SIoff->PadName);

  return 1;  
}


#define LCD_16x2_RST_ON   IO_PadSetHigh(pL->RST->PadName)
#define LCD_16x2_RST_Off 	IO_PadSetLow(pL->RST->PadName)

#define LCD_16x2_RS_ON 	  IO_PadSetHigh(pL->RS->PadName)
#define LCD_16x2_CS_ON 	  IO_PadSetHigh(pL->CS->PadName)
#define LCD_16x2_SCL_ON 	IO_PadSetHigh(pL->SCL->PadName)
#define LCD_16x2_SI_ON 	  IO_PadSetHigh(pL->SI->PadName)

#define LCD_16x2_RS_Off	 	IO_PadSetLow(pL->RS->PadName)
#define LCD_16x2_CS_Off 	IO_PadSetLow(pL->CS->PadName)
#define LCD_16x2_SCL_Off 	IO_PadSetLow(pL->SCL->PadName)
#define LCD_16x2_SI_Off 	IO_PadSetLow(pL->SI->PadName)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERY LOW LEVEL INTERFACE PHYSICAL LAYER
// we don't need a HW SPI for few bytes communication, keep the HW SPI for other things.

// SEND A BYTE (8 bit push)
void LCD_16x2_sendByte(LCD_16x2_t* pL, uint8_t byte)
{
	uint8_t i;
	for(i=0x80;i!=0;i>>=1)
	{
		if(i & byte)
			LCD_16x2_SI_ON;
		else
			LCD_16x2_SI_Off;
			
		LCD_16x2_SCL_Off;
		NOPs(DELAY_BASE);
		LCD_16x2_SCL_ON;
		NOPs(DELAY_BASE);
//		LCD_16x2_SCL_Off;
//		NOPs(DELAY_BASE);
	}
}

// SEND A COMMAND BYTE TO THE LCD
void LCD_16x2_Writecom(LCD_16x2_t* pL, uint8_t d)
{
	LCD_16x2_CS_Off; 	//CS chip select
	NOPs(5*DELAY_BASE);
	LCD_16x2_RS_Off; 	//A0 = Command
	NOPs(5*DELAY_BASE);
	LCD_16x2_sendByte(pL, d);    
	NOPs(5*DELAY_BASE);					
	LCD_16x2_CS_ON;
	NOPs(5*DELAY_BASE);	
}

// SEND A DATA BYTE TO THE LCD
void LCD_16x2_Writedata(LCD_16x2_t* pL, uint8_t d)
{
	if(pL->Cursor_X >= LCD_NB_COL_X)
		return;
	if(pL->Cursor_Y >= LCD_NB_ROW_Y)
		return;
		
	// displayable characters
	LCD_16x2_CS_Off; 	//CS
	LCD_16x2_RS_ON; 		//A0 = Data
	NOPs(5*DELAY_BASE);
	LCD_16x2_sendByte(pL, d);
	NOPs(5*DELAY_BASE);				
	LCD_16x2_CS_ON;
	NOPs(5*DELAY_BASE);	
	
	pL->Cursor_X = min2(pL->Cursor_X+1,LCD_NB_COL_X); // increase with clipping
}


/*Position Cursor y=0-1, x=0-16  or y=0, x=0-31  */
// this function should not be used by user
void LCD_16x2_GOTO_X_Y(LCD_16x2_t* pL, uint8_t col_x, uint8_t row_y)
{
	if(row_y>=LCD_NB_ROW_Y)
		return;
	
	if(col_x>=LCD_NB_COL_X)
		return;

	LCD_16x2_Writecom(pL, 0x80 + row_y*40 + col_x);  //set correct position
	pL->Cursor_X0 = pL->Cursor_X = col_x;
	pL->Cursor_Y0 = pL->Cursor_Y = row_y;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DISPLAY RAM BUFFER TO AVOID FLICKERING WHEN UPDATING...
// related functions for it and should be used by end user

void LCD_16x2_RAM_GOTO_X_Y(LCD_16x2_t* pL, uint8_t col_x, uint8_t row_y)
{
	if(row_y>=LCD_NB_ROW_Y)
		return;
	
	if(col_x>=LCD_NB_COL_X)
		return;

	pL->Cursor_X0 = pL->Cursor_X = col_x;
	pL->Cursor_Y0 = pL->Cursor_Y = row_y;
}


void LCD_16x2_FillRAM(LCD_16x2_t* pL, uint8_t c)
{
	uint8_t x,y;
	for(y=0;y<LCD_NB_ROW_Y;y++)
		for(x=0;x<LCD_NB_COL_X;x++)
			pL->RAM[x][y] = c;
}

// to avoid the display to be erased fully before being updated, we use a RAM buffer, which will avoid the flickering
void LCD_16x2_RAM_to_Display(LCD_16x2_t* pL)
{
	uint8_t x,y;
        
LCD_16x2_ActivateSignals(pL);        // patch
	for(y=0;y<LCD_NB_ROW_Y;y++)
	{
		LCD_16x2_GOTO_X_Y(pL, 0,y);
		for(x=0;x<LCD_NB_COL_X;x++)
			LCD_16x2_Writedata(pL, pL->RAM[x][y]);
	}
LCD_16x2_DeactivateSignals(pL);        // patch
	
}

void LCD_16x2_ClearScreen_RAM(LCD_16x2_t* pL)
{
	LCD_16x2_FillRAM(pL, ' ');
//	LCD_16x2_RAM_to_Display(pL);
	LCD_16x2_RAM_GOTO_X_Y(pL, 0,0);
}

// SEND A DATA BYTE TO THE LCD
void LCD_16x2_Writedata_to_RAM(LCD_16x2_t* pL, uint8_t d)
{
	if(pL->Cursor_X >= LCD_NB_COL_X)
		return;
	if(pL->Cursor_Y >= LCD_NB_ROW_Y)
		return;
		
	// displayable characters
	pL->RAM[pL->Cursor_X][pL->Cursor_Y] = d;
	pL->Cursor_X = min2(pL->Cursor_X+1,LCD_NB_COL_X); // increase with clipping
}

#define LCD_16x2_WAKE_UP                          0x30 //DATA interface 8BIT, instruction set=0
#define LCD_16x2_CLEAR_DISPLAY                    0x01 //clear display          =>1msec
#define LCD_16x2_RETURN_HOME   			  0x02 //return cursor home     => 1msec
#define LCD_16x2_ENTRY_MODE    			  0x06 //set entry mode         => 26us
#define LCD_16x2_DISPLAY_ON_CUR_OFF    		  0x0C //display on, cursor off
#define LCD_16x2_DISPLAY_ON_CUR_ON     		  0x0E //display on, cursor on
#define LCD_16x2_FUNC_SET_DATA8_LINE2_DHEIGHT_IS  0x3D //data 8 bit, 2 line, dheight font, instruction set=1
#define LCD_16x2_FUNC_SET_DATA4_LINE2_DHEIGHT_IS  0x2D //data 4 bit, 2 line, dheight font, instruction set=1
#define LCD_16x2_FUNC_SET_DATA8_LINE1_DHEIGHT_IS  0x35 //data 8 bit, 1 line, dheight font, instruction set=1
#define LCD_16x2_FUNC_SET_DATA4_LINE1_DHEIGHT_IS  0x25 //data 4 bit, 1 line, dheight font, instruction set=1
#define LCD_16x2_FUNC_SET_DATA8_LINE2_NDHEIGHT_IS 0x39 //data 8 bit, 2 line, dheight font, instruction set=1
#define LCD_16x2_INT_OSC 			  0x14 //internal osc frequency
#define LCD_16x2_POWER_CONTROL 			  0x56 //power control
#define LCD_16x2_FOLLOWER_CONTROL 		  0x6D //follower control
#define LCD_16x2_CONTRAST_SET_low 		  0x73 //contrast SET + DATA CONTRAST (LAST 4 DIGIT) //70
#define LCD_16x2_CONTRAST_SET_high 		  0x7F //contrast SET + DATA CONTRAST (LAST 4 DIGIT) //70
#define LCD_16x2_CONTRAST_SET_Default 		  0x70 //contrast SET + DATA CONTRAST (LAST 4 DIGIT) //70

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Commands
// clears the display. Equivalent to printing spaces over the screen area
// this function should not be used by user
void LCD_16x2_Clear(LCD_16x2_t* pL)
{
	LCD_16x2_Writecom(pL, LCD_16x2_CLEAR_DISPLAY);
	HAL_Delay(10);                        //to be test correct delay
}


//initialize the LCD NHD-C0216CZ-NSW-BBW-3V3
void LCD_16x2_Init(LCD_16x2_t* pL)
{
  uint16_t i;
  LCD_16x2_ActivateSignals(pL);

  // configure BL pin
  IO_PadInit(pL->BL);
//  IO_PinSetHigh(pL->BL); // this may drop supply, put delay when turn on
    IO_PadSetLow(pL->BL->PadName);

  HAL_Delay(100);
//	MCU_LCD_16x2_Init();

	pL->Cursor_X = pL->Cursor_Y = 0;
        
	LCD_16x2_RST_Off; 							//RESET
	HAL_Delay(40);
	LCD_16x2_RST_ON; 							//end reset
	HAL_Delay(40);

	LCD_16x2_Writecom(pL, LCD_16x2_WAKE_UP); 				//wake up
	HAL_Delay(2);
	LCD_16x2_Writecom(pL, LCD_16x2_WAKE_UP);				//wake up
	LCD_16x2_Writecom(pL, LCD_16x2_WAKE_UP);				//wake up
	
	LCD_16x2_Writecom(pL, LCD_16x2_FUNC_SET_DATA8_LINE2_NDHEIGHT_IS);
	LCD_16x2_Writecom(pL, LCD_16x2_INT_OSC); 				//internal osc frequency
	LCD_16x2_Writecom(pL, LCD_16x2_POWER_CONTROL); 				//power control
	LCD_16x2_Writecom(pL, LCD_16x2_FOLLOWER_CONTROL); 			//follower control
	LCD_16x2_Writecom(pL, LCD_16x2_CONTRAST_SET_Default);//low); 		//contrast
	LCD_16x2_Writecom(pL, LCD_16x2_DISPLAY_ON_CUR_OFF); 		//display on
	LCD_16x2_Writecom(pL, LCD_16x2_ENTRY_MODE);				//entry mode
	LCD_16x2_Clear(pL); 					//clear
	NOPs(300);
   while(1) {	
	// test to clear the screen manually
	LCD_16x2_ClearScreen_RAM(pL);
	LCD_16x2_Printf(pL, __DATE__);
	LCD_16x2_RAM_GOTO_X_Y(pL, 0,1);
	LCD_16x2_Printf(pL, __TIME__);
	LCD_16x2_RAM_to_Display(pL);
        return;
	for(i=0;i<10000;i++)
          LCD_16x2_ShowTemporaryMessage_1(pL, 0, "Test %d\nSTM32", i);
        NOPs(1); // hook
   };
//Test_my_lcd();        
}

// all the functions below are modified printf function from the STM32 library which simplifies the S/W application development
// this favours S/W development friendliness against code size/speed which can be done at later stage by anyone, once project proven functional.

/*******************************************************************************
* Function Name  : putint
* Description    : Write a decimal value (local function)
* Input          : data to be written
* Output         : None
* Return         : None
*******************************************************************************/

void LCD_16x2_putint_to_RAM(LCD_16x2_t* pL, uint32_t i)
{
	i= 0x30 + (i%10);	// get only the digit '0' + (byte modulo 10)
	LCD_16x2_Writedata_to_RAM(pL, i);
}/* end putint() */


void LCD_16x2_ShowTemporaryMessage_0(LCD_16x2_t* pL, uint16_t time_ms, uint8_t* string )
{
	LCD_16x2_ClearScreen_RAM(pL);
	LCD_16x2_Printf(pL, (char*)string);
	LCD_16x2_RAM_to_Display(pL);
	HAL_Delay(time_ms);
}

void LCD_16x2_ShowTemporaryMessage_1(LCD_16x2_t* pL, uint16_t time_ms, uint8_t* string, uint32_t param_1)
{
	LCD_16x2_ClearScreen_RAM(pL);
	LCD_16x2_Printf(pL, (char*)string, param_1);
	LCD_16x2_RAM_to_Display(pL);
	HAL_Delay(time_ms);
}

void LCD_16x2_ShowTemporaryMessage_2(LCD_16x2_t* pL, uint16_t time_ms, uint8_t* string, uint32_t param_1, uint32_t param_2)
{
	LCD_16x2_ClearScreen_RAM(pL);
	LCD_16x2_Printf(pL, (char*)string, param_1, param_2);
	LCD_16x2_RAM_to_Display(pL);
	HAL_Delay(time_ms);
}

//extern B_HMI_LCD16x2_t B_HMI_LCD16x2;
void LCD_16x2_Test(void)
{
   uint16_t value1, value2;
   value1 = 1234;
   value2 = 0x3456;

  //LCD_16x2_Init();
  LCD_16x2_ClearScreen_RAM(&HM_AddOn.LCD_16x2);
  LCD_16x2_Printf(&HM_AddOn.LCD_16x2,"Hi is %d \nLow is %d",value1, value2);
  LCD_16x2_RAM_to_Display(&HM_AddOn.LCD_16x2);
//HAL_Delay(1000000);

  LCD_16x2_Printf(&HM_AddOn.LCD_16x2,"1 is %d \n2 is %X",value1, value2);
  while(1);
}


void LCD_16x2_RAM_GOTO_Next_Line(LCD_16x2_t* pL)
{
	LCD_16x2_RAM_GOTO_X_Y(pL, 0, min2(pL->Cursor_Y+1,LCD_NB_ROW_Y));
}

void LCD_16x2_RAM_GOTO_HorizontalCenter(LCD_16x2_t* pL)
{
	LCD_16x2_RAM_GOTO_X_Y(pL, LCD_NB_COL_X/2-1, pL->Cursor_Y);
}




void LCD_16x2_Printf(LCD_16x2_t* pL, const char *str,...)
{
  uint32_t arg, arg1;
  int k;	
  uint32_t pow;
  va_list ap;
// u8  count_char =0;

  va_start(ap, str);
 
  while (*str) 
  {  
    if(*str==0x0A)
    {
//		Virtual_Com_Write_Char(0x0A); // rename the /n translated by the compiler string encoder into the 0x0D ...
      //LCD_16x2_Writedata(0x0D);
      LCD_16x2_RAM_GOTO_Next_Line(pL);
    }
    else
    if(*str==0x09)
    {
      // tabulation, goes to middle of the screen
      LCD_16x2_RAM_GOTO_HorizontalCenter(pL);
    }
    else
	 
    if (*str == 92)    // backslash (\)
    {
      str++;
      if (*str == 'n')
      {
//LCD_16x2_Writedata('\n'); // does not go to the next line in hyper terminal. Adding 0x0A/0D or 0D0A will not help.
        LCD_16x2_RAM_GOTO_Next_Line(pL);
      }
      else
      if(*str == 't')
      {
        // tabulation, goes to middle of the screen
        LCD_16x2_RAM_GOTO_HorizontalCenter(pL);
      }
      else
      {
        LCD_16x2_Writedata_to_RAM(pL, 92); // backslash (\)
        LCD_16x2_Writedata_to_RAM(pL, *str);
      }  
    } 
    else { /* else 1 */
     if (*str != '%')
        LCD_16x2_Writedata_to_RAM(pL, *str);           
     else /* else 2 */
     { 
        pow = 1;
        str++;  
        switch (*str)
        {        
          case '%':
                  LCD_16x2_Writedata_to_RAM(pL, '%');
                  break; // '%%' = '%' !
					
           case 'd': /* decimal */
              arg = va_arg(ap, uint32_t);
               
              if (arg & 0x80000000) /* negative */
              {
                 LCD_16x2_Writedata_to_RAM(pL, '-');
                 arg = (uint32_t) (- ((int32_t) arg));
              }     
              while ((uint32_t)(arg/(pow*10)))
              pow*=10;
              do{  
                 LCD_16x2_putint_to_RAM(pL, arg/pow);
                 pow/=10;
              } while (pow >= 1);  
           break;	
// special for 0.1 (x10) and 0.01 (x100) resolution on integers (most common for sensors)            
           case 'D': /* decimal, DECI 0.1 (/10) */
              arg = va_arg(ap, uint32_t);
               
              if (arg & 0x80000000) /* negative */
              {
                 LCD_16x2_Writedata_to_RAM(pL, '-');
                 arg = (uint32_t) (- ((int32_t) arg));
              }     
               while ((uint32_t)(arg/(pow*10)))
                pow*=10;
           
              pow = max2(10,pow);
              do{ 
                 LCD_16x2_putint_to_RAM(pL, arg/pow);
                 if(pow==10)
                   LCD_16x2_Writedata_to_RAM(pL, '.');
                 pow/=10;
              } while (pow >= 1);  
           break;	
            case 'C': /* decimal, DECI 0.01 (/100) */
              arg = va_arg(ap, uint32_t);
               
              if (arg & 0x80000000) /* negative */
              {
                LCD_16x2_Writedata_to_RAM(pL, '-');
                arg = (uint32_t) (- ((int32_t) arg));
              }     
              while ((uint32_t)(arg/(pow*10)))
                pow*=10;
           
              pow = max2(100,pow);
              do{ 
                LCD_16x2_putint_to_RAM(pL, arg/pow);
                if(pow==100)
                  LCD_16x2_Writedata_to_RAM(pL, '.');
                pow/=10;
              } while (pow >= 1);  
           break;	

          case 'M': /* decimal, MILI 0.001 (/1000) */
               arg = va_arg(ap, uint32_t);
                
               if (arg & 0x80000000) /* negative */
               {
                  LCD_16x2_Writedata_to_RAM(pL, '-');
                  arg = (uint32_t) (- ((int32_t) arg));
               }     

               while ((uint32_t)(arg/(pow*10)))
                 pow*=10;
            
               pow = max2(1000,pow);
               do{ 
                  LCD_16x2_putint_to_RAM(pL, arg/pow);
                  if(pow==1000)
                    LCD_16x2_Writedata_to_RAM(pL, '.');
                  pow/=10;
               } while (pow >= 1);  
            break;           
// end            

           case 'c': /* char */
              arg = va_arg(ap, uint32_t);				 
              LCD_16x2_Writedata_to_RAM(pL, arg);
           break;
           case 'x': /* Hexadecimal */
           case 'X':
              arg = va_arg(ap, uint32_t);
              arg1 = arg;
              arg1 = ((arg >> 4) & 0x0000000F);
              if (arg1 < 10)
                LCD_16x2_putint_to_RAM(pL, arg1);
              else  
                LCD_16x2_Writedata_to_RAM(pL, arg1 - 10 + 65 );
              arg1 = (arg & 0x0000000F);
              if (arg1 < 10)
                LCD_16x2_putint_to_RAM(pL, arg1);
              else  
                LCD_16x2_Writedata_to_RAM(pL, arg1 - 10 + 65 );
           break;            
           case '2':
              str++;
              if ((*str == 'x') || (*str == 'X'))
              {
                arg = va_arg(ap, uint32_t);					 
                arg1 = arg;
		for (k=12; k>=0 ;k-=4)
                {
                  arg1 = ((arg >> k) & 0x0000000F);
                  if (arg1 < 10)
                      LCD_16x2_putint_to_RAM(pL, arg1);
                  else  
                      LCD_16x2_Writedata_to_RAM(pL, arg1 - 10 + 65 );
                }
              }
            break;           
           case '4':
              str++;
              if ((*str == 'x') || (*str == 'X'))
              {
                arg = va_arg(ap, uint32_t); // u32 original
                arg1 = arg;
				
                for (k=28; k>=0 ;k-=4)
								{
                  arg1 = ((arg >> k) & 0x0000000F);
                  if (arg1 < 10)
                    LCD_16x2_putint_to_RAM(pL, arg1);
                  else  
			LCD_16x2_Writedata_to_RAM(pL, arg1 - 10 + 65 );
                }
              }                    
          break;                 
          default:
          break ;
        }/* end switch */
      }/* end else 2*/
     }/* end else 1 */
    str++;   
   }/* end while() */

   /* Write the USB serial buffer if USB is used */
//   Virtual_Com_Write_Buffer(&serial_usb_buffer_in[0], count_char);

   va_end(ap); 
	
}/* end SERIAL_Printf() */

#endif
