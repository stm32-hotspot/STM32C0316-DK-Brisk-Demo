
#include "main.h"
#include "brisk.h"

#ifndef sprintf
#include <stdio.h>
#endif

#include "SPI_MasterIO.h"
/*
 ******************************************************************************
 * @file    read_data_simple.c
 * @author  Sensors Software Solution Team
 * @brief   ISM330DLC driver file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
*/

/* Includes ------------------------------------------------------------------*/
#include <ism330dlc_reg.h>
#include <string.h>

// it's SPI mode here!
SPI_SlaveDevice_t ISM330DLC_SPI = { &SPI_IO_STMod, 1 };

/* Private macro -------------------------------------------------------------*/

#define TX_BUF_DIM          1000

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis1bit16_t data_raw_temperature;
float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;
static uint8_t whoamI, rst;
static uint8_t tx_buffer[TX_BUF_DIM];

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   Replace the functions "platform_write" and "platform_read" with your
 *   platform specific read and write function.
 *   This example use an STM32 evaluation board and CubeMX tool.
 *   In this case the "*handle" variable is useful in order to select the
 *   correct interface but the usage of "*handle" is not mandatory.
 */

static int32_t platform_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
  SPI_MasterIO_t* pS = &SPI_IO_STMod;
  SPI_MIO_Start( pS, 0x01);
  
  SPI_MIO_SendByte(pS, Reg & 0x7F);
  while(len--) {
    SPI_MIO_SendByte(pS,*Bufp);
    Bufp++;
  }
  
  SPI_MIO_Stop( pS, 0x01);
  
  return 0;
}

static int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
  SPI_MasterIO_t* pS = &SPI_IO_STMod;
  SPI_MIO_Start( pS, 0x01);
  
  SPI_MIO_SendByte(pS, Reg | 0x80); // read bit 8 = 1
  while(len--) {
    *Bufp = SPI_MIO_ReadByte(pS);
    Bufp++;
  }
  
  SPI_MIO_Stop( pS, 0x01);

  return 0;
}

/*
 *  Function to print messages
 */
static void tx_com( uint8_t *tx_buffer, uint16_t len )
{
}

/* Main Example --------------------------------------------------------------*/
int32_t ISM330DLC_IsPlugged(void);
int32_t ISM330DLC_IsPlugged(void) { // configure and start sensor measurements

  ConfigureSPI_MasterIO(&SPI_IO_STMod);
  SetSPI_MasterIO_Timings(&SPI_IO_STMod, 400000, 5000000 );
  
  
  /*
   *  Initialize mems driver interface
   */
  ism330dlc_ctx_t dev_ctx;
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = 0;  
  /*
   *  Check device ID
   */
//  while(1) {
    
  whoamI = 0;
  ism330dlc_device_id_get(&dev_ctx, &whoamI);
  if ( whoamI != ISM330DLC_ID )
    while(1); /*manage here device not found */

  /*
   *  Restore default configuration
   */
  ism330dlc_reset_set(&dev_ctx, PROPERTY_ENABLE);
  do {
    ism330dlc_reset_get(&dev_ctx, &rst);
  } while (rst);
  /*
   *  Enable Block Data Update
   */
  ism330dlc_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /*
   * Set Output Data Rate
   */
  ism330dlc_xl_data_rate_set(&dev_ctx, ISM330DLC_XL_ODR_12Hz5);
  ism330dlc_gy_data_rate_set(&dev_ctx, ISM330DLC_GY_ODR_12Hz5);
  /*
   * Set full scale
   */  
  ism330dlc_xl_full_scale_set(&dev_ctx, ISM330DLC_2g);
  ism330dlc_gy_full_scale_set(&dev_ctx, ISM330DLC_2000dps);
  
  /*
   * Configure filtering chain(No aux interface)
   */  
  /* Accelerometer - analog filter */
  ism330dlc_xl_filter_analog_set(&dev_ctx, ISM330DLC_XL_ANA_BW_400Hz);
  
  /* Accelerometer - LPF1 path ( LPF2 not used )*/
  //ism330dlc_xl_lp1_bandwidth_set(&dev_ctx, ISM330DLC_XL_LP1_ODR_DIV_4);
  
  /* Accelerometer - LPF1 + LPF2 path */   
  ism330dlc_xl_lp2_bandwidth_set(&dev_ctx, ISM330DLC_XL_LOW_NOISE_LP_ODR_DIV_100);
  
  /* Accelerometer - High Pass / Slope path */
  //ism330dlc_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
  //ism330dlc_xl_hp_bandwidth_set(&dev_ctx, ISM330DLC_XL_HP_ODR_DIV_100);
  
  /* Gyroscope - filtering chain */
  ism330dlc_gy_band_pass_set(&dev_ctx, ISM330DLC_HP_260mHz_LP1_STRONG);
  return 0;
}

