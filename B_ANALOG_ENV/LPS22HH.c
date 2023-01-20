/**
  ******************************************************************************
  * @file    LPS22HH.c (Pressur Sensor)
  * @author  S.Ma
  * @brief   Application to Sensor interface layer
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
#include "I2C_MasterIO.h"
/* Includes ------------------------------------------------------------------*/
#include "lps22hh.h"
#include <string.h>

//#define MKI109V2
//#define NUCLEO_STM32F411RE

// slave address is 0xB8/B9
I2C_SlaveDevice_t gSTModLPS22HH =       { &gI2C_STMod, 0xB8, 1 }; // plug and play on STMod+

#ifdef MKI109V2
#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"
#include "spi.h"
#include "i2c.h"
#endif

#ifdef NUCLEO_STM32F411RE
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#endif

/* Private macro -------------------------------------------------------------*/
#ifdef MKI109V2
#define CS_SPI2_GPIO_Port   CS_DEV_GPIO_Port
#define CS_SPI2_Pin         CS_DEV_Pin
#define CS_SPI1_GPIO_Port   CS_RF_GPIO_Port
#define CS_SPI1_Pin         CS_RF_Pin
#endif

#ifdef NUCLEO_STM32F411RE
/* N/A on NUCLEO_STM32F411RE + IKS01A1 */
/* N/A on NUCLEO_STM32F411RE + IKS01A2 */
#define CS_SPI2_GPIO_Port   0
#define CS_SPI2_Pin         0
#define CS_SPI1_GPIO_Port   0
#define CS_SPI1_Pin         0
#endif

#define TX_BUF_DIM          1000

/* Private variables ---------------------------------------------------------*/
static axis1bit32_t data_raw_pressure;
static axis1bit16_t data_raw_temperature;
static float pressure_hPa;
static float temperature_degC;
static uint8_t whoamI, rst;
//static uint8_t tx_buffer[TX_BUF_DIM];

int32_t LPS_hPa_x100;
int32_t LPS_DegC_x10;

int32_t LPS22HH_Report(u8fifo_t* pL) { // where to send out result with base char key
  
  BTEL_Printf(pL,"*E%D*F%C", LPS_hPa_x100, LPS_DegC_x10); // 
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
  gSTModLPS22HH.SubAdrBytes[0] = Reg;
  gSTModLPS22HH.pWriteByte = Bufp;
  gSTModLPS22HH.WriteByteCount = len;
  gSTModLPS22HH.pReadByte = 0;
  gSTModLPS22HH.ReadByteCount = 0;
  I2C_MasterIO_AccessSlave(&gSTModLPS22HH);
  
  return 0;
}

static int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp,
                             uint16_t len)
{
  gSTModLPS22HH.SubAdrBytes[0] = Reg;
  gSTModLPS22HH.pReadByte = Bufp;
  gSTModLPS22HH.ReadByteCount = len;
  gSTModLPS22HH.pWriteByte = 0;
  gSTModLPS22HH.WriteByteCount = 0;
  I2C_MasterIO_AccessSlave(&gSTModLPS22HH);
  
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


static  lps22hh_ctx_t dev_ctx;
static     lps22hh_reg_t reg;
//  dev_ctx.handle = &hspi2;  

/* Main Example --------------------------------------------------------------*/

int32_t LPS22HH_IsPlugged(void) {
  /*
   *  Initialize mems driver interface
   */
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = 0; // pointer to slave device  
  /*
   *  Check device ID
   */
  whoamI = 0;
  lps22hh_device_id_get(&dev_ctx, &whoamI);
  if ( whoamI != LPS22HH_ID )
    return 0; /*manage here device not found */
  /*
   *  Restore default configuration
   */
  lps22hh_reset_set(&dev_ctx, PROPERTY_ENABLE);
  do {
    lps22hh_reset_get(&dev_ctx, &rst);
  } while (rst);
  /*
   *  Enable Block Data Update
   */
  lps22hh_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /*
   * Set Output Data Rate
   */
  lps22hh_data_rate_set(&dev_ctx, LPS22HH_10_Hz_LOW_NOISE);
  return 1;
}

int32_t LPS22HH_Polling(void) {
    /*
     * Read output only if new value is available
     */
    lps22hh_read_reg(&dev_ctx, LPS22HH_STATUS, (uint8_t *)&reg, 1);

    if (reg.status.p_da)
    {
      memset(data_raw_pressure.u8bit, 0x00, sizeof(int32_t));
      lps22hh_pressure_raw_get(&dev_ctx, data_raw_pressure.u8bit);
      pressure_hPa = (float)( data_raw_pressure.i32bit)/4096.0;
      LPS_hPa_x100 = (int32_t)(pressure_hPa * 100);
//      sprintf((char*)tx_buffer, "pressure [hPa]:%6.2f\r\n", pressure_hPa);
//      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }

    if (reg.status.t_da)
    {
      memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
      lps22hh_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
      temperature_degC = (float)( data_raw_temperature.i16bit )/100.0;
      LPS_DegC_x10 = (int32_t)(temperature_degC * 10);
//      sprintf((char*)tx_buffer, "temperature [degC]:%6.2f\r\n", temperature_degC );
//      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
  return 0;
}

void LPS22HH_example_main(void);
void LPS22HH_example_main(void)
{
  LPS22HH_IsPlugged();
  
  /*
   * Read samples in polling mode (no int)
   */
  while(1) LPS22HH_Polling();

}
