/*
 * tof.h
 *
 *  Created on: Oct 8, 2025
 *      Author: Vincent
 */

#ifndef __LIDAR_H__
#define __LIDAR_H__

#include "main.h"

#define LIDAR_UART_RX_BUFFER_SIZE 256

typedef struct {
	uint16_t taille;
	uint8_t data[LIDAR_UART_RX_BUFFER_SIZE];
} Trame;

typedef struct {
	int debut;
	int fin;
} Cluster_point;

typedef struct {
	int angle;
	float distance;
} Cible_lidar;

typedef struct {
	int sample_number;
	double *angle;
	float *distance_mm;
} Parsed_data;


void lidar_init();

void lidar_callback_from_isr(UART_HandleTypeDef *huart, uint16_t Size);
Parsed_data *lidar_parsing_data();
void lidar_sampling(Parsed_data *data, float *cercle, float distance_max_mm);

void lidar_detection_target(float *cercle);

Cible_lidar detecter_lidar(float *cercle);

#endif
