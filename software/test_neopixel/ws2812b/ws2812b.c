/*
 * ws2812b.c
 *
 *  Created on: Nov 6, 2024
 *      Author: laurentf
 */

#include "ws2812b.h"

#include "main.h"

int ws2812b_init(h_ws2812b_t * h_ws2812b)
{
	for (int i = 0 ; i < WS2812B_DATA_LENGTH ; i++)
	{
		h_ws2812b->data[i] = 32;
	}

	h_ws2812b->data[WS2812B_DATA_LENGTH-1] = 0;

	return ws2812b_send_buffer(h_ws2812b);
}

int ws2812b_set_led_hex(h_ws2812b_t * h_ws2812b, uint16_t index, uint32_t hex)
{
	uint8_t red = (hex & 0xFF0000) >> 16;
	uint8_t green = (hex & 0x00FF00) >> 8;
	uint8_t blue = (hex & 0x0000FF);

	ws2812b_set_led_rgb(h_ws2812b, index, red, green, blue);
	return -1;
}

int ws2812b_set_led_rgb(h_ws2812b_t * h_ws2812b, uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	/* convert from rgb to weird ws2812b brg */
	uint32_t hex = (blue << 16) + (red << 8) + (green);

	if (index < WS2812B_LED_NUMBER)
	{
		for (int j = 0 ; j < 24 ; j++)
		{
			if (hex & (1<<j))
			{
				h_ws2812b->data[j + 24*index] = 64;
			}
			else
			{
				h_ws2812b->data[j + 24*index] = 32;
			}
		}

		return 0;
	}
	return 0;
}

int ws2812b_send_buffer(h_ws2812b_t * h_ws2812b)
{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	if (HAL_TIM_PWM_Start_DMA(h_ws2812b->htim, h_ws2812b->tim_channel, (uint32_t *) h_ws2812b->data, WS2812B_DATA_LENGTH) == HAL_OK)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
