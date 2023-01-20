/**
  ******************************************************************************
  * @file    commons.c (handy functions)
  * @author  S.Ma
  * @brief   Can be improved, STM32C0 is Cortex M0+ and does not have HW debug cycle counter, and timers are scarce resource....
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
//#include "commons.h"

void NOPs(uint32_t loop) { // global
  while(loop--) asm("nop\n"); 
}

void MinDelay_ms(uint32_t ms) { // this can be SW delay with non exact delay.
  
  while(ms--)
    MinDelay_us(1000);
}

void MinDelay_us(uint32_t us) {
  
  NOPs(us * SYS_CLK_MHZ / 2); // gross
}


int32_t Interpolate_i32 (int32_t x0, int32_t x1, int32_t y0, int32_t y1, int32_t x) { 
  int32_t dwQ;
  dwQ = ((y1-y0))*x+(x1*y0)-(x0*y1);	// overflow not checked yet
  dwQ = dwQ / (x1-x0);// we can also do roundings here
  
  return dwQ;
}

// copy memory block
void CopyFromToByteSize (void* src, const void* dst, uint32_t cnt) {
	uint8_t *d = (uint8_t*)dst;
	uint8_t *s = (uint8_t*)src;

	while(cnt--) {
          *d++ = *s++;
	}
}

void FillByteToByteSize ( uint8_t ByteValue, const void* dst, uint32_t cnt) {
	uint8_t *d = (uint8_t*)dst;

	while(cnt--) {
          *d++ = ByteValue;
	}
}


// copy strings
void StringCopy (void* dst, const void* src, uint32_t cnt) {
	uint8_t *d = (uint8_t*)dst;
	const uint8_t *s = (const uint8_t*)src;

        while(cnt--) {
          *d = *s;
          if(*d == 0) break; // after copy the last null char.
          d++;s++;
	}
}


// Compare memory block
uint32_t BytesIdenticals (const void* dst, const void* src, uint32_t cnt) {	/* ZR:same, NZ:different */

  const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;

  if(cnt==0) TrapError();
  
  while(cnt--) {
    if(*d++ != *s++) return 0;
  }

  return 1;
}

// Compare strings
uint32_t StringsIdenticals (const void* dst, const void* src, uint32_t cnt) {	/* ZR:same, NZ:different */
  
	const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;

        if(cnt==0) TrapError();

        while(cnt--) {
		if( *d != *s ) return 0;
                if(*d==0) return 1;
                d++;s++;
	};

	return 1;
}







