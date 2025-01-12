/*
 * Nucleo_HumTemp_I2C.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Maxime
 */


#include "hts221_reg.h"
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f7xx_hal.h"
#include "fatfs.h"



#include <string.h>
#include <stdio.h>
#include <Nucleo_HumTemp_I2C.h>
#include "stm32746g_discovery_lcd.h"

#include "Interface.h"
#include "rtc.h"

#define SENSOR_BUS hi2c1

const uint16_t squareSize = 120;
const uint16_t spacing = 30;
const uint16_t startX = 40;
const uint16_t startY = 80;  // Ajusté pour centrer verticalement

static int16_t data_raw_humidity;
static int16_t data_raw_temperature;
volatile float humidity_perc;
volatile float temperature_degC;
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
stmdev_ctx_t dev_ctx1;


typedef struct {
    float x0;
    float y0;
    float x1;
    float y1;
} lin_t;

lin_t lin_hum;
lin_t lin_temp;

static int32_t platform_write_hts221(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t platform_read_hts221(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void platform_delay_hts221(uint32_t ms);

float linear_interpolation(lin_t *lin, int16_t x)
{
return ((lin->y1 - lin->y0) * x + ((lin->x1 * lin->y0) -
								   (lin->x0 * lin->y1)))
	   / (lin->x1 - lin->x0);
}


void init_HumTemp(void) {
    dev_ctx1.write_reg = platform_write_hts221;
    dev_ctx1.read_reg = platform_read_hts221;
    dev_ctx1.mdelay = platform_delay_hts221;
    dev_ctx1.handle = &SENSOR_BUS;

    whoamI = 0;
    hts221_device_id_get(&dev_ctx1, &whoamI);

    if ( whoamI != HTS221_ID )
       while (1);

    hts221_hum_adc_point_0_get(&dev_ctx1, &lin_hum.x0);
    hts221_hum_rh_point_0_get(&dev_ctx1, &lin_hum.y0);
    hts221_hum_adc_point_1_get(&dev_ctx1, &lin_hum.x1);
    hts221_hum_rh_point_1_get(&dev_ctx1, &lin_hum.y1);

    hts221_temp_adc_point_0_get(&dev_ctx1, &lin_temp.x0);
    hts221_temp_deg_point_0_get(&dev_ctx1, &lin_temp.y0);
    hts221_temp_adc_point_1_get(&dev_ctx1, &lin_temp.x1);
    hts221_temp_deg_point_1_get(&dev_ctx1, &lin_temp.y1);

    hts221_block_data_update_set(&dev_ctx1, PROPERTY_ENABLE);
    hts221_data_rate_set(&dev_ctx1, HTS221_ODR_1Hz);
    hts221_power_on_set(&dev_ctx1, PROPERTY_ENABLE);

}


void valeur_Hum(void) {
	hts221_reg_t reg1;
	hts221_status_get(&dev_ctx1, &reg1.status_reg);
    if (reg1.status_reg.h_da) {
        memset(&data_raw_humidity, 0x00, sizeof(int16_t));
        hts221_humidity_raw_get(&dev_ctx1, &data_raw_humidity);
        humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity);

        if (humidity_perc < 0) {
            humidity_perc = 0;
        }
        if (humidity_perc > 100) {
            humidity_perc = 100;
        }

        snprintf((char *)tx_buffer, sizeof(tx_buffer), "%3.2f %%", humidity_perc);
        //printf((char const *)tx_buffer);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(startX + squareSize + spacing + 20, startY + 70, (uint8_t*)tx_buffer, LEFT_MODE);

    }
}


// Fonction principale pour lire les données et les afficher
void valeur_TempH(void){

	hts221_reg_t reg1;
	hts221_status_get(&dev_ctx1, &reg1.status_reg);
    if (reg1.status_reg.t_da) {
        memset(&data_raw_temperature, 0x00, sizeof(int16_t));
        hts221_temperature_raw_get(&dev_ctx1, &data_raw_temperature);
        temperature_degC = linear_interpolation(&lin_temp, data_raw_temperature);
        snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f deg", temperature_degC);
        //printf((char const *)tx_buffer);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(startX+2, startY + 70, (uint8_t*)tx_buffer, LEFT_MODE);
    }
}

float valeur_TempH_SD(void) {
    hts221_reg_t reg1;
    hts221_status_get(&dev_ctx1, &reg1.status_reg);
    if (reg1.status_reg.t_da) {
        memset(&data_raw_temperature, 0x00, sizeof(int16_t));
        hts221_temperature_raw_get(&dev_ctx1, &data_raw_temperature);
        float temp = linear_interpolation(&lin_temp, data_raw_temperature);

        // Débogage : Afficher la température
        printf("Raw Temperature: %d, Temperature: %.2f deg\n", data_raw_temperature, temp);

        return temp;
    }
    printf("Temperature not available\n");
    return 0.0f;  // Retourne 0 si la température n'est pas disponible
}


#define MAX_POINTS 10  // Nombre maximum de points à afficher dans le graphique

extern float temperatures[MAX_POINTS]; // Tableau pour stocker les températures
extern uint8_t temp_index;
extern int current_point;

void valeur_graph_TempH(void) {
    hts221_reg_t reg1;
    hts221_status_get(&dev_ctx1, &reg1.status_reg);

    if (reg1.status_reg.t_da) {
        // Lire la température brute du capteur
        memset(&data_raw_temperature, 0x00, sizeof(int16_t));
        hts221_temperature_raw_get(&dev_ctx1, &data_raw_temperature);

        // Convertir la valeur brute en °C
        temperature_degC = linear_interpolation(&lin_temp, data_raw_temperature);

        // Obtenir l'heure actuelle en secondes depuis le début (ou autre base temporelle)
        uint32_t current_time = get_current_time_in_seconds();

        // Ajouter la nouvelle valeur et mettre à jour le graphique
        update_temperature_graph(temperature_degC, current_time);

        // Rafraîchir le graphique s'il est affiché
        //draw_temperature_graph(temperature_degC, current_time, current_point);
    }
}

void value_humidity_graph(void) {

        hts221_reg_t reg1;
		hts221_status_get(&dev_ctx1, &reg1.status_reg);
		if (reg1.status_reg.h_da) {
			memset(&data_raw_humidity, 0x00, sizeof(int16_t));
			hts221_humidity_raw_get(&dev_ctx1, &data_raw_humidity);
			humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity);

			// Récupérer l'heure actuelle en minutes
			int32_t current_time = get_current_time_in_seconds();
			// Ajouter les données
			update_humidity_graph(humidity_perc, current_time);
        }
}

