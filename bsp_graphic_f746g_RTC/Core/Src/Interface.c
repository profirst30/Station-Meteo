/*
 * Interface.c
 *
 *  Created on: Jan 6, 2025
 *      Author: Maxime
 */

#include "christmas_tree_1.h"
#include "cloud_icon.h"
#include "direction_icon.h"
#include "infos_icon.h"
#include "pressure_icon.h"
#include "rain_icon.h"
#include "setting_icon.h"
#include "temp_icon.h"
#include "wind_icon.h"
#include "back_icon.h"
#include "clock_icon.h"


#include "stdio.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"


#include "rtc.h"
#include "Interface.h"
#include "Nucleo_HumTemp_I2C.h"


RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
const char* joursSemaine[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};


void getRTCTimeStr(char* timeStr) {
    // Lecture de l'heure
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    // Lecture de la date (obligatoire pour débloquer les registres)
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // Format: "HH:MM"
    sprintf(timeStr, "%02d:%02d", sTime.Hours, sTime.Minutes);
}

uint32_t get_current_time_from_rtc(void) {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    uint32_t minutes = sTime.Hours * 60 + sTime.Minutes;

    //printf("Heure actuelle: %02d:%02d, Minutes depuis minuit: %lu\n", sTime.Hours, sTime.Minutes, minutes);

    return minutes;
}

uint32_t get_current_time_in_seconds(void) {
    return HAL_GetTick() / 1000; // Convertir le temps système en secondes
}

//date et heure sur l'interface
// ===== En-tête avec la date =====
void date_Heure(void){
	char dateStr[20];
	char timeStr[6];

	//date en en-tete
    BSP_LCD_SetFont(&Font16);
    getRTCDateStr(dateStr);
    BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)dateStr, CENTER_MODE);

    // Heure au centre
	//BSP_LCD_SetFont(&Font16);
	getRTCTimeStr(timeStr);
	BSP_LCD_DisplayStringAt(0, 250, (uint8_t*)timeStr, CENTER_MODE);
}

/**
 * @brief Dessine la page menu principale
 */
void drawMenuPage(void) {


    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Configuration de la police et des couleurs
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    // ===== Section Gauche =====
    // Zone conteneur gauche
    BSP_LCD_DrawRect(40, 60, 180, 160);

    // Température
    BSP_LCD_DrawBitmap(50, 70, (uint8_t *)temp_icon_bmp);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(92, 80, (uint8_t*)"Temperature", LEFT_MODE);

    // Humidité
    BSP_LCD_DrawBitmap(50, 122, (uint8_t *)cloud_icon_bmp);
    BSP_LCD_DisplayStringAt(92, 132, (uint8_t*)"Humidite", LEFT_MODE);

    // Pluie
    BSP_LCD_DrawBitmap(50, 174, (uint8_t *)rain_icon_bmp);
    BSP_LCD_DisplayStringAt(92, 184, (uint8_t*)"Pluie", LEFT_MODE);

    // ===== Section Droite =====
    // Zone conteneur droite
    BSP_LCD_DrawRect(260, 60, 180, 160);

    // Vent
    BSP_LCD_DrawBitmap(270, 70, (uint8_t *)wind_icon_bmp);
    BSP_LCD_DisplayStringAt(312, 80, (uint8_t*)"Vent", LEFT_MODE);

    // Direction
    BSP_LCD_DrawBitmap(270, 122, (uint8_t *)direction_icon_bmp);
    BSP_LCD_DisplayStringAt(312, 132, (uint8_t*)"Direction", LEFT_MODE);

    // Pression
    BSP_LCD_DrawBitmap(270, 174, (uint8_t *)pressure_icon_bmp);
    BSP_LCD_DisplayStringAt(312, 184, (uint8_t*)"Pression", LEFT_MODE);

    // ===== Barre inférieure =====

    // Icône info à gauche
    BSP_LCD_DrawBitmap(18, 235, (uint8_t *)infos_icon_bmp);


    // Icône paramètres à droite
    BSP_LCD_DrawBitmap(438, 235, (uint8_t *)setting_icon_bmp);
}

/**
 * @brief Dessine la page Crédits
 */
