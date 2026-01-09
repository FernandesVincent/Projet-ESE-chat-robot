/*
 * ADXL343.h
 *
 *  Created on: Dec 16, 2025
 *      Author: Kelly
 */

#ifndef ADXL343_H_
#define ADXL343_H_

#include "i2c.h"
#include "gpio.h"


typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint16_t int_pin;

} h_ADXL343_t;


void ADXL343_Init(h_ADXL343_t *h_ADXL343);
void ADXL343_IntCallback(h_ADXL343_t *h_ADXL343, uint16_t GPIO_Pin);


#endif /* ADXL343_H_ */
