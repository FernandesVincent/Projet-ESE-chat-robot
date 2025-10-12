/*
 * rasbpi.c
 *
 *  Created on: Oct 12, 2025
 *      Author: fernandesvincent
 */


#include "rasbpi.h"
#include <string.h>
#include <stdio.h>




char UART3TX_Buffer[RASBPI_MSG_LENGTH];
int len = 0;

UartData uart_table[] = {
    {"TOF1", 1},
    {"TOF2", 1},
    {"TOF3", 1},
    {"TOF4", 1},
    {"TOF5", 1},
    {"TOF6", 1}, //Pour les TOF 1 au dessus du seuil, 0 en dessous
    {"LIDAR_D", 0}, // Distance LIDAR/objet
    {"LIDAR_THETA", 0}, // angle LIDAR/objet
    {"speed", 0}, //robot speed
    {"accel", 0}, // robot accel
    {"batt", 0}, //battery value with ADC
    {"role", 0}, //role of the robot
    {"cat_as_not", 0}, // not = number of touch
    {"mouse_as_not", 0},
    {"", },
};

extern char uart_json[512];

void build_json(void) {
    len += sprintf(&uart_json[len], "{");

    for (int i = 0; uart_table[i].label[0] != '\0'; i++) {
        len += sprintf(&uart_json[len], "\"%s\":%d", uart_table[i].label, uart_table[i].value);
        if (uart_table[i+1].label[0] != '\0') {
            len += sprintf(&uart_json[len], ",");
        }
    }
    len += sprintf(&uart_json[len], "}\n");
}
