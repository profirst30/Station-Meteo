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
    uint32_t byteswritten;
    FIL SDFile;
    char header[] = "Date,Time,Value\n";

    // Monter le système de fichiers
    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        printf("Erreur montage SD: %d\r\n", res);
        return res;
    }

    // Créer le fichier température avec en-tête
    res = f_open(&SDFile, "TEMP.CSV", FA_CREATE_ALWAYS | FA_WRITE);
    if(res == FR_OK) {
        res = f_write(&SDFile, header, strlen(header), (void *)&byteswritten);
        if((byteswritten == 0) || (res != FR_OK)) {
            printf("Erreur écriture en-tête température\n");
            f_close(&SDFile);
            return res;
        }
        f_close(&SDFile);
    } else {
        printf("Erreur création fichier température: %d\n", res);
        return res;
    }

    // Créer le fichier humidité avec en-tête
    res = f_open(&SDFile, "HUM.CSV", FA_CREATE_ALWAYS | FA_WRITE);
    if(res == FR_OK) {
        res = f_write(&SDFile, header, strlen(header), (void *)&byteswritten);
        if((byteswritten == 0) || (res != FR_OK)) {
            printf("Erreur écriture en-tête humidité\n");
            f_close(&SDFile);
            return res;
        }
        f_close(&SDFile);
    } else {
        printf("Erreur création fichier humidité: %d\n", res);
        return res;
    }

    printf("Système d'enregistrement initialisé avec succès\n");
    sd_logging_enabled = 1;
    return FR_OK;
}

// Fonction d'enregistrement appelée par l'interruption du timer
void log_weather_data(void) {
    FRESULT res;
    uint32_t byteswritten;
    char buffer[100];
    FIL SDFile;

    // Vérifier si le système est initialisé
    if(!sd_logging_enabled) {
        printf("Logging non activé\n");
        return;
    }

    // Récupérer l'heure et la date
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    // Essayer d'ouvrir/créer le fichier température
    res = f_open(&SDFile, "TEMP.CSV", FA_OPEN_ALWAYS | FA_WRITE);
    if(res == FR_OK) {
        // Se positionner à la fin du fichier
        f_lseek(&SDFile, f_size(&SDFile));

        // Préparer la ligne de données
        sprintf(buffer, "20%02d-%02d-%02d,%02d:%02d:%02d,%.2f\n",
                date.Year, date.Month, date.Date,
                time.Hours, time.Minutes, time.Seconds,
                temperature_degC);

        // Écrire les données
        res = f_write(&SDFile, buffer, strlen(buffer), (void *)&byteswritten);

        if((byteswritten == 0) || (res != FR_OK)) {
            printf("Erreur d'écriture température\n");
        }

        // Fermer le fichier
        f_close(&SDFile);
    } else {
        printf("Erreur ouverture fichier température: %d\n", res);
    }

    // Même chose pour l'humidité
    res = f_open(&SDFile, "HUM.CSV", FA_OPEN_ALWAYS | FA_WRITE);
    if(res == FR_OK) {
        f_lseek(&SDFile, f_size(&SDFile));

        sprintf(buffer, "20%02d-%02d-%02d,%02d:%02d:%02d,%.2f\n",
                date.Year, date.Month, date.Date,
                time.Hours, time.Minutes, time.Seconds,
                humidity_perc);

        res = f_write(&SDFile, buffer, strlen(buffer), (void *)&byteswritten);

        if((byteswritten == 0) || (res != FR_OK)) {
            printf("Erreur d'écriture humidité\n");
        }

        f_close(&SDFile);
    } else {
        printf("Erreur ouverture fichier humidité: %d\n", res);
    }

    printf("Enregistrement effectué\n");
}


