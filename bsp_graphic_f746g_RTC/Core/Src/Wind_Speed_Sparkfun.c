/*
 * Wind_Speed_Sparkfun.c
 *
 *  Created on: Dec 31, 2024
 *      Author: Maxime
 */

/*FONCTIONNEMENT:
 * Utilisation de 2 timer, dont le TIM1 en input capture
 * afin de détecté les front montant sur une minute.
 * TIM6 mets en place le timer pour l' interruption du comptage sur une minute.
 *
 * D'après la documentation une impulsion par seconde renvoyé par l'annemometre vaut 2.4 Km/h soit pour 1Hz, on a  2.4 Km/h.
 * Donc on peut obtenir une valeur grâce à f*VITESSE_PAR_IMPULSION_PAR_SECONDE.
 *
 */


#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <Wind_Speed_Sparkfun.h>
#include "stm32746g_discovery_lcd.h"

// Coefficient pour convertir la fréquence en vitesse du vent (km/h)
#define VITESSE_PAR_IMPULSION_PAR_SECONDE 2.4

//initialisation des variables
volatile uint8_t captureDone = 0;
volatile uint32_t captures[2] = {0, 0}; // Table pour les captures
volatile uint8_t captureFlag = 0; // Flag pour savoir si nous devons capturer

float frequency = 0;
float wind_speed;

uint16_t diffCapture = 0;
uint32_t pclk1_freq = 0;

volatile uint32_t pulse_count = 0; // Compteur des impulsions
volatile uint8_t minute_flag = 0;  // Flag pour indiquer que 1 minute est écoulée

const uint16_t squareSizeW = 120;
const uint16_t spacingW = 30;
const uint16_t startXW = 40;
const uint16_t startYW = 80;  // Ajusté pour centrer verticalement

extern uint8_t flag_meteo;

//initialisation de timer (à appeler dans le main)
void Wind_TIM_Init()
{
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim6);
}

//renvoi la valeur de la vitesse du vent moyenne sur 1 minute (à appeler dans la boucle while)
void Wind_Speed_Value()
{

	if (minute_flag) {

		  //remise à 0 du timer 2 (mesure de station meteo non detecté pendant 1 min)
			  flag_meteo=0;
			  HAL_TIM_Base_Stop(&htim2);        // Arrêter le timer
			  __HAL_TIM_SET_COUNTER(&htim2, 0); // Réinitialiser le compteur à zéro
			  HAL_TIM_Base_Start(&htim2);       // Redémarrer le timer

		      // Si 1 minute est écoulée, afficher les impulsions
		      //remise à 0 du timer 2 (mesure de station meteo non detecté pendant 1 min)
		      printf("Impulsions sur 1 minute : %lu\r\n", pulse_count);
		      float vitesse_Vent = pulse_count*VITESSE_PAR_IMPULSION_PAR_SECONDE/5;
		      printf("vitesse du vent: %.2f Km/h \r\n ", vitesse_Vent);
		      // Réinitialiser le computeur d'impulsions pour la prochaine minute
		      pulse_count = 0;

		      char vent[20];
			  sprintf(vent, "%.2f Km/h", vitesse_Vent);
			  BSP_LCD_SetFont(&Font16);
			  BSP_LCD_DisplayStringAt(startXW + 5, startYW + 60, (uint8_t*)vent, LEFT_MODE);

		      // Réinitialiser le flag pour le prochain intervalle de 1 minute
		      minute_flag = 0;
		      printf("Minute flag reset!\r\n"); // Vérifier que le flag est réinitialisé



			  }
		  //}
}



//routine de comptage par input capture (PIN PA8)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
      //printf("bonjour");
        pulse_count++; // Incrémenter le compteur d'impulsions
        //printf("Pulse captured: %lu\r\n", pulse_count); // Ajouter imprimé pour vérification

    }
  }
}



