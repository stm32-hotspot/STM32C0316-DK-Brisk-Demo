/*
 ******************************************************************************
 * @file    read_data_simple.c
 * @author  MEMS Software Solution Team
 * @date    14-December-2017
 * @brief   This file show the simplest way to get data from sensor.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "main.h"
#include "brisk.h"

#ifdef _B_ANALOG_ENV_BOARD_

#include "I2C_MasterIO.h"
/* Includes ------------------------------------------------------------------*/


#undef MEMS_SHARED_TYPES

#include "stts751.h"
#include <string.h>

//#define MKI109V2
//#define NUCLEO_STM32F411RE
static int32_t platform_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);  
static int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);  
// slave address is 0xB8/B9
I2C_SlaveDevice_t gSTModSTTS751 =       { &gI2C_STMod, 0x72, 1 }; // plug and play on STMod+


STTS751_t STModSTTS751 = {
  .pI2C_SlaveDevice = &gSTModSTTS751,
  
  .dev_ctx.write_reg = platform_write,
  .dev_ctx.read_reg = platform_read,
  .dev_ctx.handle = &gSTModSTTS751, // this
  
  .pReport = &u8fifo_to_SPIP[ARD_STM_SERIAL],
  
  .fReport = STTS751_Report,
};

I2C_SlaveDevice_t gBriskSTTS751 =       { &gI2C_Brisk, 0x72, 1 }; // plug and play on Local I2C

STTS751_t BriskSTTS751 = {
  .pI2C_SlaveDevice = &gBriskSTTS751,
  
  .dev_ctx.write_reg = platform_write,
  .dev_ctx.read_reg = platform_read,
  .dev_ctx.handle = &gBriskSTTS751, // this
  
  .pReport = &u8fifo_to_SPIP[ARD_STM_SERIAL],
  .fReport = STTS751_Report,  
};


int32_t STTS751_Report(u8fifo_t* pL) { // where to send out result with base char key
  
//  BTEL_Printf(pL,"*F%C", STTS_DegC_x10); // 
  NOPs(1);
  return 0;
}


/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   Replace the functions "platform_write" and "platform_read" with your
 *   platform specific read and write function.
 *   This example use an STM32 evaluation board and CubeMX tool.
 *   In this case the "*handle" variable is usefull in order to select the
 *   correct interface but the usage uf "*handle" is not mandatory.
 */

static int32_t platform_write(void *handle, uint8_t Reg, uint8_t *Bufp,
                              uint16_t len)
{
  I2C_SlaveDevice_t* pS = handle; 
  pS->SubAdrBytes[0] = Reg;
  pS->pWriteByte = Bufp;
  pS->WriteByteCount = len;
  pS->pReadByte = 0;
  pS->ReadByteCount = 0;
  I2C_MasterIO_AccessDevice(pS);
  
  return 0;
}

static int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp,
                             uint16_t len)
{
  I2C_SlaveDevice_t* pS = handle; 
  pS->SubAdrBytes[0] = Reg;
  pS->pReadByte = Bufp;
  pS->ReadByteCount = len;
  pS->pWriteByte = 0;
  pS->WriteByteCount = 0;
  I2C_MasterIO_AccessDevice(pS);
  
  return 0;
}

/*
 *  Function to print messages
 */
static void tx_com( uint8_t *tx_buffer, uint16_t len )
{
  #ifdef NUCLEO_STM32F411RE  
  HAL_UART_Transmit( &huart2, tx_buffer, len, 1000 );
  #endif
  #ifdef MKI109V2  
  CDC_Transmit_FS( tx_buffer, len );
  #endif
}


//static  stmdev_ctx_t dev_ctx;
//static     stts751_reg_t reg;
//  dev_ctx.handle = &hspi2;  

/* Main Example --------------------------------------------------------------*/

int32_t STTS751_Configure(STTS751_t* pS) {
  
  /*
   *  Check device ID
   */
  pS->whoamI.manufacturer_id = 0;
  pS->whoamI.product_id = 0;
  pS->whoamI.revision_id = 0;
  stts751_device_id_get(&pS->dev_ctx, &pS->whoamI);
  if (( pS->whoamI.product_id != STTS751_ID_0xxxx )||( pS->whoamI.manufacturer_id != 0x53))
  {
    pS->IsAvailable = 0;
    return -1; //while(1); /*manage here device not found */
  }
  
  pS->IsAvailable = 1;
  
    /* Enable interrupt on high(=49.5 degC)/low(=-4.5 degC) temperature. */
//  float temperature_high_limit = 49.5f;
//  stts751_high_temperature_threshold_set(&pS->dev_ctx, stts751_from_celsius_to_lsb(temperature_high_limit));

//  float temperature_low_limit = -4.5f;
//  stts751_low_temperature_threshold_set(&pS->dev_ctx, stts751_from_celsius_to_lsb(temperature_low_limit));
//  stts751_pin_event_route_set(&pS->dev_ctx,  PROPERTY_ENABLE);

  /* Set Output Data Rate */
  stts751_temp_data_rate_set(&pS->dev_ctx, STTS751_TEMP_ODR_1Hz);

  /* Set Resolution */
  stts751_resolution_set(&pS->dev_ctx, STTS751_10bit);
  return 1;
  
  return 0;
}


int32_t STTS751_IsPlugged(void) {
  /*
   *  Initialize mems driver interface
   */
  STTS751_t* pS = &STModSTTS751;
/*  
  pS->dev_ctx.write_reg = platform_write;
  pS->dev_ctx.read_reg = platform_read;
  pS->dev_ctx.handle = &gSTModSTTS751; // pointer to slave device  
*/
  return STTS751_Configure(pS);
}

int32_t STTS751_CheckMeasurement(STTS751_t* pS) { // in case you have multiple temp sensors, you can reuse this function

  uint8_t flag;
  if(pS->IsAvailable == 0)
    return -1;
  stts751_flag_busy_get(&pS->dev_ctx, &flag);
  if (flag) // if there is a measurement to read... then
  { /* Read temperature data */
    memset(pS->raw_temperature.u8bit, 0, sizeof(int16_t));
    stts751_temperature_raw_get(&pS->dev_ctx, &pS->raw_temperature.i16bit);
// no float!    pS->DegC_x10 = (int32_t)(stts751_from_lsb_to_celsius(pS->raw_temperature.i16bit) * 10);
    pS->DegC_x10 = ((int32_t)(pS->raw_temperature.i16bit) * 10) / 256;    // save float lib. 
    //sprintf((char*)tx_buffer, "Temperature [degC]:%3.2f\r\n", temperature_degC);
    //tx_com(tx_buffer, strlen((char const*)tx_buffer));
  }  

  return 0;
}

int32_t STTS751_Polling(void) {
  
  STTS751_t* pS = &STModSTTS751;
  return STTS751_CheckMeasurement(pS);
}

void STTS751_example_main(void);
void STTS751_example_main(void)
{
  STTS751_IsPlugged();
  
  /*
   * Read samples in polling mode (no int)
   */
  while(1) STTS751_Polling();

}

#endif