void drawCreditsPage(void) {


    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Configuration de la police et des couleurs
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);


    // Icône retour
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // ===== Titre explicatif =====
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t*)"Cette station Meteo a ete realisee par :", CENTER_MODE);

    // ===== Liste des développeurs =====
    BSP_LCD_SetFont(&Font16);  // Police plus grande pour les noms
    BSP_LCD_DisplayStringAt(0, 130, (uint8_t*)"Maxime MORET", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 160, (uint8_t*)"Tristan GROUSSARD", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 190, (uint8_t*)"Koundeme Nobel DJESSOU", CENTER_MODE);

}

/**
 * @brief Dessine la page 1
 */
void drawTempHumidRainPage(void) {

    //char tempStr[10];
    //char humidStr[10];
    char rainStr[10];

    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Configuration de la police et des couleurs
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // ===== Les trois carrés de mesures =====
    // Carrés de 120x120 pixels avec espacement de 30 pixels
    // Position Y centrée verticalement
    volatile const uint16_t squareSize = 120;
    volatile const uint16_t spacing = 30;
    volatile const uint16_t startX = 40;
    volatile const uint16_t startY = 80;  // Ajusté pour centrer verticalement

    // Carré température
    BSP_LCD_DrawRect(startX, startY, squareSize, squareSize);
    BSP_LCD_DrawBitmap(startX + 20, startY + 20, (uint8_t *)temp_icon_bmp);
    BSP_LCD_SetFont(&Font20);
    //sprintf(tempStr, "15deg");
    //BSP_LCD_DisplayStringAt(startX + 20, startY + 70, (uint8_t*)tempStr, LEFT_MODE);

    // Carré humidité
    BSP_LCD_DrawRect(startX + squareSize + spacing, startY, squareSize, squareSize);
    BSP_LCD_DrawBitmap(startX + squareSize + spacing + 20, startY + 20, (uint8_t *)cloud_icon_bmp);
    //sprintf(humidStr, "65%%");
    //BSP_LCD_DisplayStringAt(startX + squareSize + spacing + 20, startY + 70, (uint8_t*)humidStr, LEFT_MODE);

    // Carré pluviométrie
    BSP_LCD_DrawRect(startX + 2 * (squareSize + spacing), startY, squareSize, squareSize);
    BSP_LCD_DrawBitmap(startX + 2 * (squareSize + spacing) + 20, startY + 20, (uint8_t *)rain_icon_bmp);
    //sprintf(rainStr, "100 mm");
    //BSP_LCD_DisplayStringAt(startX + 2 * (squareSize + spacing) + 20, startY + 70, (uint8_t*)rainStr, LEFT_MODE);


}

/**
 * @brief Dessine la page 2
 */

void drawWindDirPressurePage(void) {

    char windStr[10];
    char dirStr[10];
    char pressureStr[10];

    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Configuration de la police et des couleurs
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // ===== Les trois carrés de mesures =====
    // Carrés de 120x120 pixels avec espacement de 30 pixels
    const uint16_t squareSize = 120;
    const uint16_t spacing = 30;
    const uint16_t startX = 40;
    const uint16_t startY = 80;  // Ajusté pour centrer verticalement

    // Carré vitesse du vent
    BSP_LCD_DrawRect(startX, startY, squareSize, squareSize);
    BSP_LCD_DrawBitmap(startX + 20, startY + 20, (uint8_t *)wind_icon_bmp);
    BSP_LCD_SetFont(&Font20);
    sprintf(windStr, "30\nkm/h");
    BSP_LCD_DisplayStringAt(startX + 20, startY + 60, (uint8_t*)windStr, LEFT_MODE);

    // Carré direction du vent
    BSP_LCD_DrawRect(startX + squareSize + spacing, startY, squareSize, squareSize);
    BSP_LCD_DrawBitmap(startX + squareSize + spacing + 20, startY + 20, (uint8_t *)direction_icon_bmp);
    //sprintf(dirStr, "65%%");
    //BSP_LCD_DisplayStringAt(startX + squareSize + spacing + 20, startY + 70, (uint8_t*)dirStr, LEFT_MODE);

    // Carré pression
    BSP_LCD_DrawRect(startX + 2 * (squareSize + spacing), startY, squareSize, squareSize);
    BSP_LCD_DrawBitmap(startX + 2 * (squareSize + spacing) + 20, startY + 20, (uint8_t *)pressure_icon_bmp);
    sprintf(pressureStr, "50\nbar");
    //BSP_LCD_DisplayStringAt(startX + 2 * (squareSize + spacing) + 20, startY + 60, (uint8_t*)pressureStr, LEFT_MODE);
}


