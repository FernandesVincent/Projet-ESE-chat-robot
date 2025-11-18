/*
 * lidar.c
 *
 *  Created on: Oct 1, 2025
 *      Author: Kelly
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "usart.h"
#include "lidar.h"
#include "gpio.h"

#define LIDAR_UART USART1
#define LIDAR_QUEUE_LENGTH 25

#define LIDAR_DIAMETER_MM 60.0f
#define LIDAR_RADIUS_MM 30.0f

#define LIDAR_DETECTION_TOLERANCE_DEG 3


extern uint8_t UART1_RxBuffer[LIDAR_UART_RX_BUFFER_SIZE];

QueueHandle_t q_usart = NULL;

void lidar_init()
{
	q_usart = xQueueCreate(LIDAR_QUEUE_LENGTH, LIDAR_UART_RX_BUFFER_SIZE);
}

/*
 * Lidar UART callback from ISR
 * Called when a full frame is received
 *
 */
void lidar_callback_from_isr(UART_HandleTypeDef *huart, uint16_t Size)
{
	BaseType_t hptw = pdFALSE;;
	if (huart->Instance == LIDAR_UART) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		Trame t;
		t.taille = Size;
		memcpy(t.data, UART1_RxBuffer, Size);

		// TODO : Envoyer pointeur de pointeur pour pas recopier le data
		xQueueSendFromISR(q_usart, &t, &hptw);

		HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, LIDAR_UART_RX_BUFFER_SIZE);
	}
	portYIELD_FROM_ISR(&hptw);
}



/*
 * Return processed lidar data
 * To be run as a FreeRTOS task
 * Reads data from the lidar queue, process it and fill the cercle array
 */
Parsed_data *lidar_parsing_data()
{
	Trame t;
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, LIDAR_UART_RX_BUFFER_SIZE);

	// TODO : gérer queue pleine et tous la

	for (;;)
	{
		xQueueReceive(q_usart, &t, portMAX_DELAY);

		uint8_t *lidar_buffer = t.data;

		uint16_t PH = (uint16_t)(lidar_buffer[1] << 8 | lidar_buffer[0]);
		uint8_t CT = lidar_buffer[2];
		uint8_t LSN = lidar_buffer[3];

		uint16_t FSA = (uint16_t)(lidar_buffer[5] << 8 | lidar_buffer[4]);
		uint16_t LSA = (uint16_t)(lidar_buffer[7] << 8 | lidar_buffer[6]);
		uint16_t CS = (uint16_t)(lidar_buffer[9] << 8 | lidar_buffer[8]);

		uint16_t SI[LSN];

		float distance_mm[LSN];
		for (int i = 0; i < LSN; i++)
			SI[i] = (uint16_t)(lidar_buffer[i*2+11] << 8 | lidar_buffer[i*2+10]);


		// HEADER verification
		if (PH == 0x55AA)
		{
			uint16_t checksumcal = PH;
			checksumcal ^= FSA;
			checksumcal ^= (uint16_t)(LSN << 8 | CT);
			checksumcal ^= LSA;
			for (int i = 0; i<LSN; i++)
				checksumcal ^= SI[i];

			// CHECKSUM verification
			if (checksumcal == CS && (CT & 0x01) == 0x00)
			{

				// PARSING DATA
				for (int i = 0; i < LSN; i++)
					distance_mm[i] = SI[i] / 4.0f;

				double angle_FSA = (FSA >> 1) / 64.0;
				double angle_LSA = (LSA >> 1) / 64.0;
				double angle_diff = angle_LSA - angle_FSA;

				if (angle_diff < 0)
					angle_diff += 360.0;

				double angle[LSN];
				for (int i = 0; i < LSN; i++)
				{
					if(LSN > 1)
						angle[i] = (i+1) * angle_diff/(LSN-1) + angle_FSA;
					else
						angle[i] = angle_FSA;

					if(distance_mm[i] > 0)
					{
						double angCorrect = atan(21.8 * (155.3 - distance_mm[i]) / (155.3 * distance_mm[i]));
						angle[i] += angCorrect * 180.0 / M_PI;
					}
					if (angle[i] >= 360)
						angle[i] -= 360.0;
				}

				/*
				 * We extract two arrays from the LIDAR data frame
				 * double angle[LSN] : angles in degrees
				 * float distance_mm[LSN] : distances in mm
				 *
				 * !!!! : distance_mm can be 0.0 if radar didn't get a return signal
				 */

				Parsed_data *data = malloc(sizeof(Parsed_data));

				data->sample_number = LSN;
				data->angle = angle;
				data->distance_mm = distance_mm;

				return data;
			}
		}
	}

}



