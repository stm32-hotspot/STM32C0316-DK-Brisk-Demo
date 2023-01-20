/**
  ******************************************************************************
  * @file    M24256D.c (EEPROM with ID page, dual slave address)
  * @author  S.Ma
  * @brief   Driver for the 32 kbyte user EEPROM, plus the 64 bytes ID page which is used on STMod+ Add-on board to store the string name of the add-on board
  *          which is used for hot plug and play detection. Here the device has dual addresses, so converted as dual slaves.
  *          When writing, takes care of page management. Could be improved on the NACK polling while writing data.
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
// EEPROM present in STMod+ add-on boards for hot plug and play implementation. 32 kbyte user data available.

//#define EEP_I2C_ADDRESS      0xAE
//#define EEPID_I2C_ADDRESS    0xBE

#define MAX_WRITE_CYCLE_MS      5
#define MAX_RETRIALS_ON_ERROR   0

#define BLANKBYTE	0xFF 

// STMod+ Memory and I2C bus
I2C_SlaveDevice_t gSTModM24256D =       { &gI2C_STMod, 0xAE, 2 }; // plug and play on STMod+
I2C_SlaveDevice_t gSTModM24256D_ID =    { &gI2C_STMod, 0xBE, 2 }; // plug and play on STMod+
EEP_t STModM24256D = { &gSTModM24256D, &gSTModM24256D_ID };

// Brisk Memory and I2C bus (Brisk V2 only) // hot plug and play only if present on board.
#if 0 // not present on STM32C0316-DK
I2C_SlaveDevice_t gBriskM24256D =       { &gI2C_Brisk, 0xAE, 2 }; // plug and play on STMod+
I2C_SlaveDevice_t gBriskM24256D_ID =    { &gI2C_Brisk, 0xBE, 2 }; // plug and play on STMod+
EEP_t BriskM24256D = { &gBriskM24256D, &gBriskM24256D_ID };
#endif

uint8_t	EEP_WaitWriteCompletion(EEP_t* pEEP);
static uint8_t EEP_PageWriteBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* ptr, uint16_t Count);

uint8_t Is_EEP_Detected(EEP_t* pEEP)
{
  uint8_t Present = I2C_MasterIO_IsSlavePresent(pEEP->pDevice) && I2C_MasterIO_IsSlavePresent(pEEP->pDeviceID);
  
  pEEP->IsAvailable = Present; // keep a trace
  
  return Present;
}


/* Private variables ---------------------------------------------------------*/
/*\
| | Generic unoptimized. To optimize poll the eeprom and wait for it to ack
\*/

uint8_t	EEP_WaitWriteCompletion(EEP_t* pEEP)	// TRUE = OK, FALSE = NG
{
//HAL_Delay(MAX_WRITE_CYCLE_MS); // ADAPT_HERE
  NOPs(MAX_WRITE_CYCLE_MS * SYS_CLK_MHZ * 1000 / 2 ); // crude, should work.
return 1;
/*	
// uint8_t countdown = MAX_WRITE_CYCLE_MS+1;
  while(countdown) {
    if(I2CIO_IsSlaveDetected(EEP_SLAVEADR))
      return TRUE;
    HAL_Delay(1);//this function may be called while interrupts are disabled.
    countdown--;
  }
  return FALSE; */
}

uint8_t EEP_ReadBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* pByte, uint16_t Count)
{
  //uint8_t RetryCount = MAX_RETRIALS_ON_ERROR;
  //uint16_t i;
  uint32_t result;
  I2C_SlaveDevice_t* pDevice = pEEP->pDevice;

  // sanity check
  if((Adr+Count)>=EEP_ADR_OOR)
    TrapError();
  if(Count==0)
    TrapError();
  
  pDevice->SubAdrBytes[0] = (Adr >> 8) & 0xFF;
  pDevice->SubAdrBytes[1] = (Adr >> 0) & 0xFF;
  pDevice->pWriteByte = 0;
  pDevice->WriteByteCount = 0; // no data write
  pDevice->pReadByte = pByte;
  pDevice->ReadByteCount = Count;
  result =  I2C_MasterIO_AccessSlave(pDevice);
  return result;
}

