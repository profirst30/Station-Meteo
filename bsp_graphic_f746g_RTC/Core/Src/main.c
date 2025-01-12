/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "rtc.h"
#include "sdmmc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include <string.h>
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#include "Interface.h"
#include "Nucleo_HumTemp_I2C.h"
#include "Nucleo_Pression_I2C.h"
#include "Rain_Measure_Sparkfun.h"
#include "SD_card.h"
#include "Wind_Dir_Sparkfun.h"
#include "Wind_Speed_Sparkfun.h"
/*#include "Rain_Measure_Sparkfun.h"
#include "Wind_Dir_Sparkfun.h"
#include "Wind_Speed_Sparkfun.h"
*/
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//Taille de l'écran

volatile TS_StateTypeDef  TS_State;
volatile uint16_t x, y;
int needRedraw=1;
int debounce = 0; // Debounce flag

// Variable pour suivre l'état du système
extern uint8_t minute_flag;  // Flag pour indiquer que 1 minute est écoulée
uint8_t flag=0;
volatile uint8_t flag_meteo=0;
extern int graph_open_flag;

uint8_t flag_ext_button=0;
uint8_t time_IRQ_acquisition;
uint8_t flag_IRQ_sleep=0;

extern uint32_t rainCount;
extern uint8_t flagRain;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void TouchTimer_Init(void);

/**
 * @brief Récupère la date formatée depuis la RTC
 * @param dateStr: buffer pour stocker la chaîne formatée
 */
void getRTCDateStr(char* dateStr) {
    // Lecture de l'heure (nécessaire avant la date)
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    // Lecture de la date
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // Format: "Jour JJ/MM/AAAA"
    sprintf(dateStr, "%02d/%02d/20%02d",
            sDate.Date,
            sDate.Month,
            sDate.Year);
}