/**
 * @brief Dessine la page de réglage date/heure
 */
/**
 * @brief Dessine la page de réglage date/heure
 */
void drawSettingsPage(void) {
    char buffer[20];
    const uint16_t buttonSize = 30;      // Taille des boutons +/-
    const uint16_t valueWidth = 60;      // Largeur zone valeur
    const uint16_t spacing = 30;         // Espacement entre les éléments

    // Lire les valeurs actuelles de la RTC
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Icône retour
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // Configuration texte
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    // Section Date
    BSP_LCD_DisplayStringAt(40, 80, (uint8_t*)"Date", LEFT_MODE);

    // Positions horizontales pour les colonnes de réglage
    const uint16_t col1 = 150;  // Jour
    const uint16_t col2 = col1 + valueWidth + spacing;  // Mois
    const uint16_t col3 = col2 + valueWidth + spacing;  // Année

    // Afficher la date actuelle en haut
    sprintf(buffer, "%02d/%02d/%04d", sDate.Date, sDate.Month, 2000 + sDate.Year);
    BSP_LCD_DisplayStringAt(0, 15, (uint8_t*)buffer, CENTER_MODE);

// Boutons et valeurs pour le jour
	BSP_LCD_DrawRect(col1, 40, buttonSize, buttonSize);  // Bouton +
	BSP_LCD_DisplayStringAt(col1 + 10, 50, (uint8_t*)"+", LEFT_MODE);
	sprintf(buffer, "%02d", sDate.Date);
	BSP_LCD_DisplayStringAt(col1 + 5, 85, (uint8_t*)buffer, LEFT_MODE);
	BSP_LCD_DrawRect(col1, 105, buttonSize, buttonSize);  // Bouton -
	BSP_LCD_DisplayStringAt(col1 + 10, 115, (uint8_t*)"-", LEFT_MODE);

	// Boutons et valeurs pour le mois
	BSP_LCD_DrawRect(col2, 40, buttonSize, buttonSize);
	BSP_LCD_DisplayStringAt(col2 + 10, 50, (uint8_t*)"+", LEFT_MODE);
	sprintf(buffer, "%02d", sDate.Month);
	BSP_LCD_DisplayStringAt(col2 + 5, 85, (uint8_t*)buffer, LEFT_MODE);
	BSP_LCD_DrawRect(col2, 105, buttonSize, buttonSize);  // Bouton -
	BSP_LCD_DisplayStringAt(col2 + 10, 115, (uint8_t*)"-", LEFT_MODE);

	// Boutons et valeurs pour l'année
	BSP_LCD_DrawRect(col3, 40, buttonSize, buttonSize);
	BSP_LCD_DisplayStringAt(col3 + 10, 50, (uint8_t*)"+", LEFT_MODE);
	sprintf(buffer, "20%02d", sDate.Year);
	BSP_LCD_DisplayStringAt(col3 - 5, 85, (uint8_t*)buffer, LEFT_MODE);
	BSP_LCD_DrawRect(col3, 105, buttonSize, buttonSize);  // Bouton -
	BSP_LCD_DisplayStringAt(col3 + 10, 115, (uint8_t*)"-", LEFT_MODE);

    // Section Heure (déplacée vers le haut)
    BSP_LCD_DisplayStringAt(40, 160, (uint8_t*)"Heure", LEFT_MODE);

    // Boutons et valeurs pour les heures (déplacés vers le haut)
    BSP_LCD_DrawRect(col1, 140, buttonSize, buttonSize);  // Bouton +
    BSP_LCD_DisplayStringAt(col1 + 10, 145, (uint8_t*)"+", LEFT_MODE);
    sprintf(buffer, "%02d", sTime.Hours);
    BSP_LCD_DisplayStringAt(col1 + 5, 180, (uint8_t*)buffer, LEFT_MODE);
    BSP_LCD_DrawRect(col1, 200, buttonSize, buttonSize);  // Bouton -
    BSP_LCD_DisplayStringAt(col1 + 10, 205, (uint8_t*)"-", LEFT_MODE);

    // Boutons et valeurs pour les minutes (déplacés vers le haut)
    BSP_LCD_DrawRect(col2, 140, buttonSize, buttonSize);
    BSP_LCD_DisplayStringAt(col2 + 10, 145, (uint8_t*)"+", LEFT_MODE);
    sprintf(buffer, "%02d", sTime.Minutes);
    BSP_LCD_DisplayStringAt(col2 + 5, 180, (uint8_t*)buffer, LEFT_MODE);
    BSP_LCD_DrawRect(col2, 200, buttonSize, buttonSize);
    BSP_LCD_DisplayStringAt(col2 + 10, 205, (uint8_t*)"-", LEFT_MODE);


    // Bouton Valider
    BSP_LCD_DrawRect(340, 220, 100, 30);
    BSP_LCD_DisplayStringAt(350, 225, (uint8_t*)"Valider",LEFT_MODE);

}

