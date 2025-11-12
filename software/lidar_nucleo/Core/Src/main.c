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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

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

typedef struct {
	uint16_t taille;
	uint8_t data[UART_RX_BUFFER_SIZE];
} Trame;

float cercle[360];

static TaskHandle_t h_task_uart;

QueueHandle_t q_usart = NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
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
	BaseType_t hptw = pdFALSE;;
	if (huart->Instance == USART1) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		Trame t;
		t.taille = Size;
		memcpy(t.data, UART1_RxBuffer, Size);

		// TODO : Envoyer pointeur de pointeur pour pas recopier le data
		xQueueSendFromISR(q_usart, &t, &hptw);

		HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);
	}
	portYIELD_FROM_ISR(&hptw)

}

int largeur_attendue(float D, float L) {
	float theta_rad = 2 * atan(D / (2 * L)); // en radians
	int theta_deg = theta_rad * 180.0 / 3.14159265; // conversion en degrés

	return theta_deg;
}

typedef struct {
	int debut;
	int fin;
} Cluster_point;

typedef struct {
	int angle;
	float distance;
} Cible_lidar;

Cible_lidar *detecter_lidar(float *cercle)
{

	Cluster_point plage[100];	// TODO : ajuster taille
	int index = 0;

	static Cible_lidar cible;

	plage[index].debut = 0;
	float distance_prec = cercle[0];
	int largeur;

	for (int i = 1; i < 360; i++) {
		if (fabs(cercle[i] - distance_prec) > 60) { // saut > 6cm
			plage[index].fin = i - 1;
			index++;
			if (index >= 100) break; // éviter dépassement
			plage[index].debut = i;
		}
		distance_prec = cercle[i];
	}

	// dernier bloc
	plage[index].fin = 359;
	index++;

	for (int i = 0; i < index; i++)
	{
		// calcul moyenne distance
		float mini=cercle[plage[i].debut];
		for (int m=plage[i].debut+1; m<=plage[i].fin; m++)
			if (cercle[m]<mini)
				mini = cercle[m];

		// calcul angle attendu pour distance
		largeur = largeur_attendue(60, mini+30);

		if (mini != 0.0)
		{
			printf("Plage %d: angle %d a %d avce minimum à %f et largeur attendu %d\r\n", i, plage[i].debut, plage[i].fin, mini, largeur);
			if(abs(plage[i].fin - plage[i].debut - largeur) <=3)
			{
				printf("====> LIDAR detecte entre %d et %d degrés à %f\r\n", plage[i].debut, plage[i].fin, mini);

				cible.angle = (plage[i].debut + plage[i].fin) / 2;
				cible.distance = mini;
				return &cible;
			}
		}
	}
	return NULL;
}


void task_uart_parsing (void * unused)
{
	Trame t;
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, UART_RX_BUFFER_SIZE);

	int index = 0;
	int distance[4] = {200, 300, 500, 800};
	int index_distance = 0;

	for (;;)
	{
		xQueueReceive(q_usart, &t, portMAX_DELAY);

		uint8_t *lidar_buffer = t.data;

		//		UBaseType_t freeSpace = uxQueueSpacesAvailable(q_usart);
		//		printf("Places libres : %lu\r\n", freeSpace);

		//		int taille = t.taille;
		//		printf("taille de la trame : %d \r\n", taille);
		//		for(int i = 0; i< taille; i++)
		//			printf("%02x ", lidar_buffer[i]);

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


			// si le checksum est verifié et on recoit des trames utiles, alors on calcul et corrige l'angle et la distance pour chaque trame
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
					// si le point il est dans une certaine distance on l'ajoute au tableau cercle
					// faire varier cette distance
					if (distance_mm[i] > 0.0 && distance_mm[i] < distance[index_distance])
					{
						if (Angle[i]<=360 && Angle[i] >=0)
							cercle[(int)Angle[i]] = distance_mm[i];
					}
				}
			}
		}
		index++;
		// Traitement des données après 50 trames recues
		// TODO : mettre ca dans une autre tache et le synchroniser avec une vrai
		// frequence d'échantillonage

		if (index%50 == 0)
		{
			// BOUCHE LES TROUS de 0 sur un degré alors que autour y'a pas de 0
			for (int i = 1; i<360; i++)
			{
				if (cercle[i] == 0.0 && cercle[i-1] != 0.0 && cercle[i+1] != 0.0)
				{
					if (fabs(cercle[i-1]-cercle[i+1]) <= 30)
						cercle[i] = (cercle[i-1] + cercle[i+1]) / 2.0;
				}
			}

			// Affichage du tableau cercle
			//						for (int i = 0; i<360; i++)
			//							printf("cercle[%d] %.2f \r\n", i, cercle[i]);
			Cible_lidar * cible = detecter_lidar(cercle);
			if (cible != NULL)
			{
				printf("Cible detectee à l'angle %d et distance %.2f mm\r\n", cible->angle, cible->distance);
				if (distance[index_distance-1] > cible->distance && index_distance > 0)
					index_distance--;
			}
			else
			{
				printf("Aucune cible detectee\r\n");
				if ( index_distance < 3 )
					index_distance++;
			}
			printf("distance valide %d\r\n", distance[index_distance]);
			memset(cercle, 0, sizeof(cercle));
		}
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
	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */


	printf("\r\n==== PROJET ESE ROBOT CHAT YDLIDAR X2 NUCLEO ====\r\n");

	q_usart = xQueueCreate(25, UART_RX_BUFFER_SIZE);

	if (xTaskCreate(task_uart_parsing, "uart", 1024, NULL, 3, &h_task_uart) != pdPASS)
	{
		printf("Error creating task uart\r\n");
		Error_Handler();
	}

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
