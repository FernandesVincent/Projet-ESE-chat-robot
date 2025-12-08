/*
  Created by Vincent Fernandes, 06 December 2025.
  This file is part of the Robot-chat project.
*/
#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;

typedef struct {
  TIM_HandleTypeDef* timer;
  uint32_t channel;
  uint8_t speed;
  char direction;
}Motors;

extern Motors right_motor;
extern Motors left_motor;

typedef struct{
  int Kd;
  int Kp;
  int Ki;
  int error;
  int integral_error;
  int derivate_error;
}PID;

extern PID right_pid;
extern PID left_pid; 

typedef struct {
  Motors *right_motor;
  Motors *left_motor;
  PID *right_pid;
  PID *left_pid;
}Robot;




void motor_right_start_fwd(void);
void motor_right_start_rev(void);
void motor_right_stop_fwd(void);
void motor_right_stop_rev(void);
void motor_left_start_fwd(void);
void motor_left_start_rev(void);
void motor_left_stop_fwd(void);
void motor_left_stop_rev(void);

void motors_init(void);
void motor_forward(uint8_t speed_percent, char motor_id);
void motor_backward(uint8_t speed_percent, char motor_id);

uint32_t motor_set_speed(uint8_t speed_percent, char motor_id);
#endif // MOTORS_H //