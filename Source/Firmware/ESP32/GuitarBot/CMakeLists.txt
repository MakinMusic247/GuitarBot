# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)
project(GuitarBot)


# Added: quick fix for the error:
# C:/esp/v6.1/esp-idf/components/esp_lcd/rgb/esp_lcd_panel_rgb.c: In function 'rgb_panel_draw_bitmap':
# C:/esp/v6.1/esp-idf/components/esp_lcd/rgb/esp_lcd_panel_rgb.c:788:1: internal compiler error: Segmentation fault
#   788 | }
idf_build_get_property(build_components BUILD_COMPONENTS)
if("esp_lcd" IN_LIST build_components)
    idf_component_get_property(esp_lcd_lib esp_lcd COMPONENT_LIB)
    set_source_files_properties(
        "${CMAKE_CURRENT_LIST_DIR}/../esp-idf/components/esp_lcd/rgb/esp_lcd_panel_rgb.c"
        PROPERTIES COMPILE_OPTIONS "-O0"
    )
endif()
