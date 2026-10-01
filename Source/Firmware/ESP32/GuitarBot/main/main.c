/*
    GuitarBot main.c
    An array stores each beat, and each note played on the beat (multiple notes = a chord)
    The song bpm determines how many beats are played per minute (e.g. 60 bpm = 1 beat per second)

*/

#include "main.h"
#include "Guitarbot.h"


#define I2C_MASTER_SCL_IO   10 //9
#define I2C_MASTER_SDA_IO   11 //8
// #define N_SERVOS            6


static const char *MAIN_TAG = "Main";

servo_t servos[N_SERVOS]; 
float bpm = 60.0f; 

TaskHandle_t xHandle = NULL;



/// @brief Task to play each beat, run every x = bpm/60 seconds
/// @param pvParameters 
void play_song_task(void * pvParameters)
{
    uint16_t beat = 0; // Start at beat zero

    TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize the last wake time with the current tick count
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Convert 1 second (1000 ms) into FreeRTOS ticks

    while (1) {
        // Wait for the next cycle (exactly 1 second from the last wake time)
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Your code here runs every 1 second
        ESP_LOGI(MAIN_TAG, "Playing beat %u", beat);

        if(beat % 2 == 0) {
            servo_set_angle(&servos[0], 45);
            servo_set_angle(&servos[1], 135);
            servo_set_angle(&servos[2], 45);
            ESP_LOGI(MAIN_TAG, "Servos set to 90");
        }
        else {
            servo_set_angle(&servos[0], 135);
            servo_set_angle(&servos[1], 45);
            servo_set_angle(&servos[2], 135);
            ESP_LOGI(MAIN_TAG, "Servos set to 180");
        }

        beat++;
    }
}


SemaphoreHandle_t lvgl_mutex;

void lvgl_task(void *arg)
{
    while (1) {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        uint32_t delay_ms = lv_timer_handler();
        xSemaphoreGive(lvgl_mutex);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}



void Waveshare_Driver_Init(void)
{
    Flash_Searching(); // SD card
}




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
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if(err != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Setup failed: %s", esp_err_to_name(err));
        return err;
    }

    // Set to default angle (90 degrees)
    vTaskDelay(pdMS_TO_TICKS(500));
    // err = pca9685_set_pwm_test(&pca9685_dev, 0, 0, 307);
    for(int i = 0; i < N_SERVOS; i++){
        err = servo_set_angle(&servos[i], 90);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // Setup the Guitarbot
    guitar_fret_servo_t *fret_servos = NULL;
    guitar_bridge_servo_t *bridge_servos = NULL;
    err = guitarbot_init(servos, &fret_servos, &bridge_servos);
    if(err != ESP_OK)
    {
        ESP_LOGE(MAIN_TAG, "Guitarbot initialization failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}


esp_err_t setupUI()
{
    Waveshare_Driver_Init();
    SD_Init();
    LCD_Init();
    LVGL_Init();   // returns the screen object
    // ui_init(servos, sizeof(servos) / sizeof(servos[0]));
    ui_init(servos); // create the screen

    lvgl_mutex = xSemaphoreCreateMutex(); // Ensures that only 1 task accesses the resources at a time

    return ESP_OK;
}


void setup_ui_task(void *arg)
{
    esp_err_t err = setupUI();
    if(err != ESP_OK) ESP_LOGE(MAIN_TAG, "UI setup failed: %s", esp_err_to_name(err));

    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 8192, NULL, 5, NULL, 1); // pin to core 1
    vTaskDelete(NULL);
}



void app_main(void)
{
    ESP_LOGI(MAIN_TAG, "Starting...\n");
    esp_err_t err;

    // Setup the servos
    err = setup();
    if(err != ESP_OK) ESP_LOGE(MAIN_TAG, "Setup failed: %s", esp_err_to_name(err));
    
    
    // err = setupUI();
    xTaskCreatePinnedToCore(setup_ui_task, "setup_ui_task", 8192, NULL, 5, NULL, 1); // setup the UI - pin to core 1
    // xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 8192, NULL, 5, NULL, 1); // create main UI task - pin to core 1

    

    // Create the task to play the song
    // xTaskCreate(&play_song_task, "play_song_task", 4096, NULL, 5, NULL);
    // xTaskCreatePinnedToCore(&play_song_task, "play_song_task", 4096, NULL, 6, NULL, 0);


    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));

        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        // lv_timer_handler();

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

