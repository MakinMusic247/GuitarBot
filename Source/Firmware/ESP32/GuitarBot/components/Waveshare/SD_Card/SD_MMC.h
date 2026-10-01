
#pragma once

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "dirent.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_log.h" 
#include <errno.h>

#include "esp_flash.h"    

#define CONFIG_EXAMPLE_PIN_CLK  14
#define CONFIG_EXAMPLE_PIN_CMD  17
#define CONFIG_EXAMPLE_PIN_D0   16
#define CONFIG_EXAMPLE_PIN_D1   -1
#define CONFIG_EXAMPLE_PIN_D2   -1
#define CONFIG_EXAMPLE_PIN_D3   -1  

#define CONFIG_SD_Card_D3       21  


#define MAX_FILE_NAME_SIZE      100  // Define maximum file name size
#define MAX_PATH_SIZE           512  // Define a larger size for the full path
#define MAX_DIRECTORY_SIZE      20   // maximum number of files in a directory to read

typedef struct {
    char* file_name;
    char* file_path;
    char* extension;
    bool is_folder;
} file_t;


esp_err_t SD_Card_CS_EN(void);
esp_err_t SD_Card_CS_Dis(void);

esp_err_t s_example_write_file(const char *path, char *data);
esp_err_t s_example_read_file(const char *path);

extern uint32_t SDCard_Size;
extern uint32_t Flash_Size;
void SD_Init(void);
void Flash_Searching(void);
FILE* Open_File(const char *file_path);
uint16_t Folder_retrieval(const char* directory, const char* fileExtension, char File_Name[][100],uint16_t maxFiles);

uint16_t Get_Folder(const char* directory, file_t **out_files);