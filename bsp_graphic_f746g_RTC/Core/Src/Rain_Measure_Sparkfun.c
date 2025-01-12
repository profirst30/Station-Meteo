/*
 * Rain_Measure_Sparkfun.c
 *
 *  Created on: Dec 31, 2024
 *      Author: Maxime
 */

/*FONCTIONNEMENT:
 * Utilisation de 2 timer, dont le TIM2 en input capture
 * afin de détecté les front montant.
 * TIM6 mets en place le timer pour l' interruption du comptage sur une minute.
 *
 * D'après la documentation, une impulsion vaut 0.2794mm.
 * Donc on peut obtenir une valeur grâce à f*VOLUME_PAR_IMPULSION.
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
#include <Rain_Measure_Sparkfun.h>
#include "rtc.h"
#include "stm32746g_discovery_lcd.h"



// Coefficient pour convertir la fréquence en volume de pluie(mm)
#define VOLUME_PAR_IMPULSION 0.2794

//initialisation de timer (à appeler dans le main)

volatile uint32_t rainCount = 0;
volatile uint8_t timeElapsed = 0;
volatile uint8_t flagRain=0;
float rainfallR=0.0;

//valeur de référence pour dessiner sur l'interface
volatile const uint16_t squareSizeR = 120;
volatile const uint16_t spacingR = 30;
volatile const uint16_t startXR = 40;
volatile const uint16_t startYR = 80;  // Ajusté pour centrer verticalement

extern uint8_t flag_meteo;


RTC_TimeTypeDef currentTime, lastTime; // Variables pour garder une trace de la minute actuelle
//RTC_DateTypeDef currentDate, lastDate; // Variables pour garder une trace du jour


//initialisation des timer d'interruption
void Rain_TIM_Init(){
	HAL_TIM_Base_Start_IT(&htim6);
}

/*
// Vérifier si le jour a changé
void check_day_change(void) {
    HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN); // Lire la date actuelle

    // Si le jour a changé (c'est-à-dire que le jour actuel est différent du dernier jour enregistré)
    if (currentDate.Date != lastDate.Date) {
        // Réinitialiser le compteur de pluie
        rainCount = 0;
        printf("Le compteur de pluie a été réinitialisé pour un nouveau jour.\r\n");

        // Mettre à jour la date du dernier jour
        lastDate.Date = currentDate.Date;
    }
}
*/
// Vérifier si la minute a changé
void check_minute_change(void) {
    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN); // Lire l'heure actuelle

    // Si la minute actuelle est différente de la dernière minute enregistrée
    if (currentTime.Minutes != lastTime.Minutes) {
        // Réinitialiser le compteur de pluie
        rainCount = 0;
        printf("Le compteur de pluie a été réinitialisé pour une nouvelle minute.\r\n");

        // Mettre à jour la dernière minute
        lastTime.Minutes = currentTime.Minutes;
    }
}


//revoie et affiche les mesures de pluie
void Draw_Rain_Measure_Value(){

	if(flagRain){

		  //remise à 0 du timer 2 (mesure de station meteo non detecté pendant 1 min)
		  flag_meteo=0;
		  HAL_TIM_Base_Stop(&htim2);        // Arrêter le timer
		  __HAL_TIM_SET_COUNTER(&htim2, 0); // Réinitialiser le compteur à zéro
		  HAL_TIM_Base_Start(&htim2);       // Redémarrer le timer

		  printf("raincount: %lu \r\n", rainCount);
		  rainfallR = rainCount * VOLUME_PAR_IMPULSION;

		  printf("Volume d'eau par seconde: %.2f mm\r\n", rainfallR);
		  // Afficher la valeur sur l'écran LCD à une position donnée (par exemple, ligne 50, colonne 50)
		  timeElapsed = 0;
		  flagRain=0;
		  char rainfallStr[20];
		  sprintf(rainfallStr, "%.2f mm", rainfallR);
		  BSP_LCD_SetFont(&Font16); // Police de caractères pour l'affichage
	      BSP_LCD_DisplayStringAt(startXR + 2 * (squareSizeR + spacingR) + 20, startYR + 70, (uint8_t*)rainfallStr, LEFT_MODE);
	}



}


//revoie les mesures de pluie
void Rain_Measure_Value(){

	if(flagRain){
		  printf("raincount: %lu \r\n", rainCount);
		  rainfallR = rainCount * VOLUME_PAR_IMPULSION;

		  printf("Volume d'eau par seconde: %.2f mm\r\n", rainfallR);
		  timeElapsed = 0;
		  flagRain=0;
		  char rainfallStr[20];
		  sprintf(rainfallStr, "%.2f mm", rainfallR);

	}
}


//routine d'interruption pour un arret à durée paramétrable

//routine d'interruption pour comptage (PIN PA15)





