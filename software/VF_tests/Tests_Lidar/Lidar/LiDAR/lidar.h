/*
 * lidar.h
 *
 *  Created on: Oct 8, 2025
 *      Author: fernandesvincent
 */

#ifndef __LIDAR_H__
#define __LIDAR_H__

#include "main.h"
#include <math.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>


#define UART_RX_BUFFER_SIZE 256
#define MAX_LSN 255 // Maximum number of sampling points
#define ANGLE_RESOLUTION_NUM 6   // 0.6°
#define ANGLE_RESOLUTION_DEN 10
#define NUMBER_OF_POINTS_PER_LAP ((360 * ANGLE_RESOLUTION_DEN / ANGLE_RESOLUTION_NUM) * 12 / 10)
#define CALCULUS_RESOLUTION 10 // number of digits after the decimal point for angle and distance calculations
#define THRESHOLD_DISTANCE_OBJECT_DETECTION (1000 * CALCULUS_RESOLUTION) // 1000 mm with calculus resolution
#define DELTA_OBJECT_DETECTION (300 * CALCULUS_RESOLUTION) // 300 mm with calculus resolution
#define OBJECT_SIZE_LIMIT (200 * CALCULUS_RESOLUTION) // 200 mm with calculus resolution
#define NUMBER_OF_OBJECTS 10

typedef struct {
	float angle;
	float distance;
} Valid_point;

typedef struct {
	uint16_t PH; //Bytes 0 & 1, fixed at 0x55AA
	uint8_t CT; //Byte 2, if CT[bit(0)]==1 beginning lap of data, if CT[bit(0)]==0 point cloud packet, all others bit (7:1) reserved
	uint8_t LSN; //Byte 3, numbers of sampling points
	uint16_t FSA; //Bytes 4 & 5, angle of the first sample point
	uint16_t LSA; //Bytes 6 & 7, angle of the last sample point
	uint16_t CS; //Bytes 8 & 9, checksum
	uint16_t SI[MAX_LSN]; //S1 on Bytes 10 & 11 then S2 on 12 & 13 etc..., distance data of the sampling points
	int checksum_flag;
} Frame;

typedef struct{
	int32_t angle; // in degree * CALCULUS_RESOLUTION
	int32_t distance; // in mm * CALCULUS_RESOLUTION
} Point;

typedef struct{
	int32_t distance[2]; // in mm * CALCULUS_RESOLUTION
	int32_t angle[2]; // in degree * CALCULUS_RESOLUTION
} Object;
void lidar_frame_parse(Frame *lidar_frame);
void lidar_frame_checksum_test(Frame *lidar_frame);
void lidar_frame_get_distance(Frame *lidar_frame);
void lidar_frame_get_angle(Frame *lidar_frame);
void lidar_lap_add_frame(Frame *lidar_frame);
void lidar_print_lap_points();
#endif
