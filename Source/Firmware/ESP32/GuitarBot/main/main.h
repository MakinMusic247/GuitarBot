#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#include "pca9685.h"
#include "servo.h"
#include "Guitarbot.h"

// UI
#include "ST7789.h"
#include "SD_MMC.h"
#include "UI_main.h"


/* 
 *  Guitar tab to be played 
 *  each array of length 6 corresponds to the strings {E,A,D,G,B,E} and the fret to be played
 *  -1 => no note played on string, 0 => open string, 1 => fret 1 of string, ...
*/
int tab[][6] = { // each array of size 6
                {-1, -1, -1, -1, -1, -1},  // beat 1
                { 0, -1, -1, -1, -1, -1},  // beat 2
                {-1,  0, -1, -1, -1, -1},  // beat 3...
                {-1, -1,  0, -1, -1, -1},
                {-1, -1, -1,  0, -1, -1},
                {-1, -1, -1, -1,  0, -1},
                {-1, -1, -1, -1, -1,  0},
                {-1, -1, -1, -1, -1, -1},
                { 1, -1, -1, -1, -1, -1},
                {-1,  1, -1, -1, -1, -1},
                {-1, -1,  1, -1, -1, -1},
                {-1, -1, -1,  1, -1, -1},
                {-1, -1, -1, -1,  1, -1},
                {-1, -1, -1, -1, -1,  1},
                {-1, -1, -1, -1, -1, -1}
              };



#endif