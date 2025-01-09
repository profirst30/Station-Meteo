/*
 * Nucleo_I2C.h
 *
 *  Created on: Dec 2, 2024
 *      Author: Maxime
 */

#ifndef INC_NUCLEO_PRESSION_I2C_H_
#define INC_NUCLEO_PRESSION_I2C_H_


#include "stm32f7xx_hal.h"
#include "lps22hh_reg.h"

void init_Pression(void);

void valeur_Pression(void);
void valeur_TempP(void);
void valeur_Pression_graph(void);
void manage_pressure_graph();




#endif /* INC_NUCLEO_PRESSION_I2C_H_ */
