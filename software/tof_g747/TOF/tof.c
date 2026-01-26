#include "tof.h"
#include "gpio.h"

#define I2C_HANDLER_PTR	&hi2c3
#define TOF_DEFAULT_DEV_ADDRESS 0x52

#define NUMBER_OF_TOF 3


VL53L0X_Dev_t dev_un = {
		.I2cHandle = I2C_HANDLER_PTR,
		.I2cDevAddr = TOF_DEFAULT_DEV_ADDRESS
};

VL53L0X_Dev_t dev_deux = {
		.I2cHandle = I2C_HANDLER_PTR,
		.I2cDevAddr = TOF_DEFAULT_DEV_ADDRESS
};

VL53L0X_Dev_t dev_trois = {
		.I2cHandle = I2C_HANDLER_PTR,
		.I2cDevAddr = TOF_DEFAULT_DEV_ADDRESS
};

tof_dev tof_devices[NUMBER_OF_TOF] = {
		{
				.dev = &dev_un,
				.dev_address = 0x54,
				.shutdown_port = TOF6_XSHUT_GPIO_Port,
				.shutdown_pin = TOF6_XSHUT_Pin,
				.int_pin = TOF6_INT_Pin
		},
		{
				.dev = &dev_deux,
				.dev_address = 0x56,
				.shutdown_port = TOF5_XSHUT_GPIO_Port,
				.shutdown_pin = TOF5_XSHUT_Pin,
				.int_pin = TOF5_INT_Pin
		},
		{
				.dev = &dev_trois,
				.dev_address = 0x58,
				.shutdown_port = TOF4_XSHUT_GPIO_Port,
				.shutdown_pin = TOF4_XSHUT_Pin,
				.int_pin = TOF4_INT_Pin
		},

};


static void tof_callibation(VL53L0X_Dev_t *dev)
{
	VL53L0X_Error ret;

	uint8_t VHV;
	uint8_t Phase;
	uint32_t refSpadCount;
	uint8_t isAperture;

	ret = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount, &isAperture);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: PerformRefSpadManagement ERROR\r\n");

	ret = VL53L0X_PerformRefCalibration(dev, &VHV, &Phase);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: PerformRefCalibration ERROR\r\n");

	ret = VL53L0X_SetOffsetCalibrationDataMicroMeter(dev, 0);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetOffsetCalibration ERROR\r\n");
}



/*
 * Cette fonction initialise UN SEUL capteur tof en :
 * 	- ranging continue
 * 	- interruption, avec interruption quand mesure prete
 *
 * 	!!! Appeler cette fonction avec les interruptions désactivé sinon ca interrompt l'initialisation
 *
 */
static void tof_init(tof_dev *tof_dev)
{
	VL53L0X_Error ret;

	HAL_GPIO_WritePin(tof_dev->shutdown_port, tof_dev->shutdown_pin, GPIO_PIN_SET);

	// Device initialization
	do
	{
		ret = VL53L0X_DataInit(tof_dev->dev);
		printf("bloqué ici snif \r\n");
	} while(ret != VL53L0X_ERROR_NONE);

	ret = VL53L0X_StaticInit(tof_dev->dev);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: StaticInit ERROR\r\n");

	// Calibration data loading
	tof_callibation(tof_dev->dev);

	// System settings
	ret = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(tof_dev->dev, 33000);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetMeasurementTiming ERROR\r\n");

	ret = VL53L0X_SetDeviceMode(tof_dev->dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetDeviceMode ERROR\r\n");

	ret = VL53L0X_SetGpioConfig(tof_dev->dev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
			VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY, VL53L0X_INTERRUPTPOLARITY_HIGH);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetGpioConfig ERROR\r\n");

	// Changing device slave address

	ret = VL53L0X_SetDeviceAddress(tof_dev->dev, tof_dev->dev_address);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetDeviceAddress ERROR\r\n");

	tof_dev->dev->I2cDevAddr = tof_dev->dev_address;
	printf("changing address to %x\r\n", tof_dev->dev->I2cDevAddr);

	ret = VL53L0X_StartMeasurement(tof_dev->dev);
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: StartMeasurement ERROR\r\n");
}


/*
 * Cette fonction sert à configurer le tof pour qu'il interrompt
 * quand la distance mesuré dépasse le threshold spécifié
 *
 * !!! Appeler dans la callback, la fonction correspondante
 */

static void tof_set_interrupt_threshold(VL53L0X_Dev_t *dev, float threshold)
{
	VL53L0X_Error ret;

	ret = VL53L0X_SetInterruptThresholds(dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
			(FixPoint1616_t) 0.0f*(1<<16), (FixPoint1616_t) threshold*(1<<16));
	if(ret != VL53L0X_ERROR_NONE) printf("tof init: SetInterruptThresholds ERROR\r\n");

	ret = VL53L0X_SetGpioConfig(dev, 0, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
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
		if (GPIO_Pin == tof_devices[i].int_pin)
		{
			ret = VL53L0X_GetRangingMeasurementData(tof_devices[i].dev, &mesure);
			if (ret == VL53L0X_ERROR_NONE)
				printf("tof%i distance %d mm\r\n", i, mesure.RangeMilliMeter);
			else
				printf("aie aie aie \r\n");
			VL53L0X_ClearInterruptMask(tof_devices[i].dev, VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH);
			break;
		}
	}
}

void tof_init_all()
{
	for (int i = 0; i < NUMBER_OF_TOF; i++)
	{
		tof_init(&tof_devices[i]);
		tof_set_interrupt_threshold(tof_devices[i].dev, 100.0);
	}
	printf("init all done \r\n");
}
