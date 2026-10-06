#include "fretboard_view.h"
#include <lvgl.h>

static const char *FB_TAG = "Fretboard";

static void draw_part_event_cb(lv_event_t * e);
lv_obj_t * create_grid(lv_obj_t * parent, uint8_t rows, uint8_t cols);
static void draw_line(lv_obj_t *parent);
static void draw_string_line(lv_obj_t *parent, uint8_t rows, uint8_t cols);
static void draw_bridge_hole(lv_obj_t *parent);
static void fret_event_handler(lv_event_t * e);
static void bridge_event_handler(lv_event_t * e);

static note_t *grid_frets = NULL; // array holding the notes for each grid cell
static note_t *grid_strings = NULL;


/// @brief Guitar fretboard view. Each table cell represents a servo that gets played when tapped
/// columns = strings = 6
/// rows = frets = N_FRET_SERVOS
/// @param parent 
/// @param servos 
/// @return 
lv_obj_t *_lv_fretboard_view_create(lv_obj_t * parent, servo_t servos[])
{
    // Create the grid
    // number of rows = N_FRET_SERVOS / 3 + 2 (3 servos per fret, + 1 for gap + 1 extra for open strings) 
    // number of cols = 6 (6 string guitar)
    lv_obj_t * grid;
    uint8_t cols = 6;
    uint8_t rows = N_FRET_SERVOS / 3;
    grid_frets = calloc(rows * cols, sizeof(note_t));
    grid_strings = calloc(cols, sizeof(note_t));

    ESP_LOGI(FB_TAG, "creating fretboard view (rows = %u, cols = %u)", rows, cols);

    if(N_SERVOS > 0){

        grid = create_grid(parent, rows, cols);
        // draw_bridge_hole(grid);
    }

    // return table;
    return grid;

}


lv_obj_t * create_grid(lv_obj_t * parent, uint8_t rows, uint8_t cols)
{
    
    // static lv_coord_t col_dsc[] = {150, 150, 150, 150, 150, 150, LV_GRID_TEMPLATE_LAST};
    // static lv_coord_t row_dsc[] = {150, 150, 150, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    // 2. Create the main Grid Container
    lv_obj_t * grid = lv_obj_create(parent);
    lv_obj_set_size(grid, lv_pct(100), lv_pct(100)); //340, 190);
    // lv_obj_center(grid);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);

    // remove padding
    lv_obj_set_style_pad_column(parent, 0, 0);
    lv_obj_set_style_pad_left(parent, 0, 0);
    lv_obj_set_style_pad_right(parent, 0, 0);

    // style the grid
    lv_obj_set_style_bg_color(grid, lv_palette_main(LV_PALETTE_AMBER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(grid, LV_OPA_COVER, LV_PART_MAIN);

    // cell styles - add fret markings
    static lv_style_t style_cell;
    lv_style_init(&style_cell);
    lv_style_set_border_width(&style_cell, 2);
    lv_style_set_border_color(&style_cell, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_border_side(&style_cell, LV_BORDER_SIDE_BOTTOM);

    lv_obj_t * label;
    lv_obj_t * obj;

    // Main fret view
    for(uint8_t row = 0; row < rows; row++){
        for(uint8_t col = 0; col < cols; col++){
            // Add active servos to the grid
            obj = lv_btn_create(grid); // create a button
            lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1); // fill whole cell
            label = lv_label_create(obj);
            lv_label_set_text_fmt(label, "c%d\nr%d", col, row);
            lv_obj_center(label);
            lv_obj_add_style(obj, &style_cell, 0);

            // create the button click event
            note_t *note = &grid_frets[row * cols + col];
            note->fret = row+1;
            note->string = col;
            lv_obj_add_event_cb(obj, fret_event_handler, LV_EVENT_ALL, note);
        }
    }

    // Add a row for open strings
    uint8_t open_string_row_idx = rows + 1; // start index - skip a row to create gap between frets and open strings
    for(int col = 0; col < 6; col++){
        obj = lv_btn_create(grid); // create a button
        lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, open_string_row_idx, 1); // fill whole cell
        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%s", STRING_NAMES[col]);
        lv_obj_center(label);

        // create the button click event
        note_t *note = &grid_strings[col];
        note->fret = 0;
        note->string = col;
        lv_obj_add_event_cb(obj, bridge_event_handler, LV_EVENT_ALL, note);
    }


    draw_string_line(grid, rows + 2, cols);

    return grid;
}



/// @brief Fret button pressed
/// @param e 
static void fret_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code != LV_EVENT_CLICKED){
        return;
    }

    ESP_LOGI(FB_TAG, "fret pressed");

    lv_obj_t * obj = lv_event_get_target(e);
    note_t *note = (note_t *)lv_event_get_user_data(e); // Get the specific servo
    if(!note){
        ESP_LOGE(FB_TAG, "Error - note not found");
    }

    ESP_LOGI(FB_TAG, "fret pressed: fret %u string %u", note->fret, note->string);

    // play the note
    if(note->fret > 24 ||  note->string >= 6){
        ESP_LOGI(FB_TAG, "invalid: fret %u string %u", note->fret, note->string);
        return;
    }

    play_note(note->string, note->fret); // TODO: use FreeRTOS task to complete - delays are blocking the UI
}


