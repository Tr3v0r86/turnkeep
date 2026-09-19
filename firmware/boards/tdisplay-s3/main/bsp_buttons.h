// bsp_buttons.h — T-Display-S3 two-button input (board-specific).
// BOOT on GPIO0, KEY on GPIO14, both active-low. This is the board's whole input
// surface (no touch, no wheel). TrevOS maps these to navigate + commit later.
#pragma once
#include <stdint.h>

typedef enum { BSP_BTN_BOOT = 0, BSP_BTN_KEY = 1 } bsp_btn_t;
typedef enum { BSP_BTN_PRESS = 0, BSP_BTN_LONG = 1, BSP_BTN_SLEEP_CHORD = 2 } bsp_btn_evt_t;

// Fired from the button poll task. If it touches LVGL, take the lvgl_port lock.
typedef void (*bsp_btn_cb_t)(bsp_btn_t btn, bsp_btn_evt_t evt, void *ctx);

void bsp_buttons_init(bsp_btn_cb_t cb, void *ctx);
