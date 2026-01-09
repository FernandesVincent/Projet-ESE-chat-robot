/*
  Created by Vincent Fernandes, 29 December 2025.
  This file is part of the Robot-chat project.
*/

#ifndef ADXL343_H
#define ADXL343_H

#include <stdint.h>
#include "i2c.h"
extern I2C_HandleTypeDef hi2c1;

typedef enum {
  ADXL343_DEV_ADDRESS = 0x53 <<1,
  ADXL343_DEVID = 0x00,
  ADXL343_THRESH_TAP = 0x1D,
  ADXL343_OFSX = 0x1E,
  ADXL343_OFSY = 0x1F,
  ADXL343_OFSZ = 0x20,
  ADXL343_DUR = 0x21,
  ADXL343_LATENT = 0x22,
  ADXL343_WINDOW = 0x23,
  ADXL343_THRESH_ACT = 0x24,
  ADXL343_THRESH_INACT = 0x25,
  ADXL343_TIME_INACT = 0x26,
  ADXL343_ACT_INACT_CTL = 0x27,
  ADXL343_THRESH_FF = 0x28,
  ADXL343_TIME_FF = 0x29,
  ADXL343_TAP_AXES = 0x2A,
  ADXL343_ACT_TAP_STATUS = 0x2B,
  ADXL343_BW_RATE = 0x2C,
  ADXL343_POWER_CTL = 0x2D,
  ADXL343_INT_ENABLE = 0x2E,
  ADXL343_INT_MAP = 0x2F,
  ADXL343_INT_SOURCE = 0x30,
  ADXL343_DATA_FORMAT = 0x31,
  ADXL343_DATAX0 = 0x32,
  ADXL343_DATAX1 = 0x33,
  ADXL343_DATAY0 = 0x34,
  ADXL343_DATAY1 = 0x35,
  ADXL343_DATAZ0 = 0x36,
  ADXL343_DATAZ1 = 0x37,
  ADXL343_FIFO_CTL = 0x38,
  ADXL343_FIFO_STATUS = 0x39
} ADXL343_registers_addresses;

typedef struct {
  uint8_t DEVID;
  uint8_t THRESH_TAP;
  uint8_t OFSX;
  uint8_t OFSY;
  uint8_t OFSZ;
  uint8_t DUR;
  uint8_t LATENT;
  uint8_t WINDOW;
  uint8_t THRESH_ACT;
  uint8_t THRESH_INACT;
  uint8_t TIME_INACT;
  uint8_t ACT_INACT_CTL;
  uint8_t THRESH_FF;
  uint8_t TIME_FF;
  uint8_t TAP_AXES;
  uint8_t ACT_TAP_STATUS;
  uint8_t BW_RATE;
  uint8_t POWER_CTL;
  uint8_t INT_ENABLE;
  uint8_t INT_MAP;
  uint8_t INT_SOURCE;
  uint8_t DATA_FORMAT;
  uint8_t DATAX0;
  uint8_t DATAX1;
  uint8_t DATAY0;
  uint8_t DATAY1;
  uint8_t DATAZ0;
  uint8_t DATAZ1;
  uint8_t FIFO_CTL;
  uint8_t FIFO_STATUS;

} ADXL343_registers_values;

#endif /* ADXL343_H */