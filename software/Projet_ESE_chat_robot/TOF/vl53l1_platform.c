/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l1_platform.h"
#include "stm32l4xx_hal.h"
#include "vl53l1_error_codes.h"
#include <string.h>
#include <time.h>
#include <math.h>

#define VL53L1_I2C_TIMEOUT 100

extern I2C_HandleTypeDef hi2c1;


int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	return HAL_I2C_Mem_Write(&hi2c1, dev, index,
	                             I2C_MEMADD_SIZE_16BIT, pdata, count, VL53L1_I2C_TIMEOUT);
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	return HAL_I2C_Mem_Read(&hi2c1, dev, index,
	                            I2C_MEMADD_SIZE_16BIT, pdata, count, VL53L1_I2C_TIMEOUT);
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	return HAL_I2C_Mem_Write(&hi2c1, dev, index, I2C_MEMADD_SIZE_16BIT, &data, 1, VL53L1_I2C_TIMEOUT);
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	uint8_t buffer[2];
	buffer[0] = data >> 8;
	buffer[1] = data & 0xFF;

	return HAL_I2C_Mem_Write(&hi2c1, dev, index,
							 I2C_MEMADD_SIZE_16BIT, buffer, 2, VL53L1_I2C_TIMEOUT);
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	uint8_t buffer[4];
	buffer[0] = (data >> 24) & 0xFF;
	buffer[1] = (data >> 16) & 0xFF;
	buffer[2] = (data >> 8) & 0xFF;
	buffer[3] = data & 0xFF;

	return HAL_I2C_Mem_Write(&hi2c1, dev, index,
							 I2C_MEMADD_SIZE_16BIT, buffer, 4, VL53L1_I2C_TIMEOUT);

}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	return HAL_I2C_Mem_Read(&hi2c1, dev, index, I2C_MEMADD_SIZE_16BIT, data, 1, VL53L1_I2C_TIMEOUT);
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	uint8_t buffer[2];
	VL53L1_Error status = HAL_I2C_Mem_Read(&hi2c1, dev, index,
										   I2C_MEMADD_SIZE_16BIT, buffer, 2, VL53L1_I2C_TIMEOUT);
	*data = ((uint16_t)buffer[0] << 8) | buffer[1];
	return status;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	uint8_t buffer[4];
	VL53L1_Error status = HAL_I2C_Mem_Read(&hi2c1, dev, index,
										   I2C_MEMADD_SIZE_16BIT, buffer, 4, VL53L1_I2C_TIMEOUT);
	*data = ((uint32_t)buffer[0] << 24) |
			((uint32_t)buffer[1] << 16) |
			((uint32_t)buffer[2] << 8)  |
			buffer[3];
	return status;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	HAL_Delay(wait_ms);
	return 0; // to be implemented
}