/*
 * Remplit le tableau cercle[360] avec les distances en mm
 * pour chaque angle en degrés
 *
 * Parce que le lidar donne des échantillons à des angles variables,
 * et parfois une trame ne correspond pas à un cercle complet,
 * et parfois des échantillons sont manquants (distance 0)
 * on remplit un tableau fixe de 360 valeurs
 * pour avoir un échantillonnage uniforme et plus complet qui représente plusieus trames
 *
 * les valeurs de distance supérieures à distance_max_mm sont ignorées
 */
void lidar_sampling(Parsed_data *data, float *cercle, float distance_max_mm)
{
	for(int i = 0; i < data->sample_number; i++)
	{
		if (data->distance_mm[i] > 0.0 && data->distance_mm[i] < distance_max_mm)
		{
			if (data->angle[i]<=360 && data->angle[i] >=0)
				cercle[(int)data->angle[i]] = data->distance_mm[i];
		}
	}
}

void lidar_detection_target(float *cercle)
{

	// Traitement des données après 50 trames recues
	// TODO : mettre ca dans une autre tache et le synchroniser avec une vrai
	// frequence d'échantillonage

	// BOUCHE LES TROUS de 0 sur un degré alors que autour y'a pas de 0
	for (int i = 1; i<360; i++)
	{
		if (cercle[i] == 0.0 && cercle[i-1] != 0.0 && cercle[i+1] != 0.0)
		{
			if (fabs(cercle[i-1]-cercle[i+1]) <= 30)
				cercle[i] = (cercle[i-1] + cercle[i+1]) / 2.0;
		}
	}

	Cible_lidar * cible = detecter_lidar(cercle);
	if (cible != NULL)
	{
		printf("Cible detectee à l'angle %d et distance %.2f mm\r\n", cible->angle, cible->distance);
	}
	else
	{
		printf("Aucune cible detectee\r\n");
	}
}

/*
 * Calcule la largeur angulaire attendue en degrés
 * 	pour un objet de diamètre D en mm à une distance L en mm
 */
static int largeur_attendue(float D, float L) {
	float theta_rad = 2 * atan(D / (2 * L)); // en radians
	int theta_deg = theta_rad * 180.0 / 3.14159265; // conversion en degrés

	return theta_deg;
}


Cible_lidar *detecter_lidar(float *cercle)
{
	Cluster_point plage[100];	// TODO : ajuster taille
	int index = 0;

	static Cible_lidar cible;

	plage[index].debut = 0;
	float distance_prec = cercle[0];
	int largeur;
	Cible_lidar cible_valide[5];
	int cible_index = 0;

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
		largeur = largeur_attendue(LIDAR_DIAMETER_MM, mini + LIDAR_RADIUS_MM);

		if (mini != 0.0)
		{
			//printf("Plage %d: angle %d a %d avce minimum à %f et largeur attendu %d\r\n", i, plage[i].debut, plage[i].fin, mini, largeur);
			if(abs(plage[i].fin - plage[i].debut - largeur) <= LIDAR_DETECTION_TOLERANCE_DEG)
			{
				printf("====> LIDAR detecte entre %d et %d degrés à %f\r\n", plage[i].debut, plage[i].fin, mini);
				cible_valide[cible_index].angle = (plage[i].debut + plage[i].fin) / 2;
				cible_valide[cible_index].distance = mini;
				cible_index++;
			}
		}
	}
	int index_cible_proche = -1;
	for (int i = 0; i < cible_index; i++)
	{
		// retourne la cible la plus proche
		if (index_cible_proche == -1 || cible_valide[i].distance < cible_valide[index_cible_proche].distance)
			index_cible_proche = i;

	}
	return (index_cible_proche != -1) ? &cible_valide[index_cible_proche] : NULL;
	// return NULL;
}
