#ifndef __SETTINGS_VIEW_H__
#define __SETTINGS_VIEW_H__


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
lv_obj_t *_lv_settings_view_create(lv_obj_t * parent, servo_t servos[], int n_servos);


#endif