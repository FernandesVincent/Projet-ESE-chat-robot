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
#define LIDAR_DETECTION_CLUSTER_MAX 100
#define LIDAR_DETECTION_VALID_CLUSTER_MAX 5

#define SEARCH_LENGTH 420	// Utiliser 360 + 60 (pour couvrir le chevauchement)
#define WRAP(i) ((i) % 360)

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

		/*
		 * TODO : Peut etre ne pas mettre de queue et juste une notification
		 * mais du coup on garantie pas une interruption arrive avant le traitement
		 * de la trame pecedente
		 * j'ai mis une queue parce que avant dans le traitement de la trame j'avais plein de print
		 * et j'ai mis une queue pour voir si je perdait des trames
		 * mais du coup là en soit, avec juste la lidar (donc ce code là),
		 * la queue n'a jamais plus d'1 élément donc on pourrait l'enlever
		 */

		xQueueSendFromISR(q_usart, &t, &hptw);

		HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, LIDAR_UART_RX_BUFFER_SIZE);
	}
	portYIELD_FROM_ISR(&hptw);
}



/*
 * Return processed LiDar data
 * To be run as a FreeRTOS task
 * Reads data from the LiDar queue, process it
 * Return Parsed_data structure pointer
 * Parsed_data contains :
 * 		- integer sample_number : number of sample
 * 		- double *angle : array of size sample_number, containing angles
 * 		- float *distance_mm : array of size sample_number, containing distance in mm
 * 		(the distance_mm[i] is measured at angle[i], with i in [0, sample_number])
 *
 * 	!!! the two arrays don't necessary covers all 360 degrees around the lidar
 * 	!!! distance_mm can be 0.0 if radar didn't get a return signal
 * 	!!! This function return pointer of Parsed_data structure which is malloc in the function,
 * 	!!! ===> don't forget to free the structure
 */
Parsed_data *lidar_parsing_data()
{
	Trame t;
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_RxBuffer, LIDAR_UART_RX_BUFFER_SIZE);

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

	// BOUCHE LES TROUS de 0 sur un degré alors que autour y'a pas de 0
	for (int i = 1; i<360; i++)
	{
		int prec = (i == 0) ? 359 : i - 1; // 0 -> 359, sinon i-1
		int suiv = (i == 359) ? 0 : i + 1; // 359 -> 0, sinon i+1

		if (cercle[i] == 0.0 && cercle[prec] != 0.0 && cercle[suiv] != 0.0)
		{
			if (fabs(cercle[prec] - cercle[suiv]) <= 30)
				cercle[i] = (cercle[prec] + cercle[suiv]) / 2.0;
		}
	}

	Cible_lidar cible = detecter_lidar(cercle);
	if (cible. distance != 0)
	{
		printf("Cible detectee à l'angle %d et distance %.2f mm\r\n", cible.angle, cible.distance);
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

/*
 * Parameters :
 * 		- float *cercle (cercle remplit par la fonction lidar_sampling)
 * Return :
 * 		- une structure de type Cible_lidar, qui contient
 * 			- l'angle où la cible est détecter
 * 			- la distance où la cible est détecter
 *
 * Cette fonction essaye de détecter une lidar.
 * Si elle renvoit (Cible_lidar){ .angle = 0, .distance = 0 }, alors elle n'a pas détecter de lidar
 *
 */
Cible_lidar detecter_lidar(float *cercle)
{
	Cluster_point clusters[LIDAR_DETECTION_CLUSTER_MAX];
	int cluster_index = 0;

	Cible_lidar cluster_valide[LIDAR_DETECTION_VALID_CLUSTER_MAX];
	int cible_index = 0;

	clusters[cluster_index].debut = 0;
	float distance_prec = cercle[WRAP(0)];
	int largeur;


	for (int i = 1; i < SEARCH_LENGTH; i++)
	{
		float distance_actuelle = cercle[WRAP(i)];

		// s'il y a un saut important dans la distance (c'est-à-dire un bord d'objet), ici 6cm
		if (fabs(distance_actuelle - distance_prec) > LIDAR_DIAMETER_MM)
		{
			clusters[cluster_index].fin = WRAP(i - 1);

			int debut_reel = clusters[cluster_index].debut;
			int fin_reel = clusters[cluster_index].fin;

			// calcul largeur du cluster
			int largeur_cluster;
			if (fin_reel >= debut_reel)
				largeur_cluster = fin_reel - debut_reel + 1;
			else
				largeur_cluster = (359 - debut_reel) + 1 + fin_reel + 1;

			// si cluster valide
			if (largeur_cluster > 0)
			{
				// calcul distance entre le lidar et l'objet (minimum dans le cluster)
				float mini=cercle[debut_reel];
				for (int m = 1; m < largeur_cluster; m++)
				{
					int idx = WRAP(debut_reel + m);
					if (cercle[idx] < mini && cercle[idx] != 0.0)
						mini = cercle[idx];
				}

				largeur = largeur_attendue(LIDAR_DIAMETER_MM, mini + LIDAR_RADIUS_MM);

				// vérifier si la largeur mesurée correspond à la largeur attendue
				if (mini != 0.0)
				{
					if(abs(largeur_cluster - largeur) <= LIDAR_DETECTION_TOLERANCE_DEG)
					{
						printf("====> LIDAR detecte entre %d et %d degrés (largeur %d) à %f (attendu %d)\r\n", debut_reel, fin_reel, largeur_cluster, mini, largeur);

						// Calcul de l'angle central
						int angle_centre;
						if (fin_reel >= debut_reel)
							angle_centre = (debut_reel + fin_reel) / 2;
						else
							angle_centre = WRAP((debut_reel + fin_reel + 360) / 2);

						cluster_valide[cible_index].angle = angle_centre;
						cluster_valide[cible_index].distance = mini;
						cible_index++;
						if (cible_index >= 5) break;
					}
				}
			}
			cluster_index++;
			if (cluster_index >= 100) break;
			clusters[cluster_index].debut = WRAP(i);
		}
		distance_prec = distance_actuelle;
	}

	// Trouver la cible la plus proche parmi les cibles valides
	int index_cible_proche = -1;
	for (int i = 0; i < cible_index; i++)
	{
		if (index_cible_proche == -1 || cluster_valide[i].distance < cluster_valide[index_cible_proche].distance)
			index_cible_proche = i;
	}

	return (index_cible_proche != -1) ? cluster_valide[index_cible_proche] : (Cible_lidar){.angle = 0, .distance = 0.0};

}

/*
 * Parameters :
 * 		- float *cercle (cercle remplit par la fonction lidar_sampling)
 * Return :
 * 		- une structure de type Cible_lidar, qui contient
 * 			- l'angle où la cible est détecter
 * 			- la distance où la cible est détecter
 *
 * Cette fonction renvoie la distance la plus proche détecter avec son angle
 * Bien peut etre pour fuir
 */
Cible_lidar detect_min_distance(float *cercle)
{
	float distance_min = 8000;
	int index_min = -1;
	for (int i = 0; i<360; i++)
	{
		if (cercle[i] != 0.0 && cercle[i] < distance_min)
		{
			distance_min = cercle[i];
			index_min = i;
		}

	}
	if (index_min != -1)
		return (Cible_lidar){ .angle = index_min, .distance = distance_min };
	else
		return (Cible_lidar){ .angle = 0, .distance = 0 }; // valeur invalide
}