uint8_t EEP_ReadID (EEP_t* pEEP)	
{
//  uint8_t RetryCount = MAX_RETRIALS_ON_ERROR;
//  uint8_t i;
  uint32_t result;
  I2C_SlaveDevice_t* pDevice = pEEP->pDeviceID;

  pDevice->SubAdrBytes[0] = 0x00;// msb
  pDevice->SubAdrBytes[1] = 0x00;// lsb;
  pDevice->pWriteByte = 0;
  pDevice->WriteByteCount = 0; // no data write
  pDevice->pReadByte = pEEP->ID_Page;
  pDevice->ReadByteCount = EEPPAGESIZE_BYTE;
  result =  I2C_MasterIO_AccessSlave(pDevice);
  return result;
}

uint8_t EEP_WriteID (EEP_t* pEEP) {

  //uint8_t RetryCount = MAX_RETRIALS_ON_ERROR;
  uint8_t i,buf[EEPPAGESIZE_BYTE];
  uint8_t result = 1;
  I2C_SlaveDevice_t* pDevice = pEEP->pDeviceID;

  pDevice->SubAdrBytes[0] = 0x00;// msb
  pDevice->SubAdrBytes[1] = 0x00;// lsb;
  pDevice->pWriteByte = pEEP->ID_Page;
  pDevice->WriteByteCount = EEPPAGESIZE_BYTE; // no data write
  pDevice->pReadByte = 0;
  pDevice->ReadByteCount = 0;
  result =  I2C_MasterIO_AccessSlave(pDevice);
  //
  EEP_WaitWriteCompletion(pEEP); // to mature
  for(i=0;i<EEPPAGESIZE_BYTE;i++) // backup original
    buf[i] = pEEP->ID_Page[i];
  
  // verify
  EEP_ReadID (pEEP);
  for(i=0;i<EEPPAGESIZE_BYTE;i++) {
    if(buf[i] != pEEP->ID_Page[i])
      break;
    pEEP->ID_Page[i] = buf[i];
  };
  if(i != EEPPAGESIZE_BYTE)
    TrapError(); // warning for hot plug (not supposed to happen for ID Page)

  return result; // if 0 : writing failed
}
#if 0 // for debug purpose, to be visible as global variable in the watch window
  uint16_t start, end, size, page, page_start, page_end, count, nb_pages;
  uint8_t result;
  uint8_t *pu8;
#endif
uint8_t EEP_WriteBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* ptr, uint16_t Count) 
{
//  uint8_t RetryCount = MAX_RETRIALS_ON_ERROR;
  //I2C_SlaveDevice_t* pDevice = pEEP->pDevice;

  uint16_t start, end, size, page, page_start, page_end, count/*, nb_pages*/;
  uint8_t result = 0;
  uint8_t *pu8;

  // sanity check
  if((Adr+Count)>=EEP_ADR_OOR)
    TrapError();
  if(Count==0)
    TrapError();
  
  page_start = Adr / 64;
  page_end = (Adr + Count - 1 ) / 64;
  // we know how many block writes we need to do
  //nb_pages = page_end - page_start + 1;
  pu8 = ptr;
  count = 0;
  
  // this is the first block write
  for(page = page_start; page <= page_end; page++) {
    
    start = max2(Adr, page * 64);
    end = min2(Adr + Count - 1, ((page+1) * 64) - 1);
    size = end - start + 1;
 
    // program eeprom from start for size bytes, using data from pointer at pu8
    NOPs(1); // do page programming, verify page writing: safe block write/verify
    result |= EEP_PageWriteBytes (pEEP, start, pu8, size);
    
    pu8 += size;
    count += size;
  }

  NOPs(1);
  return result;
}