void drawTempHumGraphPage(void) {

    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Configuration de la police et des couleurs
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    // ===== En-tête =====
    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);



    // ===== Lignes verticales de séparation =====
    const uint16_t lineStartY = 60;
    const uint16_t lineHeight = 180;

    // Première ligne verticale (1/3 de l'écran)
    BSP_LCD_DrawVLine(480/3, lineStartY, lineHeight);

    // Deuxième ligne verticale (2/3 de l'écran)
    BSP_LCD_DrawVLine(2*480/3, lineStartY, lineHeight);

    // ===== Icônes en haut de chaque colonne =====
    const uint16_t iconY = 60;

    // Colonne 1 (horloge)
    BSP_LCD_DrawBitmap(480/6 - 16, iconY, (uint8_t *)clock_icon_bmp);

    // Colonne 2 (thermomètre)
    BSP_LCD_DrawBitmap(480/2 - 16, iconY, (uint8_t *)temp_icon_bmp);

    // Colonne 3 (gouttes)
    BSP_LCD_DrawBitmap(5*480/6 - 16, iconY, (uint8_t *)rain_icon_bmp);

    // ===== Zone pour les graphiques =====
    // TODO: Ajouter les graphiques dans chaque colonne


}

void drawRainPressGraphPage(void) {

    // Efface l'écran
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    // Configuration de la police et des couleurs
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    // ===== En-tête =====
    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);


    // ===== Lignes verticales de séparation =====
    const uint16_t lineStartY = 60;
    const uint16_t lineHeight = 180;

    // Première ligne verticale (1/3 de l'écran)
    BSP_LCD_DrawVLine(480/3, lineStartY, lineHeight);

    // Deuxième ligne verticale (2/3 de l'écran)
    BSP_LCD_DrawVLine(2*480/3, lineStartY, lineHeight);

    // ===== Icônes en haut de chaque colonne =====
    const uint16_t iconY = 60;

    // Colonne 1 (horloge)
    BSP_LCD_DrawBitmap(480/6 - 16, iconY, (uint8_t *)clock_icon_bmp);

    // Colonne 2 (thermomètre)
    BSP_LCD_DrawBitmap(480/2 - 16, iconY, (uint8_t *)temp_icon_bmp);

    // Colonne 3 (gouttes)
    BSP_LCD_DrawBitmap(5*480/6 - 16, iconY, (uint8_t *)rain_icon_bmp);

    // ===== Zone pour les graphiques =====
    // TODO: Ajouter les graphiques dans chaque colonne

}


// Dessin du graphique temperature
// Définition des dimensions de l'écran
#define LCD_WIDTH 480
#define LCD_HEIGHT 275

// Définition du tableau de stockage des températures
volatile uint8_t temp_index = 0;      // Index actuel dans le tableau
float max_temperature = -9999.0; // Valeur maximale initialisée à une valeur basse

// Coordonnées initiales pour dessiner le graphique
uint16_t graph_origin_x = 30;   // Origine en X
uint16_t graph_origin_y = 240;  // Origine en Y
uint16_t graph_width = 400;     // Largeur du graphique
uint16_t graph_height = 200;    // Hauteur du graphique

// Taille des rectangles pour les points du graphique
uint16_t rect_width = 3;
uint16_t rect_height = 3;

#define MAX_POINTS 10
float temps[MAX_POINTS];
uint32_t times[MAX_POINTS];
volatile int current_point = 0;




