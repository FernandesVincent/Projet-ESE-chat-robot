/*
 * ControlMotors.c
 *
 *  Created on: Oct 1, 2025
 *      Author: pcail
 */

#include "ControlMotors.h"
#include <stdlib.h>

#define pi 3.1415
#define PWM_MAX 8500 	//ARR+1
#define SPEED_MAX 100	//so speed between -100 et 100 so that when 100 we have PWMspeed=PWM_MAX

//functions motor
void Motor_Init(Motor_HandleTypeDef* motor, TIM_HandleTypeDef* timer, uint32_t channel){
	motor->pwm_timer = timer; //assigning pointers to initial values
	motor->channel = channel;
	motor->direction = M_STOP;
	motor->spd = 0;
}

void Motor_SetSpeed(Motor_HandleTypeDef* motor, int speed){
	int PWMspeed = (int)(abs(speed) * PWM_MAX / SPEED_MAX);
	motor->spd = speed;

	if(speed >= 0){
		Motor_SetDir(motor, 'F');
		__HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->channel, PWMspeed);
	}

	if(speed <= 0){
		Motor_SetDir(motor, 'R');
		__HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->channel, PWMspeed);
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

//functions robots
void Robot_Init(Robot_struct* robot, Motor_HandleTypeDef* motor_R, Motor_HandleTypeDef* motor_L){
	robot->Right_Motor = motor_R;
	robot->Left_Motor = motor_L;
	robot->direction = 'S';
	robot->role = 0; //CAN CHANGE, FOR NOW 0 IS FOR CAT AND ONE FOR MOUSE
	robot->speed = 0;
	robot->omega = 0;
	robot->theta = 0;
}

void Robot_Forward(Robot_struct* robot){
	Motor_SetSpeed(robot->Right_Motor, 40);		//speed est à definir entre +ou- SPEED_MAX
	Motor_SetSpeed(robot->Left_Motor, 40);		//il peut aussi y avoir un delta entre les deux car moteurs pas formement 100% identiques
	robot->direction = 'F';
}

void Robot_Reverse(Robot_struct* robot){
	Motor_SetSpeed(robot->Right_Motor, -40);		//speed est à definir entre +ou- SPEED_MAX
	Motor_SetSpeed(robot->Left_Motor, -40);
	robot->direction = 'R';
}

void Robot_Stop(Robot_struct* robot){
	Motor_Stop(robot->Right_Motor);
	Motor_Stop(robot->Left_Motor);
	robot->direction = 'S';
}

void Robot_Angle(Robot_struct* robot, float angle){		//calcule les nouvelles vitesse pour que le robot tourne vers la cible
														//puis remet la vietesse pour qu'il aille droit
	;
}











