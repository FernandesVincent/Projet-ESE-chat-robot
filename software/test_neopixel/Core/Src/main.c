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
#include "dma.h"
#include "i2c.h"
#include "rng.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "brillez.h"
#include "tof.h"
#include "lidar.h"
#include "mp3.h"
#include "rasbpi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//static char str[RASBPI_MSG_LENGTH];
uint8_t lidar_buffer[UART_RX_BUFFER_SIZE];
uint8_t UART1_RxBuffer[UART_RX_BUFFER_SIZE];
char uart_json[512];
uint16_t taille = 0;
int lidar_flag = 0;
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	tof_callback(INT1_Pin);
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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {
    	build_json();
    	HAL_UART_Transmit_DMA(&huart3, (uint8_t*)uart_json, strlen(uart_json));

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_RNG_Init();
  MX_I2C1_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_UART5_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

	HAL_NVIC_DisableIRQ(EXTI4_IRQn);

	printf("\r\n==== PROJET ESE ROBOT CHAT ALL TEST ====\r\n");

//	tof_boot();
//	tof_initialization();
//	tof_enable_ranging();
//	tof_interrupt_frequency_all(5); //in Hz
//	all_off();

	HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);

	//  brillez_init();
	/* ticks 80MHz
	 * T0H 400ns 32
	 * T1H 800ns 64
	 * T0L 850ns 68
	 * T1L 450ns 36
	 * RES 50000ns 4000
	 *
	 * Send 0 : 32 + 68 = 100
	 * Send 1 : 64 + 36 = 100
	 *
	 * Green : 0x0000FF
	 * Red :   0x00FF00
	 * Blue :  0xFF0000
	 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	while (1)
	{
////////////////////////////////////////////////////////////////
//		printf("test");
//		mp3_selectStorageDevice();
//		printf("test1");
//
//		HAL_Delay(20);
//		printf("test2");
//
//		mp3_setVolume(100);
//		printf("test3");
//
//		HAL_Delay(20);
//		printf("test4");
//
//		mp3_playWithIndex(0);
//		printf("test5");
//		HAL_Delay(30000);
//		mp3_stopPlay();
///////////////////////////////////////////////////////////////
//		if (lidar_flag == 1)
//		{
//			lidar_check_frames();
//			lidar_checksum_test();
//			lidar_get_angle_and_distance();
//			lidar_flag = 0;
//
//		}

//		tof_print_distance();
//
//		if (tof_is_above_threshold(400)== 1){
//			all_off();
//			}
//		if(tof_is_between(200, 400) == 1){ //mm
//			one_on(57, 0x00FF00);
//		}
//
//		if(tof_is_between(100, 200) == 1){ //mm
//			one_on(57, 0xFF0000);
//		}
//		if(tof_is_between(0, 100) == 1){ //mm
//			one_on(57, 0x0000FF);
//		}
///////////////////////////////////////////////////////////////
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
