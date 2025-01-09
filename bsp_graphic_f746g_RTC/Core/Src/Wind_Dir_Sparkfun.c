/*
 * Wind_Dir_Sparkfun.c
 *
 *  Created on: Dec 30, 2024
 *      Author: Maxime
 */

/*FONCTIONNEMENT:
 * Programme de la girouette soit la direction du vent.
 * Les fonctions sont à utilisé au dessus du main() pour l'innitialisation et dans le while(1) pour la fonction valeur.
 */

#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <Wind_Dir_Sparkfun.h>
#include "stm32746g_discovery_lcd.h"
#include "rtc.h"

//initialisation des éléments déclarables
#define NUM_POSITIONS 16
#define Vref 3.3       // Reference voltage of the ADC

//valeur de référence pour dessiner sur l'interface
volatile const uint16_t squareSizeD = 120;
volatile const uint16_t spacingD = 30;
volatile const uint16_t startXD = 40;
volatile const uint16_t startYD = 80;  // Ajusté pour centrer verticalement

const float resistorValues[NUM_POSITIONS] = {
	33000.0, 6570.0, 8200.0, 891.0, 1000.0, 688.0, 2200.0, 1410.0,
	3900.0, 3140.0, 16000.0, 14120.0, 120000.0, 42120.0, 64900.0, 21880.0
};

// Tableau des chaînes correspondant aux directions cardinales
/*en anglais
const char* directionStrings[NUM_POSITIONS] = {
	"North", "North-Northeast", "Northeast", "East-Northeast",
	"East", "East-Southeast", "Southeast", "South-Southeast",
	"South", "South-Southwest", "Southwest", "West-Southwest",
	"West", "West-Northwest", "Northwest", "North-Northwest"
};*/

//en Français
const char* directionStrings[NUM_POSITIONS] = {
    "N", "NNE", "NE", "ENE",
    "E", "ESE", "SE", "SSE",
    "S", "SSW", "SW", "WSW",
    "W", "WNW", "NW", "NNW"
};

float externalResistor = 20000.0;  // 20k ohms external resistor
float voltage;
uint32_t adcValue = 0;
float R_sensor;

//retourne la direction du vent
void Wind_Dir_Value() {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        adcValue = HAL_ADC_GetValue(&hadc1);
        voltage = ((float)adcValue / 4095.0) * Vref;
    }
    HAL_ADC_Stop(&hadc1);

    printf("ADC Value: %lu, Voltage: %.2f V\r\n", adcValue, voltage);

    if (voltage > 0) {
        R_sensor = (externalResistor * (Vref / voltage)) - externalResistor;
        printf("Resistance: %.2f ohms\r\n", R_sensor);

        const char* direction = "Unknown";
        float minDifference = 5000.0;  // Initialisation ajustée

        for (int i = 0; i < NUM_POSITIONS; i++) {
            float difference = fabs(R_sensor - resistorValues[i]);
            printf("Checking direction %s: Difference = %.2f ohms\r\n", directionStrings[i], difference);
            if (difference < minDifference) {
                minDifference = difference;
                direction = directionStrings[i];
            }
        }
        BSP_LCD_DisplayStringAt(startXD + squareSizeD + spacingD + 20, startYD + 70, (uint8_t*)direction, LEFT_MODE);
        printf("Detected Wind Direction: %s\r\n", direction);
    } else {
        printf("Voltage is too low. Check sensor connections.\r\n");
    }

}

