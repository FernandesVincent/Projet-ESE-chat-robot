/*
 * mp3.c
 *
 *  Created on: Dec 10, 2024
 *      Author: administrateur
 */

#include "mp3.h"

uint8_t mp3_tx_buffer[MP3_TX_BUFFER_LENGTH] = {0};
extern UART_HandleTypeDef huart1;

void mp3_selectStorageDevice() {
	mp3_tx_buffer[0] = MP3_BUFFER_HEADER_0;
	mp3_tx_buffer[1] = MP3_BUFFER_HEADER_1;
	mp3_tx_buffer[2] = MP3_BUFFER_HEADER_2;
	mp3_tx_buffer[3] = MP3_CMD_SELECT_DEVICE;
	mp3_tx_buffer[4] = MP3_NO_FEEDBACK;
	mp3_tx_buffer[5] = MP3_MICRO_SD_STORAGE_H0;
	mp3_tx_buffer[6] = MP3_MICRO_SD_STORAGE_H1;
	mp3_tx_buffer[7] = MP3_BUFFER_TAIL;

	HAL_UART_Transmit(&huart1,mp3_tx_buffer,MP3_TX_BUFFER_LENGTH,10);
}

void mp3_playWithIndex(int track_index) {
	mp3_tx_buffer[0] = MP3_BUFFER_HEADER_0;
	mp3_tx_buffer[1] = MP3_BUFFER_HEADER_1;
	mp3_tx_buffer[2] = MP3_BUFFER_HEADER_2;
	mp3_tx_buffer[3] = MP3_CMD_PLAY_WITH_INDEX;
	mp3_tx_buffer[4] = MP3_NO_FEEDBACK;
	mp3_tx_buffer[5] = (((uint8_t)track_index) >> 8);
	mp3_tx_buffer[6] = (uint8_t)track_index;
	mp3_tx_buffer[7] = MP3_BUFFER_TAIL;

	HAL_UART_Transmit(&huart1,mp3_tx_buffer,MP3_TX_BUFFER_LENGTH,10);
}

void mp3_playFromFolder(int folder, int track) {
	mp3_tx_buffer[0] = MP3_BUFFER_HEADER_0;
	mp3_tx_buffer[1] = MP3_BUFFER_HEADER_1;
	mp3_tx_buffer[2] = MP3_BUFFER_HEADER_2;
	mp3_tx_buffer[3] = MP3_CMD_PLAY_FROM_FOLDER;
	mp3_tx_buffer[4] = MP3_NO_FEEDBACK;
	mp3_tx_buffer[5] = (uint8_t)folder;
	mp3_tx_buffer[6] = (uint8_t)track;
	mp3_tx_buffer[7] = MP3_BUFFER_TAIL;

	HAL_UART_Transmit(&huart1,mp3_tx_buffer,MP3_TX_BUFFER_LENGTH,10);
}

void mp3_setVolume(int volume) {
	mp3_tx_buffer[0] = MP3_BUFFER_HEADER_0;
	mp3_tx_buffer[1] = MP3_BUFFER_HEADER_1;
	mp3_tx_buffer[2] = MP3_BUFFER_HEADER_2;
	mp3_tx_buffer[3] = MP3_CMD_SET_VOLUME;
	mp3_tx_buffer[4] = MP3_NO_FEEDBACK;
	mp3_tx_buffer[5] = (((uint8_t)volume) >> 8);
	mp3_tx_buffer[6] = (uint8_t)volume;
	mp3_tx_buffer[7] = MP3_BUFFER_TAIL;

	HAL_UART_Transmit(&huart1,mp3_tx_buffer,MP3_TX_BUFFER_LENGTH,10);
}

void mp3_stopPlay() {
	mp3_tx_buffer[0] = MP3_BUFFER_HEADER_0;
	mp3_tx_buffer[1] = MP3_BUFFER_HEADER_1;
	mp3_tx_buffer[2] = MP3_BUFFER_HEADER_2;
	mp3_tx_buffer[3] = MP3_CMD_STOP_PLAY;
	mp3_tx_buffer[4] = MP3_NO_FEEDBACK;
	mp3_tx_buffer[5] = 0x00;
	mp3_tx_buffer[6] = 0x00;
	mp3_tx_buffer[7] = MP3_BUFFER_TAIL;

	HAL_UART_Transmit(&huart1,mp3_tx_buffer,MP3_TX_BUFFER_LENGTH,10);
}
