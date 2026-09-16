idf_component_register(
    SRCS "PCA9685.c" "Servo.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2c)
