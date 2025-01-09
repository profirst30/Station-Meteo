/*
 * SD_card.c
 *
 *  Created on: Jan 8, 2025
 *      Author: Maxime
 */

//#include "SD_card.h"

/*void register_SD_CARD(int *array, int size) {
    //-1- Link the micro SD disk I/O driver//
    // if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
    // {

    ///-2- Register the file system object to the FatFs module ///
    if (f_mount(&SDFatFS, (TCHAR const *)SDPath, 0) != FR_OK) {
        // FatFs Initialization Error //
        Error_Handler();
    } else {
        //-3- Create a FAT file system (format) on the logical drive //
        // WARNING: Formatting the uSD card will delete all content on the device //
        if (f_mkfs((TCHAR const *)SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer)) != FR_OK) {
            printf("Formatting failed\n");
            // FatFs Format Error //
            Error_Handler();
        } else {
            //-4- Create and Open a new text file object with write access //
            if (f_open(&SDFile, "temp.csv", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
                printf("File opening for writing failed\n");
                // 'test.csv' file Open for write Error //
                Error_Handler();
            } else {
                //-5- Write array data along with RTC time to the CSV file //
                char buffer[100];
                UINT byteswritten;
                FRESULT res;

                // Write CSV header //
                snprintf(buffer, sizeof(buffer), "Year/Month/Day;Hour:Minute:Second;Value\n");
                res = f_write(&SDFile, buffer, strlen(buffer), (void *)&byteswritten);
                if ((byteswritten == 0) || (res != FR_OK)) {
                    printf("Error writing header to file\n");
                    Error_Handler();
                }

                for (int i = 0; i < size; i++) {
                    // Fetch RTC time //
                    RTC_TimeTypeDef sTime;
                    RTC_DateTypeDef sDate;

                    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
                    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

                    // Format data into CSV row //
                    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d;%02d:%02d:%02d;%d\n",
                             2000 + sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds, array[i]);

                    res = f_write(&SDFile, buffer, strlen(buffer), (void *)&byteswritten);

                    if ((byteswritten == 0) || (res != FR_OK)) {
                        printf("Error writing to file\n");
                        // 'test.csv' file Write or EOF Error //
                        Error_Handler();
                        break;
                    }
                }

                //-6- Close the open text file //
                f_close(&SDFile);

                printf("Data written to CSV successfully\n");
            }
        }
    }

    FATFS_UnLinkDriver(SDPath);
}
*/


