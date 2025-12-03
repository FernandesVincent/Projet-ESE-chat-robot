///*
// * accel.c
// *
// *  Created on: Nov 19, 2025
// *      Author: pcail
// */
//
//#include "accel.h"
//#include <stdio.h>
////#include "cmsis_os.h"
//
//static uint8_t ADXL343_I2C_Read(ADXL343_STRUCT *Instance, uint8_t reg, uint8_t *buf, uint16_t size)
//{
//	if (Instance == NULL || buf == NULL || size == 0){
//		return 1;
//	}
//	if (HAL_I2C_Mem_Read(Instance->hi2c, Instance->devAD, reg, I2C_MEMADD_SIZE_8BIT, buf, size, HAL_MAX_DELAY) != HAL_OK){
//		return 1;
//	}
//	return 0;
//}
//
//static uint8_t ADXL_I2C_Write(ADXL343_STRUCT *Instance, uint8_t reg, uint8_t *buf, uint16_t size){
//	if (Instance == NULL || buf == NULL || size == 0){
//			return 1;
//		}
//	if (HAL_I2C_Mem_Write(Instance->hi2c, Instance->devAD, reg, I2C_MEMADD_SIZE_8BIT, buf, size, HAL_MAX_DELAY) != HAL_OK){
//			return 1;
//		}
//		return 0;
//}
//
//void AXDL_Init(ADXL343_STRUCT *Accel, I2C_HandleTypeDef *hi2c){
//	if(accel == NULL){
//		printf("Accel not found");
//	}
//
//	Accel->accelx = 0.;
//	Accel->accely = 0.;
//	Accel->accelz = 0.;
//	uint8_t deviceID = 1;
//	Accel->hi2c = hi2c;
//	uint8_t configData;
//	Accel->devAD = ADXL_ADRESS;
//
//	//verifying ID
//	if (ADXL343_I2C_Read(Accel, ADXL_REG_DEVID, &deviceID, 1) != 0){
//			prinf("error reading right ID");
//	}
//	if (deviceID != ADXL_DEVID){
//		printf("wrong device ID");
//	}
//	//verif pwr_ctrl
//	configData = 0x08;
//	if (ADXL343_I2C_Write(Accel, ADXL_REG_PWR_CTRL, &configData, 1) != 0){
//		printf("erreur config PWR_CTRL");
//	}
//	//config data format
//	configData = 0x8B; //self test bit + full res + range (here 0b11 ie 16g)
//	if (ADXL343_I2C_Write(Accel, ADXL_REG_DATA_FORMAT, &configData, 1) != 0){
//		printf("erreur data format");
//	}
//	//config
//}
//
//
//uint8_t ADXL_Set_Offset_Values(ADXL343_STRUCT *Accel, uint8_t *offsetX, uint8_t *offsetY, uint8_t offsetZ){
//	uint8_t offsetData[3];
//	offsetData[0] = offsetY;
//	offsetData[1] = offsetZ;
//
//
//}
//
//
//
//
//
//
//
//
//
//
