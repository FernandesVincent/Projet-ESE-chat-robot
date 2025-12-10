/*
 * accel.c
 *
 *  Created on: Nov 19, 2025
 *      Author: pcail
 */

#include "accel.h"
#include <stdio.h>
//#include "cmsis_os.h"

static uint8_t ADXL_I2C_Read(ADXL343_STRUCT* Instance, uint8_t regist, uint8_t* buffer, uint16_t size)
{
	if (Instance == NULL || buffer == NULL || size == 0){
		return 1;
	}
	if (HAL_I2C_Mem_Read(Instance->hi2c, Instance->devAD, regist, I2C_MEMADD_SIZE_8BIT, buffer, size, HAL_MAX_DELAY) != HAL_OK){
		return 1;
	}
	return 0;
}

static uint8_t ADXL_I2C_Write(ADXL343_STRUCT* Instance, uint8_t regist, uint8_t* buffer, uint16_t size){
	if (Instance == NULL || buffer == NULL || size == 0){
			return 1;
		}
	if (HAL_I2C_Mem_Write(Instance->hi2c, Instance->devAD, regist, I2C_MEMADD_SIZE_8BIT, buffer, size, HAL_MAX_DELAY) != HAL_OK){
			return 1;
		}
		return 0;
}

uint8_t AXDL_Init(ADXL343_STRUCT *Accel, I2C_HandleTypeDef *hi2c){
	if(accel == NULL){
		printf("Accel not found\r\n");
	}

	Accel->accelx = 0.;
	Accel->accely = 0.;
	Accel->accelz = 0.;
	uint8_t deviceID = 1;
	Accel->hi2c = hi2c;
	uint8_t configData;
	Accel->devAD = ADXL_ADRESS;

	//verifying ID
	if (ADXL343_I2C_Read(Accel, ADXL_REG_DEVID, &deviceID, 1) != 0){
			prinf("error reading right ID\r\n");
			return 1;
	}
	if (deviceID != ADXL_DEVID){
		printf("wrong device ID\r\n");
		return 1;
	}
	//verif pwr_ctrl
	configData = 0x08;
	if (ADXL343_I2C_Write(Accel, ADXL_REG_POWER_CTL, &configData, 1) != 0){
		printf("erreur config PWR_CTRL\r\n");
		return 1;
	}
	//config data format
	configData = 0x8B; //self test bit + full res + range (here 0b11 ie 16g)
	if (ADXL343_I2C_Write(Accel, ADXL_REG_DATA_FORMAT, &configData, 1) != 0){
		printf("erreur data format\r\n");
		return 1;
	}
	//config offset to 0
	ADXL_Set_Offset_Values(Accel, 0, 0, 0);
	//config tap threshold
	//16g so 62.5 mg/LSB
	if (ADXL_Set_Tap_Thres(Accel, 70) != 0){
		printf("error config tap threshold\r\n");	//70*0.0625=4.375g to activate
		return 1;
	}
	//config max tap duration
	//The scale factor is 625 μs/LSB.
	if (ADXL_Set_Tap_Dur(Accel, 90) != 0){
		printf("error config duration\r\n");
		return 1;
	}
	//config axes x, y
	configData = 0x06;		//enable x and y
	if (ADXL_I2C_Write(Accel, ADXL_REG_TAP_AXES, &configData, 1) != 0){
		printf("error config axes\r\n");
		return 1;
	}
	//config freq
	configData = RATE_200HZ;
	if (ADXL_I2C_Write(Accel, ADXL_REG_BW_RATE, &configData, 1) != 2){
		printf("error config freq\r\n");
		return 1;
	}
	//config interruption map
	configData = 0x40;	//only single tap active for ex
	if (ADXL_I2C_Write(Accel, ADXL_REG_INT_MAP, &configData, 1) != 0){
		prinf("error config interruption map\r\n");
		return 1;
	}
	//reset of interrupts
	configData = 0x00;
	if (ADXL_I2C_Write(Accel, ADXL_REG_INT_ENABLE, &configData, 1) != 0){
		printf("erreur reset interrupt\r\n");
		return 1;
	}
	//config init interrupt
	configData = 0x40;	//interrupt only for single tap
	if (ADXL_I2C_Write(Accel, ADXL_REG_INT_ENABLE, &configData, 1) != 0){
		printf("error init interrupt\r\n");
		return 1;
	}
	printf("init finished\r\n");
	return 0;
}


uint8_t ADXL_Set_Offset_Values(ADXL343_STRUCT* Accel, uint8_t offsetX, uint8_t offsetY, uint8_t offsetZ){
	uint8_t offsetData[3];
	offsetData[0] = offsetY;
	offsetData[1] = offsetZ;

	if (ADXL343_I2C_Read(Accel, ADXL_REG_OFSX, offsetData, 3) != 0){
		return 1;
	}
	return 0;
}

uint8_t ADXL_Set_Tap_Thres(ADXL343_STRUCT* Accel, uint8_t threshold){
	if (ADXL_I2C_Write(Accel, ADXL_REG_THRESH_TAP, &threshold, 1) != 0){
		return 1;
	}
	return 0;
}

uint8_t ADXL_Set_Tap_Dur(ADXL343_STRUCT* Accel, uint8_t duration){
	if (ADXL_I2C_Write(Accel, ADXL_REG_DUR, &duration, 1) != 0){
		return 1;
	}
	return 0;
}

uint8_t ADXL_Get_Offset_Values(ADXL343_STRUCT* Accel, uint8_t offsetX, uint8_t offsetY, uint8_t offsetZ){
	;
}







