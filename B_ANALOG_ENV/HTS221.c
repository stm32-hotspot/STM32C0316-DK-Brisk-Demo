/**
  ******************************************************************************
  * @file    HTS221.c (ST Humidity sensor driver)
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

#include "main.h"
#include "brisk.h"
#include "I2C_MasterIO.h"

#include "hts221.h"
#include "hts221_reg.h"


/* STMicroelectronics evaluation boards definition
 *
 * Please uncomment ONLY the evaluation boards in use.
 * If a different hardware is used please comment all
 * following target board and redefine yours.
 */
//#define STEVAL_MKI109V3
//#define NUCLEO_F411RE_X_NUCLEO_IKS01A2


I2C_SlaveDevice_t gSTModHTS221 =       { &gI2C_STMod, 0xBE, 1 }; // plug and play on STMod+

#if defined(STEVAL_MKI109V3)
/* MKI109V3: Define communication interface */
#define SENSOR_BUS hspi2

/* MKI109V3: Vdd and Vddio power supply values */
#define PWM_3V3 915

#elif defined(NUCLEO_F411RE_X_NUCLEO_IKS01A2)
/* NUCLEO_F411RE_X_NUCLEO_IKS01A2: Define communication interface */
#define SENSOR_BUS hi2c1

#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "hts221_reg.h"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static axis1bit16_t data_raw_humidity;
static axis1bit16_t data_raw_temperature;
static float humidity_perc;
static float temperature_degC;
static uint8_t whoamI;
//static uint8_t tx_buffer[1000];


int32_t HTS_RH_x10;
int32_t HTS_DegC_x10;

int32_t HTS221_Report(u8fifo_t* pL) { // where to send out result with base char key
  
  BTEL_Printf(pL,"*A%D*B%D", HTS_RH_x10, HTS_DegC_x10); // A = HTS_RH, B = HTS_DegC
  return 0;
}

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void tx_com(uint8_t *tx_buffer, uint16_t len);
static void platform_init(void);


/*
 *  Function used to apply coefficient
 */
typedef struct {
  float x0;
  float y0;
  float x1;
  float y1;
} lin_t;
float linear_interpolation(lin_t *lin, int16_t x);
float linear_interpolation(lin_t *lin, int16_t x)
{
 return ((lin->y1 - lin->y0) * x +  ((lin->x1 * lin->y0) - (lin->x0 * lin->y1)))
        / (lin->x1 - lin->x0);
}


static  hts221_ctx_t dev_ctx;
static   lin_t lin_hum;
static   axis1bit16_t coeff;
static   lin_t lin_temp;
static    hts221_reg_t reg;
/* Main Example --------------------------------------------------------------*/

int32_t HTS221_IsPlugged(void) {
  /* Initialize platform specific hardware */
  platform_init();

  /* Initialize mems driver interface */
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = 0; // pointer to slave device

  /* Check device ID */
  whoamI = 0;
  hts221_device_id_get(&dev_ctx, &whoamI);
  if ( whoamI != HTS221_ID )
    return 0; /*manage here device not found */

  /* Read humidity calibration coefficient */
  hts221_hum_adc_point_0_get(&dev_ctx, coeff.u8bit);
  lin_hum.x0 = (float)coeff.i16bit;
  hts221_hum_rh_point_0_get(&dev_ctx, coeff.u8bit);
  lin_hum.y0 = (float)coeff.u8bit[0];
  hts221_hum_adc_point_1_get(&dev_ctx, coeff.u8bit);
  lin_hum.x1 = (float)coeff.i16bit;
  hts221_hum_rh_point_1_get(&dev_ctx, coeff.u8bit);
  lin_hum.y1 = (float)coeff.u8bit[0];

  /* Read temperature calibration coefficient */
  hts221_temp_adc_point_0_get(&dev_ctx, coeff.u8bit);
  lin_temp.x0 = (float)coeff.i16bit;
  hts221_temp_deg_point_0_get(&dev_ctx, coeff.u8bit);
  lin_temp.y0 = (float)coeff.u8bit[0];
  hts221_temp_adc_point_1_get(&dev_ctx, coeff.u8bit);
  lin_temp.x1 = (float)coeff.i16bit;
  hts221_temp_deg_point_1_get(&dev_ctx, coeff.u8bit);
  lin_temp.y1 = (float)coeff.u8bit[0];

  /* Enable Block Data Update */
  hts221_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set Output Data Rate */
  hts221_data_rate_set(&dev_ctx, HTS221_ODR_1Hz);
  /* Device power on */
  hts221_power_on_set(&dev_ctx, PROPERTY_ENABLE);
  return 1;
}

int32_t HTS221_Polling(void) {

  /* Initialize platform specific hardware */

    /* Read output only if new value is available */

    hts221_status_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.h_da)
    {
      /* Read humidity data */
      memset(data_raw_humidity.u8bit, 0x00, sizeof(int16_t));
      hts221_humidity_raw_get(&dev_ctx, data_raw_humidity.u8bit);
      humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity.i16bit);
      if (humidity_perc < 0) humidity_perc = 0;
      if (humidity_perc > 100) humidity_perc = 100;
      HTS_RH_x10 = (int32_t)(humidity_perc * 10);
      //sprintf((char*)tx_buffer, "Humidity [%%]:%3.2f\r\n", humidity_perc);
      //tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
    if (reg.status_reg.t_da)
    {
      /* Read temperature data */
      memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
      hts221_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
      temperature_degC = linear_interpolation(&lin_temp, data_raw_temperature.i16bit);
      HTS_DegC_x10 = (int32_t)(temperature_degC * 10);
      //sprintf((char*)tx_buffer, "Temperature [degC]:%6.2f\r\n", temperature_degC );
      //tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
  
    return 0;
}

void HTS221_example_main(void);
void HTS221_example_main(void)
{
  HTS221_IsPlugged();
  while(1) HTS221_Polling();
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len)
{
  
  gSTModHTS221.SubAdrBytes[0] = reg | 0x80;
  gSTModHTS221.pWriteByte = bufp;
  gSTModHTS221.WriteByteCount = len;
  gSTModHTS221.pReadByte = 0;
  gSTModHTS221.ReadByteCount = 0;
  I2C_MasterIO_AccessSlave(&gSTModHTS221); // do the transaction

  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
  
  gSTModHTS221.SubAdrBytes[0] = reg | 0x80;
  gSTModHTS221.pWriteByte = 0;
  gSTModHTS221.WriteByteCount = 0;
  gSTModHTS221.pReadByte = bufp;
  gSTModHTS221.ReadByteCount = len;
  I2C_MasterIO_AccessSlave(&gSTModHTS221); // do the transaction

  return 0;
}

/*
 * @brief  Send buffer to console (platform dependent)
 *
 * @param  tx_buffer     buffer to trasmit
 * @param  len           number of byte to send
 *
 */
static void tx_com(uint8_t *tx_buffer, uint16_t len)
{
  #ifdef NUCLEO_F411RE_X_NUCLEO_IKS01A2
  HAL_UART_Transmit(&huart2, tx_buffer, len, 1000);
  #endif
  #ifdef STEVAL_MKI109V3
  CDC_Transmit_FS(tx_buffer, len);
  #endif
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
static void platform_init(void)
{
#ifdef STEVAL_MKI109V3
  TIM3->CCR1 = PWM_3V3;
  TIM3->CCR2 = PWM_3V3;
  HAL_Delay(1000);
#endif
}

