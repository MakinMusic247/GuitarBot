#include "lvgl.h"
#include "demos/lv_demos.h"

#include "LVGL_Driver.h"
#include "SD_MMC.h"
#include "Servo.h"



#define EXAMPLE1_LVGL_TICK_PERIOD_MS  1000


// void Backlight_adjustment_event_cb(lv_event_t * e);

void ui_init(servo_t servos[]);
// void LVGL_Backlight_adjustment(uint8_t Backlight);