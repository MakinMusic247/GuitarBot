#include "pca9685.h"

static const char *PCA9685_TAG = "PCA9685";



/****************************************************************************************************/
/* 
*   I2C helper functions 
*   For more information look at datasheet: https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf
*   Particularly section 9. Bus Transactions
*/


/// @brief Write to a specific register (fig 20 in datasheet) 
/// START condition --> control register --> data for register
/// @param dev_handle   pca9685 device handler
/// @param reg     the target register to write to
/// @param value        the data to write to the register
/// @return 
static esp_err_t pca9685_write_reg(pca9685_dev_t *dev, uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value }; // Combine the register and data into 1 packet
    return i2c_master_transmit(dev->i2c_dev, buf, sizeof(buf), I2C_TIMEOUT_MS);
}


// write an array including register and buffer
static esp_err_t pca9685_write_reg_buf(pca9685_dev_t *dev, uint8_t reg, uint8_t *value, size_t len)
{
    if (!dev || !value) return ESP_ERR_INVALID_ARG;
    uint8_t buf[len + 1]; // allow memory for register + data (which are combined into single transaction)
    // Combine the register and data values into a single buffer
    buf[0] = reg;
    memcpy(&buf[1], value, len);
    return i2c_master_transmit(dev->i2c_dev, buf, sizeof(buf), I2C_TIMEOUT_MS);
}


/// @brief Read a register
/// @param dev_handle   pca9685 device handler
/// @param register     the target register to write to
/// @param out_value    the data read from the register
/// @return 
static esp_err_t pca9685_read_reg(pca9685_dev_t *dev, uint8_t reg, uint8_t *out_value)
{
    return i2c_master_transmit_receive(dev->i2c_dev, &reg, 1, out_value, 1, I2C_TIMEOUT_MS);
}



/// @brief calculate the prescale for the desired frequency
/// Prescale value = round( (osc_clock) / (4096 x update_rate) ) -1
/// @param 
/// @return 
static uint8_t calculate_prescale(pca9685_dev_t *dev)
{
    return (uint8_t)(roundf( PCA9685_INTERNAL_OSCILLATOR_FREQ / (4096 * dev->pwm_freq_hz) )) -1;
}




/// @brief Wake up the device
/// MODE1_SLEEP is 0x10 --> 0b00010000
/// @return 
esp_err_t pca9685_sleep(pca9685_dev_t *dev, bool sleep)
{
    esp_err_t err;

    if(!dev) return ESP_ERR_INVALID_ARG;

    if(sleep) ESP_LOGI(PCA9685_TAG, "Sleeping");
    else ESP_LOGI(PCA9685_TAG, "Waking up");

    // Get the current mode1 bits
    uint8_t mode1_current;
    err = pca9685_read_reg(dev, PCA9685_MODE1, &mode1_current);

    // toggle the sleep bit (bit 4) --> 1 = sleep, 0 = wake
    // set bit --> new = original | (1 << bit)
    // clear bit --> new = orginal & ~(1 << bit)
    uint8_t mode1_new = sleep
                        ? (mode1_current | (1 << MODE1_SLEEP_BIT))
                        : (mode1_current & ~(1 << MODE1_SLEEP_BIT));

    if(mode1_new == mode1_current) return ESP_OK; // already set
    
    err = pca9685_write_reg(dev, PCA9685_MODE1, mode1_new);
    return err;
}


// Enable the allcall mode
esp_err_t pca9685_set_allcall(pca9685_dev_t *dev, bool enable)
{
    esp_err_t err;

    if (!dev) return ESP_ERR_INVALID_ARG;

    if (enable) ESP_LOGI(PCA9685_TAG, "Enabling ALLCALL");
    else ESP_LOGI(PCA9685_TAG, "Disabling ALLCALL");

    uint8_t mode1_current;
    err = pca9685_read_reg(dev, PCA9685_MODE1, &mode1_current);
    if (err != ESP_OK) return err;

    uint8_t mode1_new = enable
                        ? (mode1_current | MODE1_ALLCAL)
                        : (mode1_current & ~MODE1_ALLCAL);

    if (mode1_new == mode1_current) return ESP_OK; // already in the requested state

    return pca9685_write_reg(dev, PCA9685_MODE1, mode1_new);
}



/// @brief Set the prescaler
/// sleep --> set prescaler
/// @return 
esp_err_t pca9685_set_prescaler(pca9685_dev_t *dev, uint8_t prescaler)
{
    esp_err_t err;
    ESP_LOGI(PCA9685_TAG, "Setting prescaler %u", prescaler);
    if(!dev) return ESP_ERR_INVALID_ARG;

    // Sleep (write sleep bit to 1 in mode 1 register)
    err = pca9685_sleep(dev, true);

    // Set prescaler
    err = pca9685_write_reg(dev, PCA9685_PRESCALE, prescaler);

    return err;
}



