#ifndef __TOF_H__
#define __TOF_H__ 

#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include "stm32g4xx_hal.h"
#include <stdio.h>
#include "i2c.h"



typedef struct {
	VL53L0X_Dev_t *dev;				// device handler for VL53L0X API calls

	uint8_t dev_address;			// slave address of TOF device
	GPIO_TypeDef* shutdown_port;  	// shutdown port of TOF device
	uint16_t shutdown_pin;			// shutdown pin of TOF device
	uint16_t int_pin;				// INT pin of TOF device

} tof_dev;


void tof_init_all();


void tof_read(uint16_t GPIO_Pin);

void tof_thresholded(uint16_t GPIO_Pin);


#endif /* __TOF_H__ */