void valeur_TempH_archive(void) {
    hts221_reg_t reg1;
    FIL tempFile;        // Handle du fichier pour la température
    UINT byteswritten;   // Nombre de bytes écrits
    char buffer[100];    // Buffer pour formater les données
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    // Vérifie si la température est disponible
    hts221_status_get(&dev_ctx1, &reg1.status_reg);
    if (reg1.status_reg.t_da) {
        // Lecture des données brutes et conversion
        memset(&data_raw_temperature, 0x00, sizeof(int16_t));
        hts221_temperature_raw_get(&dev_ctx1, &data_raw_temperature);
        temperature_degC = linear_interpolation(&lin_temp, data_raw_temperature);

        // Affiche sur l'écran LCD
        snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f deg", temperature_degC);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(startX + 2, startY + 70, (uint8_t *)tx_buffer, LEFT_MODE);

        // Ouvre le fichier pour ajouter des données
        if (f_open(&tempFile, "temperature.csv", FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
            // Récupère la date et l'heure actuelles
            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

            // Formate les données pour les écrire
            snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d;%02d:%02d:%02d;%.2f\n",
                     2000 + sDate.Year, sDate.Month, sDate.Date,
                     sTime.Hours, sTime.Minutes, sTime.Seconds, temperature_degC);

            // Écrit dans le fichier
            f_write(&tempFile, buffer, strlen(buffer), &byteswritten);

            // Ferme le fichier
            f_close(&tempFile);
            printf("Temperature data written to temperature.csv\n");
        } else {
            printf("Failed to open temperature.csv for writing\n");
        }
    }
}


static int32_t platform_write_hts221(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
	  reg |= 0x80;
	  HAL_I2C_Mem_Write(handle, HTS221_I2C_ADDRESS, reg,
	                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
	  return 0;
}

static int32_t platform_read_hts221(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
	  reg |= 0x80;
	  HAL_I2C_Mem_Read(handle, HTS221_I2C_ADDRESS, reg,
	                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	  return 0;
}

static void platform_delay_hts221(uint32_t ms) {
    HAL_Delay(ms);
}