void draw_temperature_graph(float *temps, uint32_t *times, int num_points) {
    int offsetY = 20; // Décalage vers le bas

    // Effacer l'écran
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, 0, 480, 275); // Effacer l'écran (fond blanc)

    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // Couleur des axes
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    // Ajouter un titre
    BSP_LCD_SetFont(&Font16); // Police pour le titre
    BSP_LCD_DisplayStringAt(0, 10 + offsetY, (uint8_t *)"Temperature", CENTER_MODE);

    // Dessiner les axes
    BSP_LCD_DrawLine(80, 200 + offsetY, 400, 200 + offsetY); // Axe X (horizontal)
    BSP_LCD_DrawLine(80, 200 + offsetY, 80, 50 + offsetY);   // Axe Y (vertical)

    // Ajouter des noms aux axes
    BSP_LCD_SetFont(&Font16); // Police pour les axes
    BSP_LCD_DisplayStringAt(190, 210 + offsetY, (uint8_t *)"Time(min)", CENTER_MODE); // Nom de l'axe X
    BSP_LCD_DisplayStringAt(25, 30 + offsetY, (uint8_t *)"Temp(C)", LEFT_MODE);           // Nom de l'axe Y

    // Ajouter des graduations sur l'axe Y (température de 0°C à 50°C)
    for (int i = 0; i <= 5; i++) {
        int y = 200 + offsetY - (i * 30); // Espacement uniforme entre graduations (30 px)
        char label[10];
        sprintf(label, "%d", i * 10); // Température (0°C à 50°C par pas de 10°C)
        BSP_LCD_DisplayStringAt(50, y - 5, (uint8_t *)label, LEFT_MODE);
        BSP_LCD_DrawLine(75, y, 80, y); // Petite graduation
    }

    // Ajouter des graduations sur l'axe X (temps)
    for (int i = 0; i < num_points; i++) {
        int x = 80 + (i * 30); // Position X des graduations
        char label[10];
        sprintf(label, "%lu", times[i]); // Afficher le temps en secondes
        BSP_LCD_DisplayStringAt(x - 20, 210 + offsetY, (uint8_t *)label, LEFT_MODE);
        BSP_LCD_DrawLine(x, 200 + offsetY, x, 195 + offsetY); // Petite graduation
    }

    // Tracer les points et relier par des lignes
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

    for (int i = 0; i < num_points - 1; i++) {
        // Coordonnées du point courant
        int x1 = 80 + (i * 30);
        int y1 = 200 + offsetY - ((temps[i] - 0) * 3); // Adapter l'échelle pour la plage 0°C-50°C (3 px par °C)

        // Coordonnées du point suivant
        int x2 = 80 + ((i + 1) * 30);
        int y2 = 200 + offsetY - ((temps[i + 1] - 0) * 3);

        // Tracer le point et la ligne
        BSP_LCD_FillCircle(x1, y1, 2); // Cercle pour le point
        BSP_LCD_DrawLine(x1, y1, x2, y2); // Ligne entre les points
    }

    // Dernier point
    int last_x = 80 + ((num_points - 1) * 30);
    int last_y = 200 + offsetY - ((temps[num_points - 1] - 0) * 3);
    BSP_LCD_FillCircle(last_x, last_y, 2); // Point final
}


void update_temperature_graph(float new_temp, uint32_t current_time) {
    // Ajouter la nouvelle température et l'heure au tableau
    if (current_point < MAX_POINTS) {
        temps[current_point] = new_temp;
        times[current_point] = current_time;
        current_point++;
    } else {
        // Décaler les données vers la gauche pour ajouter le nouveau point
        for (int i = 0; i < MAX_POINTS - 1; i++) {
            temps[i] = temps[i + 1];
            times[i] = times[i + 1];
        }
        temps[MAX_POINTS - 1] = new_temp;
        times[MAX_POINTS - 1] = current_time;
    }
    draw_temperature_graph(temps, times, current_point);

}



#define MAX_POINTS 10
float humidities[MAX_POINTS];
uint32_t times[MAX_POINTS];



