#include "motors.h"

/* ===================== MOTORS ===================== */

Motors_pcb_v1 right_motor_v1 ={
  .timer = &htim8,
  .channel_fwd = TIM_CHANNEL_2,
  .channel_rev = TIM_CHANNEL_3,
  .speed = 0,
  .direction = 'F'
};

Motors_pcb_v1 left_motor_v1 ={
  .timer = &htim4,
  .channel_fwd = TIM_CHANNEL_1,
  .channel_rev = TIM_CHANNEL_2,
  .speed = 0,
  .direction = 'F'
};

/* ===================== PID ===================== */

PID pid_right = {
  .Kd = 0,
  .Kp = 0, 
  .Ki = 0,
  .error = 0,
  .prev_error = 0,
  .sum_error = 0,
  .diff_error = 0
};

PID pid_left = {
  .Kd = 0,
  .Kp = 0, 
  .Ki = 0,
  .error = 0,
  .prev_error = 0,
  .sum_error = 0,
  .diff_error = 0
};

/* ===================== ROBOT ===================== */

Robot_pcb_v1 robot = {
  .right_motor = &right_motor_v1,
  .left_motor  = &left_motor_v1,
  .right_pid   = &pid_right,
  .left_pid    = &pid_left
};

/* ===================== MOTOR CONTROL ===================== */
/* ---------- RIGHT MOTOR ---------- */

void motor_right_start_fwd(void){
  HAL_TIM_PWM_Start(robot.right_motor->timer, robot.right_motor->channel_fwd);
}

void motor_right_start_rev(void){
  HAL_TIM_PWM_Start(robot.right_motor->timer, robot.right_motor->channel_rev);
}

void motor_right_stop_fwd(void){
  HAL_TIM_PWM_Stop(robot.right_motor->timer, robot.right_motor->channel_fwd);
}

void motor_right_stop_rev(void){
  HAL_TIM_PWM_Stop(robot.right_motor->timer, robot.right_motor->channel_rev);
}

/* ---------- LEFT MOTOR ---------- */

void motor_left_start_fwd(void){
  HAL_TIM_PWM_Start(robot.left_motor->timer, robot.left_motor->channel_fwd);
}

void motor_left_start_rev(void){
  HAL_TIM_PWM_Start(robot.left_motor->timer, robot.left_motor->channel_rev);
}

void motor_left_stop_fwd(void){
  HAL_TIM_PWM_Stop(robot.left_motor->timer, robot.left_motor->channel_fwd);
}

void motor_left_stop_rev(void){
  HAL_TIM_PWM_Stop(robot.left_motor->timer, robot.left_motor->channel_rev);
}

/* ===================== INIT ===================== */

void motors_init(void){

  // motor_right_start_fwd();
  // motor_right_start_rev();
  // motor_left_start_fwd();
  // motor_left_start_rev();

  motors_stop_all();

  // __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel_fwd, 0);
  // __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel_rev, 0);

  // __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel_fwd, 0);
  // __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel_rev, 0);
}

/* ===================== STOP ===================== */

void motors_stop_all(){

  motor_stop_right();
  motor_stop_left();
}

void motor_stop_right(){

  __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel_fwd, 0);
  __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel_rev, 0);

  motor_right_stop_fwd();
  motor_right_stop_rev();
}

void motor_stop_left(){

  __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel_fwd, 0);
  __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel_rev, 0);

  motor_left_stop_fwd();
  motor_left_stop_rev();
}

/* ===================== MOVEMENTS ===================== */

void motor_forward(uint8_t speed_percent, char motor_id, int duration_ms){
  uint32_t duty = motor_set_speed(speed_percent, motor_id);

  if(motor_id == 'R'){
    motor_right_start_fwd();
    motor_right_stop_rev();
    motor_speed_ramp_up(duty, 'R', "FWD");
  }
  else if(motor_id == 'L'){
    motor_left_start_fwd();
    motor_left_stop_rev();
    motor_speed_ramp_up(duty, 'L', "FWD");
  }
  if(duration_ms > 0){
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    if(motor_id == 'R'){
      motor_stop_right();
    }
    else if(motor_id == 'L'){
      motor_stop_left();
    }
  }
}

void motor_backward(uint8_t speed_percent, char motor_id, int duration_ms){
  uint32_t duty = motor_set_speed(speed_percent, motor_id);

  if(motor_id == 'R'){
    motor_right_start_rev();
    motor_right_stop_fwd();
    motor_speed_ramp_up(duty, 'R', "REV");
  }
  else if(motor_id == 'L'){
    motor_left_start_rev();
    motor_left_stop_fwd();
    motor_speed_ramp_up(duty, 'L', "REV");

  }
  if(duration_ms > 0){
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    if(motor_id == 'R'){
      motor_stop_right();
    }
    else if(motor_id == 'L'){
      motor_stop_left();
    }
  }
}

void motor_turn(uint8_t speed_right, uint8_t speed_left, char direction, int duration_ms){

  if(direction == 'F'){
    motor_forward(speed_right, 'R', duration_ms);
    motor_forward(speed_left, 'L', duration_ms);
  }
  else if(direction == 'B'){
    motor_backward(speed_right, 'R', duration_ms);
    motor_backward(speed_left, 'L', duration_ms);
  }
  else{
    printf("Direction is not valid\r\n");
  }
}

/* ===================== SPEED ===================== */

uint32_t motor_set_speed(uint8_t speed_percent, char motor_id){
  if(speed_percent > 100){
    speed_percent = 100;
  }
  if(motor_id == 'R'){
    return (robot.right_motor->timer->Init.Period * speed_percent) / 100;
  }
  else if(motor_id == 'L'){
    return (robot.left_motor->timer->Init.Period * speed_percent) / 100;
  }
  return 0;
}

void motor_speed_ramp_up(uint32_t objective, char motor_id, char *direction){
  uint32_t duty_step = objective / 10;
  if (duty_step == 0) {
    duty_step = 1;
  }
  for (uint32_t duty = 0; duty <= objective; duty += duty_step)
  {
    if (motor_id == 'R') {
      if(strcmp(direction, "FWD") == 0){
        __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel_fwd, duty);
      }
      else{
        __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel_rev, duty);
      }
    }
    else if (motor_id == 'L') {
      if(strcmp(direction, "FWD") == 0){
        __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel_fwd, duty);
      }
      else{
        __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel_rev, duty);
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
