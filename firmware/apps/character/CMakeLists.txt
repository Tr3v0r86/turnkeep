idf_component_register(SRCS "character_app.c" "core/character_core.c"
                       INCLUDE_DIRS "include" "core/include"
                       REQUIRES trevos nvs_flash)
