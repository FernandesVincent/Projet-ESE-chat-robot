/*
 * lidar.c
 *
 *  Created on: Oct 1, 2025
 *      Author: fernandesvincent
 */

#include "lidar.h"

extern uint8_t lidar_buffer[UART_RX_BUFFER_SIZE];


Point lap_points[NUMBER_OF_POINTS_PER_LAP];
Point frame_points[MAX_LSN];

void lidar_frame_parse(Frame *lidar_frame){
	lidar_frame->PH = (uint16_t)(lidar_buffer[1] << 8 | lidar_buffer[0]);
	lidar_frame->CT = lidar_buffer[2];
	lidar_frame->LSN = lidar_buffer[3];
	
	lidar_frame->FSA = (uint16_t)(lidar_buffer[5] << 8 | lidar_buffer[4]);
	lidar_frame->LSA = (uint16_t)(lidar_buffer[7] << 8 | lidar_buffer[6]);
	lidar_frame->CS = (uint16_t)(lidar_buffer[9] << 8 | lidar_buffer[8]);

	if (lidar_frame->PH == 0x55AA)
	{
		for (int i = 0; i < lidar_frame->LSN; i++){
			lidar_frame->SI[i] = (uint16_t)(lidar_buffer[i*2+11] << 8 | lidar_buffer[i*2+10]);
		}
	}
}

void lidar_frame_checksum_test(Frame *lidar_frame){
		uint16_t checksumcal;
		checksumcal = lidar_frame->PH;
		checksumcal ^= lidar_frame->FSA;
		checksumcal ^= (uint16_t)(lidar_frame->LSN << 8 | lidar_frame->CT);
		checksumcal ^= lidar_frame->LSA;
		for (int i = 0; i<lidar_frame->LSN; i++){
			checksumcal ^= lidar_frame->SI[i];
		}
		if (checksumcal == lidar_frame->CS && (lidar_frame->CT & 0x01) == 0x00){
			lidar_frame->checksum_flag = 1;
		}
		else{
			lidar_frame->checksum_flag = 0;
		}
}

void lidar_frame_get_distance(Frame *lidar_frame){
	for (int i = 0; i < lidar_frame->LSN; i++){
		frame_points[i].distance = (int32_t)(lidar_frame->SI[i] * CALCULUS_RESOLUTION / 4);
		if (frame_points[i].distance > 2000 * CALCULUS_RESOLUTION){
			frame_points[i].distance = 2000 * CALCULUS_RESOLUTION;
		}
	}
}

void lidar_frame_get_angle(Frame *lidar_frame){
	if(lidar_frame->checksum_flag == 1){
		int32_t Angle_FSA = (int32_t)(((lidar_frame->FSA >> 1) / 64)* CALCULUS_RESOLUTION);
		int32_t Angle_LSA = (int32_t)(((lidar_frame->LSA >> 1) / 64)* CALCULUS_RESOLUTION);
		int32_t Angle_diff = (int32_t)(Angle_LSA - Angle_FSA);

		if (Angle_diff < 0){
			Angle_diff += 360.0 * CALCULUS_RESOLUTION;
		}

		lidar_frame_get_distance(lidar_frame);

		for (int i = 0; i < lidar_frame->LSN; i++){
			if(lidar_frame->LSN > 1){
				frame_points[i].angle = (i+1) * Angle_diff/(lidar_frame->LSN-1) + Angle_FSA;
			}
			else{
				frame_points[i].angle = Angle_FSA;
			}

			if(frame_points[i].distance > 0){
				double AngCorrect = atan(21.8 * (155.3 - (double)frame_points[i].distance / CALCULUS_RESOLUTION)/ (155.3 * ((double)frame_points[i].distance / CALCULUS_RESOLUTION)));
				frame_points[i].angle += (int32_t)(AngCorrect * 180.0 / M_PI * CALCULUS_RESOLUTION);
			}

			if (frame_points[i].angle >= 360 * CALCULUS_RESOLUTION){
				frame_points[i].angle -= 360 * CALCULUS_RESOLUTION;
			}
		}
		lidar_frame->checksum_flag = 0;
	}
	else{
		printf("CS_flag incorrect\r\n");
	}
}

