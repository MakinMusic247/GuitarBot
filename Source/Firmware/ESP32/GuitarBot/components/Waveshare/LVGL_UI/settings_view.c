#include "settings_view.h"
#include <lvgl.h>

// Styles
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

// Struct that updates and tracks the servo position and displays on UI
typedef struct {
    servo_t *servo;
    lv_obj_t *angle_label;
    lv_obj_t *meter;
    lv_meter_indicator_t *indicator;
    uint8_t servo_channel;
} servo_settings_t;


static void arc_value_changed_event_cb(lv_event_t * e);
static void lv_servo_arc(lv_obj_t * parent);
static void create_servo_view(lv_obj_t * parent, servo_t *servo, uint8_t servo_num);

static const char * btnm_map[] = { "-", "+", "\n", "reset", "" };

static servo_t *servos_arr = NULL; // holds the servos 
static int servo_count = 0;
static servo_settings_t *servo_settings = NULL; // holds the servo UI settings







/***************************/
// Static functions


static void set_indicator_value(void * indic, int32_t v, lv_obj_t * meter)
{
    lv_meter_set_indicator_value(meter, indic, v);
}


static void update_servo(servo_settings_t *settings, float new_angle)
{
    ESP_LOGI("Settings UI", "Setting servo angle");
    servo_set_angle(settings->servo, new_angle);
    set_indicator_value(settings->indicator, (int32_t)new_angle, settings->meter);
    lv_label_set_text_fmt(settings->angle_label, "%f deg", new_angle);
}


static void btn_matrix_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    servo_settings_t *settings = (servo_settings_t *)lv_event_get_user_data(e); // Get the specific servo
    if(!settings){
        ESP_LOGE("Settings UI", "Error - servo settings not found");
    }

    // Get the current servo angle
    float current_angle = settings->servo->current_angle;
    float new_angle = 90.0;

    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);

        // LV_LOG_USER("%s was pressed\n", txt);
        ESP_LOGI("Settings UI", "%s was pressed (id %u)\n", txt, id);
        if(id == 0){
            ESP_LOGI("Settings UI", "decrease angle", txt);
            new_angle = current_angle - 5;
        }
        else if(id == 1){
            ESP_LOGI("Settings UI", "increase angle", txt);
            new_angle = current_angle + 5;
        }
        else if(id == 2){
            ESP_LOGI("Settings UI", "reset to default", txt);
            new_angle = 90.0;
        }
        else{
            ESP_LOGI("Settings UI", "no action", txt);
            return;
        }

        ESP_LOGI("Settings UI", "Current angle = %f, New angle = %f", current_angle, new_angle);

        // Update
        update_servo(settings, new_angle);

    }
}


/***************************/


lv_obj_t *_lv_settings_view_create(lv_obj_t * parent, servo_t servos[], int n_servos)
{
    /*Create a panel*/
    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_height(panel1, lv_pct(100)); //LV_SIZE_CONTENT);
    lv_obj_set_width(panel1, lv_pct(100));

    // Set a flex flow to add new items horizontally
    lv_obj_set_flex_flow(panel1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_row(panel1, 4, 0);

    servos_arr = servos;
    servo_count = n_servos;

    if(n_servos > 0){
        servo_settings = calloc(n_servos, sizeof(servo_settings_t)); // initialize the array with empty values

        // Create the content
        for(int i=0; i<n_servos; i++){
            create_servo_view(panel1, &servos[i], i);
        }
    }

    return panel1;
}


void create_servo_view(lv_obj_t * parent, servo_t *servo, uint8_t servo_num)
{
    // initialize a new servo_settings instance
    if(servo_settings == NULL || servo_num >= servo_count || servo == NULL){
        return;
    }

    // servo_settings[servo_num].servo_channel = servo_num;
    servo_settings_t *settings = &servo_settings[servo_num]; // Get the pointer to the current servo being set up
    settings->servo = servo;
    settings->servo_channel = servo_num;

    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_width(card, LV_SIZE_CONTENT);
    lv_obj_set_height(card, lv_pct(100)); //LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, 6, 0);
    lv_obj_set_style_pad_row(card, 2, 0);

    lv_obj_t *name_label = lv_label_create(card);
    lv_label_set_text_fmt(name_label, "Servo %u", servo_num);
    // lv_label_set_text(name_label, "Servo");
    lv_obj_set_size(name_label, 80, 10);


    /* Angle indicator */
    lv_obj_t * meter = lv_meter_create(card);
    lv_obj_set_size(meter, 80, 80);
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale, 0, 180, 180, 180);
    lv_meter_set_scale_ticks(meter, scale, 9, 2, 8, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 4, 3, 12, lv_color_black(), 10);

    lv_meter_indicator_t * indic;
    // lv_meter_set_indicator_start_value(meter, indic, 0);
    // lv_meter_set_indicator_end_value(meter, indic, 180);
    indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
    lv_meter_set_indicator_value(meter, indic, 90);

    // label for tracking angle value
    settings->angle_label = lv_label_create(card);
    lv_label_set_text_fmt(settings->angle_label, "%d deg", 90);

    settings->indicator = indic;
    settings->meter = meter;


    /* angle control buttons */
    lv_obj_t * btnm1 = lv_btnmatrix_create(card);
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_obj_set_size(btnm1, 80, 80);
    lv_btnmatrix_set_btn_width(btnm1, 0, 1);
    lv_btnmatrix_set_btn_width(btnm1, 1, 1);
    lv_btnmatrix_set_btn_width(btnm1, 2, 2);
    lv_obj_set_style_pad_row(btnm1, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_column(btnm1, 6, LV_PART_MAIN);
    lv_obj_add_event_cb(btnm1, btn_matrix_event_handler, LV_EVENT_ALL, settings); // Add the settings to the *user_data param - lets us track which servo is updated

}


static void arc_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);

    lv_label_set_text_fmt(label, "%d%%", lv_arc_get_value(arc));

    /*Rotate the label to the current position of the arc*/
    lv_arc_rotate_obj_to_angle(arc, label, 25);
}



/* Angle indicator */
// lv_obj_t * label = lv_label_create(card);
// lv_obj_t * arc = lv_arc_create(card);
// lv_obj_set_size(arc, 80, 80);
// lv_arc_set_rotation(arc, 180);
// lv_arc_set_bg_angles(arc, 0, 180);
// lv_arc_set_range(arc, 0, 180);
// lv_arc_set_value(arc, 90);
// // lv_obj_center(arc);
// lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); // make readonly
// lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
// lv_obj_add_event_cb(arc, arc_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);
// lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL); // Manually update the label for the first time