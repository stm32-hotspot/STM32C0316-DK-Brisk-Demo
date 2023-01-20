/**
  ******************************************************************************
  * @file    BTEL_drivers.c (bluetooth electronics android app, STM32 Implementation side of it)
  * @author  S.Ma
  * @brief   Implement MCU side BTEL protocol using USART
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

// here is the theory
// BTEL (and pBTEL) contains the info about Bluetooth electronics Application and reporting
// The present drawn dashboard is split in 2 parts, the left side is fixed with up/down navigation of sub pages which are drawn on the right side of the phone dashboard screen
// The left pane user controls are intercepted first by the master device.
// All the reporting (for now) comes from the selected device.
// STM32 and generate / report their own panel (0)
// each add-on board provide multiple pannels (1+) max 9 panels from 1..9
// by default each report through SIF(APP_SERIAL)
// The MCU will listen to its own add-on board UART possibly connected to HC-06 through SIF(STM_SERIAL)
// SIF comes from USART Rx (Add-on board if present and compatible, SWD SIF otherwise)
// Turn OFF BTEL reporting (sent by master)
// Turn ON BTEL reporting (sent by master)
// Down (pane++) (sent by dashboard)
// Up (pane--) (sent by dashboard)
// Left (previous slave) (sent by master)
// Right (next slave) (sent by master)
// Resend Pane download (sent by master)

// Now we need to trigger the first panel download. How?
// By command
// By actions

BTEL_t BTEL; // this is for the MCU on this board. Being Master or Slave or having the BT add-on HC-06 change the behaviour
BTEL_t* pBTEL = &BTEL;
void BTEL_Incoming(void);

void BTEL_ProcessMessage(void);

void TestSringToFrac(void);
void BTEL_TestPrintf(void);

void BTEL_Enable(void) {
  BTEL.Enabled = 1;
}

void BTEL_Disable(void) {
  BTEL.Enabled = 0;
}

void BTEL_ResetIncoming(void);
void BTEL_ResetIncoming(void) {
  
  BTEL.in_ndx = 0; // waiting for *
//    BTEL.out_ndx = 0; // waiting for filling
  BTEL.in_start = 0; // start receive multibyte when non zero
  BTEL.in_stop = 0; // completed when second * has been received
  
}

void BTEL_Init(void) {

 // setup at serial com port SPIP_SIF.C file   BTEL.Enabled = 1; // by default, deactivate BTEL (no JS nor micropython installed)
  BTEL.TimeSlice = -1; // could be disabled if negative
  
  BTEL_ResetIncoming();
  
  BTEL.PanelSelector = 0;
  BTEL.PanelSelectorMax = 0; // no add-on board detected for now
  BTEL.DeviceSelector = 0;//MAX_SLAVE; // master
  
  //no need on V3, push user key without add-on boards for 5 seconds and the panel will be pushed out... 
  //BTEL.Delayed_PanelCountdown_100ms = 300; // if nothing works, after 30 seconds, a panel redraw will be done
//    return;
  // let's test our incoming string to value
   //TestSringToFrac();
   //BTEL_TestPrintf();
}



void BTEL_Deinit(void);
void BTEL_Polling(void) {
  
  BTEL_Incoming();
}

void BTEL_Incoming(void) { // any chars coming from the phone's app?
  
  char c;
  
  if(BTEL.Enabled==0) return;
  
  while(u8fifo_to_SPIP[BT_STL_SERIAL].bCount) { // some characters to process
    
    // read a char from incoming FIFO
    c = (uint8_t) Clipu8fifo_Down(&u8fifo_to_SPIP[BT_STL_SERIAL]);
    BTEL.incoming[BTEL.in_ndx] = c;
    
    if(BTEL.in_ndx>=(sizeof(BTEL.incoming)-2))
      TrapError(); // overflow!

    if((BTEL.in_start==0)&&(c=='*')) { // we captured a start '*'
      BTEL.in_start = 1;
      continue; // don't store the first '*' ^^^^
    }
    
    if(BTEL.in_start) { // if multichar reception
      if(c!='*') {
        BTEL.in_ndx++; // store the char
        continue; // intermediate
      }
      // * received
      BTEL.incoming[BTEL.in_ndx++] = 0; // Null terminated string formatting
      BTEL.in_stop = 1; // full frame received
      // let's process the command

    }
    
    // something valid to process has arrived
    LeftPaneProcess();
    RightPaneProcess();
    // everybody got the chance to intercept the command... mono or multisource
    BTEL_ResetIncoming();
    
  }; // continue emptying the FIFO
  
}


void BTEL_50msec(void) {
}



int32_t StringToFrac(char s[], int32_t* pVal) { // tested ok
  
  int pos, digit, sign, dotpos, c, res, startpos, frac;
  
  res = 0;
  sign = 0;
  startpos = -1;
  dotpos = -1;
  frac = 0;
  for(pos = 0; (c=s[pos])!=0; pos++) { // browse the string until 256 char or null char
    
    if(pos>=256) return -1;
    
    switch(c) {
    case '-': 
      if(startpos >= 0) return -1; // error
      sign = -1;
      startpos = pos;
      continue;
    case  '+':
      if(startpos >= 0) return -1; // error
      sign = +1;
      startpos = pos;
      continue;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
 	digit = c - '0';
	res = (10*res) + digit;
        if(startpos < 0) {
          startpos = pos;
          if(sign==0)
            sign = +1;
        }
        if(dotpos>=0)
          frac++; // a digit past comma
        continue;
    case '.':
      if(dotpos>=0) TrapError(); //return -1
      dotpos = pos;
      break;
    default:
      c = 0;
      break;
    }
    
    if(startpos < 0) continue; // conversion hasn't started yet
    if(c == 0) // conversion started and invalid char found: stop convert
      break; // stop on-going conversion
  }
  
  if(startpos<0) // failed conversion
    return -1;
  
  // integer sign and dot
  res *= sign;
  *pVal = res;

  return frac; // no error
    
}

const char s1[] = "A3456dfg";
const char s2[] = "B34.56dfg";
const char s3[] = "C3.456dfg";
const char s4[] = "D-3456fgh";
const char s5[] = "F-34.56fgh";
const char s6[] = "G-34 56fgh";
const char s7[] = "E--3456fgh";

// test the function
void TestSringToFrac(void) {
  
  volatile int32_t frac;
  int32_t result;
  
  frac = StringToFrac((char*)s1, &result);
  frac = StringToFrac((char*)s2, &result);
  frac = StringToFrac((char*)s3, &result);
  frac = StringToFrac((char*)s4, &result);
  frac = StringToFrac((char*)s5, &result);
  frac = StringToFrac((char*)s6, &result);
  frac = StringToFrac((char*)s7, &result);
NOPs(1);
}
  
void BTEL_putint_to_RAM(u8fifo_t* pL, uint32_t i);
void BTEL_putint_to_RAM(u8fifo_t* pL, uint32_t i)
{
	i= '0' + (i%10);	// get only the digit '0' + (byte modulo 10)
	AddTou8fifo(pL, i);
}/* end putint() */



