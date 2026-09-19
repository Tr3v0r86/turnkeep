// bsp_display.c — LilyGo T-Display-S3 display BSP: ST7789, 170x320, 8-bit i80.
//
// Pins are reference from the LilyGo schematic. First light (2026-06-20) confirmed
// the panel powers up via GPIO15 and renders R/G/B/W correctly with swap_color_bytes
// + colour inversion + a 35px column gap, so the colour path below is settled.
#include "bsp_display.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_POWER_ON 15   // LCD power enable -> HIGH or the screen stays dark
#define PIN_BL       38   // backlight
#define PIN_RD        9   // 8080 RD -> tie HIGH
#define PIN_WR        8   // i80 pixel clock
#define PIN_DC        7
#define PIN_CS        6
#define PIN_RST       5
#define PIN_D0       39
#define PIN_D1       40
#define PIN_D2       41
#define PIN_D3       42
#define PIN_D4       45
#define PIN_D5       46
#define PIN_D6       47
#define PIN_D7       48
// GAP under landscape (swap_xy/MADCTL MV on): esp_lcd_panel_st7789 adds x_gap to
// CASET and y_gap to RASET in draw_bitmap BEFORE the MV transpose happens in the
// controller. With MV on, CASET bounds the 320 extent (needs gap 0) and RASET bounds
// the 240-wide GRAM column axis where the panel sits at cols 35..204 (needs gap 35).
// So the 35px offset lives on Y in landscape, NOT X. (35,0) overflows CASET to 354 =>
// garbage band; (0,35) addresses [0,319]x[35,204] cleanly. Panel offset is symmetric
// 35/35 so mirror cannot reintroduce it. See esp_lcd_panel_st7789.c draw_bitmap.
#define X_GAP         0
#define Y_GAP        35

static const char *TAG = "bsp_disp";

void bsp_display_backlight_on(void) { gpio_set_level(PIN_BL, 1); }

void bsp_display_power_off(esp_lcd_panel_handle_t panel)
{
    gpio_set_level(PIN_BL, 0);
    if (panel) esp_lcd_panel_disp_on_off(panel, false);
    gpio_set_level(PIN_POWER_ON, 0);
}

esp_err_t bsp_display_init(esp_lcd_panel_io_handle_t *ret_io, esp_lcd_panel_handle_t *ret_panel)
{
    gpio_config_t gc = {
        .pin_bit_mask = (1ULL << PIN_POWER_ON) | (1ULL << PIN_RD) | (1ULL << PIN_BL),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&gc);
    gpio_set_level(PIN_POWER_ON, 1);
    gpio_set_level(PIN_RD, 1);
    gpio_set_level(PIN_BL, 0);                 // off until the first frame
    vTaskDelay(pdMS_TO_TICKS(20));

    esp_lcd_i80_bus_handle_t bus = NULL;
    esp_lcd_i80_bus_config_t bus_cfg = {
        .dc_gpio_num = PIN_DC,
        .wr_gpio_num = PIN_WR,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = { PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7 },
        .bus_width = 8,
        .max_transfer_bytes = BSP_LCD_H_RES * 80 * 2,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_i80_bus(&bus_cfg, &bus), TAG, "i80 bus");

    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_io_i80_config_t io_cfg = {
        .cs_gpio_num = PIN_CS,
        .pclk_hz = 6 * 1000 * 1000,
        .trans_queue_depth = 10,
        .dc_levels = { .dc_idle_level = 0, .dc_cmd_level = 0, .dc_dummy_level = 0, .dc_data_level = 1 },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .flags = { .swap_color_bytes = 1 },    // high-byte-first RGB565 to the ST7789
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i80(bus, &io_cfg, &io), TAG, "panel io");

    esp_lcd_panel_handle_t panel = NULL;
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = PIN_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7789(io, &panel_cfg, &panel), TAG, "st7789");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, true));
    // Rotation (swap_xy/mirror) is owned by esp_lvgl_port, NOT here. lvgl_port_add_disp
    // re-applies swap_xy+mirror from disp_cfg.rotation at rotation-0 init, clobbering
    // anything set on the panel before it. Setting them here is dead code + the source
    // of the upright-portrait + garbage-band bug. See main.c disp_cfg.rotation.
    // Gap is NOT touched by lvgl_port, so it stays set here: (35,0) in BOTH orientations
    // because esp_lcd swaps x/y before adding the gap.
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel, X_GAP, Y_GAP));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

    *ret_io = io;
    *ret_panel = panel;
    ESP_LOGI(TAG, "ST7789 %dx%d i80 up", BSP_LCD_H_RES, BSP_LCD_V_RES);
    return ESP_OK;
}
