/*
 * Interface.h
 *
 *  Created on: Jan 6, 2025
 *      Author: Maxime
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

void getRTCTimeStr(char* timeStr);
void drawMenuPage(void);
void drawCreditsPage(void);
void drawTempHumidRainPage(void);
void drawWindDirPressurePage(void);
void drawSettingsPage(void);
void drawTempHumGraphPage(void);
void drawRainPressGraphPage(void);
void draw_temperature_graph();
void update_temperature_graph(float new_temp, uint32_t current_time);
void draw_humidity_graph(float *humidities, uint32_t *times, int num_points);
void update_humidity_graph(float humidity, uint32_t current_time);
void draw_pressure_graph(float *pressures, uint32_t *times, int num_points);
void update_pressure_graph(float pressure, uint32_t current_time);

uint32_t get_current_time_from_rtc(void);
void date_Heure(void);
uint32_t get_current_time_in_seconds(void);

#endif /* INC_INTERFACE_H_ */