void draw_humidity_graph(float *humidities, uint32_t *times, int num_points) {
    int offsetY = 20; // Décalage vers le bas

    // Effacer l'écran
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, 0, 480, 275); // Effacer l'écran (fond blanc)

    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // Couleur des axes
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    // Ajouter un titre
    BSP_LCD_SetFont(&Font16); // Police pour le titre
    BSP_LCD_DisplayStringAt(0, 10 + offsetY, (uint8_t *)"Humidity", CENTER_MODE);

    // Dessiner les axes
    BSP_LCD_DrawLine(80, 200 + offsetY, 400, 200 + offsetY); // Axe X (horizontal)
    BSP_LCD_DrawLine(80, 200 + offsetY, 80, 50 + offsetY);   // Axe Y (vertical)

    // Ajouter des noms aux axes
    BSP_LCD_SetFont(&Font16); // Police pour les axes
    BSP_LCD_DisplayStringAt(190, 210 + offsetY, (uint8_t *)"Time(min)", CENTER_MODE); // Nom de l'axe X
    BSP_LCD_DisplayStringAt(25, 30 + offsetY, (uint8_t *)"Humidity(%)", LEFT_MODE);    // Nom de l'axe Y

    // Ajouter des graduations sur l'axe Y (humidité de 0% à 100%)
    for (int i = 0; i <= 5; i++) {
        int y = 200 + offsetY - (i * 30); // Espacement uniforme entre graduations (30 px)
        char label[10];
        sprintf(label, "%d", i * 20); // Humidité (0% à 100% par pas de 20%)
        BSP_LCD_DisplayStringAt(50, y - 5, (uint8_t *)label, LEFT_MODE);
        BSP_LCD_DrawLine(75, y, 80, y); // Petite graduation
    }

    // Ajouter des graduations sur l'axe X (temps)
    for (int i = 0; i < num_points; i++) {
		int x = 80 + (i * 30); // Position X des graduations
		char label[10];
		sprintf(label, "%lu", times[i]); // Afficher le temps en secondes
		BSP_LCD_DisplayStringAt(x - 20, 210 + offsetY, (uint8_t *)label, LEFT_MODE);
		BSP_LCD_DrawLine(x, 200 + offsetY, x, 195 + offsetY); // Petite graduation
	}

    // Tracer les points et relier par des lignes
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

    for (int i = 0; i < num_points - 1; i++) {
        // Coordonnées du point courant
        int x1 = 80 + (i * 30);
        int y1 = 200 + offsetY - ((humidities[i] - 0) * 1.5); // Adapter l'échelle pour l'humidité (2 px par %)

        // Coordonnées du point suivant
        int x2 = 80 + ((i + 1) * 30);
        int y2 = 200 + offsetY - ((humidities[i + 1] - 0) * 1.5);

        // Tracer le point et la ligne
        BSP_LCD_FillCircle(x1, y1, 2); // Cercle pour le point
        BSP_LCD_DrawLine(x1, y1, x2, y2); // Ligne entre les points
    }

    // Dernier point
    int last_x = 80 + ((num_points - 1) * 30);
    int last_y = 200 + offsetY - ((humidities[num_points - 1] - 0) * 2);
    BSP_LCD_FillCircle(last_x, last_y, 2); // Point final
}


void update_humidity_graph(float humidity, uint32_t current_time) {
    // Ajouter la nouvelle humidité et l'heure dans le tableau
    if (current_point < MAX_POINTS) {
        humidities[current_point] = humidity;
        times[current_point] = current_time;
        current_point++;
    } else {
        // Déplacer tous les points pour faire de la place au nouveau point
        for (int i = 0; i < MAX_POINTS - 1; i++) {
            humidities[i] = humidities[i + 1];
            times[i] = times[i + 1];
        }
        humidities[MAX_POINTS - 1] = humidity;
        times[MAX_POINTS - 1] = current_time;
    }

    // Redessiner le graphique avec les nouvelles données
    draw_humidity_graph(humidities, times, current_point);
}



#define MAX_POINTS 10
float pressures[MAX_POINTS];
uint32_t times[MAX_POINTS];

