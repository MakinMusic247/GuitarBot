#ifndef __GUITARBOT_H__
#define __GUITARBOT_H__

#include <stdio.h>
#include <inttypes.h>
#include "servo.h"

typedef struct {
    servo_t servo;              // servo reference
    uint8_t fret;               // which fret the servo is placed on (1-24)
    uint8_t left_string;        // string is pressed down by the left side of the servo (EADGBe --> 1,2,3,4,5,6)
    uint8_t right_string;       // string is pressed down by the right side of the servo
    uint8_t prefered_string;    // which string is preferred to be played if both strings are requested at the same time
} guitar_fret_servo_t;

typedef struct {
    servo_t servo;          // servo reference
    uint8_t left_string;    // string is pressed down by the left side of the servo
    uint8_t right_string;   // string is pressed down by the right side of the servo
} guitar_string_servo_t;



#endif