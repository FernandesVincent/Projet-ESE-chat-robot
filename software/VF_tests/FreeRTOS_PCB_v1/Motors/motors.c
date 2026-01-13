// /*
//   Created by Vincent Fernandes, 06 December 2025.
//   This file is part of the Robot-chat project.
//  */

// #include "motors.h"

// Motors right_motor ={
//   .timer = &htim8,
//   .channel = TIM_CHANNEL_3,
//   .speed = 0,
//   .direction = 'F'
// };

// Motors left_motor ={
//   .timer = &htim4,
//   .channel = TIM_CHANNEL_2,
//   .speed = 0,
//   .direction = 'F'
// };

// PID pid_right = {
//   .Kd = 0,
//   .Kp = 0, 
//   .Ki = 0,
//   .error = 0,
//   .prev_error = 0,
//   .sum_error = 0,
//   .diff_error = 0
// };

// PID pid_left = {
//   .Kd = 0,
//   .Kp = 0, 
//   .Ki = 0,
//   .error = 0,
//   .prev_error = 0,
//   .sum_error = 0,
//   .diff_error = 0
// };

// Robot robot = {
//   .right_motor = &right_motor,
//   .left_motor = &left_motor,
//   .right_pid = &pid_right,
//   .left_pid = &pid_left
// };



// void motor_right_start_fwd(void){
//     HAL_TIM_PWM_Start(robot.right_motor->timer, robot.right_motor->channel);
// }
// void motor_right_start_rev(){
//     HAL_TIMEx_PWMN_Start(robot.right_motor->timer, robot.right_motor->channel);
// }
// void motor_left_start_fwd(void){
//     HAL_TIM_PWM_Start(robot.left_motor->timer, robot.left_motor->channel);
// } 
// void motor_left_start_rev(){
//     HAL_TIMEx_PWMN_Start(robot.left_motor->timer, robot.left_motor->channel);
// }
// void motor_right_stop_fwd(void){
//     HAL_TIM_PWM_Stop(robot.right_motor->timer, robot.right_motor->channel);
// }
// void motor_right_stop_rev(void){
//     HAL_TIMEx_PWMN_Stop(robot.right_motor->timer, robot.right_motor->channel);
// }
// void motor_left_stop_fwd(void){
//     HAL_TIM_PWM_Stop(robot.left_motor->timer, robot.left_motor->channel);
// }
// void motor_left_stop_rev(void){
//       HAL_TIMEx_PWMN_Stop(robot.left_motor->timer, robot.left_motor->channel);
// }

// // TODO : Change parameters in motors movements in order to use speed in "m/s, motor_id, time" as parameters
// // TODO : Make some others functions like ramp up or ramp down to accelerate and decelerate.
// // TODO : maybe put PID structures inside motors instead of robot 
// void motors_init(void){

//   motor_right_start_fwd();
//   motor_right_start_rev();
//   motor_left_start_fwd();
//   motor_left_start_rev();

// 	__HAL_TIM_MOE_ENABLE(robot.right_motor->timer);
//   __HAL_TIM_MOE_ENABLE(robot.left_motor->timer);

// 	__HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, 0);
// 	__HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, 0);
// }

// void motors_stop_all(){
//   __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, 0);
// 	__HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, 0);

//   motor_right_stop_fwd();
//   motor_right_stop_rev();
//   motor_left_stop_fwd();
//   motor_left_stop_rev();
// }


// void motor_forward(uint8_t speed_percent, char motor_id){
//     uint32_t duty  = motor_set_speed(speed_percent, motor_id);
//     if(motor_id == 'R'){
//       motor_right_start_fwd();
//       motor_right_stop_rev();
//       __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, duty);
//     }
//     else if (motor_id == 'L'){
//       motor_left_start_fwd();
//       motor_left_stop_rev();
//       __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, duty);
//     }
// }

