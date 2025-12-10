/*
 * accel.h
 *
 *  Created on: Nov 19, 2025
 *      Author: pcail
 */

#ifndef INC_ACCEL_H_
#define INC_ACCEL_H_

#include <stdio.h>
#include "i2c.h"

#define ADXL_ADRESS (0x53 << 1)		//adxl343 has i2c on 7 bits so need to put it on 8 bits
#define ADXL_DEVID (0xe5)				//The DEVID register holds a fixed device ID code of 0xE5 (345 octal) (datasheet)


typedef enum Registers{
	ADXL_REG_DEVID 				= 0x00, 			//REG is for register def (datasheet p.23)
	ADXL_REG_THRESH_TAP 		= 0x1d,
	ADXL_REG_OFSX 				= 0x1e,
	ADXL_REG_OFSY 				= 0x1f,
	ADXL_REG_OFSZ 				= 0x20,
	ADXL_REG_DUR 				= 0x21,
	ADXL_REG_ACT_INAT_CTL 		= 0x27,
	ADXL_REG_TAP_AXES 			= 0x2a,
	ADXL_REG_ACT_TAP_STATUS 	= 0x2b,
	ADXL_REG_BW_RATE 			= 0x2c,
	ADXL_REG_POWER_CTL 			= 0x2d,
	ADXL_REG_INT_ENABLE 		= 0x2e,
	ADXL_REG_INT_MAP 			= 0x2f,
	ADXL_REG_INT_SOURCE 		= 0x30,
	ADXL_REG_DATA_FORMAT 		= 0x31,
	ADXL_REG_DATAX0				= 0x32,
	ADXL_REG_DATAX1				= 0x33,
	ADXL_REG_DATAY0				= 0x34,
	ADXL_REG_DATAY1				= 0x35,
	ADXL_REG_DATAZ0				= 0x36,
	ADXL_REG_DATAZ1				= 0x37,
	ADXL_REG_FIFO_CTL 			= 0x38
};

#define ADXL_I2C hi2c

typedef struct ADXL{
	float accelx;
	float accely;
	float accelz;
	uint16_t devAD;	//device adress
	uint8_t devID;
	uint8_t range;
	uint8_t rate;
	I2C_HandleTypeDef *hi2c;
}ADXL343_STRUCT;

typedef enum{
	RATE_3200HZ = 0x0f,		//output data rate (different current consumption for each rate)
	RATE_1600HZ = 0x0e,
	RATE_800HZ = 0x0d,
	RATE_400HZ = 0x0c,
	RATE_200HZ = 0x0b,
	RATE_100HZ = 0x0a,
	RATE_50HZ = 0x09,
	RATE_25HZ = 0x08,
	RATE_12_5HZ = 0x07,
	RATE_6_25HZ = 0x06,
	RATE_3_13HZ = 0x05,
	RATE_1_56HZ = 0x04,
	RATE_0_78HZ = 0x03,
	RATE_0_39HZ = 0x02,
	RATE_0_20HZ = 0x01,
	RATE_0_1OHZ = 0x00
}ADXL_Rate;

typedef enum{
	RANGE_2G = 0b00,		//to set the g range of the ADXL343
	RANGE_4G = 0b01,
	RANGE_8G = 0b10,
	RANGE_16G = 0b11
}ADXL_Range;


void ADXL_Init(ADXL343_STRUCT *accel, I2C_HandleTypeDef *hi2c);


#endif /* INC_ACCEL_H_ */
