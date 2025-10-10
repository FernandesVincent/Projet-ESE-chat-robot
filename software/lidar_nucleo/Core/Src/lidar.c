/*
 * lidar.c
 *
 *  Created on: Oct 1, 2025
 *      Author: Kelly
 */

#include <math.h>
#include "lidar.h"
#include <stdio.h>

extern uint8_t lidar_buffer[UART_RX_BUFFER_SIZE];
extern uint8_t UART1_RxBuffer[UART_RX_BUFFER_SIZE];
extern uint16_t taille;

#define MAX_LSN 360

uint16_t PH; //Bytes 0 & 1, fixed at 0x55AA
uint8_t CT; //Byte 2, if CT[bit(0)]==1 beginning lap of data, if CT[bit(0)]==0 point cloud packet, all others bit (7:1) reserved
uint8_t LSN; //Byte 3, numbers of sampling points
uint16_t FSA; //Bytes 4 & 5, angle of the first sample point
uint16_t LSA; //Bytes 6 & 7, angle of the last sample point
uint16_t CS; //Bytes 8 & 9, checksum
uint16_t SI[MAX_LSN]; //S1 on Bytes 10 & 11 then S2 on 12 & 13 etc..., distance data of the sampling point

int checksum_flag = 0;
float led_angle = 0;
float led_distance = 0;
double Angle[MAX_LSN];
float distance_mm[MAX_LSN];


void lidar_check_frames(){
	PH = (uint16_t)(lidar_buffer[1] << 8 | lidar_buffer[0]);
	CT = lidar_buffer[2];
	LSN = lidar_buffer[3];
	FSA = (uint16_t)(lidar_buffer[5] << 8 | lidar_buffer[4]);
	LSA = (uint16_t)(lidar_buffer[7] << 8 | lidar_buffer[6]);
	CS = (uint16_t)(lidar_buffer[9] << 8 | lidar_buffer[8]);

	if (PH == 0x55AA)
	{
		for (int i = 0; i < LSN; i++){
			SI[i] = (uint16_t)(lidar_buffer[i*2+11] << 8 | lidar_buffer[i*2+10]);
		}
	}
}
void lidar_checksum_test(){
	if (PH == 0x55AA){
		//		for (int i = 0; i < LSN; i++)
		//			SI[i] = (uint16_t)(lidar_buffer[i*2+11] << 8 | lidar_buffer[i*2+10]);
//		printf("PH=0x%X CT=0x%X LSN=%d FSA=%d LSA=%d CS=%d\n", PH, CT, LSN, FSA, LSA, CS);

		uint16_t checksumcal = PH;
		checksumcal ^= FSA;
		checksumcal ^= (uint16_t)(LSN << 8 | CT);
		checksumcal ^= LSA;

		for (int i = 0; i<LSN; i++){
			checksumcal ^= SI[i];
		}
		if (checksumcal == CS && (CT & 0x01) == 0x00)
		{
			checksum_flag = 1;
		}
	}
}

void lidar_get_angle_and_distance(){
	if(checksum_flag == 1){
		double Angle_FSA = (FSA >> 1) / 64.0;
		double Angle_LSA = (LSA >> 1) / 64.0;
		double Angle_diff = Angle_LSA - Angle_FSA;

		if (Angle_diff < 0)
			Angle_diff += 360.0;

		for (int i = 0; i < LSN; i++){

			if(LSN > 1){
				Angle[i] = (i+1) * Angle_diff/(LSN-1) + Angle_FSA;
			}
			else{
				Angle[i] = Angle_FSA;
			}

			distance_mm[i] = SI[i] / 4.0f;

			if(distance_mm[i] > 0)
			{
				double AngCorrect = atan(21.8 * (155.3 - distance_mm[i]) / (155.3 * distance_mm[i]));
				Angle[i] += AngCorrect * 180.0 / M_PI;
			}

			if (Angle[i] >= 360){
				Angle[i] -= 360.0;
			}

			if (distance_mm[i] > 0.0 && distance_mm[i] < 200)
			{
				Valid_point point = {Angle[i], distance_mm[i]};

				static int counter = 0;
				if(counter++ % 50 == 0)
				{
					led_angle = point.angle;
					led_distance = point.distance;

					printf("object detecte a %f mm vers %f degre \r\n", point.distance, point.angle);
				}
			}
		}
		checksum_flag = 0;
	}
//	else{
//		printf("CS_flag incorrect\r\n");
//	}
}