int32_t ISM330DLC_Polling(void);
int32_t ISM330DLC_Polling(void) { // check if measurements ready to read

  ism330dlc_ctx_t dev_ctx; // yet to be global variable
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = 0;  
  
    /*
   * Read samples in polling mode (no int)
   */
    /*
     * Read output only if new value is available
     */
    ism330dlc_reg_t reg;
    ism330dlc_status_reg_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.xlda)
    {
      /* Read magnetic field data */
      memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
      ism330dlc_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
      acceleration_mg[0] = ism330dlc_from_fs2g_to_mg( data_raw_acceleration.i16bit[0]);
      acceleration_mg[1] = ism330dlc_from_fs2g_to_mg( data_raw_acceleration.i16bit[1]);
      acceleration_mg[2] = ism330dlc_from_fs2g_to_mg( data_raw_acceleration.i16bit[2]);
      
      sprintf((char*)tx_buffer, "Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
              acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    } 
    
    if (reg.status_reg.gda)
    {
      /* Read magnetic field data */
      memset(data_raw_angular_rate.u8bit, 0x00, 3*sizeof(int16_t));
      ism330dlc_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
      angular_rate_mdps[0] = ism330dlc_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
      angular_rate_mdps[1] = ism330dlc_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
      angular_rate_mdps[2] = ism330dlc_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);
      
      sprintf((char*)tx_buffer, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
              angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }    
    
    if (reg.status_reg.tda)
    {   
      /* Read temperature data */
      memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
      ism330dlc_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
      temperature_degC = ism330dlc_from_lsb_to_celsius( data_raw_temperature.i16bit );
       
      sprintf((char*)tx_buffer, "Temperature [degC]:%6.2f\r\n", temperature_degC );
      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
  return 0;
}

int32_t ISM330DLC_Report(u8fifo_t* pL);
int32_t ISM330DLC_Report(u8fifo_t* pL) {
  
  BTEL_Printf(pL,"*G%D", (int32_t)(temperature_degC * 10) ); // 
  
  BTEL_Printf(pL,"*H%D", (int32_t)(acceleration_mg[0] * 10) ); // 
  BTEL_Printf(pL,"*I%D", (int32_t)(acceleration_mg[1] * 10) ); // 
  BTEL_Printf(pL,"*J%D", (int32_t)(acceleration_mg[2] * 10) ); // 
  
  BTEL_Printf(pL,"*N%D", (int32_t)(angular_rate_mdps[0] * 10) ); // 
  BTEL_Printf(pL,"*M%D", (int32_t)(angular_rate_mdps[1] * 10) ); // 
  BTEL_Printf(pL,"*O%D", (int32_t)(angular_rate_mdps[2] * 10) ); // 
  
  return 0;
}

void ISM330DLC_example_main(void);
void ISM330DLC_example_main(void)
{
    ISM330DLC_IsPlugged();
    while(1) ISM330DLC_Polling();
}
