/**
  ******************************************************************************
  * @file    spi_master_io.h
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

#ifndef _SPI_MASTER_IO_H_ // implement SPI master communication with GPIO (SW bit banging)
#define _SPI_MASTER_IO_H_

#include "io_pins.h"

void SPIIO_Test(void);

void SPIIO_SendByte(uint8_t byte);
uint8_t SPIIO_ReadByte(void);
uint8_t SPIIO_Dummy(void);
void SPIIO_Start(void);
void SPIIO_Stop(void);

void SPIIO_SetDelay(uint8_t delay);
uint8_t SPIIO_GetDelay(void);

//===============================-------------------------------------------->

// This master SPI is monodirectional only by GPIOs. To be used by polling method (blocking)

typedef struct {

  IO_Pad_t* MISO; // we need the pointer to the pin
  IO_Pad_t* MOSI; // we need the pointer to the pin
  IO_Pad_t* MOSI_as_MISO_3W;
  IO_Pad_t* SCK; // we need the pointer to the pin
  IO_Pad_t* NSSs[8]; // list of NSS pins, can be more than one (max 8 pins)  

  // simplified DMA, both TX and RX? (does not support both directions, supposed to be bidirectional, with other direction point to 0000 or fail.
  uint32_t TX;
  uint32_t RX;
  uint32_t bCount;

//  Timer_t* Timer; // this will be to control time ticks
//  uint8_t Cn; //  a number between 0 and 3

  uint32_t (*fnWaitMethod)(uint32_t);
  uint32_t ctWaitMethod;
  uint32_t WaitParam;
  
//  RangedValue_t Bps; // input
//  MCU_Clocks_t* Clocks;
  
//  StuffsArtery_t* SA; // this points to Job feeding

} SPI_MasterIO_t;

typedef struct {

  SPI_MasterIO_t* pM;
  
//  uint8_t SlaveAdr;

  uint8_t SubAdrByteCount;  
  uint8_t SubAdrBytes[4]; // first byte is slave address, if any
  
  uint8_t *pWriteByte;
  uint16_t WriteByteCount;
  
  uint8_t *pReadByte;
  uint16_t ReadByteCount;

//  uint8_t SlaveAdrWriteNack /*: 1*/;
//  uint8_t SlaveAdrReadNack /*: 1*/;
  
// uint8_t WasDetected; // status for sweep and hot plug detection
// uint8_t IsDetected; // status for sweep and hot plug detection
  
} SPI_SlaveDevice_t;

void SetSPI_MasterIO_Timings(SPI_MasterIO_t* M, uint32_t MinBps, uint32_t MaxBps ); // 1200000, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_FirstBit_MSB
void SetSPI_MasterIO_Format(SPI_MasterIO_t* M, uint32_t CPol, uint32_t CPha, uint32_t FirstBit );
void ConfigureSPI_MasterIO(SPI_MasterIO_t* M);
void EnableSPI_MasterIO(SPI_MasterIO_t* M);

uint32_t SPI_MIO_Start(SPI_MasterIO_t* S, uint32_t BitMask);
uint32_t SPI_MIO_SendByte(SPI_MasterIO_t* S, uint8_t byte);
uint32_t SPI_MIO_ReadByte(SPI_MasterIO_t* S);
uint32_t SPI_MIO_Stop(SPI_MasterIO_t* S, uint32_t BitMask);

extern SPI_MasterIO_t SPI_IO_STMod;
extern SPI_MasterIO_t SPI_IO_ARD;

#endif


