idf_component_register(SRCS "Guitarbot.c" "utils.c"
                       INCLUDE_DIRS "include"
                       REQUIRES Servo Waveshare json_parser) # TODO: separate the SD_Card component from Waveshare
