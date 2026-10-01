#ifndef __SETTINGS_PAGE_H__
#define __SETTINGS_PAGE_H__


#include <string.h>
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "examples/lv_examples.h"
#include "LVGL_Driver.h"

#include "Guitarbot.h"
#include "Servo.h"



/// @brief Create a settings view to test and control servos
/// @param parent 
/// @return 
lv_obj_t *_lv_settings_page_create(lv_obj_t * parent, servo_t servos[]);


#endif