/*
 * Nucleo_I2C.h
 *
 *  Created on: Dec 2, 2024
 *      Author: Maxime
 */

#ifndef INC_NUCLEO_HUMTEMP_I2C_H_
#define INC_NUCLEO_HUMTEMP_I2C_H_


#include "stm32f7xx_hal.h"
#include "hts221_reg.h"


void init_HumTemp(void);

void valeur_Hum(void);
void valeur_TempH(void);
void valeur_graph_TempH(void);
void value_humidity_graph(void);
void valeur_TempH_archive(void);
float valeur_TempH_SD(void);


#endif /* INC_NUCLEO_PRESSION_I2C_H_ */
