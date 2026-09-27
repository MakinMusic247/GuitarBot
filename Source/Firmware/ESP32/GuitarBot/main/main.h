#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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


// typedef struct {
//     servo_t servo;              // servo reference
//     uint8_t fret;               // which fret the servo is placed on (1-24)
//     uint8_t left_string;        // string is pressed down by the left side of the servo (EADGBe --> 1,2,3,4,5,6)
//     uint8_t right_string;       // string is pressed down by the right side of the servo
//     uint8_t prefered_string;    // which string is preferred to be played if both strings are requested at the same time
// } guitar_fret_servo_t;

// typedef struct {
//     servo_t servo;          // servo reference
//     uint8_t left_string;    // string is pressed down by the left side of the servo
//     uint8_t right_string;   // string is pressed down by the right side of the servo
// } guitar_string_servo_t;


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