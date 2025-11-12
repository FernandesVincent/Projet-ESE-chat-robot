/*
 * ControlMotors.h
 *
 *  Created on: Oct 1, 2025
 *      Author: pcail
 */

#ifndef INC_CONTROLMOTORS_H_
#define INC_CONTROLMOTORS_H_
#include "tim.h"

#define M_FORWARD 'F'
#define M_REVERSE 'R'
#define M_STOP 'S'

//Période d'échantillonage à rajouter? Celle du lidar?
#define L 0.17 //width between the two wheels in m

// Structure for the motor
typedef struct Motor_HandleTypeDef {
	TIM_HandleTypeDef* pwm_timer;    // Timer for the PWM
	uint32_t channel;                // Channel for the timer
	char direction;                  // Mode/direction of the motor
	int spd;                         // speed of the motor
} Motor_HandleTypeDef;


//Structure for the robot
typedef struct Mvt_frame {
	char direction;		//direction
	float speed;		//vitesse
	float omega;		//vitesse angulaire
	float theta;		//angle de detection
	uint8_t role;		//role
	struct Motor_HandleTypeDef Right_Motor;	//2 moteurs
	struct Motor_HandleTypeDef Left_Motor;
} Robot_struct;


//prototyping the functions
//motors
void Motor_Init(Motor_HandleTypeDef* motor, TIM_HandleTypeDef* timer, uint32_t channel);
void Motor_SetSpeed(Motor_HandleTypeDef* motor, int spd);
void Motor_SetDir(Motor_HandleTypeDef* motor, char direction);
void Motor_Forward(Motor_HandleTypeDef* motor);
void Motor_Reverse(Motor_HandleTypeDef* motor);
void Motor_Stop(Motor_HandleTypeDef* motor);
//robot
void Robot_Init(Robot_struct* robot, Motor_HandleTypeDef* motor_R, Motor_HandleTypeDef* motor_L);
void Robot_Forward(Robot_struct* robot);
void Robot_Reverse(Robot_struct* robot);
void Robot_Stop(Robot_struct* robot);
void Robot_SetAngle(Robot_struct* robot, float angle);


#endif /* INC_CONTROLMOTORS_H_ */
