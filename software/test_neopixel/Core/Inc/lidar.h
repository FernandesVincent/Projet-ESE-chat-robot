/*
 * tof.h
 *
 *  Created on: Oct 8, 2025
 *      Author: Vincent
 */

#ifndef __LIDAR_H__
#define __LIDAR_H__


#include "main.h"

#define UART_RX_BUFFER_SIZE 256

typedef struct {
	float angle;
	float distance;
} Valid_point;

void lidar_check_frames();
void lidar_checksum_test();
void lidar_get_angle_and_distance();

#endif
