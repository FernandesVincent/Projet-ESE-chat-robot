/*
 * ControlMotors.c
 *
 *  Created on: Oct 1, 2025
 *      Author: pcail
 */

#include "ControlMotors.h"
#include <stdlib.h>

#define pi 3.1415
#define PWM_MAX 8500 	//à vérifier avec le microcontroller qu'on a
#define SPEED_MAX 500	//idem

void Motor_Init(Motor_HandleTypeDef* motor, TIM_HandleTypeDef* timer, uint32_t channel){
	motor->pwm_timer = timer; //assigning pointers to initial values
	motor->channel = channel;
	motor->direction = M_STOP;
	motor->spd = 0;
}

void Motor_SetSpeed(Motor_HandleTypeDef* motor, int speed){
	int PWMspeed = (int)(speed * PWM_MAX / SPEED_MAX);

}
