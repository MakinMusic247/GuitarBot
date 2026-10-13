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


/// @brief parse a string into a guitar tab json object
/// @param json 
/// @param json_str 
/// @return 
esp_err_t parse_json(jparse_ctx_t *json, char *json_str);


#endif