#include "tof.h"
#include "gpio.h"

#define I2C_HANDLER_PTR	&hi2c3

#define NUMBER_OF_TOF 1
tof_dev tof_devices[NUMBER_OF_TOF] = {
		{
				.dev = {
						.I2cHandle = I2C_HANDLER_PTR,
						.I2cDevAddr = 0x52
				},
				.dev_address = 0x54,
				.shutdown_port = TOF6_XSHUT_GPIO_Port,
				.shutdown_pin = TOF6_XSHUT_Pin,
				.int_pin = TOF6_INT_Pin
		}
};

tof_dev ouiii = {
		.dev = {
				.I2cHandle = I2C_HANDLER_PTR,
				.I2cDevAddr = 0x52
		},
		.dev_address = 0x54,
		.shutdown_port = TOF6_XSHUT_GPIO_Port,
		.shutdown_pin = TOF6_XSHUT_Pin,
		.int_pin = TOF6_INT_Pin
};

VL53L0X_Dev_t dev = {
		.I2cHandle = I2C_HANDLER_PTR,
		.I2cDevAddr = 0x52
};

/*
 * Cette fonction initialise UN SEUL capteur tof en :
 * 	- ranging continue
 * 	- interruption, avec interruption quand mesure prete
 *
 * 	!!! Appeler cette fonction avec les interruptions désactivé sinon ca interrompt l'initialisation
 *
 */
void tof_init()
{
	uint8_t VHV;
	uint8_t Phase;
	uint32_t refSpadCount;
	uint8_t isAperture;

	VL53L0X_Error ret;

	HAL_GPIO_WritePin(TOF6_XSHUT_GPIO_Port, TOF6_XSHUT_Pin, GPIO_PIN_SET);

	do
	{
		ret = VL53L0X_DataInit(&dev);
	} while(ret != VL53L0X_ERROR_NONE);

	ret = VL53L0X_StaticInit(&dev);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: StaticInit ERROR\r\n");

	ret = VL53L0X_PerformRefCalibration(&dev, &VHV, &Phase);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: PerformRefCalibration ERROR\r\n");

	ret = VL53L0X_PerformRefSpadManagement(&dev, &refSpadCount, &isAperture);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: PerformRefSpadManagement ERROR\r\n");

	ret = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&dev, 33000);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetMeasurementTiming ERROR\r\n");

	ret = VL53L0X_SetDeviceMode(&dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetDeviceMode ERROR\r\n");

	ret = VL53L0X_SetGpioConfig(&dev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
			VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY, VL53L0X_INTERRUPTPOLARITY_HIGH);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetGpioConfig ERROR\r\n");

	// changing device slave address
	//	ret = VL53L0X_SetDeviceAddress(&dev, tof_device->dev_address);
	//	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetDeviceAddress ERROR\r\n");
	//	tof_device->dev.I2cDevAddr = tof_device->dev_address;
	//	dev = tof_device->dev;
	//	printf("changing address to %x %x\r\n", tof_device->dev.I2cDevAddr, dev.I2cDevAddr);

	ret = VL53L0X_StartMeasurement(&dev);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: StartMeasurement ERROR\r\n");
}



/*
 * Cette fonction sert à configurer le tof pour qu'il interrompt
 * quand la distance mesuré dépasse le threshold spécifié
 *
 * !!! Appeler dans la callback, la fonction correspondante
 */

void tof_set_interrupt_threshold(float threshold)
{

	VL53L0X_Error ret;

	ret = VL53L0X_SetInterruptThresholds(&dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
			(FixPoint1616_t) 0.0f*(1<<16), (FixPoint1616_t) threshold*(1<<16));
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetInterruptThresholds ERROR\r\n");

	ret = VL53L0X_SetGpioConfig(&dev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
			VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH, VL53L0X_INTERRUPTPOLARITY_HIGH);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetGpioConfig ERROR\r\n");

}

/*
 * Function to be called in interruption callback
 * remet l'interruption en mode quand mesure prete
 */
void tof_read(uint16_t GPIO_Pin)
{
	VL53L0X_Dev_t dev;
	VL53L0X_RangingMeasurementData_t mesure;
	int ret;

	if (GPIO_Pin == TOF6_INT_Pin)
	{
		ret = VL53L0X_GetRangingMeasurementData(&dev, &mesure);
		if (ret == VL53L0X_ERROR_NONE)
			printf("%d mm\r\n", mesure.RangeMilliMeter);
		else
			printf("aie aie aie \r\n");
		VL53L0X_ClearInterruptMask(&dev, VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY);
	}
}

/*
 * Function to be called in interruption callback
 * ca ractive l'interruption en mode dépassement de seuil
 */
void tof_thresholded(uint16_t GPIO_Pin)
{
	VL53L0X_RangingMeasurementData_t mesure;
	int ret;

	for (int i = 0; i < NUMBER_OF_TOF; i++)
	{
		if (GPIO_Pin == TOF6_INT_Pin)
		{
			//			printf("slave address %x\r\n", ouiii.dev.I2cDevAddr);
			ret = VL53L0X_GetRangingMeasurementData(&dev, &mesure);
			if (ret == VL53L0X_ERROR_NONE)
				printf("distance %d mm\r\n", mesure.RangeMilliMeter);
			else
				printf("aie aie aie \r\n");
			VL53L0X_ClearInterruptMask(&dev, VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH);
			break;
		}
	}

}

void tof_init_all()
{
	//	for (int i = 0; i < NUMBER_OF_TOF; i++)
	//	{
	//		tof_init(&tof_devices[i]);
	//		tof_set_interrupt_threshold(&tof_devices[i], 100.0);
	//	}
	tof_init();
	tof_set_interrupt_threshold(100.0);
}