/// @brief Reset the device
/// @return 
esp_err_t pca9685_reset(pca9685_dev_t *dev)
{
    esp_err_t err;
    ESP_LOGI(PCA9685_TAG, "resetting");
    if(!dev) return ESP_ERR_INVALID_ARG;


    return err;
}



/****************************************************************************************************/
/* PCA9685 functions */

esp_err_t pca9685_init(i2c_master_bus_handle_t *bus, uint8_t addr, gpio_num_t sda_gpio, gpio_num_t scl_gpio, pca9685_dev_t *out_dev)
{
    esp_err_t err;

    ESP_LOGI(PCA9685_TAG, "initializing PCA9685 @ address %02X", addr);

    /* Ensure arguments are valid, not null */
    if(!bus || !sda_gpio || !scl_gpio){ // null args
        ESP_LOGE(PCA9685_TAG, "invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    // Initialize empty PCA9685 device struct
    if(!out_dev) out_dev = calloc(1, sizeof(pca9685_dev_t));
    if(!out_dev){
        ESP_LOGE(PCA9685_TAG, "Memory not allocated");
        return ESP_ERR_NO_MEM;
    }


    /* I2C initialization - add device to master bus */
    ESP_LOGI(PCA9685_TAG, "Add device to I2C master bus");

    // Set the i2c device configuration
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = PCA9685_I2C_SCL_FREQ,
    };

    i2c_master_dev_handle_t dev_handle;

    // Add the device to the I2C master bus
    err = i2c_master_bus_add_device(*bus, &dev_cfg, &dev_handle);

    // If adding device failed --> remove from bus and return
    if (err != ESP_OK) {
        ESP_LOGE(PCA9685_TAG, "PCA9685 initialization failed: %s", esp_err_to_name(err));
        pca9685_deinit(&dev_handle, out_dev);
        return err;
    } 

    // add setup to the PCA9685 device struct
    out_dev->i2c_dev = dev_handle;
    out_dev->pwm_freq_hz = 50.0f; // 50 Hz is default for servos


    /* Initialize the PCA9685 (sleep mode --> set prescaler --> wake --> set modes) */
    ESP_LOGI(PCA9685_TAG, "configuring device");

    // Set sleep mode
    // err = pca9685_sleep(out_dev, true);
    err = pca9685_write_reg(out_dev, PCA9685_MODE1, MODE1_SLEEP | MODE1_ALLCAL);
    if(err != ESP_OK){
        ESP_LOGE(PCA9685_TAG, "Setting sleep mode failed: %s", esp_err_to_name(err));
        pca9685_deinit(&dev_handle, out_dev);
        return err;
    }

    // Set the prescaler
    uint8_t prescaler = 121;
    err = pca9685_write_reg(out_dev, PCA9685_PRESCALE, prescaler);
    if(err != ESP_OK){
        ESP_LOGE(PCA9685_TAG, "Setting prescaler failed: %s", esp_err_to_name(err));
        pca9685_deinit(&dev_handle, out_dev);
        return err;
    }

    // Set the prescaler
    // uint8_t prescaler = calculate_prescale(out_dev); // calculate the prescaler using the datasheet equation
    // prescaler = 121; // TODO: hardcoded
    // err = pca9685_set_prescaler(out_dev, prescaler);
    // if(err != ESP_OK){
    //     ESP_LOGE(PCA9685_TAG, "Setting prescaler failed: %s", esp_err_to_name(err));
    //     pca9685_deinit(&dev_handle, out_dev);
    //     return err;
    // }

    // err = pca9685_write_reg(out_dev, PCA9685_MODE1, (1 << MODE1_SLEEP_BIT) | (1 << MODE1_ALLCAL));
    // uint8_t prescale = 121;
    // err = pca9685_write_reg(out_dev, PCA9685_PRESCALE, prescale);

    
    // Wake up the device
    pca9685_sleep(out_dev, false);

    // Get the current state of mode 1
    uint8_t mode1_current;
    err = pca9685_read_reg(out_dev, PCA9685_MODE1, &mode1_current);

    // Turn on auto-increment mode (MODE1_AI) - let's us set the PWM of the channels in a single I2C transaction
    err = pca9685_write_reg(out_dev, PCA9685_MODE1, mode1_current | MODE1_AI);
    if(err != ESP_OK){
        ESP_LOGE(PCA9685_TAG, "Setting Auto Increment failed: %s", esp_err_to_name(err));
        pca9685_deinit(&dev_handle, out_dev);
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(1)); // short delay (>= 500us delay required on wake up)

    // Set totem-poll mode (channel outputs can sink or source current)
    err = pca9685_write_reg(out_dev, PCA9685_MODE2, MODE2_OUTDRV);
    if(err != ESP_OK){
        ESP_LOGE(PCA9685_TAG, "Setting totem-poll outputs failed: %s", esp_err_to_name(err));
        pca9685_deinit(&dev_handle, out_dev);
        return err;
    }

    ESP_LOGI(PCA9685_TAG, "PCA9685 initialization successful");

    return ESP_OK;
}



esp_err_t pca9685_deinit(i2c_master_dev_handle_t *dev_handle, pca9685_dev_t *dev)
{
    esp_err_t err;
    ESP_LOGI(PCA9685_TAG, "removing PCA9685 device");

    if(!dev_handle || !dev) return ESP_ERR_INVALID_ARG;
    err = i2c_master_bus_rm_device(*dev_handle);
    free(dev); // remove device struct from memory
    return err;
}




/*
*   Read the datasheet section 7.3.3 LED output and PWM control:
*   The turn-on time of each LED driver output and the duty cycle of PWM can be controlled independently using the LEDn_ON and LEDn_OFF registers.
*   There are two 12-bit registers (LED0_ON_L + LED0_ON_H) and (LED0_OFF_L + LED0_OFF_H)
*   By default can set 'on' parameter to zero, since it sets the delay. The off will determine the amount of PWM ON and OFF
*/
esp_err_t pca9685_set_pwm(pca9685_dev_t *dev, uint8_t channel, uint16_t on, uint16_t off)
{
    ESP_LOGI(PCA9685_TAG, "Setting PWM channel %u \non=%u, off=%u", channel, on, off);
    esp_err_t err;

    if(!dev) return ESP_ERR_INVALID_ARG;

    // Create the 4-bit payload - we want to target the 4 LED on registers - LED0_ON_L, LED0_ON_H, LED0_OFF_L, LED0_OFF_H (addresses 0x06 --> 0x09)
    uint8_t base_register = PCA9685_LED0_ON_L + (4 * channel);
    // Values from table 7 of datasheet. 0xFF (0b11111111) mask to ensure that no reserved bits are written to 
    uint8_t data[4] = {
        (uint8_t)(on & 0xFF), // the low 8 bits LED0_ON_L
        (uint8_t)((on >> 8) & 0xFF), // the high 3 bits (bit shift by 8) LED0_ON_H
        (uint8_t)(off & 0xFF), // the low 8 bits LED0_OFF_L
        (uint8_t)((off >> 8) & 0xFF) // the high 3 bits (bit shift by 8) LED0_OFF_H
    };
    
    return pca9685_write_reg_buf(dev, base_register, data, sizeof(data)); // write the data to the LED registers
}


esp_err_t pca9685_set_pwm_test(pca9685_dev_t *dev, uint8_t channel, uint16_t on, uint16_t off)
{
    ESP_LOGI(PCA9685_TAG, "Setting PWM channel %u \non=%u, off=%u", channel, on, off);
    esp_err_t err;

    if(!dev) return ESP_ERR_INVALID_ARG;

    // Create the 4-bit payload - we want to target the 4 LED on registers - LED0_ON_L, LED0_ON_H, LED0_OFF_L, LED0_OFF_H (addresses 0x06 --> 0x09)
    uint8_t base_register = PCA9685_LED0_ON_L + (4 * channel);
    // Values from table 7 of datasheet. 0xFF (0b11111111) mask to ensure that no reserved bits are written to 
    uint8_t data[5] = {
        (uint8_t)base_register,
        (uint8_t)(on & 0xFF), // the low 8 bits LED0_ON_L
        (uint8_t)((on >> 8) & 0xFF), // the high 3 bits (bit shift by 8) LED0_ON_H
        (uint8_t)(off & 0xFF), // the low 8 bits LED0_OFF_L
        (uint8_t)((off >> 8) & 0xFF) // the high 3 bits (bit shift by 8) LED0_OFF_H
    };
    
    return i2c_master_transmit(dev->i2c_dev, data, sizeof(data), I2C_TIMEOUT_MS);
    // return pca9685_write_reg_buf(dev, base_register, data, sizeof(data)); // write the data to the LED registers
}


/// @brief Wrapper around pca9685_set_pwm, that converts a pulse in microseconds int the ON and OFF counters
/// @param dev 
/// @param channel 
/// @param pulse_us 
/// @return 
esp_err_t pca9685_set_pwm_pulse(pca9685_dev_t *dev, uint8_t channel, uint16_t pulse_us)
{
    ESP_LOGI(PCA9685_TAG, "Setting PWM channel %u \npulse=%u", channel, pulse_us);
    esp_err_t err;

    if(!dev) return ESP_ERR_INVALID_ARG;

    // Get the period of a pulse in microseconds (1u / x Hz = y microseconds)
    float period_us = 1000000.0f / dev->pwm_freq_hz;
    
    // Calculate the on and off time (in proportion to the 4096 counter of the PCA9685)
    // OFF = (pulse * n_steps) / period --> gives us number of OFF ticks
    uint16_t off = (uint16_t)roundf((pulse_us * 4096) / period_us); // round to nearest int
    // Ensure valid OFF time
    if(off > 4095) off = 4095;

    // Set the PWM
    return pca9685_set_pwm(dev, channel, 0, off);
}




esp_err_t pca9685_get_pwm(pca9685_dev_t *dev, uint16_t *on, uint16_t *off)
{
    return ESP_OK;
}
 