/********************************************************************************
  * @file    BMX055.c
  * @author  MCD Application Team
  * @brief   This file provides the standard USB requests following chapter 9.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *******************************************************************************/

#include "main.h"
#include "brisk.h"
#include "bmm050.h" // if moved lower, causes lots of repeated warnings
#include "bmg160.h" // same

int32_t BMX055_Init(void);
int32_t BMX055_50ms(void);

extern struct bmm050_t bmm050;
extern struct bmg160_t bmg160;

// Magneto
struct bmm050_mag_data_s16_t data;             /* Structure used for read the mag xyz data*/
//struct bmm050_mag_s32_data_t data_s32;         /* Structure used for read the mag xyz data with 32 bit output*/
struct bmm050_mag_s32_data_t BMX_MAG_data_s32;         /* Structure used for read the mag xyz data with 32 bit output*/
struct bmm050_mag_data_float_t data_float;     /* Structure used for read the mag xyz data with float output*/

// Gyro 
s16	v_gyro_datax_s16, v_gyro_datay_s16, v_gyro_dataz_s16 = BMG160_INIT_VALUE;/* variable used for read the sensor data*/
struct bmg160_data_t data_gyro;/* structure used for read the sensor data - xyz*/
struct bmg160_data_t gyro_xyzi_data;/* structure used for read the sensor data - xyz and interrupt status*/

s8 BMM050_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMM050_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMG160_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMG160_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BMG160_delay_msek(u32 msek);
void BMM050_delay_msek(u32 msek);

extern struct bmm050_t bmm050;
extern struct bmg160_t bmg160;

int32_t BMX055_Init(void) {

  /* Variable used to get the data rate*/
  u8 v_data_rate_u8 = BMM050_INIT_VALUE;
  /* Variable used to set the data rate*/
  u8 v_data_rate_value_u8 = BMM050_INIT_VALUE;
  /* result of communication results*/
  s32 com_rslt = ERROR;
  
	bmg160.bus_write = BMG160_I2C_bus_write;
	bmg160.bus_read = BMG160_I2C_bus_read;
	bmg160.delay_msec = BMG160_delay_msek;
	bmg160.dev_addr = BMG160_I2C_ADDR1;
        
	bmm050.bus_write = BMM050_I2C_bus_write;
	bmm050.bus_read = BMM050_I2C_bus_read;
	bmm050.delay_msec = BMM050_delay_msek;
	bmm050.dev_addr = BMM050_I2C_ADDRESS;        

/*	Based on the user need configure I2C or SPI interface.
*	It is sample code to explain how to use the bmm050 API*/
/*--------------------------------------------------------------------------*
*  This function used to assign the value/reference of
*	the following parameters
*	I2C address
*	Bus Write
*	Bus read
*	company_id
*-------------------------------------------------------------------------*/
  com_rslt = bmm050_init(&bmm050);

/*	For initialization it is required to set the mode of
*	the sensor as "NORMAL"
*	but before set the mode needs to configure the power control bit
*	in the register 0x4B bit BMM050_INIT_VALUE should be enabled
*	This bit is enabled by calling bmm050_init function
*	For the Normal data acquisition/read/write is possible in this mode
*	by using the below API able to set the power mode as NORMAL*/
  /* Set the power mode as NORMAL*/
  com_rslt += bmm050_set_functional_state(BMM050_NORMAL_MODE);

	/* This API used to Write the data rate of the sensor, input
	value have to be given
	data rate value set from the register 0x4C bit 3 to 5*/
	v_data_rate_value_u8 = BMM050_DATA_RATE_30HZ;/* set data rate of 30Hz*/
	com_rslt += bmm050_set_data_rate(v_data_rate_value_u8);

	/* This API used to read back the written value of data rate*/
	com_rslt += bmm050_get_data_rate(&v_data_rate_u8);

//=== gyro

	/* variable used for read the gyro bandwidth data*/
	u8	v_gyro_value_u8 = BMG160_INIT_VALUE;
	/* variable used for set the gyro bandwidth data*/
	u8 v_bw_u8 = BMG160_INIT_VALUE;
	/* result of communication results*/
//-	s32 com_rslt = ERROR;

 /*	Based on the user need configure I2C or SPI interface.
  *	It is example code to explain how to use the bmg160 API*/
/*--------------------------------------------------------------------------*
 *  This function used to assign the value/reference of
 *	the following parameters
 *	Gyro I2C address
 *	Bus Write
 *	Bus read
 *	Gyro Chip id
 *----------------------------------------------------------------------------*/
	com_rslt = bmg160_init(&bmg160);
/*----------------------------------------------------------------------------*/
/*	For initialization it is required to set the mode of the sensor as "NORMAL"
 *	data acquisition/read/write is possible in this mode
 *	by using the below API able to set the power mode as NORMAL
 *	NORMAL mode set from the register 0x11 and 0x12
 *	While sensor in the NORMAL mode idle time of at least 2us(micro seconds)
 *	is required to write/read operations
 *	0x11 -> bit 5,7 -> set value as BMG160_INIT_VALUE
 *	0x12 -> bit 6,7 -> set value as BMG160_INIT_VALUE
 *	Note:
 *		If the sensor is in the fast power up mode idle time of least
 *		450us(micro seconds) required for write/read operations
 */

/*-------------------------------------------------------------------------*/
	/* Set the gyro power mode as NORMAL*/
	com_rslt += bmg160_set_power_mode(BMG160_MODE_NORMAL);
/* This API used to Write the bandwidth of the gyro sensor
	input value have to be give 0x10 bit BMG160_INIT_VALUE to 3
	The bandwidth set from the register */
	v_bw_u8 = C_BMG160_BW_230HZ_U8X;/* set gyro bandwidth of 230Hz*/
	com_rslt += bmg160_set_bw(v_bw_u8);

/* This API used to read back the written value of bandwidth for gyro*/
	com_rslt += bmg160_get_bw(&v_gyro_value_u8);
        
  return 0;
}

