/*
 * brillez.c
 *
 *  Created on: Nov 6, 2024
 *      Author: laurentf
 */

#include "brillez.h"
#include "ws2812b.h"
#include "tim.h"

h_ws2812b_t h_ws2812b = {
		.htim = &htim1,
		.tim_channel = TIM_CHANNEL_1
};

void brillez_init()
{
	ws2812b_init(&h_ws2812b);
}

void chaser_forward(uint32_t color, uint32_t delay)
{
	for (int i = 0 ; i < WS2812B_LED_NUMBER ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, i, color);
		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}

	for (int i = 0 ; i < WS2812B_LED_NUMBER ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, i, 0x000000);
		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}
}

void chaser_backward(uint32_t color, uint32_t delay)
{
	for (int i = 0 ; i < WS2812B_LED_NUMBER ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, WS2812B_LED_NUMBER-i-1, color);
		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}

	for (int i = 0 ; i < WS2812B_LED_NUMBER ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, WS2812B_LED_NUMBER-i-1, 0x000000);
		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}
}

void chaser_from_center(uint32_t color, uint32_t delay)
{
	uint16_t center = WS2812B_LED_NUMBER/2;

	for (int i = 0 ; i < center ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, center+i, color);
		ws2812b_set_led_hex(&h_ws2812b, center-i-1, color);

		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}

	for (int i = 0 ; i < center ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, center+i, 0);
		ws2812b_set_led_hex(&h_ws2812b, center-i-1, 0);

		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}
}

void chaser_to_center(uint32_t color, uint32_t delay)
{
	uint16_t center = WS2812B_LED_NUMBER/2;

	for (int i = 0 ; i < center ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, i, color);
		ws2812b_set_led_hex(&h_ws2812b, WS2812B_LED_NUMBER-i-1, color);

		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}

	for (int i = 0 ; i < center ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, i, 0);
		ws2812b_set_led_hex(&h_ws2812b, WS2812B_LED_NUMBER-i-1, 0);

		ws2812b_send_buffer(&h_ws2812b);

		HAL_Delay(delay);
	}
}

void blink(uint32_t color, uint32_t delay)
{

	for (int i = 0 ; i < WS2812B_LED_NUMBER ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, i, color);
	}

	ws2812b_send_buffer(&h_ws2812b);

	HAL_Delay(delay);

	for (int i = 0 ; i < WS2812B_LED_NUMBER ; i++)
	{
		ws2812b_set_led_hex(&h_ws2812b, i, 0);
	}

	ws2812b_send_buffer(&h_ws2812b);

	HAL_Delay(delay);

}

void one_on(int led_number,uint32_t color){
	ws2812b_set_led_hex(&h_ws2812b, led_number, color);
	ws2812b_send_buffer(&h_ws2812b);

};

void all_off(){
	for(int i=0; i<WS2812B_LED_NUMBER; i++){
		ws2812b_set_led_hex(&h_ws2812b,i,0);
	};
};

void lidar(float theta){
	int index = (int)((float)theta/3.75);
	all_off();
	HAL_Delay(100);
	one_on(index, 0xFF0000);
//	one_on(index-1, 0x007700);
//	one_on(index+1, 0x007700);
//	one_on(index-2, 0x000044);
//	one_on(index+2, 0x000044);


};
