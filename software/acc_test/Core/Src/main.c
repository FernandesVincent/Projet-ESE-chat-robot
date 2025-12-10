/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ADXL343_ADDR        (0x53 << 1)   // Adresse I2C 7 bits : 0x53
#define REG_DEVID           0x00
#define REG_POWER_CTL       0x2D
#define REG_DATA_FORMAT     0x31
#define REG_BW_RATE         0x2C
#define REG_INT_ENABLE      0x2E
#define REG_INT_MAP         0x2F
#define REG_INT_SOURCE      0x30
#define REG_DATAX0          0x32

// Bits
#define MEASURE_BIT         3
#define FULL_RES_BIT        (1 << 3)
#define RANGE_16G           0x03
#define DATA_READY_BIT     	(1 << 7)
#define SINGLE_TAP     	(1 << 6)


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int chr)
{
	HAL_UART_Transmit(&huart3, (uint8_t*) &chr, 1, HAL_MAX_DELAY);
	return chr;
}


void ADXL343_Write(uint8_t reg, uint8_t value)
{
	uint8_t val;

	HAL_I2C_Mem_Write(&hi2c1, ADXL343_ADDR, reg, 1, &value, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, ADXL343_ADDR, &val, 1, HAL_MAX_DELAY);
	if (val != value)
		printf("ADXL343 error writing in register 0x%02X, value returned :  0x%02X\r\n", reg, val);

}


void ADXL343_Read(uint8_t reg, uint8_t *buffer, uint8_t len)
{
	HAL_I2C_Mem_Read(&hi2c1, ADXL343_ADDR, reg, 1, buffer, len, HAL_MAX_DELAY);
}

void ADXL343_ReadXY(int16_t *x, int16_t *y)
{
	uint8_t raw[4];

	// Lecture : X0, X1, Y0, Y1
	ADXL343_Read(REG_DATAX0, raw, 4);

	*x = (int16_t)((raw[1] << 8) | raw[0]);
	*y = (int16_t)((raw[3] << 8) | raw[2]);
}

void ADXL343_Init(void)
{
	uint8_t devid = 0;
	uint8_t val = 0;

	ADXL343_Read(REG_DEVID, &devid, 1);

	printf("ADXL343 Device ID: 0x%02X\r\n", devid);
	if (devid != 0xE5) {
		Error_Handler();
	}

	ADXL343_Write(REG_POWER_CTL, (1<<3));

	ADXL343_Write(REG_DATA_FORMAT, FULL_RES_BIT | RANGE_16G);	// full resolution et +/-16g

	ADXL343_Write(REG_BW_RATE, 0x0A);		// 100 Hz

	ADXL343_Write(REG_INT_MAP, 0xff);		// on utilise INT2

	ADXL343_Write(REG_INT_ENABLE, DATA_READY_BIT);		// activer interruption data ready

	ADXL343_Read(REG_INT_SOURCE, &val, 1);	// lecture pour reset des interruptions
	printf("ADXL343 int source : 0x%02X\r\n", val);

	int16_t x, y;

	ADXL343_Read(REG_INT_SOURCE, &val, 1);
	ADXL343_ReadXY(&x, &y);

}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_9)
	{
		printf("ok\r\n");

		uint8_t val;

		int16_t x, y;

		ADXL343_Read(REG_INT_SOURCE, &val, 1);
		ADXL343_ReadXY(&x, &y);

		printf("ACC x: %d, y: %d\r\n", x, y);
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	printf("======== Starting ADXL343 example ========= \r\n");

	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	ADXL343_Init();
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		//		uint8_t val;
		//		ADXL343_Read(REG_INT_SOURCE, &val, 1);	// lecture pour reset des interruptions
		//		printf("ADXL343 int source : 0x%02X\r\n", val);
		//
		//		if ((val & 0x80) != 0)
		//		{
		//			int16_t x, y;
		//			uint8_t val;
		//
		//
		//			ADXL343_ReadXY(&x, &y);
		//
		//			printf("ACC x: %d, y: %d\r\n", x, y);
		//
		//		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
