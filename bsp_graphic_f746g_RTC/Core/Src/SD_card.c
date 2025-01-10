/*
 * SD_card.c
 *
 *  Created on: Jan 8, 2025
 *      Author: Maxime
 */

#include "SD_card.h"
#include "Nucleo_HumTemp_I2C.h"
#include "Nucleo_Pression_I2C.h"


// Variables globales
FATFS fs;
FIL fil_temp, fil_hum, fil_press;
uint8_t sd_logging_enabled = 0;
extern float humidity_perc;
extern float temperature_degC;

// Initialisation du système d'enregistrement
FRESULT init_sd_logging(void) {
    FRESULT res;

    // Monter le système de fichiers
    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        printf("Erreur montage SD: %d\r\n", res);
        return res;
    }

    // Ouvrir/créer les fichiers avec en-têtes si nouveaux
    const char* files[] = {"temp.csv", "hum.csv", "press.csv"};
    FIL* files_handle[] = {&fil_temp, &fil_hum, &fil_press};

    for(int i = 0; i < 3; i++) {
        res = f_open(files_handle[i], files[i], FA_WRITE | FA_OPEN_ALWAYS);
        if (res == FR_OK) {
            if(f_size(files_handle[i]) == 0) {
                f_printf(files_handle[i], "Date,Time,Value\n");
            }
            f_close(files_handle[i]);
        } else {
            printf("Erreur ouverture %s: %d\r\n", files[i], res);
            return res;
        }
    }

    sd_logging_enabled = 1;
    printf("Système d'enregistrement initialisé\r\n");
    return FR_OK;
}

// Fonction d'enregistrement appelée par l'interruption du timer
void log_weather_data(void) {
    if(!sd_logging_enabled) return;

    WeatherData_t data;
    char buffer[100];
    UINT bw;

    // Récupérer l'heure et la date
    HAL_RTC_GetTime(&hrtc, &data.time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &data.date, RTC_FORMAT_BIN);

    // Récupérer les mesures (utiliser vos fonctions existantes)
    data.temperature = temperature_degC;  // Votre variable globale existante
    data.humidity = humidity_perc;        // Votre variable globale existante
    //data.pressure = pressure_hPa;         // Votre variable globale existante

    // Enregistrer température
    f_open(&fil_temp, "temp.csv", FA_WRITE | FA_OPEN_EXISTING);
    f_lseek(&fil_temp, f_size(&fil_temp));
    sprintf(buffer, "20%02d-%02d-%02d,%02d:%02d:%02d,%.2f\n",
            data.date.Year, data.date.Month, data.date.Date,
            data.time.Hours, data.time.Minutes, data.time.Seconds,
            data.temperature);
    f_write(&fil_temp, buffer, strlen(buffer), &bw);
    f_close(&fil_temp);

    // Même chose pour humidité et pression...
    // [Code similaire pour les autres fichiers]
}