/**
 * @brief Récupère l'heure formatée depuis la RTC
 * @param timeStr: buffer pour stocker la chaîne formatée
 */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  MX_TIM3_Init();
  MX_RTC_Init();
  MX_SDMMC1_SD_Init();
  MX_I2C1_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
  BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  init_HumTemp();
  init_Pression();
  //BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  TouchTimer_Init();
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim7);

  __HAL_TIM_SET_COUNTER(&htim5, 0);           // Réinitialise le compteur à 0
  __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_UPDATE); // Efface le drapeau d'interruption
  HAL_TIM_Base_Start_IT(&htim5);

  __HAL_TIM_SET_COUNTER(&htim8, 0);           // Réinitialise le compteur à 0
  __HAL_TIM_CLEAR_FLAG(&htim8, TIM_FLAG_UPDATE); // Efface le drapeau d'interruption
  HAL_TIM_Base_Start_IT(&htim8);

  __HAL_TIM_SET_COUNTER(&htim2, 0);           // Réinitialise le compteur à 0
  __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE); // Efface le drapeau d'interruption
  HAL_TIM_Base_Start_IT(&htim2);

  init_sd_logging();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
         //machine d' etat de l'inteface graphique
         if (needRedraw) {
             switch (flag) {
                 case 0:
                     drawMenuPage();
                     needRedraw = 0; // Reset the redraw flag
                     break;

                 case 1:
                	 drawTempHumidRainPage();
                     needRedraw = 0; // Reset the redraw flag
                     break;

                 case 2:
                      drawWindDirPressurePage();
                      needRedraw = 0; // Reset the redraw flag
                      break;

                 case 3:
                      drawSettingsPage();
                      needRedraw = 0; // Reset the redraw flag
                      break;

                  case 4:
                      drawCreditsPage();
                      needRedraw = 0; // Reset the redraw flag
                      break;

                  case 5:
                	  valeur_graph_TempH();
                      //needRedraw = 0; // Reset the redraw flag
                      break;

                  case 6:
                	  value_humidity_graph();
                      //needRedraw = 0; // Reset the redraw flag
                      break;

                  case 7:
                	  //drawTempHumGraphPage();
                      needRedraw = 0; // Reset the redraw flag
                      break;

                  case 8:
                	  manage_pressure_graph();
                      //needRedraw = 0; // Reset the redraw flag
                      break;

                  case 9:
                	  //drawRainPressGraphPage();
                      needRedraw = 0; // Reset the redraw flag
                      break;
                }
         }

         //affichage des valeurs toutes les 5 secondes
         else if(time_IRQ_acquisition)
         {
			 if(flag==1){
					 valeur_TempH();
					 valeur_Hum();
					 if(flag_meteo==0){
						 Draw_Rain_Measure_Value();
					 }
				 }

  	  	  	 else if(flag==2){
  	  	  		 valeur_Pression();
  	  	  		 if(flag_meteo==0){
					 Wind_Dir_Value();
					 Wind_Speed_Value();
  	  	  		 }
  			 }

			 time_IRQ_acquisition=0;
         }

		 //condition de mise en sommeil et de réveil
		 else if (flag_IRQ_sleep == 1) {
			BSP_LCD_DisplayOff();
			printf("Entrée en sommeil\r\n");

			// Entrer en mode STOP
			HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			//HAL_SuspendTick() et HAL_ResumeTick() non utilisé pour la RTC;


			//reactivation et redessinage de l'interface
			SystemClock_Config(); // Reconfigure les horloges
			BSP_LCD_Init();
			BSP_LCD_DisplayOn();  // Rallume l'écran
			BSP_LCD_SetFont(&Font16); // Police de caractères pour l'affichage

			needRedraw = 1;

			//redémarrage du compteur
			HAL_TIM_Base_Stop_IT(&htim5);  // Arrêter le timer en mode veille
			__HAL_TIM_SET_COUNTER(&htim5, 0);  // Réinitialiser le compteur du timer
			__HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_UPDATE); // Efface le drapeau d'interruption
			HAL_TIM_Base_Start_IT(&htim5);  // Redémarrer le timer
			printf("Réveil de l'écran\r\n");

  	  	 }

         //changement d'état par interruption sur bouton du touch screen
         if (TS_State.touchDetected && !debounce) {

        	 debounce=1;
			   //hum Temp Rain button
			  if (flag == 0 && x > 40 && x < 220 && y > 60 && y < 220) {
				  flag = 1;
				  needRedraw = 1; // Set the redraw flag

			  //return menu button
			  } else if ((flag == 1 || flag == 2 || flag == 3 || flag == 4) && x > 10 && x < 40 && y > 10 && y < 40) {
				  flag = 0;
				  needRedraw = 1; // Set the redraw flag

			   //wind dir pressure button
			  } else if (flag == 0 && x > 260 && x < 440 && y > 60 && y < 220) {
				  flag = 2;
				  needRedraw = 1; // Set the redraw flag

			   //setting button
			  } else if (flag == 0 && x > 438 && x < 480 && y > 235 && y < 272) {
				  flag = 3;
				  needRedraw = 1; // Set the redraw flag

			  //credits button
			  } else if (flag == 0 && x > 18 && x < 58 && y > 235 && y < 272) {
				  flag = 4;
				  needRedraw = 1; // Set the redraw flag

			  //Temperature graph
			  } else if (flag == 1 && x > 40 && x < 160 && y > 80 && y < 200) {
				  flag = 5;
				  needRedraw = 1; // Set the redraw flag

			  //humidity graph
         	  } else if (flag == 1 && x > 180 && x < 300 && y > 80 && y < 200){
         		 flag = 6;
         		 needRedraw = 1; // Set the redraw flag

         	  //Rain graph
         	  } /*else if (flag == 1 && x > 320 && x < 440 && y > 80 && y < 200){
          		 flag = 7;
          		 needRedraw = 1; // Set the redraw flag

          	  //Pressure graph
         	  }*/ else if (flag == 2 && x > 320 && x < 440 && y > 80 && y < 200){
				 flag = 8;
				 needRedraw = 1; // Set the redraw flag

			  //Wind Graph
			  }/*else if (flag == 2 && x > 40 && x < 160 && y > 80 && y < 200){
				 flag = 9;
				 needRedraw = 1; // Set the redraw flag

			  //return data page 1 button
			  }*/ else if ((flag == 5 || flag == 6 || flag == 7) && x > 10 && x < 40 && y > 10 && y < 40) {
				  flag = 1;
				  needRedraw = 1; // Set the redraw flag

			  //return data page 2 button
			  } else if ((flag == 8 || flag == 9) && x > 10 && x < 40 && y > 10 && y < 40) {
				  flag = 2;
				  needRedraw = 1; // Set the redraw flag

			  } else if (flag == 3) {
				      const uint16_t col1 = 150;
				      const uint16_t col2 = col1 + 90;
				      const uint16_t col3 = col2 + 90;
				      const uint16_t buttonSize = 30;

				      // Bouton retour
				      if (x > 10 && x < 40 && y > 10 && y < 40) {
				          flag = 0;
				          needRedraw = 1;
				      }
				      // === Boutons Date ===
				      // Jour +
				      else if (x >= col1 && x <= col1 + buttonSize && y >= 40 && y <= 70) {
				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sDate.Date = (sDate.Date % 31) + 1;
				          needRedraw = 1;
				      }
				      // Jour -
				      else if (x >= col1 && x <= col1 + buttonSize && y >= 105 && y <= 135) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sDate.Date = (sDate.Date > 1) ? sDate.Date - 1 : 31;
				          needRedraw = 1;
				      }
				      // Mois +
				      else if (x >= col2 && x <= col2 + buttonSize && y >= 40 && y <= 70) {
				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sDate.Month = (sDate.Month % 12) + 1;
				          needRedraw = 1;
				      }
				      // Mois -
				      else if (x >= col2 && x <= col2 + buttonSize && y >= 105 && y <= 135) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sDate.Month = (sDate.Month > 1) ? sDate.Month - 1 : 12;
				          needRedraw = 1;
				      }
				      // === Boutons Heure ===
				      // Heures +
				      else if (x >= col1 && x <= col1 + buttonSize && y >= 140 && y <= 170) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sTime.Hours = (sTime.Hours + 1) % 24;
				          needRedraw = 1;
				      }
				      // Heures -
				      else if (x >= col1 && x <= col1 + buttonSize && y >= 200 && y <= 230) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sTime.Hours = (sTime.Hours > 0) ? sTime.Hours - 1 : 23;
				          needRedraw = 1;
				      }
				      // Minutes +
				      else if (x >= col2 && x <= col2 + buttonSize && y >= 140 && y <= 170) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sTime.Minutes = (sTime.Minutes + 1) % 60;
				          needRedraw = 1;
				      }
				      // Minutes -
				      else if (x >= col2 && x <= col2 + buttonSize && y >= 200 && y <= 230) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          sTime.Minutes = (sTime.Minutes > 0) ? sTime.Minutes - 1 : 59;
				          needRedraw = 1;
				      }
				      // Année +
				      else if (x >= col3 && x <= col3 + buttonSize && y >= 40 && y <= 70) {
				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          if(sDate.Year < 99) {  // Limite pour format BCD
				              sDate.Year++;
				              HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				              needRedraw = 1;
				          }
				      }
				      // Année -
				      else if (x >= col3 && x <= col3 + buttonSize && y >= 105 && y <= 135) {

				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          if(sDate.Year > 0) {
				              sDate.Year--;
				              HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				              needRedraw = 1;
				          }
				      }
				      // Bouton Valider
				      else if (x >= 340 && x <= 440 && y >= 220 && y <= 250) {
				          HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          // Mise à jour de la date et de l'heure
				          HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				          HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          flag = 0;
				          needRedraw = 1;
				      }

				      // Si une valeur a été modifiée, on met à jour la RTC
				      if (needRedraw) {
				          if (flag == 3) {  // Si on est toujours sur la page de réglage
				              HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				              HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				          }
				      }
				  //remise à 0 du timer 5 si l'écran est touché (Mise en sommeil).

				  }
			  	  HAL_TIM_Base_Stop(&htim5);
			  	  __HAL_TIM_SET_COUNTER(&htim5, 0);
			  	  HAL_TIM_Base_Start(&htim5);


		  } else if (!TS_State.touchDetected) {
			  debounce = 0; // Reset debounce flag when touch is released
		  }
		 //HAL_Delay(50); // Delay to reduce loop speed and debounce effect
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void TouchTimer_Init() {
    BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        while (1);
    }
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        while (1);
    }
}

