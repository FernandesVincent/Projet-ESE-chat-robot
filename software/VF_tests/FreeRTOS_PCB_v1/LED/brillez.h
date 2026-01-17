/*
 * brillez.h
 *
 *  Created on: Nov 6, 2024
 *      Author: laurentf
 */

#ifndef BRILLEZ_H_
#define BRILLEZ_H_

#include <stdint.h>
#include "tim.h"
#include "ws2812b.h"
#include "motors.h"
#include "tof.h"


void brillez_init();
void chaser_forward(uint32_t color, uint32_t delay);
void chaser_backward(uint32_t color, uint32_t delay);
void chaser_from_center(uint32_t color, uint32_t delay);
void chaser_to_center(uint32_t color, uint32_t delay);
void blink(uint32_t color, uint32_t delay);
void one_on(int led_number,uint32_t color);
void all_off();
void lidar(float theta);
void led_tof_above();
#endif /* BRILLEZ_H_ */
