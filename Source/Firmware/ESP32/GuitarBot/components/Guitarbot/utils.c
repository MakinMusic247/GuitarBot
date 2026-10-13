#include "utils.h"

static const char *UTILS_TAG = "UTILS";


char str_val[64];
int int_val = 0;

// property names for reading tab
const char *info = "info";
const char *info_title = "title";
const char *info_track_name = "track_name";
const char *info_tempo = "tempo";


esp_err_t parse_json(jparse_ctx_t *jctx, char *json_str)
{
    ESP_LOGI(UTILS_TAG, "Parsing json");
    // TODO: ensure valid json - check starts and ends with '{' '}'

    int ret = json_parse_start(jctx, json_str, strlen(json_str));
	if (ret != OS_SUCCESS) {
		ESP_LOGE(UTILS_TAG, "Error parsing json");
		return ESP_ERR_INVALID_ARG;
	}

    if (json_obj_get_object(jctx, info) == OS_SUCCESS) {
		printf("reading info object\n");
		if (json_obj_get_string(jctx, info_title, str_val, sizeof(str_val)) == OS_SUCCESS)
			printf("Title: %s\n", str_val);
        if (json_obj_get_string(jctx, info_track_name, str_val, sizeof(str_val)) == OS_SUCCESS)
			printf("Track name: %s\n", str_val);
        if (json_obj_get_int(jctx, "int_val", &int_val) == OS_SUCCESS)
		    printf("int_val %d\n", int_val);
		json_obj_leave_object(jctx);
	}

    return ESP_OK;
}