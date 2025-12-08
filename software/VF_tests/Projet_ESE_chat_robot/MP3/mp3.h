/*
 * mp3.h
 *
 *  Created on: Dec 10, 2024
 *      Author: administrateur
 */

#ifndef INC_MP3_H_
#define INC_MP3_H_

#include "main.h"

#define MP3_TX_BUFFER_LENGTH 8
#define MP3_BUFFER_HEADER_0 0x7E
#define MP3_BUFFER_HEADER_1 0xFF
#define MP3_BUFFER_HEADER_2 0x06
#define MP3_NO_FEEDBACK 0x00
#define MP3_FEEDBACK 0x01
#define MP3_BUFFER_TAIL 0xEF

#define MP3_MICRO_SD_STORAGE_H0 0x00
#define MP3_MICRO_SD_STORAGE_H1 0x02

#define MP3_CMD_PLAY_WITH_INDEX 0x03
#define MP3_CMD_SET_VOLUME 0x06
#define MP3_CMD_SELECT_DEVICE 0x09
#define MP3_CMD_PLAY 0x0D
#define MP3_CMD_PAUSE 0x0E
#define MP3_CMD_PLAY_FROM_FOLDER 0x0F
#define MP3_CMD_STOP_PLAY 0x16


void mp3_selectStorageDevice();
void mp3_playWithIndex(int track_index);
void mp3_playFromFolder(int folder, int track);
void mp3_setVolume(int volume);
void mp3_pause();
void mp3_stopPlay();

#endif /* INC_MP3_H_ */
