// bsp_display.h — LilyGo T-Display-S3 display BSP (board-specific).
// TrevOS and app-cores sit on top of this; they never touch these pins.
#pragma once
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

// Landscape 320x170 (TrevOS design). Panel swap_xy in bsp; gap stays (35,0).
// Wrapped in #ifndef so a per-target -D can override
// the resolution without forking this header. The real T3 device build sets no override,
// so 320/170 stand. (Used by the sim to run the shared shell at the CYD's 240x320.)
#ifndef BSP_LCD_H_RES
#define BSP_LCD_H_RES 320
#endif
#ifndef BSP_LCD_V_RES
#define BSP_LCD_V_RES 170
#endif

// Power the panel, bring up the i80 bus + ST7789, set the 35px column gap and
// colour inversion. Backlight is left OFF; call bsp_display_backlight_on() after
// the first frame is on the buffer so there is no white flash at boot.
esp_err_t bsp_display_init(esp_lcd_panel_io_handle_t *ret_io, esp_lcd_panel_handle_t *ret_panel);
void bsp_display_backlight_on(void);
void bsp_display_power_off(esp_lcd_panel_handle_t panel);
