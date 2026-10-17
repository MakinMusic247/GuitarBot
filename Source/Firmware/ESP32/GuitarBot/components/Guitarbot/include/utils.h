/* 
*   Utilility functions for GuitarBot
*   official ESP-IDF json_paser component: https://github.com/espressif/json_parser/tree/master
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string.h>
#include "json_parser.h"
#include <esp_err.h>
#include "esp_log.h"
#include "Guitarbot.h"


#define json_test_str	"{\n\"str_val\" :    \"JSON Parser\",\n" \
			"\t\"float_val\" : 2.0,\n" \
			"\"int_val\" : 2017,\n" \
			"\"bool_val\" : false,\n" \
			"\"supported_el\" :\t [\"bool\",\"int\","\
			"\"float\",\"str\"" \
			",\"object\",\"array\"],\n" \
			"\"features\" : { \"objects\":true, "\
			"\"arrays\":\"yes\"},\n"\
			"\"int_64\":109174583252}"



// guitar_tab_t *guitar_tab_create(const char *name, uint16_t tempo, size_t n_beats);



// /// @brief parse a string into a guitar tab json object
// /// @param json 
// /// @param json_str 
// /// @param out_tab
// /// @return 
// esp_err_t parse_tab(char *json_str, guitar_tab_t *out_tab);


#endif