// void motor_backward(uint8_t speed_percent, char motor_id){
//     uint32_t duty  = motor_set_speed(speed_percent, motor_id);
//     if(motor_id == 'R'){
//       motor_right_start_rev();
//       motor_right_stop_fwd();
//       __HAL_TIM_SET_COMPARE(robot.right_motor->timer, robot.right_motor->channel, duty);
//     }
//     else if (motor_id == 'L'){
//       motor_left_start_rev();
//       motor_left_stop_fwd();
//       __HAL_TIM_SET_COMPARE(robot.left_motor->timer, robot.left_motor->channel, duty);
//     }
// }

// void motor_turn(uint8_t speed_right, uint8_t speed_left, char direction){
//     uint32_t duty_right  = motor_set_speed(speed_right, 'R');
//     uint32_t duty_left  = motor_set_speed(speed_left, 'L');
//     if(direction == 'F'){
//       motor_forward(duty_right, 'R');
//       motor_forward(duty_left, 'L');
//     }
//     else if(direction == 'B'){
//       motor_backward(duty_right, 'R');
//       motor_backward(duty_left, 'L');     
//     }
//     else{
//       printf("Direction is not valid\r\n");
//     }
// }

// uint32_t motor_set_speed(uint8_t speed_percent, char motor_id){
//     if(speed_percent > 100){
//         speed_percent = 100;
//     }
//     else if (speed_percent < 30)
//     {
//         speed_percent = 30;
//     }

//     if(motor_id == 'R'){
//         uint32_t duty = (robot.right_motor->timer->Init.Period * speed_percent) / 100;
//         return duty;
//     }
//     else if(motor_id == 'L'){
//         uint32_t duty = (robot.left_motor->timer->Init.Period * speed_percent) / 100;
//         return duty;
//     }
//   return 0;
// }

// //////////////////////////////////////////////////////////////////////////////////////
// /*Encoder Part*/
// //////////////////////////////////////////////////////////////////////////////////////
// /*TODO : Test PID to have values for Kd, Kp, Ki. Probably using a PI instead of a PID
// make a table to register all real speed values for a given time to check how the PID is working*/

// void start_encoder(){
//   HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

// }

// #define CPR 224.4f   

// int32_t prev_pos = 0;
// const float dt = 0.01f;

// float get_real_rpm(void){
//     int32_t pres_pos = (int32_t)__HAL_TIM_GET_COUNTER(&htim3);
    

//     int32_t delta = pres_pos - prev_pos;

//     if(delta < 0){
//       printf("Going Backward\r\n");
//     }
//     else if(delta > 0){
//       printf("Going Forward \r\n");
//     }
//     else{
//       printf("Motor Stopped\r\n");
//     }

//     prev_pos = pres_pos;

//     float cps = delta / dt;
//     float rpm = (cps / CPR) * 60.0f;

//     return rpm;
// }

// // int past_error_right = 0;
// // int sum_error_right = 0;
// // int past_error_left = 0;
// // int sum_error_left = 0;

// // void compute_pid(uint8_t speed_th, float speed_real, char motor_id){
// //     if (motor_id == 'R'){
// //       int Kd = robot.right_pid->Kd;
// //       int Ki = robot.right_pid->Ki;
// //       int Kp = robot.right_pid->Kp;
// //     }

// // }

// uint8_t compute_pid_right(uint8_t speed_th, float speed_real){
//     uint8_t pid_out = 0; 
//     uint8_t speed_PWM = 0;
//     robot.right_pid->error = speed_th - speed_real;
//     pid_out += robot.right_pid->Kp* robot.right_pid->error;

//     robot.right_pid->sum_error += robot.right_pid->error *dt;
//     pid_out += robot.right_pid->Ki * robot.right_pid->sum_error;

//     robot.right_pid->diff_error = (robot.right_pid->error - robot.right_pid->prev_error)/dt;
//     pid_out += robot.right_pid->Kd * robot.right_pid->diff_error;

//     speed_PWM = pid_out / 100 * robot.right_motor->timer->Init.Period;
//     return speed_PWM;
// }



