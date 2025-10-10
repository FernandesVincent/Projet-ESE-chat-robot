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
#define L xx //width between the two wheels

// Structure for the motor
typedef struct Motor_HandleTypeDef {
	TIM_HandleTypeDef* pwm_timer;    // Timer for the PWM
	uint32_t channel;                // Channel for the timer
	char direction;                  // Mode/direction of the motor
	int spd;                         // speed of the motor
} Motor_HandleTypeDef;


//Structure for the robot
//a faire après que les moteurs tournent


//prototyping the functions
void Motor_Init(Motor_HandleTypeDef* motor, TIM_HandleTypeDef* timer, uint32_t channel);
void Motor_SetSpeed(Motor_HandleTypeDef* motor, int spd);
void Motor_SetDir(Motor_HandleTypeDef* motor, char direction);
void Motor_Start(Motor_HandleTypeDef* motor);
void Motor_Stop(Motor_HandleTypeDef* motor);
//reste les prototypes du robot à faire


#endif /* INC_CONTROLMOTORS_H_ */
