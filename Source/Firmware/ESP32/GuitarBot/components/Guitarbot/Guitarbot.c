#include <stdio.h>
#include "Guitarbot.h"


static const char *GB_TAG = "GB";

static servo_t *servos_arr = NULL;
static guitar_fret_servo_t *fret_servos = NULL;
static guitar_bridge_servo_t *bridge_servos = NULL;

guitar_tab_t *tab; // the current guitar tab

char str_val[64];
int int_val = 0;

// property names for reading tab json
const char *info_prop = "info";
const char *info_title_prop = "title";
const char *info_track_name_prop = "track_name";
const char *info_tempo_prop = "tempo";
const char *tab_prop = "tab";
const char *tab_measures_prop = "measures";
const char *tab_beats_prop = "beats";
const char *tab_beat_number_prop = "number";
const char *fret_prop = "fret";
const char *string_prop = "string";


bool isTabPlaying = false;
bool setupComplete = false;


bool tab_playing()
{
    return isTabPlaying;
}

bool setup_complete()
{
    return setupComplete;
}


/// @brief Search for a servo given the string and fret
///        Servos are grouped into groups of 3 per fret (1st fret: servos 0,1,2, 2nd fret: servos 3,4,5 ...)
///        Use integer arithmetic with the fret to find the servo array index. Then use the string to find the servo horn direction (top or bottom)
/// @param string 
/// @param fret 
/// @return 
static esp_err_t lookup_servo(uint8_t string, uint8_t fret, guitar_fret_servo_t *out_fret_servo, guitar_bridge_servo_t *out_bridge_servo)
{
    memset(out_fret_servo, 0, sizeof(*out_fret_servo));
    memset(out_bridge_servo, 0, sizeof(*out_bridge_servo));

    if (string >= 6) {
        ESP_LOGE(GB_TAG, "Invalid string %u", string);
        return ESP_ERR_NOT_FOUND;
    }

    /* Servos are grouped into group of 3 per fret */ 
    // To get the first servo of each fret:
    // E.g. fret 1 --> fret idx = (1-1)*3 = servos[0] 
    // E.g. fret 2 --> fret idx = (2-1)*3 = servos[3]
    // To get the correct string:
    // E.g. fret 1, string[2] (D) --> servo idx = fret_idx + string/2 = 0 + 2/2 = servos[1]
    // E.g. fret 2, string[5] (e) --> servo idx = 3 + 5/2 = servos[6]
    if(fret > 0){
        int fret_idx = (fret - 1) * 3 + string/2; // Get the array index from the fret ( fret = idx / 3 + 1 --> (idx + 1) * 3 )
        ESP_LOGI(GB_TAG, "fret: %d, string: %d = servo[%d]", fret, string, fret_idx);
    
        // check valid
        if(fret_idx >= N_FRET_SERVOS){
            ESP_LOGE(GB_TAG, "Invalid servo index");
            return ESP_ERR_NOT_FOUND;
        }

        *out_fret_servo = fret_servos[fret_idx];
    }
    
    /* String index */
    *out_bridge_servo = bridge_servos[string/2];
    
    return ESP_OK;
}



/***************************************************************************************************/
// Play tab functions