static uint8_t EEP_PageWriteBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* ptr, uint16_t Count) {

  //uint8_t RetryCount = MAX_RETRIALS_ON_ERROR;
  //uint16_t i;
  uint8_t i,buf[EEPPAGESIZE_BYTE];
  uint32_t result;
  I2C_SlaveDevice_t* pDevice = pEEP->pDevice;

  // sanity check
  if((Adr+Count)>=EEP_ADR_OOR)
    TrapError();
  if(Count==0)
    TrapError();
  
  pDevice->SubAdrBytes[0] = (Adr >> 8) & 0xFF;
  pDevice->SubAdrBytes[1] = (Adr >> 0) & 0xFF;
  pDevice->pWriteByte = ptr;
  pDevice->WriteByteCount = Count; // no data write
  pDevice->pReadByte = 0;
  pDevice->ReadByteCount = 0;
  result =  I2C_MasterIO_AccessSlave(pDevice);
  // process result here
  
  // wait for completion
  EEP_WaitWriteCompletion(pEEP); // for now, it's a max delay
  
  // now we need to verify, so we read and store in the buffer
  EEP_ReadBytes (pEEP, Adr, buf, Count); // store is in a temp buffer

  for(i=0;i<Count;i++) { // we could use a macro for 
    if(buf[i] != *ptr++)
      break; // failed
  };
  if(i != Count) // possible retrials... 
    TrapError(); // warning for hot plug (not supposed to happen for ID Page)
  
  return result;
}

//=================== 8>< -----------------------------------------------------
/*\
| | Test most of the EEPROM functions
| | Use this function from your main loop for validation/debug purpose
\*/

// void EEP_WriteBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* ptr, uint16_t Count) 

uint8_t Buf[1024];

void	EEP_Test(void)
{
  //uint8_t eeprom_buffer[16];
  uint16_t i;
return;    
//  Is_EEP_Detected(&STModM24256D);
  
  //void EEP_ReadBytes (EEP_t* pEEP, uint16_t Adr, uint8_t* pByte, uint16_t Count)
  //EEP_ReadBytes (&STModM24256D, 0x0001, STModM24256D.DataPage, 4);  
  //EEP_ReadID (&STModM24256D);
//  EEP_WriteID (&STModM24256D);
  
  // test writes
  for(i=0;i<sizeof(Buf);i++)
    Buf[i] = i % sizeof(Buf);
  
  EEP_WriteBytes (&STModM24256D, 0x0100, Buf, 1); // beg of page
  EEP_WriteBytes (&STModM24256D, 0x0100, Buf, 4); // beg of page
  EEP_WriteBytes (&STModM24256D, 0x013C, Buf, 4); // end of page
  EEP_WriteBytes (&STModM24256D, 0x013E, Buf, 4); // 2 overlap page
  EEP_WriteBytes (&STModM24256D, 0x013C, Buf, 4); // end of page
  EEP_WriteBytes (&STModM24256D, 0x3EF0, Buf, 64); // a full page
  EEP_WriteBytes (&STModM24256D, 0x03FF, Buf, 66); // end of page  
  EEP_WriteBytes (&STModM24256D, 0x05FE, Buf, 140); // end of page  
  
  EEP_WriteBytes (&STModM24256D, 0x7FFE, Buf, 140); // end of page    // should trigger an error
  
  //HAL_Delay(100);
  NOPs(100 * SYS_CLK_MHZ * 1000 / 2);
  NOPs(1);
 return;
  /*
  while(1) {
  EEP_ReadBytes(0x0000, eeprom_buffer, 8);
  for(i=0;i<8;i++)
    eeprom_buffer[i] = eeprom_buffer[i] ^ 0xFF;
  
  EEP_WriteBytes(0x0000, eeprom_buffer, 8);
  EEP_ReadBytes(0x0000, eeprom_buffer, 8);  

  for(i=0;i<8;i++)
    eeprom_buffer[i] = eeprom_buffer[i] ^ 0xFF;
  
  EEP_WriteBytes(0x0000, eeprom_buffer, 8);
  
  }
  */
}

uint8_t EEP_SetID_Page(EEP_t* pEEP, char* pStringID) {
  
  uint16_t i;
  if(pStringID==0) TrapError();
  
  for(i=0;i<EEPPAGESIZE_BYTE;i++,pStringID++) {
    pEEP->ID_Page[i] = *pStringID;
    if(*pStringID==0) break;
  }
  
  if(i>16) TrapError(); // we limit the String ID to 16 characters
  
  return 0;
}
