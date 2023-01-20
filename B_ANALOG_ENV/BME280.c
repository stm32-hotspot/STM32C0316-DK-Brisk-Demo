/**
  ******************************************************************************
  * @file    BME280.c (Plumbing to bosch's SW maintained drivers)
  * @author  S.Ma
  * @brief   Application to Vendor specific API Adaptation layer
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

#include "bme280_defs.h"
#include "bme280_support.h"

#include "bme280.h"


// slave address is 0xEC/ED
I2C_SlaveDevice_t gSTModBME280 =       { &gI2C_STMod, 0xEC, 1 }; // plug and play on STMod+

struct bme280_dev BME280dev;
struct bme280_data comp_data;

int32_t bme280_pressure;
int32_t bme280_temperature;
int32_t bme280_humidity;//zobi

int32_t BME280_Report(u8fifo_t* pL) { // where to send out result with base char key
  
  BTEL_Printf(pL,"*I%C*J%D*K%D", (int32_t)(comp_data.pressure *100) , (int32_t)(comp_data.temperature *10), (int32_t)(comp_data.humidity *10) ); // O and P
  return 0;
}

int8_t BME280_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t BME280_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

  gSTModBME280.SubAdrBytes[0] = reg_addr;
  gSTModBME280.pReadByte = reg_data;
  gSTModBME280.ReadByteCount = len;
  gSTModBME280.pWriteByte = 0;
  gSTModBME280.WriteByteCount = 0;
  I2C_MasterIO_AccessSlave(&gSTModBME280); // do the transaction

  return rslt;
}

int8_t BME280_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t BME280_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
  int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

  gSTModBME280.SubAdrBytes[0] = reg_addr;
  gSTModBME280.pWriteByte = reg_data;
  gSTModBME280.WriteByteCount = len;
  gSTModBME280.pReadByte = 0;
  gSTModBME280.ReadByteCount = 0;
  I2C_MasterIO_AccessSlave(&gSTModBME280); // do the transaction

  return rslt;
}

void BME280_delay_ms(uint32_t period);
void BME280_delay_ms(uint32_t period)
{
  HAL_Delay(period);
}
  
int32_t BME280_IsPlugged(void) {

  int8_t rslt = BME280_OK;
  uint8_t settings_sel;
    
  BME280dev.dev_id = BME280_I2C_ADDR_PRIM;
  BME280dev.intf = BME280_I2C_INTF;
  BME280dev.read = BME280_i2c_read;
  BME280dev.write = BME280_i2c_write;
  BME280dev.delay_ms = BME280_delay_ms;

  rslt = bme280_init(&BME280dev);

  /* Recommended mode of operation: Indoor navigation */
  BME280dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  BME280dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  BME280dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  BME280dev.settings.filter = BME280_FILTER_COEFF_16;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

  rslt = bme280_set_sensor_settings(settings_sel, &BME280dev);
  
  if(rslt==BME280_OK) return 1; // present
  
  return 0; // not present
}

int32_t BME280_Polling(void) { // should be called every 50msec (40+msec)

  int8_t rslt;
  //printf("Temperature, Pressure, Humidity\r\n");
  /* Continuously stream sensor data */
  rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &BME280dev);
  /* Wait for the measurement to complete and print data @25Hz */
  // BME280dev.delay_ms(40);
  rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &BME280dev);
  // Temp x 100 degC
  // Humid x 1024 RH%
  // pressure x 100 hPA
  // print_sensor_data(&comp_data);
  bme280_pressure = comp_data.pressure;
  bme280_temperature = comp_data.temperature;
  bme280_humidity = comp_data.humidity;
  
  return 0;
}