/// @brief play a note given a string and fret
/// If fret is 0 --> play an open string
/// TODO: handle using 6 bridge servos
/// @param string 
/// @param fret 
/// @return 
esp_err_t play_note(uint8_t string, uint8_t fret)
{
    ESP_LOGI(GB_TAG, "Playing note\nstring %u, fret %u", string, fret);

    // Find matching servo
    guitar_fret_servo_t fret_servo = {0};
    guitar_bridge_servo_t bridge_servo = {0};
    esp_err_t err = lookup_servo(string, fret, &fret_servo, &bridge_servo);
    if (err != ESP_OK) {
        return err;
    }

    if(!bridge_servo.servo){
        ESP_LOGI(GB_TAG, "Servo not found for fret %u string %u", fret, string);
        return ESP_ERR_NOT_FOUND;
    }
    if(fret > 0 && !bridge_servo.servo){
        ESP_LOGI(GB_TAG, "Servo not found for fret %u string %u", fret, string);
        return ESP_ERR_NOT_FOUND;
    }

    // Get the angle  
    float fret_angle = SERVO_ANGLE_DEFAULT;
    float bridge_angle = SERVO_ANGLE_DEFAULT;
    if(string % 2 == 0){ // even strings (0,2,4) = (E,D,B)
        fret_angle = SERVO_ANGLE_TOP;
        // Check the current angle 
        bridge_angle = bridge_servo.servo->current_angle == BRIDGE_SERVO_ANGLE_TOP ? SERVO_ANGLE_DEFAULT : BRIDGE_SERVO_ANGLE_TOP;
        // bridge_angle = BRIDGE_SERVO_ANGLE_TOP;
    }
    else{
        fret_angle = SERVO_ANGLE_BOTTOM;
        bridge_angle = bridge_servo.servo->current_angle == BRIDGE_SERVO_ANGLE_BOTTOM ? SERVO_ANGLE_DEFAULT : BRIDGE_SERVO_ANGLE_BOTTOM;
        // bridge_angle = BRIDGE_SERVO_ANGLE_BOTTOM;
    }

    // Check current angle - if already top/bottom - reset to default
    if(fret > 0){
        // TODO: reset string only if needed
        reset_string(string, fret); // reset frets on same string that block the target fret
        err = servo_set_angle(fret_servo.servo, fret_angle); // fret in default
        vTaskDelay(pdMS_TO_TICKS(200)); // slight delay to let servo hit fret
    }
    else{
        reset_string(string, 1); // reset all frets on same string
    }
    err = servo_set_angle(bridge_servo.servo, bridge_angle); // bridge to top
    vTaskDelay(pdMS_TO_TICKS(200));


    // If no servo exists - try to find closest corresponding note

    return ESP_OK;

}


