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

#define I2C_MASTER_SCL_IO   9
#define I2C_MASTER_SDA_IO   8
#define N_SERVOS            3


static const char *MAIN_TAG = "Main";

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

servo_t servos[N_SERVOS];


esp_err_t setup()
{
    esp_err_t err;
    ESP_LOGI(MAIN_TAG, "Setting up servos");

    /* Setup the I2C master bus */
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    // Initialize the I2C master bus
    err = i2c_new_master_bus(&bus_config, &bus_handle);
    if(err != ESP_OK){
        ESP_LOGE(MAIN_TAG, "I2C initialization failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(MAIN_TAG, "I2C master bus initialized successfully.");


    /* Setup the PCA9685 and servo motors */

    // PCA9685
    pca9685_dev_t pca9685_dev;
    err = pca9685_init(&bus_handle, PCA9685_I2C_ADDRESS_DEFAULT, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, &pca9685_dev);
    if(err != ESP_OK){
        ESP_LOGI(MAIN_TAG, "PCA9685 initialization failed: %s", esp_err_to_name(err));
        return err;
    }

    // Setup servos in a list
    for(int i = 0; i < N_SERVOS; i++){
        ESP_LOGI(MAIN_TAG, "Setting up servo channel: %d", i);
        servo_config_t servo_config = {
            .dev = pca9685_dev,
            .channel = i
        };
        err = servo_init(&servo_config, &servos[i]);
        if (err != ESP_OK) {
            ESP_LOGE(MAIN_TAG, "servo_init failed for channel %d: %s", i, esp_err_to_name(err));
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    if(err != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Setup failed: %s", esp_err_to_name(err));
        return err;
    }

    // Set to default angle (90 degrees)
    vTaskDelay(pdMS_TO_TICKS(1000));
    // err = pca9685_set_pwm_test(&pca9685_dev, 0, 0, 307);
    for(int i = 0; i < N_SERVOS; i++){
        err = servo_set_angle(&servos[i], 90);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    return ESP_OK;


}



void app_main(void)
{
    ESP_LOGI(MAIN_TAG, "Starting...\n");
    esp_err_t err;

    err = setup();
    if(err != ESP_OK) ESP_LOGE(MAIN_TAG, "Setup failed: %s", esp_err_to_name(err));

    while(1)
    {
        // for(int i=0; i < N_SERVOS; i++){
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        //     servo_set_angle(&servos[i], 0);
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        //     servo_set_angle(&servos[i], 90);
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        //     servo_set_angle(&servos[i], 180);
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        // }
    }

    
}

