/*
 * brillez.c
 *
 *  Created on: Nov 6, 2024
 *      Author: laurentf
 */

#include "brillez.h"

h_ws2812b_t h_ws2812b = {
		.htim = &htim16,
		.tim_channel = TIM_CHANNEL_1
};

void brillez_init()
{
	ws2812b_init(&h_ws2812b);
}


void one_on(int led_number,uint32_t color){
	ws2812b_set_led_hex(&h_ws2812b, led_number, color);
	ws2812b_send_buffer(&h_ws2812b);

};

void all_off(){
	for(int i=0; i<WS2812B_LED_NUMBER; i++){
		ws2812b_set_led_hex(&h_ws2812b,i,0);
		ws2812b_send_buffer(&h_ws2812b);
	};
};

void lidar(float theta){
	int index = (int)((float)theta/3.75); //96 LEDs, 360/96=3.75
	all_off();
	vTaskDelay(pdMS_TO_TICKS(100));
	one_on(index, 0xFF0000);
	vTaskDelay(pdMS_TO_TICKS(500));
	all_off();
};

void led_tof_above(){
	all_off();
	vTaskDelay(pdMS_TO_TICKS(100));
	for(int i=0;i<WS2812B_LED_NUMBER;i++){
		one_on(i, 0x00FF00);
		vTaskDelay(pdMS_TO_TICKS(50));
	};
	vTaskDelay(pdMS_TO_TICKS(500));
	all_off();
};