/// @brief Flow:
/// 1. For each note in the beat - get the servos and requested position (left/right) (servos[], servo_direction_t.ARM_TOP)
/// 2. 
/// @param beat 
/// @return 
esp_err_t play_beat(beat_t *beat)
{
    if(!beat || beat->n_notes < 1){
        ESP_LOGE(GB_TAG, "Beat is null");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(GB_TAG, "Playing beat %u (%u notes)", beat->num, beat->n_notes);


    // Loop through each note of beat
    for(int i=0; i < beat->n_notes; i++){
        note_t *note = &beat->notes[i];
        guitar_fret_servo_t fret_servo;
        guitar_bridge_servo_t bridge_servo;
        lookup_servo(note->string, note->fret, &fret_servo, &bridge_servo);

        // play_note(beat->n_notes[i], );
    }



    // If no servo exists - try to find closest corresponding note
    // If two consecutive strings (E-A, D-G, B-e) of same fret are 


    return ESP_OK;

}


/// @brief Reset all of the frets on a given string
/// @param string the string to reset 
/// @param fret starting index to reset all frets (e.g. if 0 --> reset all, if 2 --> reset all frets 2 and above)
/// TODO: get logic working
/// @return 
esp_err_t reset_string(uint8_t string, uint8_t fret_offset){
    esp_err_t err;

    if(string > 5){
        ESP_LOGE(GB_TAG, "Invalid string: %u", string);
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(GB_TAG, "Resetting string %s", STRING_NAMES[string]);

    guitar_fret_servo_t fret_servo = {0};
    // guitar_bridge_servo_t bridge_servo = {0};

    uint8_t n_frets = get_fret_count();
    if(fret_offset > n_frets) return ESP_ERR_INVALID_ARG; // ensure target fret in bounds
    if(fret_offset <= 0) fret_offset = 1;

    for(uint8_t f=fret_offset; f<n_frets+1; f++){
        int fret_idx = (f - 1) * 3 + string/2; // Get the array index from the fret ( fret = idx / 3 + 1 --> (idx + 1) * 3 )
        ESP_LOGI(GB_TAG, "fret: %d, string: %d = servo[%d]", f, string, fret_idx);

        // check valid
        if(fret_idx >= N_FRET_SERVOS && fret_idx < 0){
            ESP_LOGE(GB_TAG, "Invalid servo index");
            return ESP_ERR_NOT_FOUND;
        }

        fret_servo = fret_servos[fret_idx];
        if(fret_servo.servo){
            err = servo_set_angle(fret_servo.servo, SERVO_ANGLE_DEFAULT); // fret in default
            vTaskDelay(pdMS_TO_TICKS(20)); // slight delay to let servo hit fret
        }
    }

    return ESP_OK;
    
}


/// @brief Create the guitar tab struct, ensuring enough memory allocated for the beats
/// @param name 
/// @param tempo 
/// @param n_beats 
/// @return 
guitar_tab_t *guitar_tab_create(const char *name, uint16_t tempo, size_t n_beats)
{
    tab = calloc(1, sizeof(guitar_tab_t) + n_beats * sizeof(beat_t));
    if (tab == NULL) return NULL;

    strncpy(tab->name, name, sizeof(tab->name) - 1);   // calloc already zeroed the terminator
    tab->tempo = tempo;
    tab->n_beats = n_beats;
    return tab;
}



int get_tab_beat_count(jparse_ctx_t *jctx)
{
    ESP_LOGI(GB_TAG, "Get tab beat count");

    int n_measure_beats = 0;
    int fret = 0;
    int string = 0;
    int beat_count = 0;
    int measure_count = 0;

    /* Read the tab music (broken into measures - beats - notes) */
    if (json_obj_get_object(jctx, tab_prop) != OS_SUCCESS) {
        ESP_LOGE(GB_TAG, "Error reading json tab property");
        return ESP_ERR_NOT_FOUND;
    }
    else{
        // Read the measures
        if (json_obj_get_array(jctx, tab_measures_prop, &measure_count) == OS_SUCCESS) {
            /* Initialize the guitar tab object */
            if(measure_count > 0){
                for(int i = 0; i < measure_count; i++){
                    // Get the measure object
                    if (json_arr_get_object(jctx, i) == OS_SUCCESS){
                        // Get the measure beats array
                        if(json_obj_get_array(jctx, tab_beats_prop, &n_measure_beats) == OS_SUCCESS) {
                            if(n_measure_beats > 0)
                            {
                                // Loop through each beat of the measure
                                for(int j = 0; j < n_measure_beats; j++){
                                    // printf("beat[%d/%d]\n", j, n_measure_beats);
                                    beat_count++; // increment the beat count
                                }
                           }
                            json_obj_leave_array(jctx);
                        }
                        json_arr_leave_object(jctx);
                    }
                }
            }
		    json_obj_leave_array(jctx);
	    }
		json_obj_leave_object(jctx);
	}

    return beat_count;
}


/// @brief Extract the measures/beats/notes from the tab
/// @param jctx Json context object
/// @param n_measures number of measures read in the tab
/// @param n_beats number of beats read in the tab
/// @param beats array of beats
/// @return 
esp_err_t read_tab_notes(jparse_ctx_t *jctx, int n_beats)
{
    ESP_LOGI(GB_TAG, "Reading json tab property");

    int n_measure_beats = 0;
    int fret = 0;
    int string = 0;
    int beat_count = 0;
    int measure_count = 0;

    // beats = calloc(1, sizeof(beat_t) * n_beats); // initialize an empty beat array

    /* Read the tab music (broken into measures - beats - notes) */
    if (json_obj_get_object(jctx, tab_prop) != OS_SUCCESS) {
        ESP_LOGE(GB_TAG, "Error reading json tab property");
        return ESP_ERR_NOT_FOUND;
    }
    else{
		printf("reading tab object\n");
		
        // Read the measures
        if (json_obj_get_array(jctx, tab_measures_prop, &measure_count) == OS_SUCCESS) {
            printf("%d measures\n", measure_count);
            /* Initialize the guitar tab object */
            if(measure_count > 0){
                for(int i = 0; i < measure_count; i++){
                    // Get the measure object
                    if (json_arr_get_object(jctx, i) == OS_SUCCESS){

                        // Get the measure beats array
                        if(json_obj_get_array(jctx, tab_beats_prop, &n_measure_beats) == OS_SUCCESS) {
                            ESP_LOGI(GB_TAG, "measure[%d/%d] has %d beats", i, measure_count, n_measure_beats);

                            if(n_measure_beats > 0)
                            {
                                // Loop through each beat of the measure
                                for(int j = 0; j < n_measure_beats; j++){
                                    // ESP_LOGI(GB_TAG, "beat[%d/%d]\n", j, n_measure_beats);

                                    // Get the note
                                    if (json_arr_get_object(jctx, j) == OS_SUCCESS){

                                        // Initialize the beat
                                        int n_notes = 1; // TODO: hardcoded
                                        // tab->beats[beat_count].n_notes = n_notes;
                                        tab->beats[beat_count].n_notes = n_notes;
                                        tab->beats[beat_count].num = beat_count;
                                        tab->beats[i].notes = calloc(1, sizeof(note_t) * n_notes);

                                        if (json_obj_get_int(jctx, string_prop, &string) == OS_SUCCESS) tab->beats[i].notes[0].string = string;
                                        else tab->beats[i].notes[0].string = STRING_FRET_VALUE_NULL; // Set to 255 (null)

                                        if (json_obj_get_int(jctx, fret_prop, &fret) == OS_SUCCESS) tab->beats[i].notes[0].fret = fret;
                                        else tab->beats[i].notes[0].fret = STRING_FRET_VALUE_NULL; // Set to 255 (null)

                                        ESP_LOGI(GB_TAG, "Beat[%d/%d]: string %d, fret %d", beat_count, n_beats, tab->beats[i].notes[0].string, tab->beats[i].notes[0].fret);

                                        json_arr_leave_object(jctx);
                                    } 
                                    else{
                                        ESP_LOGE(GB_TAG, "Couldn't read note object");
                                    }

                                    beat_count++; // increment the beat count
                                }
                           }
                            json_obj_leave_array(jctx);
                        }
                        json_arr_leave_object(jctx);
                    }
                }
            }
		    json_obj_leave_array(jctx);
	    }
		json_obj_leave_object(jctx);
	}

    return ESP_OK;
}




esp_err_t parse_tab(char *json_str)
{
    ESP_LOGI(GB_TAG, "Parsing json");
    // TODO: ensure valid json

	jparse_ctx_t jctx;
	char tab_name[64];
    uint16_t tempo = 60;
    int beat_count = 0;
    beat_t **beats;

    int ret = json_parse_start(&jctx, json_str, strlen(json_str));
	if (ret != OS_SUCCESS) {
		ESP_LOGE(GB_TAG, "Error parsing json");
		return ESP_ERR_INVALID_ARG;
	}

	/* Read the tab overview info (song name, tempo, etc) */
    if (json_obj_get_object(&jctx, info_prop) == OS_SUCCESS) {
		printf("reading info object\n");
		if (json_obj_get_string(&jctx, info_title_prop, str_val, sizeof(str_val)) == OS_SUCCESS) {
			printf("Title: %s\n", str_val);
			strcpy(tab_name, str_val);
		}
        if (json_obj_get_string(&jctx, info_track_name_prop, str_val, sizeof(str_val)) == OS_SUCCESS) {
			printf("Track name: %s\n", str_val);
		}
			
        if (json_obj_get_int(&jctx, info_tempo_prop, &int_val) == OS_SUCCESS){
            printf("Tempo %d\n", int_val);
            tempo = int_val;
        } 
		json_obj_leave_object(&jctx);
	}

    /* Read the tab music (broken into measures - beats - notes) */
    beat_count = get_tab_beat_count(&jctx); // Get the beat count
    if(beat_count > 0){
        ESP_LOGI(GB_TAG, "Beat count = %d", beat_count);
        // Initialize the guitar_tab_t
        guitar_tab_create(tab_name, tempo, beat_count);
        
        read_tab_notes(&jctx, beat_count);
    }
    else{
        ESP_LOGE(GB_TAG, "No beats detected");
    }

	json_parse_end(&jctx);

    return ESP_OK;
}



/// @brief Read the guitar tab from the SD card file
/// @param file_path 
/// @return 
esp_err_t read_tab(const char *file_path)
{
    ESP_LOGI(GB_TAG, "Reading guitar tab: %s", file_path);
    esp_err_t err;
    
    // Read the SD card file
    // FILE* tab_file = Open_File(file_path);
    // err = s_example_read_file(file_path);
    err = sd_read_file(file_path);
    if(err != ESP_OK){
        ESP_LOGE(GB_TAG, "Error reading guitar tab: %s", esp_err_to_name(err));
    }

    // parse the file content as json
    err = parse_tab(file_buffer);
    if(err != ESP_OK){
        ESP_LOGE(GB_TAG, "Error parsing guitar tab json: %s", esp_err_to_name(err));
    }


    // Clear up memory
    // free(file_buffer);
    
    return ESP_OK;
}



/*****************************************************************************************************/
// initialisation


void setup_bridge_servo(servo_t *servo, uint8_t servo_num, uint8_t top_string, uint8_t bottom_string, guitar_bridge_servo_t *out_bridge_servos)
{
    ESP_LOGI(GB_TAG, "Initialize bridge servo channel %u", servo_num);

    if(out_bridge_servos == NULL || servo_num >= N_SERVOS || servo == NULL){
        ESP_LOGE(GB_TAG, "Create servo failed - check servo is initialized");
        return;
    }

    guitar_bridge_servo_t *bridge_servo = &out_bridge_servos[servo_num]; // Get the pointer to the current servo being set up
    bridge_servo->servo = servo;
    bridge_servo->top_string = top_string;
    bridge_servo->bottom_string = bottom_string;
    bridge_servo->current_angle = SERVO_ANGLE_DEFAULT;
    bridge_servo->top_string_angle = SERVO_ANGLE_TOP;
    bridge_servo->bottom_string_angle = SERVO_ANGLE_BOTTOM;
}


void setup_fret_servo(servo_t *servo, uint8_t servo_num, uint8_t fret_num, uint8_t top_string, uint8_t bottom_string, guitar_fret_servo_t *out_fret_servos)
{
    ESP_LOGI(GB_TAG, "Initialize fret servo channel %u", servo_num);

    if(out_fret_servos == NULL || servo_num >= N_SERVOS || servo == NULL){
        ESP_LOGE(GB_TAG, "Create servo failed - check servo is initialized");
        return;
    }

    guitar_fret_servo_t *fret_servo = &out_fret_servos[servo_num]; // Get the pointer to the current servo being set up
    fret_servo->servo = servo;
    fret_servo->top_string = top_string;
    fret_servo->bottom_string = bottom_string;
    fret_servo->fret = fret_num;
    fret_servo->prefered_string = top_string;
    fret_servo->current_angle = SERVO_ANGLE_DEFAULT;
    fret_servo->top_string_angle = SERVO_ANGLE_TOP;
    fret_servo->bottom_string_angle = SERVO_ANGLE_BOTTOM;

}


// Guitarbot initialisation
esp_err_t guitarbot_init(servo_t servos[], guitar_fret_servo_t **out_fret_servos, guitar_bridge_servo_t **out_bridge_servos)
{
    ESP_LOGI(GB_TAG, "Initializing GuitarBot");
    esp_err_t err;

    if(!servos){
        ESP_LOGE(GB_TAG, "Guitarbot initialization failed: invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    servos_arr = servos;

    // Setup the servo configuration for Guitarbot
    int n_fret_servos = N_SERVOS - N_BRIDGE_SERVOS; // Remaining servos after provisioning bridge servos
    guitar_bridge_servo_t *_bridge_servos = calloc(N_BRIDGE_SERVOS, sizeof(guitar_bridge_servo_t)); // initialize the array with empty values
    guitar_fret_servo_t *_fret_servos = calloc(N_FRET_SERVOS, sizeof(guitar_fret_servo_t)); // initialize the array with empty values

    ESP_LOGI(GB_TAG, "Setting up bridge");
    // Setup bridge servo pairs E-A, D-G, B-E
    int offset = 0;
    for(int b=0; b<N_BRIDGE_SERVOS; b++){
        setup_bridge_servo(&servos[b], b, b+offset, b+1+offset, _bridge_servos);
        offset++;
    }

    ESP_LOGI(GB_TAG, "Setting up frets");
    // setup fret servos pairs 1/E-A, 1/D-G, 1/D-G, 2/E-A ...
    int fret = 0;
    int top_string = 0;
    int bottom_string = 0;
    for(int f=0; f<n_fret_servos; f++){
        uint8_t pair = f % 3; // servos are grouped into 3 per fret
        fret = f / 3 + 1;
        top_string = pair * 2; // E D B strings
        bottom_string = pair * 2 + 1; // A G e strings
        
        setup_fret_servo(&servos[f+N_BRIDGE_SERVOS], f, fret, top_string, bottom_string, _fret_servos);
    }

    fret_servos = _fret_servos;
    bridge_servos = _bridge_servos;

    *out_fret_servos = _fret_servos;
    *out_bridge_servos = _bridge_servos;
    
    setupComplete = true;

    return ESP_OK;

}



esp_err_t test_play()
{
    esp_err_t err;

    ESP_LOGI(GB_TAG, "Playing test song");
    isTabPlaying = true;

    // Set servos to default
    for(int i = 0; i < N_BRIDGE_SERVOS; i++){
        
        err = servo_set_angle(bridge_servos[i].servo, SERVO_ANGLE_DEFAULT);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    for(int j = 0; j < N_FRET_SERVOS; j++){
        
        err = servo_set_angle(fret_servos[j].servo, SERVO_ANGLE_DEFAULT);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
    for(int i = 0; i < 3; i++){
        // TOP STRING
        err = servo_set_angle(fret_servos[i].servo, SERVO_ANGLE_DEFAULT); // fret in default
        vTaskDelay(pdMS_TO_TICKS(200));
        err = servo_set_angle(bridge_servos[i].servo, BRIDGE_SERVO_ANGLE_TOP); // bridge to top
        vTaskDelay(pdMS_TO_TICKS(2000));

        err = servo_set_angle(fret_servos[i].servo, SERVO_ANGLE_TOP); // fret to top
        vTaskDelay(pdMS_TO_TICKS(200));
        err = servo_set_angle(bridge_servos[i].servo, SERVO_ANGLE_DEFAULT); // bridge return to center
        vTaskDelay(pdMS_TO_TICKS(2000));

        // BOTTOM STRING
        err = servo_set_angle(fret_servos[i].servo, SERVO_ANGLE_DEFAULT); // fret in default
        vTaskDelay(pdMS_TO_TICKS(200));
        err = servo_set_angle(bridge_servos[i].servo, BRIDGE_SERVO_ANGLE_BOTTOM); // bridge to top
        vTaskDelay(pdMS_TO_TICKS(2000));

        err = servo_set_angle(fret_servos[i].servo, SERVO_ANGLE_BOTTOM); // fret to top
        vTaskDelay(pdMS_TO_TICKS(200));
        err = servo_set_angle(bridge_servos[i].servo, SERVO_ANGLE_DEFAULT); // bridge return to center
        vTaskDelay(pdMS_TO_TICKS(2000));

    }

    // Set servos to default
    for(int i = 0; i < N_BRIDGE_SERVOS; i++){
        
        err = servo_set_angle(bridge_servos[i].servo, SERVO_ANGLE_DEFAULT);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    for(int j = 0; j < N_FRET_SERVOS; j++){
        
        err = servo_set_angle(fret_servos[j].servo, SERVO_ANGLE_DEFAULT);
        vTaskDelay(pdMS_TO_TICKS(100));
    }


    isTabPlaying = false;

    return ESP_OK;

}



/*********************************************************************************/
// helper functions

/// @brief Get the number of frets
/// @return 
uint8_t get_fret_count()
{
    return N_FRET_SERVOS / 3;
}

/// @brief Reset the tab - free memory
/// @return 
esp_err_t reset_tab()
{
    free(tab);
}