void draw_pressure_graph(float *pressures, uint32_t *times, int num_points) {
    int offsetY = 20; // Décalage vers le bas

    // Effacer l'écran
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, 0, 480, 275); // Effacer l'écran (fond blanc)

    // Icône retour en haut à gauche
    BSP_LCD_DrawBitmap(10, 10, (uint8_t *)back_icon_bmp);

    // Couleur des axes
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    // Ajouter un titre
    BSP_LCD_SetFont(&Font16); // Police pour le titre
    BSP_LCD_DisplayStringAt(0, 10 + offsetY, (uint8_t *)"Pressure", CENTER_MODE);

    // Dessiner les axes
    BSP_LCD_DrawLine(80, 200 + offsetY, 400, 200 + offsetY); // Axe X (horizontal)
    BSP_LCD_DrawLine(80, 200 + offsetY, 80, 50 + offsetY);   // Axe Y (vertical)

    // Ajouter des noms aux axes
    BSP_LCD_SetFont(&Font16); // Police pour les axes
    BSP_LCD_DisplayStringAt(190, 210 + offsetY, (uint8_t *)"Time(sec)", CENTER_MODE); // Nom de l'axe X
    BSP_LCD_DisplayStringAt(25, 30 + offsetY, (uint8_t *)"Pressure(hPa)", LEFT_MODE); // Nom de l'axe Y

    // Ajouter des graduations sur l'axe Y (pression de 0 à 1200 hPa)
    for (int i = 0; i <= 6; i++) {
        int y = 200 + offsetY - (i * 25); // Espacement uniforme entre graduations (25 px)
        char label[10];
        sprintf(label, "%d", i * 200); // Pression (0 à 1200 hPa par pas de 200 hPa)
        BSP_LCD_DisplayStringAt(50, y - 5, (uint8_t *)label, LEFT_MODE);
        BSP_LCD_DrawLine(75, y, 80, y); // Petite graduation
    }

    // Ajouter des graduations sur l'axe X (temps)
    for (int i = 0; i < num_points; i++) {
        int x = 80 + (i * 30); // Position X des graduations
        char label[10];
        sprintf(label, "%lu", times[i]); // Afficher le temps en secondes
        BSP_LCD_DisplayStringAt(x - 20, 210 + offsetY, (uint8_t *)label, LEFT_MODE);
        BSP_LCD_DrawLine(x, 200 + offsetY, x, 195 + offsetY); // Petite graduation
    }

    // Tracer les points et relier par des lignes
    BSP_LCD_SetTextColor(LCD_COLOR_RED); // Utilisation d'une couleur rouge pour le graphique de pression

    // Ajuster l'échelle de la pression pour l'affichage (0 à 1200 hPa)
    for (int i = 0; i < num_points - 1; i++) {
        // Coordonnées du point courant
        int x1 = 80 + (i * 30);
        int y1 = 200 + offsetY - (pressures[i] * 150 / 1200); // Calcul de l'échelle pour la pression

        // Vérification des coordonnées y1
        if (y1 < 50 + offsetY) y1 = 50 + offsetY; // Limiter à l'axe Y bas
        if (y1 > 200 + offsetY) y1 = 200 + offsetY; // Limiter à l'axe Y haut

        // Coordonnées du point suivant
        int x2 = 80 + ((i + 1) * 30);
        int y2 = 200 + offsetY - (pressures[i + 1] * 150 / 1200);

        // Vérification des coordonnées y2
        if (y2 < 50 + offsetY) y2 = 50 + offsetY; // Limiter à l'axe Y bas
        if (y2 > 200 + offsetY) y2 = 200 + offsetY; // Limiter à l'axe Y haut

        // Tracer le point et la ligne
        BSP_LCD_FillCircle(x1, y1, 2); // Cercle pour le point
        BSP_LCD_DrawLine(x1, y1, x2, y2); // Ligne entre les points
    }

    // Dernier point
    int last_x = 80 + ((num_points - 1) * 30);
    int last_y = 200 + offsetY - (pressures[num_points - 1] * 150 / 1200);

    // Vérification de la position du dernier point
    if (last_y < 50 + offsetY) last_y = 50 + offsetY;
    if (last_y > 200 + offsetY) last_y = 200 + offsetY;

    BSP_LCD_FillCircle(last_x, last_y, 2); // Point final
}


void update_pressure_graph(float pressure, uint32_t current_time) {
    // Ajouter la nouvelle pression et l'heure dans le tableau
    if (current_point < MAX_POINTS) {
        pressures[current_point] = pressure;
        times[current_point] = current_time;
        current_point++;
    } else {
        // Déplacer tous les points pour faire de la place au nouveau point
        for (int i = 0; i < MAX_POINTS - 1; i++) {
            pressures[i] = pressures[i + 1];
            times[i] = times[i + 1];
        }
        pressures[MAX_POINTS - 1] = pressure;
        times[MAX_POINTS - 1] = current_time;
    }

    // Redessiner le graphique avec les nouvelles données
    draw_pressure_graph(pressures, times, current_point);
}