void BTEL_Printf(u8fifo_t* pL, const char *str,...)
{
  uint32_t arg, arg1;
  int k;	
  uint32_t pow;
  va_list ap;
// u8  count_char =0;
  if(BTEL.Enabled==0) return;

  va_start(ap, str);
 
  while (*str) 
  {  
    if(*str==0x0A)
    {
//		Virtual_Com_Write_Char(0x0A); // rename the /n translated by the compiler string encoder into the 0x0D ...
      //LCD_16x2_Writedata(0x0D);
      AddTou8fifo(pL, *str);
    }
    else
    if(*str==0x09)
    {
      // tabulation, goes to middle of the screen
      AddTou8fifo(pL, *str);
    }
    else
	 
    if (*str == 92)    // backslash (\)
    {
      str++;
      if (*str == 'n')
      {
//LCD_16x2_Writedata('\n'); // does not go to the next line in hyper terminal. Adding 0x0A/0D or 0D0A will not help.
        AddTou8fifo(pL, 0x0A);
      }
      else
      if(*str == 't')
      {
        // tabulation, goes to middle of the screen
        AddTou8fifo(pL, 0x0D); // tochange
      }
      else
      {
        AddTou8fifo(pL, 92); // backslash (\)
        AddTou8fifo(pL, *str);
      }  
    } 
    else { /* else 1 */
     if (*str != '%')
        AddTou8fifo(pL, *str);           
     else /* else 2 */
     { 
        pow = 1;
        str++;  
        switch (*str)
        {        
          case '%':
                  AddTou8fifo(pL, '%');
                  break; // '%%' = '%' !
					
           case 'd': /* decimal */
              arg = va_arg(ap, uint32_t);
               
              if (arg & 0x80000000) /* negative */
              {
                 AddTou8fifo(pL, '-');
                 arg = (uint32_t) (- ((int32_t) arg));
              }     
              while ((uint32_t)(arg/(pow*10)))
              pow*=10;
              do{  
                 BTEL_putint_to_RAM(pL, arg/pow);
                 pow/=10;
              } while (pow >= 1);  
           break;	
// special for 0.1 (x10) and 0.01 (x100) resolution on integers (most common for sensors)            
           case 'D': /* decimal, DECI 0.1 (/10) */
              arg = va_arg(ap, uint32_t);
               
              if (arg & 0x80000000) /* negative */
              {
                 AddTou8fifo(pL, '-');
                 arg = (uint32_t) (- ((int32_t) arg));
              }     
               while ((uint32_t)(arg/(pow*10)))
                pow*=10;
           
              pow = max2(10,pow);
              do{ 
                 BTEL_putint_to_RAM(pL, arg/pow);
                 if(pow==10)
                   AddTou8fifo(pL, '.');
                 pow/=10;
              } while (pow >= 1);  
           break;	
            case 'C': /* decimal, DECI 0.01 (/100) */
              arg = va_arg(ap, uint32_t);
               
              if (arg & 0x80000000) /* negative */
              {
                AddTou8fifo(pL, '-');
                arg = (uint32_t) (- ((int32_t) arg));
              }     
              while ((uint32_t)(arg/(pow*10)))
                pow*=10;
           
              pow = max2(100,pow);
              do{ 
                BTEL_putint_to_RAM(pL, arg/pow);
                if(pow==100)
                  AddTou8fifo(pL, '.');
                pow/=10;
              } while (pow >= 1);  
           break;	

          case 'M': /* decimal, MILI 0.001 (/1000) */
               arg = va_arg(ap, uint32_t);
                
               if (arg & 0x80000000) /* negative */
               {
                  AddTou8fifo(pL, '-');
                  arg = (uint32_t) (- ((int32_t) arg));
               }     

               while ((uint32_t)(arg/(pow*10)))
                 pow*=10;
            
               pow = max2(1000,pow);
               do{ 
                  BTEL_putint_to_RAM(pL, arg/pow);
                  if(pow==1000)
                    AddTou8fifo(pL, '.');
                  pow/=10;
               } while (pow >= 1);  
            break;           
// end            

           case 'c': /* char */
              arg = va_arg(ap, uint32_t);				 
              AddTou8fifo(pL, arg);
           break;
           case 'x': /* Hexadecimal */
           case 'X':
              arg = va_arg(ap, uint32_t);
              arg1 = arg;
              arg1 = ((arg >> 4) & 0x0000000F);
              if (arg1 < 10)
                BTEL_putint_to_RAM(pL, arg1);
              else  
                AddTou8fifo(pL, arg1 - 10 + 65 );
              arg1 = (arg & 0x0000000F);
              if (arg1 < 10)
                BTEL_putint_to_RAM(pL, arg1);
              else  
                AddTou8fifo(pL, arg1 - 10 + 65 );
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
                      BTEL_putint_to_RAM(pL, arg1);
                  else  
                      AddTou8fifo(pL, arg1 - 10 + 65 );
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
                    BTEL_putint_to_RAM(pL, arg1);
                  else  
			AddTou8fifo(pL, arg1 - 10 + 65 );
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

void BTEL_TestPrintf(void) {
  
  int v1,v2;
  v1 = 123456;
  v2 = 234567;
  
  while(1) {
    
    BTEL_Printf(&u8fifo_from_SPIP[BT_STL_SERIAL], "*TBriskV2*U%i*V%D*W%C*%M*", v1, v1, v1, v1);
    HAL_Delay(1000);
    v1++;    
    BTEL_Printf(&u8fifo_from_SPIP[BT_STL_SERIAL], "*TAdd-on*U%i*V%D*W%C*%M*", v2, v2, v2, v2);
    HAL_Delay(1000);
    v1++;    
  }
  
}


void BTEL_SelectDevice(void) {
  
      BTEL.DeviceSelector = 0;
//  BTEL.PanelSelector = 0; // go to brisk panel selector
//  BTEL_SetPanel(); // draw pannel
}

// we will assume these are contiguous in memory to start simple
// start from * until * or 1500 chars transmitted

int32_t TransmitPanel(u8fifo_t* pL, char* pS) {
  
  int32_t count = 0;
  char c;
  
  // search for the first *
  do {
    c = *pS++;
    count++;
    if(count>1500) return 1;
  }while(c!='*');
  
  AddTou8fifo(pL, c);
  
  // send until '*' or 1023 chars
  do {
    c = *pS++;
    count++;
    if(count>1500)
    {
      NOPs(1);
      return 1;
    };
    if(c==0) return 0;//continue;
    AddTou8fifo(pL, c);
  }while(1);//c!='*');
    
//  return 0;
}



