/*
*   Driver for PCA9685 16 channel PWM Driver
*   Handle the I2C setup and transactions for each PCA9685 device
*   Datasheet: https://www.nxp.com/products/power-drivers/lighting-driver-and-controller-ics/led-drivers/16-channel-12-bit-pwm-fm-plus-ic-bus-led-driver:PCA9685
*   More information about the esp idf I2C library can be found here: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html
*   "The I2C master bus is designed on a bus-device model. i2c_master_bus_config_t and i2c_device_config_t are required separately to allocate the I2C master bus instance and I2C device instance"
*/

#ifndef __PCA9685_H__
#define __PCA9685_H__

#include "esp_log.h"
#include "driver/i2c_master.h"
#include <esp_err.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/*
*   Register Addresses
*   datasheet section 7.3 - Register Definitions
*/  
#define PCA9685_MODE1 0x00          //  Mode register 1
#define PCA9685_MODE2 0x01          //  Mode register 2

#define PCA9685_SUBADR1 0x02        // I2C-bus subaddress 1
#define PCA9685_SUBADR2 0x03        // I2C-bus subaddress 2
#define PCA9685_SUBADR3 0x04        // I2C-bus subaddress 3
#define PCA9685_ALLCALLADR 0x05     // LED All Call I2C-bus address 
#define PCA9685_LED0_ON_L 0x06      // LED0 on tick, low byte
#define PCA9685_LED0_ON_H 0x07      // LED0 on tick, high byte
#define PCA9685_LED0_OFF_L 0x08     // LED0 off tick, low byte
#define PCA9685_LED0_OFF_H 0x09     // LED0 off tick, high byte 
#define PCA9685_ALLLED_ON_L 0xFA    // load all the LEDn_ON registers, low
#define PCA9685_ALLLED_ON_H 0xFB    // load all the LEDn_ON registers, high 
#define PCA9685_ALLLED_OFF_L 0xFC   // load all the LEDn_OFF registers, low
#define PCA9685_ALLLED_OFF_H 0xFD   // load all the LEDn_OFF registers,high
#define PCA9685_PRESCALE 0xFE       // Prescaler for PWM output frequency
#define PCA9685_TESTMODE 0xFF       // defines the test mode to be entered

#define MIN_PRESCALE 0x03
#define MAX_PRESCALE 0xff

/*
*   MODE 1
    note: MODE1 is the main configuration register
*/
#define MODE1_ALLCAL 0x01           // respond to LED All Call I2C-bus address 
#define MODE1_SUB3 0x02             // respond to I2C-bus subaddress 3 
#define MODE1_SUB2 0x04             // respond to I2C-bus subaddress 2 
#define MODE1_SUB1 0x08             // respond to I2C-bus subaddress 1 
#define MODE1_SLEEP 0x10            // Low power mode. Oscillator off 
#define MODE1_SLEEP_BIT 4           // 
#define MODE1_AI 0x20               // Auto-Increment enabled 
#define MODE1_EXTCLK 0x40           // Use EXTCLK pin clock 
#define MODE1_RESTART 0x80          // Restart enabled 

// MODE2 bits
#define MODE2_OUTNE_0 0x01          // Active LOW output enable input 
#define MODE2_OUTNE_1 0x02          // Active LOW output enable input - high impedience 
#define MODE2_OUTDRV 0x04           // totem pole structure vs open-drain 
#define MODE2_OCH 0x08              // Outputs change on ACK vs STOP 
#define MODE2_INVRT 0x10            // Output logic state inverted 

#define PCA9685_I2C_ADDRESS_DEFAULT 0x40            // Default PCA9685 I2C Slave Address 
#define PCA9685_I2C_SCL_FREQ 400000                 // Standard mode = 100KHz, Fast-mode = 400KHz, Fast-mode+ = 1000KHz
#define PCA9685_INTERNAL_OSCILLATOR_FREQ 25000000   // internal oscillator of PCA9685
#define I2C_TIMEOUT_MS 1000                         // Set a default timeout for any I2C transaction


// Struct to cleanly handle multiple servo drivers
typedef struct {
    i2c_master_dev_handle_t i2c_dev;    // ESP I2C master device handler
    float pwm_freq_hz;                  // PCA9685 frequency - default to 50Hz for SG90 servo
} pca9685_dev_t;



/// @brief Initialize the pca9685 device (to be added to the master bus)
/// @param pca9685_dev  PCA9685 device struct
/// @param bus          I2C shared bus
/// @param addr         I2C address (default = 0x40)
/// @param sda_gpio     ESP32 GPIO pin for SDA
/// @param scl_gpio     ESP32 GPIO pin for SCL
/// @param out_dev      device class created by the initialisation flow
/// @return 
esp_err_t pca9685_init(i2c_master_bus_handle_t *bus, uint8_t addr, gpio_num_t sda_gpio, gpio_num_t scl_gpio, pca9685_dev_t *out_dev);


/// @brief de-initialize the PCA9685 device (remove from I2C bus)
/// @param pca9685_dev 
/// @return 
esp_err_t pca9685_deinit(i2c_master_dev_handle_t *dev_handle, pca9685_dev_t *pca9685_dev);


/// @brief set the pwm (on-off) of a given channel
/// Datasheet 7.3.3 LED output and PWM control
/// There are two registers per channel - LEDn_ON and LEDn_OFF (on and off time)
/// A 12-bit counter is continuously run from 0-4095 (000h-0FFFh)
/// The on and off times are compared with the counter to set the on/off output of the PWM
/// @param dev 
/// @param channel 
/// @param on at what point of the 4096-part cycle the PWM output ON (default to 0 if no delay)
/// @param off at what point of the 4096-part cycle the PWM output OFF
/// @return 
esp_err_t pca9685_set_pwm(pca9685_dev_t *dev, uint8_t channel, uint16_t on, uint16_t off);

esp_err_t pca9685_set_pwm_test(pca9685_dev_t *dev, uint8_t channel, uint16_t on, uint16_t off);


/// @brief Set the pwm of a given channel given a pulse
/// @param dev 
/// @param channel 
/// @param pulse_us 
/// @return 
esp_err_t pca9685_set_pwm_pulse(pca9685_dev_t *dev, uint8_t channel, uint16_t pulse_us);


/// @brief get the pwm frequency of a given channel
/// @param dev 
/// @param on 
/// @return 
esp_err_t pca9685_get_pwm(pca9685_dev_t *dev, uint16_t *on, uint16_t *off);


#endif