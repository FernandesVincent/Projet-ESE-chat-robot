#ifndef __TOF_H__
#define __TOF_H__ 

#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include "stm32g4xx_hal.h"
#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include <stdio.h>
#include "i2c.h"
#include "vl53l0x_platform.h"
#include <stdbool.h>
#include "main.h"
#include "stm32g4xx_hal.h"
#include "vl53l0x_api.h"
#include "vl53l0x_def.h"
#include "vl53l0x_platform.h"
#include "motors.h"


#define TOF_DEFAULT_DEV_ADDR 0x29<<1
#define TOF_I2C hi2c3
#define NUMBER_OF_TOFS 6
#define TIMING_BUDGET_US 200000
#define THRESHOLD 700 // in mm
typedef struct{
  GPIO_TypeDef* Port;
  uint16_t Pin;
}TOF_PinPort; 

typedef struct{
  VL53L0X_Dev_t* dev;
  uint8_t new_adress;
  TOF_PinPort* xshut;
  TOF_PinPort* gpio;
}TOF;

extern bool isStarted; 



void VL53L0X_InitSingleSensor(int index);
int VL53L0X_ReadDistance(int index);

void VL53L0X_Calibration(TOF *t);
void VL53L0X_EnableSingleSensor(int index);
void VL53L0X_ChangeDeviceAddress(TOF *t);
void VL53L0X_EnableAllSensors(void);
void VL53L0X_InitAllSensor(void);
void VL53L0X_Init(TOF *t);
void VL53L0X_Param(TOF *t, uint32_t timing_budget_us, VL53L0X_DeviceModes mode);
void VL53L0X_StartMeasure(int index);
bool VL53L0X_IsAboveThreshold(int index);
#endif /* __TOF_H__ */