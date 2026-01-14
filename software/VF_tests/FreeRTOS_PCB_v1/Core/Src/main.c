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
#include "cmsis_os.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tof.h"
#include "vl53l0x_api.h"
#include "vl53l0x_device.h"
#include <stdio.h>
#include "motors.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

static TaskHandle_t MotorTaskHandle;
static TaskHandle_t TOFTaskHandle;

bool motors_enabled = true;

void task_motors(void * unused){
  while(1){
    if(motors_enabled ==true){
      motor_forward(80, 'R', 0);
      motor_forward(65, 'L', 0);
    }
    else{
      motor_backward(80, 'R', 0);
      motor_backward(65, 'L', 0);
      vTaskDelay(pdMS_TO_TICKS(3000));
      motors_stop_all();
      motor_turn(30, 65, 'F', 0);
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
    vTaskDelay(pdMS_TO_TICKS(50)); 
  }
}
bool turn[6] = {false, false, false, false, false, false};

void test_tofs(void * unused){
  while(1){
  for(uint8_t i = 0; i < 6; i++){
    turn[i] = VL53L0X_IsAboveThreshold(i);
    vTaskDelay(pdMS_TO_TICKS(1000));
    }
    bool allFalse = !(turn[0] || turn[1] || turn[2] || turn[3] || turn[4] || turn[5]);
    if(allFalse){
      motors_enabled = true;
    }
  vTaskDelay(pdMS_TO_TICKS(100));
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
  MX_ADC1_Init();
  MX_I2C3_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM16_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	printf("TEST TOFs\r\n");
	VL53L0X_InitAllSensor();
  printf("TEST MOTORS left 47\r\n");
  motors_init();
  printf("MOTORS INITIALIZED\r\n");
  
  // test_motor_left();
  // printf("LEFT MOTOR TESTED\r\n");
  // HAL_Delay(2000);
  // test_motor_right();
  // printf("RIGHT MOTOR TESTED\r\n");

  if (xTaskCreate(task_motors, "MOTORS", 256, NULL, 3, &MotorTaskHandle) != pdPASS){
		printf("Error creating task b1");
		Error_Handler();
	}
  else{
    printf("Motor task created\r\n");
  }

  if(xTaskCreate(test_tofs, "TOFS", 512, NULL, 2, &TOFTaskHandle) != pdPASS){
    printf("Error creating task test tofs");
    Error_Handler();
  }
  else{
    printf("TOF task created\r\n");
  }


  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) 
	{

    // motor_forward(40, 'R');
    // HAL_Delay(5000);
    // motor_stop_right();
    // HAL_Delay(5000);

// #################################### TOF
		// VL53L0X_IsAboveThreshold(0);
    // HAL_Delay(1000);
    // VL53L0X_IsAboveThreshold(1);
    // HAL_Delay(1000);
    // VL53L0X_IsAboveThreshold(2);
    // HAL_Delay(1000);
    // VL53L0X_IsAboveThreshold(3);
    // HAL_Delay(1000);
    // VL53L0X_IsAboveThreshold(4);
    // HAL_Delay(1000);
    // VL53L0X_IsAboveThreshold(5);
    // HAL_Delay(1000);
// #################################### TOF

// #################################### MOTORS

    






    // motor_forward(60, 'R');
    // // motor_forward(20, 'L');
    // HAL_Delay(5000);
    // motor_backward(40, 'R');
    // // motor_backward(80,'L');
    // HAL_Delay(3000);
// #################################### MOTORS



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
