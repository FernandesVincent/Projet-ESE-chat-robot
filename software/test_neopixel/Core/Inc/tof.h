/*
 * tof.h
 *
 *  Created on: Sep 22, 2025
 *      Author: Kelly
 */

#ifndef INC_TOF_H_
#define INC_TOF_H_

#include "gpio.h"

typedef struct {
	GPIO_TypeDef *port;  // GPIO port (GPIOA, GPIOB, etc.)
	uint16_t pin;        // GPIO pin (GPIO_PIN_0, GPIO_PIN_1, ...)
} Gpio;

typedef struct {
	uint16_t dev_address;	// slave address of TOF device
	Gpio shutdown_pin;		// shutdown pin of TOF device
	Gpio int_pin;			// INT pin of TOF device (to trigger interruption when measure is done)
} tof_dev;


void tof_boot();
void tof_initialization();
void tof_enable_ranging();
void tof_callback(uint16_t GPIO_Pin);
void tof_interrupt_frequency_all(int freq);
int tof_is_above_threshold(int threshold);
int tof_is_below_threshold(int threshold);
int tof_is_between(int low, int high);
void tof_print_distance();
#endif /* INC_TOF_H_ */
