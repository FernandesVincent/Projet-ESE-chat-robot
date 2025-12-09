#ifndef __TOF_H__
#define __TOF_H__ 

#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include "stm32l4xx_hal.h"
#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include <stdio.h>
#include "i2c.h"

extern VL53L0X_Dev_t dev;
extern VL53L0X_RangingMeasurementData_t measure;
extern uint8_t data;
extern uint8_t VHV;
extern uint8_t Phase;
extern uint32_t refSpadCount;
extern uint8_t isAperture;
extern uint8_t dataReady;


typedef struct{
  VL53L0X_Dev_t dev;

}TOF;

void VL53L0X_InitSensor();
void VL53L0X_ReadDistance();

#endif /* __TOF_H__ */