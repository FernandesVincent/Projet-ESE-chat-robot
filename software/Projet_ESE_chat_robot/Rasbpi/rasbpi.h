/*
 * rasbpi.h
 *
 *  Created on: Oct 12, 2025
 *      Author: fernandesvincent
 */

#ifndef __RASBPI_H__
#define __RASBPI_H__

#include "main.h"

#define RASBPI_MSG_LENGTH 256

typedef struct {
    char label[32];
    int value;
} UartData;

void build_json(void);

#endif