void lidar_lap_add_frame(Frame *lidar_frame){
	static int lap_index = 0;
	if (lidar_frame->CT & 0x01){ // Beginning lap of data
		lidar_print_lap_points(); // Print previous lap points
		lap_index = 0;
	}
	else{ // Point cloud packet
		for (int i = 0; i < lidar_frame->LSN; i++){
			if (lap_index < NUMBER_OF_POINTS_PER_LAP){
				lap_points[lap_index].angle = frame_points[i].angle;
				lap_points[lap_index].distance = frame_points[i].distance;
				lap_index++;
			}
		}
	}
}

void lidar_print_lap_points(){
    for (int i = 0; i < NUMBER_OF_POINTS_PER_LAP; i++){
        int32_t angle_fixed = lap_points[i].angle;
        int32_t distance_fixed = lap_points[i].distance;

        printf("Point %d: Angle = %" PRId32 ".%03" PRId32 " deg, Distance = %" PRId32 ".%03" PRId32 " mm\r\n", 
               i, 
               angle_fixed / CALCULUS_RESOLUTION, 
               angle_fixed % CALCULUS_RESOLUTION,
               distance_fixed / CALCULUS_RESOLUTION, 
               distance_fixed % CALCULUS_RESOLUTION);
    }
}


void lidar_object_detection(Object *objects){
	int object_count = 0;
	int max_distance = 0;
	
	for (int i = 0; i < NUMBER_OF_POINTS_PER_LAP; i++) {
		if (lap_points[i].distance > max_distance) {
			max_distance = lap_points[i].distance;
		}
	}

	int j = 1; 
	while (j < NUMBER_OF_POINTS_PER_LAP - 1) {
		int delta = max_distance - lap_points[j].distance;
		if (delta >= DELTA_OBJECT_DETECTION && object_count < NUMBER_OF_OBJECTS){
			objects[object_count].distance[0] = lap_points[j].distance;
			objects[object_count].angle[0] = lap_points[j].angle;

			while (j < NUMBER_OF_POINTS_PER_LAP - 1 && (max_distance - lap_points[j].distance) >= DELTA_OBJECT_DETECTION){
				j++;
			}
			objects[object_count].distance[1] = lap_points[j-1].distance;
			objects[object_count].angle[1] = lap_points[j-1].angle;
			object_count++;
		}
		j++;
	}
}

void lidar_object_calculate_size(Object *detected_objects, int object_count){
	for (int i = 0; i < object_count; i++){
		int32_t angle_start = detected_objects[i].angle[0];
		int32_t angle_end = detected_objects[i].angle[1];
		int32_t d1 = detected_objects[i].distance[0];
		int32_t d2 = detected_objects[i].distance[1];

		int32_t angle_diff = angle_end - angle_start;
		if (angle_diff < 0) {
				angle_diff += 360 * CALCULUS_RESOLUTION;
		}

		double angle_rad = ((double)angle_diff / CALCULUS_RESOLUTION) * (M_PI / 180.0);
		double size = sqrt((double)d1 * d1 + (double)d2 * d2 - 2.0 * d1 * d2 * cos(angle_rad));
		if(size >= OBJECT_SIZE_LIMIT_MIN && size <= OBJECT_SIZE_LIMIT_MAX){
			detected_objects[i].valid = true;
		}
		else{
			detected_objects[i].valid = false;
		}
	}
}
int32_t distance_to_object;
int32_t angle_to_object;

void lidar_object_get_distance(Object *detected_objects, int object_count){
	int j;
	int min_distance = THRESHOLD_DISTANCE_OBJECT_DETECTION + 1;
	for(int i = 0; i < object_count; i++){
		if(detected_objects[i].valid){
			for (j = 0; j < NUMBER_OF_POINTS_PER_LAP; j++) {
				if (lap_points[j].distance < min_distance) {
					min_distance = lap_points[j].distance;
				}
			}
			distance_to_object = min_distance;
			angle_to_object = lap_points[j].angle;
		}
	}
} 