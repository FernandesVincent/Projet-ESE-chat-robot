/*
  Created by Vincent Fernandes, 29 December 2025.
  This file is part of the Robot-chat project.
*/

#include "ADXL343.h"

ADXL343_registers_values ADXL343_REG_VAL;

void ADXL343_init(void){
    // Initialization code for ADXL343 accelerometer
    // Configure all aspects as needed then enable measurement
} 

void ADXL343_read_id(void){
  HAL_I2C_Mem_Read(&hi2c1,ADXL343_DEV_ADDRESS, ADXL343_DEVID, I2C_MEMADD_SIZE_8BIT, &ADXL343_REG_VAL.DEVID, 1, HAL_MAX_DELAY);
}

void ADXL343_enable_mesaurement(void){
  uint8_t power_ctl_value = 0x08; // Set the Measure bit (D3) to 1
  HAL_I2C_Mem_Write(&hi2c1, ADXL343_DEV_ADDRESS, ADXL343_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &power_ctl_value, 1, HAL_MAX_DELAY);
}

void ADXL343_enable_low_power_mode(void){
  uint8_t bw_rate_value = 0x10; // Set the Low Power bit (D4) to 1
  HAL_I2C_Mem_Write(&hi2c1, ADXL343_DEV_ADDRESS, ADXL343_BW_RATE, I2C_MEMADD_SIZE_8BIT, &bw_rate_value, 1, HAL_MAX_DELAY);
}

void ADXL343_enable_auto_sleep_mode(void){
}

void ADXL343_enable_standby_mode(void){
  uint8_t power_ctl_value = 0x00; // Clear the Measure bit (D3) to enter standby mode
  HAL_I2C_Mem_Write(&hi2c1, ADXL343_DEV_ADDRESS, ADXL343_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &power_ctl_value, 1, HAL_MAX_DELAY);
}

void ADXL343_enable_interrupts(void){
  uint8_t int_enable_value = 0x80; // Enable Data Ready interrupt (D7)
}
