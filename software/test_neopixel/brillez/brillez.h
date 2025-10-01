/*
 * brillez.h
 *
 *  Created on: Nov 6, 2024
 *      Author: laurentf
 */

#ifndef BRILLEZ_H_
#define BRILLEZ_H_

#include <stdint.h>

void brillez_init();
void chaser_forward(uint32_t color, uint32_t delay);
void chaser_backward(uint32_t color, uint32_t delay);
void chaser_from_center(uint32_t color, uint32_t delay);
void chaser_to_center(uint32_t color, uint32_t delay);
void blink(uint32_t color, uint32_t delay);

#endif /* BRILLEZ_H_ */
