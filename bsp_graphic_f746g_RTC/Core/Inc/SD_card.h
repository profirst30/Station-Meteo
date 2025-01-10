/*
 * SD_card.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Maxime
 */



#ifndef SD_CARD_H
#define SD_CARD_H

#include "fatfs.h"
#include "rtc.h"
#include <stdio.h>
#include <string.h>

// Structure pour les données météo
typedef struct {
    float temperature;
    float humidity;
    float pressure;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} WeatherData_t;

// Variables externes
extern FATFS fs;
extern FIL fil_temp, fil_hum, fil_press;
extern uint8_t sd_logging_enabled;

// Prototypes des fonctions
FRESULT init_sd_logging(void);
void log_weather_data(void);

#endif /* SD_CARD_H */
