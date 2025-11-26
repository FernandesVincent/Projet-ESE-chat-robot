/*
 * accel.c
 *
 *  Created on: Nov 19, 2025
 *      Author: pcail
 */

#include "accel.h"
#include <stdio.h>
//#include "cmsis_os.h"

static void ADXL_I2C_Read(){

}

static void ADXL_I2C_Write(){

}

void AXDL_Init(ADXL343_STRUCT *accel, I2C_HandleTypeDef *hi2c){
	if(accel == NULL){
		prinf("Accel not found");
	}

	accel->accelx = 0.;
	accel->accely = 0.;
	accel->accelz = 0.;
	accel->devID = 1;
	accel->hi2c = hi2c;
	uint8_t configData;


}
