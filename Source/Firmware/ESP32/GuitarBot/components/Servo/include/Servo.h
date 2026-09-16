/*
*   Abstracted class for controlling servo motors with the PCA9685
*   The servo runs at 50Hz (period = 20ms), controlling the pulse width sets the angle of the servo arm
*   More info: http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf
*   Example: 
*       Center (0 deg)  --> 1.5ms pulse 
*       Right (90 deg)  --> 2ms pulse
*       Left  (-90 deg) --> 1ms pulse
*/


#ifndef __SERVO_H__
#define __SERVO_H__

#include "esp_log.h"
#include <esp_err.h>
#include <stdbool.h>
#include <stdio.h>

#include "pca9685.h"


/*
*   Standard values
*   SG90 servos typically operate at 50Hz PWM
*   Typical pulse width range is 1000 --> 2000 us (0 --> 180 degrees)
*/
#define SERVO_FREQ_HZ_DEFAULT   50      // Default frequency for SG90 servo
#define SERVO_MIN_PULSE_US      500     
#define SERVO_MAX_PULSE_US      2400
#define SERVO_MIN_ANGLE         0.0f    
#define SERVO_MAX_ANGLE         180.0f
#define SERVO_DEFAULT_ANGLE     90.0f



// Servo class - used for interacting with servo motor (set and check angle)
typedef struct {
    pca9685_dev_t dev;      // servo attached to PCA9685 board
    uint8_t channel;        // channel of PCA9685
    float min_angle;        // minimum angle of servo
    float max_angle;        // maximum angle of servo
    float default_angle;    // default starter angle of servo
    float current_angle;    // current angle of servo
} servo_t;

// Servo config class
typedef struct {
    pca9685_dev_t dev;  // servo attached to PCA9685 board
    uint8_t channel;    // channel of PCA9685
} servo_config_t;



/// @brief Initialize the servo motor
/// @param config servo configuration type
/// @param out_servo outputted servo type
/// @return 
esp_err_t servo_init(servo_config_t *config, servo_t *out_servo);


esp_err_t servo_set_angle(servo_t *servo, float angle);


// esp_err_t servo_get_angle();


// esp_err_t servo_set_center();


#endif
