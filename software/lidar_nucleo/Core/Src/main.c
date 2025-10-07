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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define UART_RX_BUFFER_SIZE 256


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


uint8_t UART1_RxBuffer[UART_RX_BUFFER_SIZE];
uint16_t taille = 0;
int lidar_flag = 0;
uint8_t lidar_buffer[UART_RX_BUFFER_SIZE];


typedef struct {
	float angle;
	float distance;
} Valid_point;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int chr)
{
	HAL_UART_Transmit(&huart2, (uint8_t*) &chr, 1, HAL_MAX_DELAY);
	return chr;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART1) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		taille = Size;
		lidar_flag = 1;
		for (int i = 0; i<Size; i++)
			lidar_buffer[i] = UART1_RxBuffer[i];

		HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);
	}

}

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_DMA(&huart1, buffer, 12);
}
 */


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
	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */


	printf("\r\n==== PROJET ESE ROBOT CHAT YDLIDAR X2 NUCLEO ====\r\n");

	HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);

	// Démarrer le DMA sur USART1
	// HAL_UART_Receive_DMA(&huart1, dma_rx_buffer, RX_BUF_SIZE);


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1)
	{
		if (lidar_flag == 1)
		{

//			printf("taille de la trame : %d \r\n", taille);
//			for(int i = 0; i< taille; i++)
//				printf("%02x ", lidar_buffer[i]);

			uint16_t PH = (uint16_t)(lidar_buffer[1] << 8 | lidar_buffer[0]);
			uint8_t CT = lidar_buffer[2];
			uint8_t LSN = lidar_buffer[3];

			// printf("\n\r %d échantillons\r\n", LSN);

			uint16_t FSA = (uint16_t)(lidar_buffer[5] << 8 | lidar_buffer[4]);
			uint16_t LSA = (uint16_t)(lidar_buffer[7] << 8 | lidar_buffer[6]);
			uint16_t CS = (uint16_t)(lidar_buffer[9] << 8 | lidar_buffer[8]);

			uint16_t SI[LSN];
			float distance_mm[LSN];
			for (int i = 0; i < LSN; i++)
				SI[i] = (uint16_t)(lidar_buffer[i*2+11] << 8 | lidar_buffer[i*2+10]);

			/*
			printf("PH %04x \r\n", PH);
			printf("FSA %04x \r\n", FSA);
			printf("LSA %04x \r\n", LSA);
			printf("CS %04x \r\n", CS);

			for (int i = 0; i<LSN; i++)
				printf(" SI[i] %04x \r\n",  SI[i]);
			 */


			if (PH == 0x55AA)
			{
				// printf("header pass\r\n");

				uint16_t checksumcal = PH;
				checksumcal ^= FSA;
				checksumcal ^= (uint16_t)(LSN << 8 | CT);
				checksumcal ^= LSA;
				for (int i = 0; i<LSN; i++)
					checksumcal ^= SI[i];

				// printf("checksumcal %04x \r\n", checksumcal);

				if (checksumcal == CS && (CT & 0x01) == 0x00)
				{
					// printf("CHECKSUM pass\r\n");

					for (int i = 0; i < LSN; i++)
						distance_mm[i] = SI[i] / 4.0f;

					double Angle_FSA = (FSA >> 1) / 64.0;
					double Angle_LSA = (LSA >> 1) / 64.0;
					double Angle_diff = Angle_LSA - Angle_FSA;

					if (Angle_diff < 0)
						Angle_diff += 360.0;

					double Angle[LSN];
					for (int i = 0; i < LSN; i++)
					{
						if(LSN > 1)
							Angle[i] = (i+1) * Angle_diff/(LSN-1) + Angle_FSA;
						else
							Angle[i] = Angle_FSA;

						if(distance_mm[i] > 0)
						{
							double AngCorrect = atan(21.8 * (155.3 - distance_mm[i]) / (155.3 * distance_mm[i]));
							Angle[i] += AngCorrect * 180.0 / M_PI;
						}
						if (Angle[i] >= 360)
							Angle[i] -= 360.0;
					}


					for(int i = 0; i < LSN; i++)
					{
						//printf("angle: %f distance: %.2f mm \r\n", Angle[i], distance_mm[i]);
						if (distance_mm[i] > 0.0 && distance_mm[i] < 200)
						{
							Valid_point point = {Angle[i], distance_mm[i]};
							printf("object detecte a %f mm  vers %f degre \r\n", point.distance , point.angle);
						}
					}
				}
			}
			lidar_flag = 0;

		}

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
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
