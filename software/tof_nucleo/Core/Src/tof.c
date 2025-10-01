/*
 * tof.c
 *
 *  Created on: Sep 22, 2025
 *      Author: Kelly
 */

#include "tof.h"
#include "main.h"
#include <stdbool.h>

#define NUMBER_OF_TOF 1

/*
 * List of TOF devices
 */
tof_dev tof_devices[NUMBER_OF_TOF] = {
	{
			.dev_address = 0x54,
			.shutdown_pin = {GPIOA, GPIO_PIN_8},
			.int_pin = {GPIOA, TOF1_Pin}
	},
	{
			.dev_address = 0x58,
			.shutdown_pin = {GPIOC, GPIO_PIN_8},
			.int_pin = {GPIOA, TOF2_Pin}
	}
};

/*
 * Function that boots every TOF
 * and changes its I2C slave address to the one specified
 */
void tof_boot()
{

	int state = 0;
	int status = 0;
	int default_address = 0x52;

	for (int i=0; i< NUMBER_OF_TOF; i++)
	{
		tof_dev tof = tof_devices[i];

		// BOOTING TOF
		do {
			HAL_GPIO_WritePin(tof.shutdown_pin.port, tof.shutdown_pin.pin, GPIO_PIN_SET);
			status = VL53L1X_BootState(default_address, &state);
			HAL_Delay(2);
			printf("TOF not booted\n\r");
		} while(state == 0);

		printf("TOF %d booted\n\r", i);

		// CHANGING TOF I2C slave address
		do {
			status = VL53L1X_SetI2CAddress(default_address, tof.dev_address);
		} while(status != 0);

		printf("TOF %d I2C address changed to %02x\n\r", i, tof.dev_address);
	}
}

/*
 * Function that initialize every TOF
 */
void tof_initialization()
{

	for (int i=0; i< NUMBER_OF_TOF; i++)
	{
		tof_dev tof = tof_devices[i];
		while(VL53L1X_SensorInit(tof.dev_address) != 0);
	}
	printf("TOF initialization done\n\r");
}

/*
 * Function that enables ranging
 * and therefore allows measurement
 */
void tof_enable_ranging()
{

	for (int i=0; i< NUMBER_OF_TOF; i++)
	{
		tof_dev tof = tof_devices[i];
		while(VL53L1X_StartRanging(tof.dev_address) != 0);
	}
	printf("TOF ranging enabled\n\r");
}

/*
 * Callback function to be called
 * for handing interruption
 * It displays information of the measure
 */
void tof_callback(uint16_t GPIO_Pin)
{
	int status = 0;
	uint8_t RangeStatus;
	uint16_t Distance;
	uint16_t SignalRate;
	uint16_t AmbientRate;
	uint16_t SpadNum;

	for (int i=0; i< NUMBER_OF_TOF; i++)
	{
		tof_dev tof = tof_devices[i];
		if (GPIO_Pin == tof.int_pin.pin){
			status = VL53L1X_GetRangeStatus(tof.dev_address, &RangeStatus);
			status = VL53L1X_GetDistance(tof.dev_address, &Distance);
			status = VL53L1X_GetSignalRate(tof.dev_address, &SignalRate);
			status = VL53L1X_GetAmbientRate(tof.dev_address, &AmbientRate);
			status = VL53L1X_GetSpadNb(tof.dev_address, &SpadNum);
			status = VL53L1X_ClearInterrupt(tof.dev_address); /* clear interrupt has to be called to enable next interrupt*/
//			printf("TOF %d: %u, %u, %u, %u, %u \n\r", i, RangeStatus, Distance, SignalRate, AmbientRate, SpadNum);
			printf("TOF %d distance: %u mm \n\r", i, Distance);
			break;
		}
	}
}

/*
 * Function that returns true if the given distance measured is above spcified threshold parameter
 *
 * @param distance_measured
 * @param threshold
 */
//bool tof_is_above_threshold(int distance_measured, int threshold){
//	printf("ALERT");
////	return distance_measured > threshold;
//}

void tof_is_above_threshold(int threshold){
	uint16_t Distance;
	for (int i=0; i< NUMBER_OF_TOF; i++)
	{
		tof_dev tof = tof_devices[i];
		if(VL53L1X_GetDistance(tof.dev_address, &Distance > threshold)){
			printf("Alert !");
		}
	}
}
