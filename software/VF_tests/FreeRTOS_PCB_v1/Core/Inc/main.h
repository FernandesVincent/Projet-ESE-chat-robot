/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NRST_Pin GPIO_PIN_10
#define NRST_GPIO_Port GPIOG
#define NRST_EXTI_IRQn EXTI15_10_IRQn
#define Encodeur1_PhaseB_Pin GPIO_PIN_0
#define Encodeur1_PhaseB_GPIO_Port GPIOC
#define Encodeur1_PhaseA_Pin GPIO_PIN_1
#define Encodeur1_PhaseA_GPIO_Port GPIOC
#define ADC_bat_Pin GPIO_PIN_1
#define ADC_bat_GPIO_Port GPIOA
#define UART_PI_TX_Pin GPIO_PIN_2
#define UART_PI_TX_GPIO_Port GPIOA
#define UART_PI_RX_Pin GPIO_PIN_3
#define UART_PI_RX_GPIO_Port GPIOA
#define INT5_Pin GPIO_PIN_4
#define INT5_GPIO_Port GPIOA
#define INT5_EXTI_IRQn EXTI4_IRQn
#define XSHUT5_Pin GPIO_PIN_5
#define XSHUT5_GPIO_Port GPIOA
#define INT0_Pin GPIO_PIN_6
#define INT0_GPIO_Port GPIOA
#define INT0_EXTI_IRQn EXTI9_5_IRQn
#define XSHUT0_Pin GPIO_PIN_7
#define XSHUT0_GPIO_Port GPIOA
#define INT2_Pin GPIO_PIN_4
#define INT2_GPIO_Port GPIOC
#define XSHUT2_Pin GPIO_PIN_5
#define XSHUT2_GPIO_Port GPIOC
#define XSHUT1_Pin GPIO_PIN_0
#define XSHUT1_GPIO_Port GPIOB
#define INT1_Pin GPIO_PIN_1
#define INT1_GPIO_Port GPIOB
#define INT1_EXTI_IRQn EXTI1_IRQn
#define XSHUT3_Pin GPIO_PIN_2
#define XSHUT3_GPIO_Port GPIOB
#define INT3_Pin GPIO_PIN_10
#define INT3_GPIO_Port GPIOB
#define XSHUT4_Pin GPIO_PIN_11
#define XSHUT4_GPIO_Port GPIOB
#define INT4_Pin GPIO_PIN_12
#define INT4_GPIO_Port GPIOB
#define INT4_EXTI_IRQn EXTI15_10_IRQn
#define Encodeur2_PhaseA_Pin GPIO_PIN_6
#define Encodeur2_PhaseA_GPIO_Port GPIOC
#define Encoder2_PhaseB_Pin GPIO_PIN_7
#define Encoder2_PhaseB_GPIO_Port GPIOC
#define I2C_TOF_SCL_Pin GPIO_PIN_8
#define I2C_TOF_SCL_GPIO_Port GPIOC
#define I2C_TOF_SDA_Pin GPIO_PIN_9
#define I2C_TOF_SDA_GPIO_Port GPIOC
#define UART_ST_LINK_TX_Pin GPIO_PIN_10
#define UART_ST_LINK_TX_GPIO_Port GPIOC
#define UART_ST_LINK_RX_Pin GPIO_PIN_11
#define UART_ST_LINK_RX_GPIO_Port GPIOC
#define UART_MP3_TX_Pin GPIO_PIN_12
#define UART_MP3_TX_GPIO_Port GPIOC
#define UART_MP3_RX_Pin GPIO_PIN_2
#define UART_MP3_RX_GPIO_Port GPIOD
#define PWM_LED_Pin GPIO_PIN_4
#define PWM_LED_GPIO_Port GPIOB
#define REV2_Pin GPIO_PIN_6
#define REV2_GPIO_Port GPIOB
#define FWD2_Pin GPIO_PIN_7
#define FWD2_GPIO_Port GPIOB
#define REV1_Pin GPIO_PIN_8
#define REV1_GPIO_Port GPIOB
#define FWD1_Pin GPIO_PIN_9
#define FWD1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
