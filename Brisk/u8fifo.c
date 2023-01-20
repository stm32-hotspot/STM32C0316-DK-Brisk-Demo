/**
  ******************************************************************************
  * @file    u8fifo.c (Software FIFO -cyclic buffer-)
  * @author  S.Ma
  * @brief   Asynchroneous data move implementing a SW FIFO with notification of "empty/full" to control peripheral interrupts.
  *          The fifo has statistics to measure the largest used size for memory tuning phase, which makes the SW a bit more complex than an optimized one.
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

// This creates a tube, byte size, between 2 different processes, being under interrupt or not.
// When calling these functions, disable interrupt before optimizing later on.
// This is a circular buffer which can add/remove byte from either end (head or tail)
// we kept only the functions to implement a FIFO.

uint32_t Hooku8fifo_NoLongerEmpty(u8fifo_t* pu8fifo, uint32_t (*fn)(uint32_t), uint32_t ct) {

  pu8fifo->ctNoLongerEmpty = ct;  
  pu8fifo->fnNoLongerEmpty = fn;
  return 0;
}

uint32_t Hooku8fifo_Emptied(u8fifo_t* pu8fifo, uint32_t (*fn)(uint32_t), uint32_t ct) {

  pu8fifo->ctEmptied = ct;    
  pu8fifo->fnEmptied = fn;
  return 0;
}

//==========================================================
// sw fifo creation, setup
uint32_t Newu8fifo(u8fifo_t* pu8fifo, uint32_t begin, int32_t size) {

  if(size==0)
    while(1); // no memory for it?

  pu8fifo->pbLowest = begin; // left is decreasing memory address with rollover circular buffer
  pu8fifo->pbHighest = begin + (size - 1); // tight is increasing memory address with rollover circular buffer inclusive pointer
  
  // valid SRAM space
  pu8fifo->bCountLimit = size; // we validate the strand size (action can occur)
  pu8fifo->bCount = 0;
  return begin;
}

//==========================================================
// manage the left side

static uint32_t _Clipu8fifo_Down(u8fifo_t* pu8fifo);
static uint32_t _Clipu8fifo_Down(u8fifo_t* pu8fifo) { // 0 or 1, returns the bit pointer

//  if(bCountLimit==0) while(1); // error
  
  if(pu8fifo->bCount==0) // if strand empty, nothing to read from it, error
    while(1); // error, nothing on this strand, check its size is non zero first!

  pu8fifo->bCount--;  // strand not empty. Get the left bit first, reduce strand size
  pu8fifo->Out = *(uint8_t*) pu8fifo->pbDown; // we create one more bit space on the left (lower memory)
  
  // point to the new left side
  pu8fifo->pbDown++;
  if(pu8fifo->pbDown>pu8fifo->pbHighest) // circular memory space bottom reached.
    pu8fifo->pbDown = pu8fifo->pbLowest; // jump to higher end
  
  if(pu8fifo->bCount==0) {
    if(pu8fifo->fnEmptied) {// if the strand turns empty, tell someone?
      pu8fifo->fnEmptied(pu8fifo->ctEmptied);
    }else{
      pu8fifo->FlagEmptied = 1;
    };
  };

  // one item was added, check if someone is ready to empty it
//TBD  if(pu8fifo->fnIn) ((uint32_t(*)(uint32_t))pu8fifo->fnIn)(pu8fifo->ctIn);
  
  return pu8fifo->Out;// return read valid bit
}

//==========================================================
// manage the right side
static uint32_t _Glueu8fifo_Up(u8fifo_t* pu8fifo);
static uint32_t _Glueu8fifo_Up(u8fifo_t* pu8fifo) {

  if(pu8fifo->bCountLimit==0) while(1); // error
  
  if(pu8fifo->bCount==0) { // if strand empty: Create the first bit
    pu8fifo->pbUp = pu8fifo->pbDown = pu8fifo->pbLowest; // arbitrary left is the start creation side
  }
  else {  // the strand exist, check fullness
    if(pu8fifo->bCount>=pu8fifo->bCountLimit) {
      pu8fifo->FlagFull=1;
      return 0;
      //while(1); // too big! improve memory allocation, flag, roll over etc... depends on scheme
    }
    // we create one more bit space on the left (lower memory)
    pu8fifo->pbUp++;
    if(pu8fifo->pbUp>pu8fifo->pbHighest) // rollover if out of range
      pu8fifo->pbUp = pu8fifo->pbLowest;
  };
  
  // write in memory and increase bit counter safely
  *(uint8_t*)pu8fifo->pbUp = pu8fifo->In;
  pu8fifo->bCount++;
  if(pu8fifo->bCount>pu8fifo->bCountMax) pu8fifo->bCountMax = pu8fifo->bCount;
  
  if(pu8fifo->bCount==1) {
    if(pu8fifo->fnNoLongerEmpty) { // if the strand not empty, tell someone?
      pu8fifo->fnNoLongerEmpty(pu8fifo->ctNoLongerEmpty); //!!! this can activate interrupt IRQ...
    }else{
      pu8fifo->FlagNoLongerEmpty = 1;
    };
  }

  // one item was added, check if someone is ready to empty it
//TBD  if(pu8fifo->fnOut) ((uint32_t(*)(uint32_t))pu8fifo->fnOut)(pu8fifo->ctOut);

  return pu8fifo->pbUp;
}



uint32_t AddTou8fifo(u8fifo_t* pu8fifo, uint32_t In) {
  
  pu8fifo->In = In;
  return Glueu8fifo_Up(pu8fifo);
}


// reading the strand content: The strand does not kill the bits when cut!

// here we make the interrupt disable wrapper for fifo functions. not optimized.
// good enough to make progress


uint32_t Clipu8fifo_Down(u8fifo_t* pu8fifo) {
  
  uint32_t result;
  uint32_t tmp = __get_PRIMASK();
  __set_PRIMASK(tmp | 1);
  result = _Clipu8fifo_Down(pu8fifo);
  __set_PRIMASK(tmp);
  return result;
}

uint32_t Glueu8fifo_Up(u8fifo_t* pu8fifo) {
  
  uint32_t result;
  uint32_t tmp = __get_PRIMASK();
  __set_PRIMASK(tmp | 1);
  result = _Glueu8fifo_Up(pu8fifo);
  __set_PRIMASK(tmp);
  return result;
}