void PollTouchScreen(void)
{
	static TS_StateTypeDef prev_state;    /* Previous touch state from the touch sensor used from BSP package */
	TS_StateTypeDef ts;                   /* Actual touch state from the touch sensor used from BSP package */
	BSP_TS_GetState(&ts);                 /* Read the touch state from touch sensor (BSP API)*/
	TS_State.touchDetected = ts.touchDetected;  /* Store pressed state */

	/* Check if the touch is pressed */
	if ((prev_state.touchDetected != ts.touchDetected))
	{
		prev_state.touchDetected = ts.touchDetected;
		/* Check touch variations */
		if ((ts.touchX[0] != 0) && (ts.touchY[0] != 0))
		{
		  prev_state.touchX[0] = ts.touchX[0];
		  prev_state.touchY[0] = ts.touchY[0];
		}
		x = prev_state.touchX[0];
		y = prev_state.touchY[0];
	}
}

//interruption timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */

  /* USER CODE BEGIN Callback 1 */
  //routine de l'écran tactile
  if (htim->Instance == TIM3)
  {
	  PollTouchScreen();
 	  date_Heure();

  }
  //routine d'interruption pour un arret à 1 minute (Annemometre)
  else if (htim->Instance == TIM6) {
	  // Timer pour mesurer la valeur moyenne du vent
	  minute_flag = 1; // Activer le flag indiquant la fin de 1 minute (5s pour le test)
	  printf("Minute flag set!\r\n"); // Pour vérifier que l'interruption est déclenchée
  }

  //capteur
  else if(htim->Instance == TIM8) {
  	  // Timer valeur toute les 5s
	  time_IRQ_acquisition=1; // Activer le flag indiquant la fin de 1 minute
    }

  else if(htim->Instance == TIM7) {
	  	//écriture des valeurs sur la carte SD;
          printf("Tentative d'enregistrement...\n");  // Debug
          log_weather_data();
    }

  else if(htim->Instance == TIM5) {
	  // Timer 30 seconde atteint(mise en veille)
	  flag_IRQ_sleep=1; // Activer le flag indiquant la fin de 1 minute
  }

  else if(htim->Instance == TIM2) {
  	  // Timer si 1 minute atteint
  	  flag_meteo=1; // Activer le flag indiquant la fin de 1 minute
  }
  /* USER CODE END Callback 1 */
}

//interruption GPIO
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    UNUSED(GPIO_Pin);

    if(GPIO_Pin == GPIO_PIN_15){
    	  rainCount++;
    	  flagRain=1;
    }

    else if (GPIO_Pin == sleep_button_Pin) {
          flag_ext_button = 1;
    	  flag_IRQ_sleep=0;
    }
}

PUTCHAR_PROTOTYPE
{
/* Place your implementation of fputc here */
/* e.g. write a character to the USART1 and Loop until the end of transmission */
HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);
return ch;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
