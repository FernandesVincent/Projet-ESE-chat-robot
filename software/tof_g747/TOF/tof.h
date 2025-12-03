#ifndef __TOF_H__
#define __TOF_H__ 

#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include "stm32g4xx_hal.h"
#include <stdio.h>
#include "i2c.h"



typedef struct {
	VL53L0X_Dev_t dev;				// device handler for VL53L0X API calls

	uint8_t dev_address;			// slave address of TOF device
	GPIO_TypeDef* shutdown_port;  	// shutdown port of TOF device
	uint16_t shutdown_pin;			// shutdown pin of TOF device
	uint16_t int_pin;				// INT pin of TOF device

} tof_dev;


void tof_init_all();


void tof_read(uint16_t GPIO_Pin);

void tof_thresholded(uint16_t GPIO_Pin);


#endif /* __TOF_H__ */
#ifndef __TOF_H__
#define __TOF_H__

#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include "stm32g4xx_hal.h"
#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include <stdio.h>
#include "i2c.h"
#include "gpio.h"

extern VL53L0X_Dev_t dev;
extern VL53L0X_RangingMeasurementData_t measure;
extern uint8_t data;
extern uint8_t VHV;
extern uint8_t Phase;
extern uint32_t refSpadCount;
extern uint8_t isAperture;
extern uint8_t dataReady;

void VL53L0X_InitSensor();
void VL53L0X_ReadDistance();


void tof_init();
void tof_read(uint16_t GPIO_Pin);

#endif /* __TOF_H__ */
