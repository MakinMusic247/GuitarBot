/*
*   Guitarbot uses SG90 servos to play songs on the guitar
*   Guitar tabs (structured as .json files) are converted into instructions for the hardware
*   A PCA9685 16 channel PWM driver board is used to control the servo motors
*   Structure: 
*       the first 3 channels are reserved for the open strings (placed on the bridge of the guitar)
*       the next n channels (4-16) are available for the fretboard
*       The fret servos are ordered by fret and string (servo_1 is fret 1, E+A strings, servo_2 is fret 1, D+G strings, etc)
*/

#ifndef __GUITARBOT_H__
#define __GUITARBOT_H__

#include <stdio.h>
#include <inttypes.h>
#include "servo.h"
#include "sdkconfig.h"


// Total number of servos being used
#ifndef CONFIG_GUITARBOT_NUM_SERVOS
    #define CONFIG_GUITARBOT_NUM_SERVOS 6 
#endif
// How many of the servos (first n channels) are for the open strings
#ifndef CONFIG_GUITARBOT_NUM_BRIDGE_SERVOS
    #define CONFIG_GUITARBOT_NUM_BRIDGE_SERVOS 3
#endif

// Default servo angles
#ifndef CONFIG_SERVO_ANGLE_DEFAULT
    #define CONFIG_SERVO_ANGLE_DEFAULT 90
#endif
#ifndef CONFIG_SERVO_ANGLE_TOP
    #define CONFIG_SERVO_ANGLE_TOP 160
#endif
#ifndef CONFIG_SERVO_ANGLE_BOTTOM
    #define CONFIG_SERVO_ANGLE_BOTTOM 35
#endif

#define N_SERVOS        CONFIG_GUITARBOT_NUM_SERVOS
#define N_BRIDGE_SERVOS CONFIG_GUITARBOT_NUM_BRIDGE_SERVOS
#define N_FRET_SERVOS   N_SERVOS -  N_BRIDGE_SERVOS

/* Servo angles */
#define SERVO_ANGLE_DEFAULT     CONFIG_SERVO_ANGLE_DEFAULT
#define SERVO_ANGLE_TOP         CONFIG_SERVO_ANGLE_TOP 
#define SERVO_ANGLE_BOTTOM      CONFIG_SERVO_ANGLE_BOTTOM 

/* Bridge servos are reversed (placed opposite way to fret servos) */
#define BRIDGE_SERVO_ANGLE_TOP         75
#define BRIDGE_SERVO_ANGLE_BOTTOM      105


typedef enum { ARM_TOP = 0, ARM_BOTTOM = 1 } servo_direction_t;


typedef struct {
    servo_t *servo;             // servo reference
    uint8_t fret;               // which fret the servo is placed on (1-24)
    uint8_t top_string;         // string is pressed down by the top side of the servo (EADGBe --> 1,2,3,4,5,6)
    uint8_t bottom_string;      // string is pressed down by the bottom side of the servo
    uint8_t prefered_string;    // which string is preferred to be played if both strings are requested at the same time
    float top_string_angle;     // servo angle to hit top string
    float bottom_string_angle;  // servo angle to hit bottom string
    float current_angle;        // current angle of the servo arm
} guitar_fret_servo_t;

typedef struct {
    servo_t *servo;              // servo reference
    uint8_t top_string;         // string is pressed down by the top side of the servo
    uint8_t bottom_string;      // string is pressed down by the bottom side of the servo
    float top_string_angle;     // servo angle to hit top string
    float bottom_string_angle;  // servo angle to hit bottom string
    float current_angle;        // current angle of the servo arm
} guitar_bridge_servo_t;


/// @brief A music note containing
typedef struct {
    uint8_t string; // 0-5 (EADGBe)
    uint8_t fret;   // Fret (0 = open string, 1 = fret 1, ...)
} note_t;

typedef struct {
    uint16_t num;   // beat number
    note_t *notes;  // An array of notes
    size_t n_notes; // number of notes in this beat
} beat_t;



/// @brief Play a note on the guitar given a string and fret
/// @param string 
/// @param fret 
/// @return 
esp_err_t play_note(uint8_t string, uint8_t fret);

esp_err_t guitarbot_init(servo_t servos[], guitar_fret_servo_t **out_fret_servos, guitar_bridge_servo_t **out_bridge_servos);

esp_err_t test_play();

bool tab_playing();
bool setup_complete();



#endif