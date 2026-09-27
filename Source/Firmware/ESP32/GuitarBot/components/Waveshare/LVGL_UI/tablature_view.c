#include "tablature_view.h"
#include <lvgl.h>


/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ta_event_cb(lv_event_t * e);
void example1_increase_lvgl_tick(lv_timer_t * t);


static disp_size_t disp_size;

// static lv_obj_t * tv;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;


// static const lv_font_t * font_large;
// static const lv_font_t * font_normal;

static lv_timer_t * auto_step_timer;
// static lv_color_t original_screen_bg_color;

// static lv_timer_t * meter2_timer;

lv_obj_t * SD_Size;
lv_obj_t * FlashSize;
lv_obj_t * Board_angle;
lv_obj_t * Backlight_slider;



lv_obj_t * _lv_tablature_view_create(lv_obj_t * parent)
{
    /*Create a panel*/
    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_height(panel1, LV_SIZE_CONTENT);

    lv_obj_t * panel1_title = lv_label_create(panel1);
    lv_label_set_text(panel1_title, "Onboard parameter");
    lv_obj_add_style(panel1_title, &style_title, 0);

    lv_obj_t * SD_label = lv_label_create(panel1);
    lv_label_set_text(SD_label, "SD Card");
    lv_obj_add_style(SD_label, &style_text_muted, 0);

    SD_Size = lv_textarea_create(panel1);
    lv_textarea_set_one_line(SD_Size, true);
    lv_textarea_set_placeholder_text(SD_Size, "SD Size");
    lv_obj_add_event_cb(SD_Size, ta_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * Flash_label = lv_label_create(panel1);
    lv_label_set_text(Flash_label, "Flash Size");
    lv_obj_add_style(Flash_label, &style_text_muted, 0);

    FlashSize = lv_textarea_create(panel1);
    lv_textarea_set_one_line(FlashSize, true);
    lv_textarea_set_placeholder_text(FlashSize, "Flash Size");
    lv_obj_add_event_cb(FlashSize, ta_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * Backlight_label = lv_label_create(panel1);
    lv_label_set_text(Backlight_label, "Backlight brightness");
    lv_obj_add_style(Backlight_label, &style_text_muted, 0);

    Backlight_slider = lv_slider_create(panel1);                                 
    lv_obj_add_flag(Backlight_slider, LV_OBJ_FLAG_CLICKABLE);    
    lv_obj_set_size(Backlight_slider, 200, 35);              
    lv_obj_set_style_radius(Backlight_slider, 3, LV_PART_KNOB);               // Adjust the value for more or less rounding                                            
    lv_obj_set_style_bg_opa(Backlight_slider, LV_OPA_TRANSP, LV_PART_KNOB);                               
    // lv_obj_set_style_pad_all(Backlight_slider, 0, LV_PART_KNOB);                                            
    lv_obj_set_style_bg_color(Backlight_slider, lv_color_hex(0xAAAAAA), LV_PART_KNOB);               
    lv_obj_set_style_bg_color(Backlight_slider, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR);             
    lv_obj_set_style_outline_width(Backlight_slider, 2, LV_PART_INDICATOR);  
    lv_obj_set_style_outline_color(Backlight_slider, lv_color_hex(0xD3D3D3), LV_PART_INDICATOR);      
    lv_slider_set_range(Backlight_slider, 5, Backlight_MAX);              
    lv_slider_set_value(Backlight_slider, LCD_Backlight, LV_ANIM_ON);  
    lv_obj_add_event_cb(Backlight_slider, Backlight_adjustment_event_cb, LV_EVENT_VALUE_CHANGED, NULL);


    static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);


    /*Create the top panel*/
    static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_1_row_dsc[] = {
    LV_GRID_CONTENT,  /*Title*/
    5,                /*Separator*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_CONTENT,  /*Box title*/
    40,               /*Box*/
    LV_GRID_TEMPLATE_LAST               
    };

    lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);
    lv_obj_set_grid_cell(panel1_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(SD_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(SD_Size, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(Flash_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 4, 1);
    lv_obj_set_grid_cell(FlashSize, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(Backlight_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 14, 1);
    lv_obj_set_grid_cell(Backlight_slider, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 15, 1);

    // auto_step_timer = lv_timer_create(example1_increase_lvgl_tick, 100, NULL);

    return panel1;
}


void example1_increase_lvgl_tick(lv_timer_t * t)
{
  char buf[100]; 
  
  snprintf(buf, sizeof(buf), "%ld MB\r\n", SDCard_Size);
  lv_textarea_set_placeholder_text(SD_Size, buf);
  snprintf(buf, sizeof(buf), "%ld MB\r\n", Flash_Size);
  lv_textarea_set_placeholder_text(FlashSize, buf);
//   snprintf(buf, sizeof(buf), "%.2f V\r\n", BAT_analogVolts);
//   lv_textarea_set_placeholder_text(BAT_Volts, buf);
//   snprintf(buf, sizeof(buf), "X:%.2f  Y:%.2f  Z:%.2f\r\n", Accel.x, Accel.y, Accel.z);
//   lv_textarea_set_placeholder_text(Board_angle, buf);
//   snprintf(buf, sizeof(buf), "%d.%d.%d   %d:%d:%d\r\n",datetime.year,datetime.month,datetime.day,datetime.hour,datetime.minute,datetime.second);
//   lv_textarea_set_placeholder_text(RTC_Time, buf);
//   if(Scan_finish)
//     // snprintf(buf, sizeof(buf), "WIFI: %d    BLE: %d    ..Scan Finish.\r\n",WIFI_NUM,BLE_NUM);
//     snprintf(buf, sizeof(buf), "WIFI: %d     ..Scan Finish.\r\n",WIFI_NUM);
//   else
//     snprintf(buf, sizeof(buf), "WIFI: %d  \r\n",WIFI_NUM);
//     // snprintf(buf, sizeof(buf), "WIFI: %d    BLE: %d\r\n",WIFI_NUM,BLE_NUM);
//   lv_textarea_set_placeholder_text(Wireless_Scan, buf);
  lv_slider_set_value(Backlight_slider, LCD_Backlight, LV_ANIM_ON); 
  LVGL_Backlight_adjustment(LCD_Backlight);
}


void Backlight_adjustment_event_cb(lv_event_t * e) {
  uint8_t Backlight = lv_slider_get_value(lv_event_get_target(e));  
  if (Backlight <= Backlight_MAX)  {
    lv_slider_set_value(Backlight_slider, Backlight, LV_ANIM_ON); 
    LCD_Backlight = Backlight;
    LVGL_Backlight_adjustment(Backlight);
  }
  else
    printf("Volume out of range: %d\n", Backlight);

}


static void ta_event_cb(lv_event_t * e)
{
}

void LVGL_Backlight_adjustment(uint8_t Backlight) {
  Set_Backlight(Backlight);                                 
}