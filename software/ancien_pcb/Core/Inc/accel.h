/*
 * accel.h
 *
 *  Created on: Nov 19, 2025
 *      Author: pcail
 */

#ifndef INC_ACCEL_H_
#define INC_ACCEL_H_

#include <stdio.h>
#include "i2c.h"

#define ADXL_address (0x53 << 1)		//adxl353 has i2c on 7 bits so need to put it on 8 bits
#define ADXL_DEVID (0xe5)				//The DEVID register holds a fixed device ID code of 0xE5 (345 octal) (datasheet)

#define ADXL_REG_DEVID (0x00) 			//REG is for register def (datasheet p.23)
#define ADXL_REG_THRESH_TAP (0x1d)
#define ADXL_REG_OFSX (0x1e)
#define ADXL_REG_OFSY (0x1f)
#define ADXL_REG_OFSZ (0x20)
#define ADXL_REG_DUR (0x21)
#define ADXL_REG_ACT_INAT_CTL (0x27)
#define ADXL_REG_


#endif /* INC_ACCEL_H_ */
