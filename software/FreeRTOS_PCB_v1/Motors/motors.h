/*
  Created by Vincent Fernandes, 06 December 2025.
  This file is part of the Robot-chat project.
*/
#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;

typedef struct {
  TIM_HandleTypeDef* timer;
  uint32_t channel;
  uint8_t speed;
  char direction;
}Motors;

typedef struct {
  TIM_HandleTypeDef *timer;
  uint32_t channel_fwd;
  uint32_t channel_rev;
  uint32_t speed;
  char direction;
} Motors_pcb_v1;

extern Motors right_motor;
extern Motors left_motor;

typedef struct{
  int Kd;
  int Kp;
  int Ki;
  int error;
  int prev_error;
  int sum_error;
  int diff_error;
}PID;

extern PID right_pid;
extern PID left_pid; 

typedef struct {
  Motors *right_motor;
  Motors *left_motor;
  PID *right_pid;
  PID *left_pid;
}Robot;


typedef struct {
  Motors_pcb_v1 *right_motor;
  Motors_pcb_v1 *left_motor;
  PID *right_pid;
  PID *left_pid;
}Robot_pcb_v1;


void motor_right_start_fwd(void);
void motor_right_start_rev(void);
void motor_right_stop_fwd(void);
void motor_right_stop_rev(void);
void motor_left_start_fwd(void);
void motor_left_start_rev(void);
void motor_left_stop_fwd(void);
void motor_left_stop_rev(void);

void motors_init(void);
void motor_forward(uint8_t speed_percent, char motor_id, int duration_ms);
void motor_backward(uint8_t speed_percent, char motor_id, int duration_ms);
void motors_stop_all();
void motor_stop_right();
void motor_stop_left();
void motor_turn(uint8_t speed_right, uint8_t speed_left, char direction, int duration_ms);

void test_motor_right(void);
void test_motor_left(void);
void motor_speed_ramp_up(uint32_t objective, char motor_id, char *direction);
uint32_t motor_set_speed(uint8_t speed_percent, char motor_id);
#endif // MOTORS_H //