int32_t BMX055_50ms(void) {

  s32 com_rslt = ERROR;
//************************* START READ SENSOR DATA(X,Y and Z axis) ********
// BMM050 Magneto
  /* accessing the bmm050_mdata parameter by using data*/
  /* Reads the mag x y z data*/
  com_rslt += bmm050_read_mag_data_XYZ(&data);

  /* accessing the bmm050_mdata_float parameter by using data_float*/
  /* Reads mag xyz data output as 32bit value*/
  com_rslt += bmm050_read_mag_data_XYZ_float(&data_float);

  /* accessing the bmm050_mdata_s32 parameter by using data_s32*/
  /* Reads mag xyz data output as float value*/
  com_rslt += bmm050_read_mag_data_XYZ_s32(&BMX_MAG_data_s32);//data_s32);

//************************* END READ SENSOR DATA(X,Y and Z axis) ************
/******************* Read Gyro data xyz**********************/
	com_rslt += bmg160_get_data_X(&v_gyro_datax_s16);/* Read the gyro X data*/

	com_rslt += bmg160_get_data_Y(&v_gyro_datay_s16);/* Read the gyro Y data*/

	com_rslt += bmg160_get_data_Z(&v_gyro_dataz_s16);/* Read the gyro Z data*/

/* accessing the  bmg160_data_t parameter by using data_gyro*/
	com_rslt += bmg160_get_data_XYZ(&data_gyro);/* Read the gyro XYZ data*/

/* accessing the bmg160_data_t parameter by using gyro_xyzi_data*/
/* Read the gyro XYZ data and interrupt status*/
	com_rslt += bmg160_get_data_XYZI(&gyro_xyzi_data);


    return 0;
}

#ifdef BMX055_ENABLED
int32_t BMX055_Report(u8fifo_t* pL) {
  
  BTEL_Printf(pL,"*T%D", (int32_t)(BMX_MAG_data_s32.datax * 10) ); // 
  BTEL_Printf(pL,"*U%D", (int32_t)(BMX_MAG_data_s32.datay * 10) ); // 
  BTEL_Printf(pL,"*V%D", (int32_t)(BMX_MAG_data_s32.dataz * 10) ); // 
  BTEL_Printf(pL,"*W%D", (int32_t)(BMX_MAG_data_s32.resistance * 10) ); // 

  BTEL_Printf(pL,"*X%d", (int32_t)(v_gyro_datax_s16) ); // 
  BTEL_Printf(pL,"*Y%d", (int32_t)(v_gyro_datay_s16) ); // 
  BTEL_Printf(pL,"*Z%d", (int32_t)(v_gyro_dataz_s16) ); // 
  
  // accelero is missing because there is one in the STM32 base board. could be added here
  return 0;
}
#endif
