/*
  Created by Vincent Fernandes, 06 December 2025.
  This file is part of the Robot-chat project.
 */

#include "motors.h"

Motors right_motor ={
  .timer = &htim1,
  .channel = TIM_CHANNEL_1,
  .speed = 0,
  .direction = 'F'
};

Motors left_motor ={
  .timer = &htim1,
  .channel = TIM_CHANNEL_2,
  .speed = 0,
  .direction = 'F'
};

PID pid_right = {
  .Kd = 0,
  .Kp = 0, 
  .Ki = 0,
  .error = 0,
  .integral_error = 0,
  .derivate_error = 0
};

PID pid_left = {
  .Kd = 0,
  .Kp = 0, 
  .Ki = 0,
  .error = 0,
  .integral_error = 0,
  .derivate_error = 0
};

Robot robot = {
  .right_motor = &right_motor,
  .left_motor = &left_motor,
  .right_pid = &pid_right,
  .left_pid = &pid_left
};



void motor_right_start_fwd(void){
    HAL_TIM_PWM_Start(robot.right_motor->timer, robot.right_motor->channel);
}
void motor_right_start_rev(){
    HAL_TIMEx_PWMN_Start(robot.right_motor->timer, robot.right_motor->channel);
}
void motor_left_start_fwd(void){
    HAL_TIM_PWM_Start(robot.left_motor->timer, robot.left_motor->channel);
} 
void motor_left_start_rev(){
    HAL_TIMEx_PWMN_Start(robot.left_motor->timer, robot.left_motor->channel);
}
void motor_right_stop_fwd(void){
    HAL_TIM_PWM_Stop(robot.right_motor->timer, robot.right_motor->channel);
}
void motor_right_stop_rev(void){
    HAL_TIMEx_PWMN_Stop(robot.right_motor->timer, robot.right_motor->channel);
}
void motor_left_stop_fwd(void){
    HAL_TIM_PWM_Stop(robot.left_motor->timer, robot.left_motor->channel);
}
void motor_left_stop_rev(void){
      HAL_TIMEx_PWMN_Stop(robot.left_motor->timer, robot.left_motor->channel);
}

void motors_init(void){

  motor_right_start_fwd();
  motor_right_start_rev();
  motor_left_start_fwd();
  motor_left_start_rev();

	__HAL_TIM_MOE_ENABLE(robot.right_motor->timer);
  __HAL_TIM_MOE_ENABLE(robot.left_motor->timer);

	__HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, 0);
	__HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, 0);
}

void motor_forward(uint8_t speed_percent, char motor_id){
    uint32_t duty  = motor_set_speed(speed_percent, motor_id);
    if(motor_id == 'R'){
      motor_right_start_fwd();
      motor_right_stop_rev();
      __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, duty);
    }
    else if (motor_id == 'L'){
      motor_left_start_fwd();
      motor_left_stop_rev();
      __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, duty);
    }
}

void motor_backward(uint8_t speed_percent, char motor_id){
    uint32_t duty  = motor_set_speed(speed_percent, motor_id);
    if(motor_id == 'R'){
      motor_right_start_rev();
      motor_right_stop_fwd();
      __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, duty);
    }
    else if (motor_id == 'L'){
      motor_left_start_rev();
      motor_left_stop_fwd();
      __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, duty);
    }
}


uint32_t motor_set_speed(uint8_t speed_percent, char motor_id){
    if(speed_percent > 100){
        speed_percent = 100;
    }
    else if (speed_percent < 30)
    {
        speed_percent = 30;
    }

    if(motor_id == 'R'){
        uint32_t duty = (robot.right_motor->timer->Init.Period * speed_percent) / 100;
        return duty;
    }
    else if(motor_id == 'L'){
        uint32_t duty = (robot.left_motor->timer->Init.Period * speed_percent) / 100;
        return duty;
    }
  return 0;
}

