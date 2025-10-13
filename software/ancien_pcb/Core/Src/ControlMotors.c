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
	motor->spd = speed;

	if(speed >= 0){
		Motor_SetDir(motor, 'F');
		__HAL_TIME_SET_COMPARE(motor->pwm_timer, motor->channel, PWMspeed);
	}

	if(speed <= 0){
		Motor_SetDir(motor, 'R');
		__HAL_TIME_SET_COMPARE(motor->pwm_timer, motor->channel, abs(PWMspeed));
	}
}


void Motor_SetDir(Motor_HandleTypeDef* motor, char direction){
	motor->direction = direction;

	switch(direction){
	case 'F':
		HAL_TIM_PWM_Start(motor->pwm_timer, motor->channel);
		HAL_TIMEx_PWMN_Stop(motor->pwm_timer, motor->channel);
		break;
	case 'R':
		HAL_TIMEx_PWMN_Start(motor->pwm_timer, motor->channel);
		HAL_TIM_PWM_Stop(motor->pwm_timer, motor->channel);
		break;
	case 'S':
		HAL_TIM_PWM_Stop(motor->pwm_timer, motor->channel);
		HAL_TIMEx_PWMN_Stop(motor->pwm_timer, motor->channel);
		break;
	}
}

	void Motor_Forward(Motor_HandleTypeDef* motor){
		Motor_SetDir(motor, 'F');
	}

	void Motor_Reverse(Motor_HandleTypeDef* motor){
		Motor_SetDir(motor, 'R');
	}

	void Motor_Stop(Motor_HandleTypeDef* motor){
		Motor_SetDir(motor, 'S');
	}

	//reste fonctions du robot avec les 2 moteurs












