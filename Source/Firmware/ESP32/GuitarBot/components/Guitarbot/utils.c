#include "utils.h"

static const char *UTILS_TAG = "UTILS";


// char str_val[64];
// int int_val = 0;

// // property names for reading tab
// const char *info = "info";
// const char *info_title = "title";
// const char *info_track_name = "track_name";
// const char *info_tempo = "tempo";
// const char *tab = "tab";
// const char *tab_measures = "measures";


// guitar_tab_t *guitar_tab_create(const char *name, uint16_t tempo, size_t n_beats)
// {
//     guitar_tab_t *tab = calloc(1, sizeof(guitar_tab_t) + n_beats * sizeof(beat_t));
//     if (tab == NULL) return NULL;

//     strncpy(tab->name, name, sizeof(tab->name) - 1);   // calloc already zeroed the terminator
//     tab->tempo = tempo;
//     tab->n_beats = n_beats;
//     return tab;
// }



// esp_err_t parse_tab(char *json_str, guitar_tab_t *out_tab)
// {
//     ESP_LOGI(UTILS_TAG, "Parsing json");
//     // TODO: ensure valid json

// 	jparse_ctx_t jctx;
// 	char tab_name[64];
// 	// char 

//     int ret = json_parse_start(&jctx, json_str, strlen(json_str));
// 	if (ret != OS_SUCCESS) {
// 		ESP_LOGE(UTILS_TAG, "Error parsing json");
// 		return ESP_ERR_INVALID_ARG;
// 	}

// 	/* Read the tab overview info (song name, tempo, etc) */
//     if (json_obj_get_object(&jctx, info) == OS_SUCCESS) {
// 		printf("reading info object\n");
// 		if (json_obj_get_string(&jctx, info_title, str_val, sizeof(str_val)) == OS_SUCCESS) {
// 			printf("Title: %s\n", str_val);
// 			// out_tab->name = strdup(str_val);
// 		}
//         if (json_obj_get_string(&jctx, info_track_name, str_val, sizeof(str_val)) == OS_SUCCESS) {
// 			printf("Track name: %s\n", str_val);
// 		}
			
//         if (json_obj_get_int(&jctx, info_tempo, &int_val) == OS_SUCCESS)
// 		    printf("Tempo %d\n", int_val);
// 		json_obj_leave_object(&jctx);
// 	}

// 	// Initialize the guitar_tab_t

// 	json_parse_end(&jctx);

//     return ESP_OK;
// }