idf_component_register(SRCS "main_character.c" "bsp_display.c" "bsp_buttons.c" "button_filter.c"
    INCLUDE_DIRS "."
    REQUIRES esp_lcd driver esp_lvgl_port trevos character nvs_flash esp_hw_support)
target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_LVGL_H_INCLUDE_SIMPLE)
