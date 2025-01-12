/*
 * Nucleo_I2C.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Maxime
 */

#include <Nucleo_Pression_I2C.h>
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f7xx_hal.h"

#include <string.h>
#include <stdio.h>
#include "lps22hh_reg.h"
#include "stm32746g_discovery_lcd.h"
#include "Interface.h"

#define SENSOR_BUS hi2c1
#define BOOT_TIME  5 //ms
#define TX_BUF_DIM 1000


const uint16_t squareSizeP = 120;
const uint16_t spacingP = 23;
const uint16_t startXP = 40;
const uint16_t startYP = 80;  // Ajusté pour centrer verticalement


static uint32_t data_raw_pressure;
volatile static float pressure_hPa;
static uint8_t whoamI, rst;
static uint8_t tx_buffer[TX_BUF_DIM];



static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void platform_delay(uint32_t ms);
stmdev_ctx_t dev_ctx;

lps22hh_reg_t reg;


void init_Pression(void) {

    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;
    dev_ctx.handle = &SENSOR_BUS;

    platform_delay(BOOT_TIME);

    whoamI = 0;
    lps22hh_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != LPS22HH_ID) {
        while (1); // Gérer l'erreur si le capteur n'est pas trouvé
    }

    lps22hh_reset_set(&dev_ctx, PROPERTY_ENABLE);

    do {
        lps22hh_reset_get(&dev_ctx, &rst);
    } while (rst);

    lps22hh_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    lps22hh_data_rate_set(&dev_ctx, LPS22HH_10_Hz_LOW_NOISE);
}

void valeur_Pression(void) {


    lps22hh_read_reg(&dev_ctx, LPS22HH_STATUS, (uint8_t *)&reg, 1);

    if (reg.status.p_da) {
        memset(&data_raw_pressure, 0x00, sizeof(uint32_t));
        lps22hh_pressure_raw_get(&dev_ctx, &data_raw_pressure);
        pressure_hPa = lps22hh_from_lsb_to_hpa(data_raw_pressure);
        snprintf((char *)tx_buffer, sizeof(tx_buffer), "%6.2f hPa", pressure_hPa);
        //printf((char const *)tx_buffer);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(startXP+2 * (squareSizeP + spacingP) + 20, startYP + 60, (uint8_t*)tx_buffer, LEFT_MODE);

    }
}


void valeur_Pression_graph(void) {


    lps22hh_read_reg(&dev_ctx, LPS22HH_STATUS, (uint8_t *)&reg, 1);

    if (reg.status.p_da) {
        memset(&data_raw_pressure, 0x00, sizeof(uint32_t));
        lps22hh_pressure_raw_get(&dev_ctx, &data_raw_pressure);
        pressure_hPa = lps22hh_from_lsb_to_hpa(data_raw_pressure);

        uint32_t current_time = get_current_time_in_seconds();

        update_pressure_graph(pressure_hPa, current_time);

    }
}

void manage_pressure_graph() {
    static uint32_t last_second = 0;  // Dernière seconde enregistrée
    uint32_t current_second = get_current_time_in_seconds(); // Récupérer l'heure en secondes

    // Si une seconde s'est écoulée, mettre à jour le graphique
    if (current_second != last_second) {
        last_second = current_second; // Mettre à jour la dernière seconde enregistrée
        valeur_Pression_graph(); // Lire la pression et mettre à jour le graphique
    }
}

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
    HAL_I2C_Mem_Write(handle, LPS22HH_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)bufp, len, 1000);
    return 0;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
    HAL_I2C_Mem_Read(handle, LPS22HH_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
    return 0;
}

static void platform_delay(uint32_t ms) {
    HAL_Delay(ms);
}

