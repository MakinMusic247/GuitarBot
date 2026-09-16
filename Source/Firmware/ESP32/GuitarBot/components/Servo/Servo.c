#include "Servo.h"

static const char *SERVO_TAG = "Servo";


/***********************************************************************************/
// Helper functions


static uint16_t convert_angle_to_pulse_us(float angle)
{
    ESP_LOGI(SERVO_TAG, "Converting angle to pulse");
    // Get range for servo angle and pulse
    float angle_range = SERVO_MAX_ANGLE - SERVO_MIN_ANGLE;
    float pulse_range = SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US;

    // Map the angle to pulse using interpolation
    // y = out_min + ( (x - x_min) * (out_max - out_min) ) / (in_max - in_min)
    float pulse = SERVO_MIN_PULSE_US + ( ((angle - SERVO_MIN_ANGLE) * pulse_range)/(angle_range));
    ESP_LOGI(SERVO_TAG, "angle = %f --> pulse = %f", angle, pulse);
    return (uint16_t)pulse;
}



/***********************************************************************************/
// Main servo control functions

esp_err_t servo_init(servo_config_t *config, servo_t *out_servo)
{
    esp_err_t err;
    ESP_LOGI(SERVO_TAG, "initializing servo");

    if(!config) return ESP_ERR_INVALID_ARG;

    // Initialize empty servo struct
    if(!out_servo) out_servo = calloc(1, sizeof(servo_t));
    if(!out_servo){
        ESP_LOGE(SERVO_TAG, "Memory not allocated");
        return ESP_ERR_NO_MEM;
    }

    out_servo->dev = config->dev;
    out_servo->channel = config->channel;
    out_servo->default_angle = SERVO_DEFAULT_ANGLE;
    out_servo->max_angle = SERVO_MAX_ANGLE;
    out_servo->min_angle = SERVO_MIN_ANGLE;
    out_servo->current_angle = out_servo->default_angle; // needs to be calculated

    return ESP_OK;

}



esp_err_t servo_set_angle(servo_t *servo, float angle)
{
    esp_err_t err;
    ESP_LOGI(SERVO_TAG, "Setting servo angle %f", angle);

    if(!servo){
        ESP_LOGE(SERVO_TAG, "Invalid arguments. Check that servo is initialized and angle is valid");
        return ESP_ERR_INVALID_ARG;
    }

    // Clamp the servo angle
    if(angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
    if(angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;

    // Convert the angle to pulse
    uint16_t pulse = convert_angle_to_pulse_us(angle);

    // Set the PWM pulse on the PCA9685
    pca9685_dev_t *pca9685 = &servo->dev;
    err = pca9685_set_pwm_pulse(pca9685, servo->channel, pulse);
    if(err != ESP_OK){
        ESP_LOGE(SERVO_TAG, "Setting PWM pulse failed %s", esp_err_to_name(err));
        return err;
    }

    servo->current_angle = angle;
    return ESP_OK;

}
