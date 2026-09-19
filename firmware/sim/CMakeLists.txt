cmake_minimum_required(VERSION 3.16)
project(turnkeep_sim C ASM)
set(CMAKE_C_STANDARD 11)
set(REPO ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(MAIN ${REPO}/boards/tdisplay-s3/main)
set(UI ${REPO}/os/trevos/ui)
set(LVGL_DIR "${REPO}/boards/tdisplay-s3/managed_components/lvgl__lvgl" CACHE PATH "LVGL 9.6 source directory; populated by the device build")
find_package(SDL2 REQUIRED CONFIG)
find_package(Threads REQUIRED)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
set(LV_BUILD_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE PATH "" FORCE)
add_subdirectory(${LVGL_DIR} ${CMAKE_BINARY_DIR}/lvgl)
target_link_libraries(lvgl PUBLIC SDL2::SDL2)
set(CHARACTER_SOURCES
    ${MAIN}/main_character.c
    ${REPO}/os/trevos/trevos.c
    ${REPO}/apps/character/character_app.c
    ${REPO}/apps/character/core/character_core.c
    ${UI}/trevos_ui.c
    ${UI}/fonts/plex_mono_b_72.c
    ${UI}/fonts/plex_mono_b_25.c
    ${UI}/fonts/plex_sans_b_28.c
    ${UI}/fonts/plex_sans_b_22.c
    ${UI}/fonts/plex_sans_b_16.c
    ${UI}/fonts/plex_sans_b_12.c
    ${UI}/fonts/plex_sans_r_14.c
    ${UI}/fonts/plex_sans_r_10.c
    ${UI}/fonts/plex_mono_sb_13.c
    ${UI}/fonts/plex_mono_sb_11.c
    ${UI}/fonts/plex_mono_sb_10.c
    ${UI}/fonts/plex_mono_sb_9.c
    ${CMAKE_CURRENT_SOURCE_DIR}/sim_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs/sim_lvgl_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs/sim_nvs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs/sim_bsp_display.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs/sim_buttons.c
)

add_executable(charactersim ${CHARACTER_SOURCES})
target_include_directories(charactersim BEFORE PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/shims
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${REPO}/os/trevos/include
    ${REPO}/apps/character/include
    ${REPO}/apps/character/core/include
    ${MAIN}
)
target_compile_definitions(charactersim PRIVATE
    app_main=board_app_main
    LV_LVGL_H_INCLUDE_SIMPLE
    CHARACTER_SIM=1
    BSP_NO_DEV_SHOT=1
    TT_NO_LAYER_FX=1
)
target_link_libraries(charactersim lvgl SDL2::SDL2 Threads::Threads)
