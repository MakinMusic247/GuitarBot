#ifndef __TABLATURE_VIEW_H__
#define __TABLATURE_VIEW_H__


#include "lvgl.h"
#include "demos/lv_demos.h"
#include "examples/lv_examples.h"

#include "LVGL_Driver.h"
#include "SD_MMC.h"


#define EXAMPLE1_LVGL_TICK_PERIOD_MS  1000


/// @brief Create a file system to display guitar tabs from SD card
/// @param parent 
/// @return 
lv_obj_t *_lv_tablature_view_create(lv_obj_t * parent);


void Backlight_adjustment_event_cb(lv_event_t * e);
void LVGL_Backlight_adjustment(uint8_t Backlight);



#endif