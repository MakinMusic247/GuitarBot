#include "UI_main.h"
#include <demos/lv_demos.h>
#include <demos/music/lv_demo_music.h>
#include <lvgl.h>
#include "tablature_view.h"
#include "settings_view.h"




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
static void Tabviewer_create(lv_obj_t * parent); // Guitar tablature viewer page
static void Settings_create(lv_obj_t * parent, servo_t servos[], int n_servos);  // Settings page

// static void ta_event_cb(lv_event_t * e);
// void example1_increase_lvgl_tick(lv_timer_t * t);
/**********************
 *  STATIC VARIABLES
 **********************/
static disp_size_t disp_size;

static lv_obj_t * tv;
lv_style_t style_text_muted;
lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;


static const lv_font_t * font_large;
static const lv_font_t * font_normal;

// static lv_timer_t * auto_step_timer;
static lv_color_t original_screen_bg_color;

// static lv_timer_t * meter2_timer;

// lv_obj_t * SD_Size;
// lv_obj_t * FlashSize;
// lv_obj_t * Board_angle;
// lv_obj_t * Backlight_slider;



void ui_init(servo_t servos[], int n_servos){

  disp_size = DISP_SMALL;                            

  font_large = LV_FONT_DEFAULT;                             
  font_normal = LV_FONT_DEFAULT;                         
  
  lv_coord_t tab_h;
  tab_h = 45;
  #if LV_FONT_MONTSERRAT_18
    font_large     = &lv_font_montserrat_18;
  #else
    LV_LOG_WARN("LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
  #endif
  #if LV_FONT_MONTSERRAT_12
    font_normal    = &lv_font_montserrat_12;
  #else
    LV_LOG_WARN("LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
  #endif
  
  lv_style_init(&style_text_muted);
  lv_style_set_text_opa(&style_text_muted, LV_OPA_90);

  lv_style_init(&style_title);
  lv_style_set_text_font(&style_title, font_large);

  lv_style_init(&style_icon);
  lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
  lv_style_set_text_font(&style_icon, font_large);

  lv_style_init(&style_bullet);
  lv_style_set_border_width(&style_bullet, 0);
  lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

  tv = lv_tabview_create(lv_scr_act(), LV_DIR_BOTTOM, tab_h);

  lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

  lv_obj_t * t1 = lv_tabview_add_tab(tv, "My Tabs");
  lv_obj_t * t2 = lv_tabview_add_tab(tv, "Settings");

  Tabviewer_create(t1);
  Settings_create(t2, servos, n_servos);
  
}


/// @brief Create the guitar tab view page
/// @param parent 
static void Tabviewer_create(lv_obj_t * parent)
{
    original_screen_bg_color = lv_obj_get_style_bg_color(parent, 0);
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x343247), 0);
    
    // _lv_tablature_view_create(parent);
  
}


/// @brief Create the settings page
/// @param parent 
static void Settings_create(lv_obj_t * parent, servo_t servos[], int n_servos)
{
  original_screen_bg_color = lv_obj_get_style_bg_color(parent, 0);
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x343247), 0);

  _lv_settings_view_create(parent, servos, n_servos);
}




// void Backlight_adjustment_event_cb(lv_event_t * e) {
//   uint8_t Backlight = lv_slider_get_value(lv_event_get_target(e));  
//   if (Backlight <= Backlight_MAX)  {
//     lv_slider_set_value(Backlight_slider, Backlight, LV_ANIM_ON); 
//     LCD_Backlight = Backlight;
//     LVGL_Backlight_adjustment(Backlight);
//   }
//   else
//     printf("Volume out of range: %d\n", Backlight);

// }


// static void ta_event_cb(lv_event_t * e)
// {
// }

// void LVGL_Backlight_adjustment(uint8_t Backlight) {
//   Set_Backlight(Backlight);                                 
// }