static void bridge_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code != LV_EVENT_CLICKED){
        return;
    }

    ESP_LOGI(FB_TAG, "string pressed");

    lv_obj_t * obj = lv_event_get_target(e);
    note_t *note = (note_t *)lv_event_get_user_data(e); // Get the specific servo
    if(!note){
        ESP_LOGE(FB_TAG, "Error - note not found");
    }

    ESP_LOGI(FB_TAG, "fret pressed: fret %u string %u", note->fret, note->string);

    // play the note
    if(note->fret > 24 ||  note->string >= 6){
        ESP_LOGI(FB_TAG, "invalid: fret %u string %u", note->fret, note->string);
        return;
    }

    play_note(note->string, note->fret); // TODO: use FreeRTOS task to complete - delays are blocking the UI
}



static void draw_string_line(lv_obj_t *parent, uint8_t rows, uint8_t cols)
{
    static lv_point_t line_points[] = {{0, 0}, {0, 240}}; // Height can be temporary; STRETCH handles sizing

    // Style the line color/width if desired
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_line_width(&style_line, 3);
        

    for(int col = 0; col < cols; col++){
        lv_obj_t * line = lv_line_create(parent);
        lv_line_set_points(line, line_points, 2);
        lv_obj_add_style(line, &style_line, 0);

        // Place the string line across all rows
        lv_obj_set_grid_cell(line, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_STRETCH, 0, rows);
    }
}



static void draw_bridge_hole(lv_obj_t *parent)
{
    int CANVAS_WIDTH = 240;
    int CANVAS_HEIGHT = 320;
    lv_color_t canvas_buffer[CANVAS_WIDTH * CANVAS_HEIGHT];

    lv_obj_t * canvas = lv_canvas_create(parent);
    lv_canvas_set_buffer(canvas, canvas_buffer, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    
    // Clear canvas background to transparent/screen color
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);//lv_theme_get_color_bg(parent)

    // 3. Initialize the drawing descriptor
    lv_draw_arc_dsc_t  draw_dsc;
    lv_draw_arc_dsc_init(&draw_dsc);
    draw_dsc.color = lv_palette_main(LV_PALETTE_BROWN);
    draw_dsc.width = 5;

    uint16_t radius = CANVAS_WIDTH / 2;
    int16_t center_x = CANVAS_WIDTH / 2;
    int16_t center_y = CANVAS_HEIGHT; // Put center at the absolute bottom edge of canvas

    lv_canvas_draw_arc(canvas, center_x, center_y, radius, 180, 360, &draw_dsc);

    lv_obj_align(canvas, LV_ALIGN_BOTTOM_MID, 0, 0);
}



// static void draw_bridge_hole(lv_obj_t *parent)
// {
//     lv_obj_t * bottom_arc = lv_arc_create(parent);

//     /* Remove the interactive knob/handle so it acts as a static shape */
//     lv_obj_remove_style(bottom_arc, NULL, LV_PART_KNOB);
//     lv_obj_clear_flag(bottom_arc, LV_OBJ_FLAG_CLICKABLE);

//     /* Size the arc to match the bottom curve requirement */
//     lv_obj_set_size(bottom_arc, 200, 200); 

//     /* Set angles: 180 to 360 degrees creates a bottom semi-circle opening upwards */
//     lv_arc_set_angles(bottom_arc, 180, 360);

//     /* Draw the full arc background instead of a partial foreground value indicator */
//     lv_arc_set_bg_angles(bottom_arc, 180, 360);

//     /* Align it perfectly to the bottom center of the screen */
//     lv_obj_align(bottom_arc, LV_ALIGN_BOTTOM_MID, 0, 100); 

//     /* Optional: Style the arc's color and thickness */
//     lv_obj_set_style_arc_width(bottom_arc, 10, LV_PART_MAIN);
//     lv_obj_set_style_arc_color(bottom_arc, lv_palette_main(LV_PALETTE_BROWN), LV_PART_MAIN);
// }



static void draw_line(lv_obj_t *parent)
{
    lv_obj_t * line = lv_line_create(parent);

    /* 3. Define the line points (e.g., a 40-pixel vertical line) */
    static lv_point_t line_points[] = { {0, 0}, {0, 60} };
    lv_line_set_points(line, line_points, 2);

    /* 4. Center the line inside the container */
    lv_obj_center(line); /* This places the line exactly in the middle of the cell */

    /* Optional: Style the line */
    lv_obj_set_style_line_width(line, 3, 0);
    lv_obj_set_style_line_color(line, lv_color_hex(0xFF0000), 0);

}



static void draw_part_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    /*If the cells are drawn...*/
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id /  lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        // /*Make the texts in the first cell center aligned*/
        // if(row == 0) {
        //     dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
        //     dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
        //     dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        // }
        // /*In the first column align the texts to the right*/
        // else if(col == 0) {
        //     dsc->label_dsc->align = LV_TEXT_ALIGN_RIGHT;
        // }

        /*MAke every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}
