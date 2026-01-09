/*
 * ADXL343.c
 *
 *  Created on: Dec 16, 2025
 *      Author: Kelly
 */


#include "ADXL343.h"

#include <stdio.h>


#define ADXL343_ADDR        (0x53 << 1)
#define REG_DEVID           0x00
#define REG_POWER_CTL       0x2D
#define REG_DATA_FORMAT     0x31
#define REG_BW_RATE         0x2C
#define REG_INT_ENABLE      0x2E
#define REG_INT_MAP         0x2F
#define REG_INT_SOURCE      0x30
#define REG_DATAX0          0x32

#define REG_THRESH_TAP      0x1D
#define REG_DUR				0x21
#define REG_LATENT			0x22
#define REG_WINDOW			0x23
#define REG_TAP_AXES		0x2A
#define REG_ACT_TAP_SOURCE  0x32

#define MEASURE_MODE     	(1 << 3)
#define FULL_RES_BIT        (1 << 3)
#define RANGE_16G           0x03
#define DATA_READY_BIT     	(1 << 7)
#define SINGLE_TAP     		(1 << 6)

#define INT_PIN 			GPIO_PIN_9


static void ADXL343_Write(h_ADXL343_t *h_ADXL343, uint8_t reg, uint8_t value)
{
	uint8_t val;

	HAL_I2C_Mem_Write(h_ADXL343->hi2c, ADXL343_ADDR, reg, 1, &value, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(h_ADXL343->hi2c, ADXL343_ADDR, &val, 1, HAL_MAX_DELAY);
	if (val != value)
		printf("ADXL343 error writing in register 0x%02X, value returned :  0x%02X\r\n", reg, val);

}

static void ADXL343_Read(h_ADXL343_t *h_ADXL343, uint8_t reg, uint8_t *buffer, uint8_t len)
{
	HAL_I2C_Mem_Read(h_ADXL343->hi2c, ADXL343_ADDR, reg, 1, buffer, len, HAL_MAX_DELAY);
}


void ADXL343_ReadXY(h_ADXL343_t *h_ADXL343, int16_t *x, int16_t *y)
{
	uint8_t raw[4];

	// Lecture : X0, X1, Y0, Y1
	ADXL343_Read(h_ADXL343, REG_DATAX0, raw, 4);

	*x = (int16_t)((raw[1] << 8) | raw[0]);
	*y = (int16_t)((raw[3] << 8) | raw[2]);
}


void ADXL343_Init(h_ADXL343_t *h_ADXL343)
{
	uint8_t devid = 0;
	uint8_t val = 0;

	ADXL343_Read(h_ADXL343, REG_DEVID, &devid, 1);

	printf("ADXL343 Device ID: 0x%02X\r\n", devid);
	if (devid != 0xE5) {
		Error_Handler();
	}

	ADXL343_Write(h_ADXL343, REG_POWER_CTL, MEASURE_MODE);

	ADXL343_Write(h_ADXL343, REG_INT_ENABLE, 0);

	ADXL343_Write(h_ADXL343, REG_DATA_FORMAT, FULL_RES_BIT | RANGE_16G);


	ADXL343_Write(h_ADXL343, REG_INT_ENABLE, 0);


	ADXL343_Write(h_ADXL343, REG_THRESH_TAP, 70);		// THRESH_TAP = 0.5 g
	ADXL343_Write(h_ADXL343, REG_DUR, 90);				// DUR = 50ms / 0.625ms ≈ 80 → 0x50
	ADXL343_Write(h_ADXL343, REG_LATENT, 0);
	ADXL343_Write(h_ADXL343, REG_WINDOW, 0);
	ADXL343_Write(h_ADXL343, REG_TAP_AXES, 0x06);

	ADXL343_Write(h_ADXL343, REG_BW_RATE, 0x0B);		// 200 Hz

	ADXL343_Write(h_ADXL343, REG_INT_MAP, SINGLE_TAP);
	ADXL343_Write(h_ADXL343, REG_INT_ENABLE, SINGLE_TAP);


	ADXL343_Read(h_ADXL343, REG_ACT_TAP_SOURCE, &val, 1);
	printf("ADXL343 ACT TAP source : 0x%02X\r\n", val);

	ADXL343_Read(h_ADXL343, REG_INT_SOURCE, &val, 1);
	printf("ADXL343 int source : 0x%02X\r\n", val);

}


void ADXL343_IntCallback(h_ADXL343_t *h_ADXL343, uint16_t GPIO_Pin)
{
	if (GPIO_Pin == h_ADXL343->int_pin)
	{
		printf("okkk TAP DETECTED\r\n");

		uint8_t val;

		ADXL343_Read(h_ADXL343, REG_INT_SOURCE, &val, 1);
		printf("ADXL343 int source : 0x%02X\r\n", val);

	}
}

