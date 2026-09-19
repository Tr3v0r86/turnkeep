#include "bsp_buttons.h"
#include "bsp_display.h"
#include "character_app.h"
#include "trevos.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "lvgl.h"

#ifndef CHARACTER_SIM
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#endif

static const char *TAG = "character_case";
static esp_lcd_panel_handle_t s_panel;

static void sleep_now(void)
{
    if (!character_app_prepare_sleep()) {
        ESP_LOGE(TAG, "sleep refused: state not saved");
        return;
    }
#ifdef CHARACTER_SIM
    ESP_LOGI(TAG, "simulated sleep after stable chord release");
#else
    ESP_LOGI(TAG, "sleep entry levels: GPIO0=%d GPIO14=%d",
             gpio_get_level(GPIO_NUM_0), gpio_get_level(GPIO_NUM_14));
    bsp_display_power_off(s_panel);
    // EXT1 switches these pads to RTC IO; keep the active-low buttons inactive in sleep.
    ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(GPIO_NUM_0));
    ESP_ERROR_CHECK(rtc_gpio_pullup_en(GPIO_NUM_0));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_dis(GPIO_NUM_14));
    ESP_ERROR_CHECK(rtc_gpio_pullup_en(GPIO_NUM_14));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(
        (1ULL << 0) | (1ULL << 14), ESP_EXT1_WAKEUP_ANY_LOW));
    ESP_LOGI(TAG, "deep sleep; GPIO0 or GPIO14 wakes");
    esp_deep_sleep_start();
#endif
}

static void on_btn(bsp_btn_t btn, bsp_btn_evt_t evt, void *ctx)
{
    (void)ctx;
    if (!lvgl_port_lock(0)) return;
    if (evt == BSP_BTN_SLEEP_CHORD) sleep_now();
    else if (btn == BSP_BTN_KEY && evt == BSP_BTN_PRESS) trev_input_turn(TREV_TURN_NEXT);
    else if (btn == BSP_BTN_KEY && evt == BSP_BTN_LONG) trev_input_turn(TREV_TURN_PREV);
    else if (btn == BSP_BTN_BOOT && evt == BSP_BTN_PRESS) trev_input_commit();
    else if (btn == BSP_BTN_BOOT && evt == BSP_BTN_LONG) trev_input_back();
    lvgl_port_unlock();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

#ifndef CHARACTER_SIM
    esp_sleep_wakeup_cause_t wake = esp_sleep_get_wakeup_cause();
    if (wake == ESP_SLEEP_WAKEUP_EXT1) {
        ESP_LOGW(TAG, "woke by EXT1 mask=0x%llx",
                 (unsigned long long)esp_sleep_get_ext1_wakeup_status());
    } else {
        ESP_LOGI(TAG, "wake cause=%d", wake);
    }
#endif

    esp_lcd_panel_io_handle_t io;
    ESP_ERROR_CHECK(bsp_display_init(&io, &s_panel));
    const lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&port_cfg));

    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle = io,
        .panel_handle = s_panel,
        .buffer_size = BSP_LCD_H_RES * 40,
        .double_buffer = true,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        // Character Case is installed opposite the legacy T3 orientation.
        .rotation = { .swap_xy = true, .mirror_x = true, .mirror_y = false },
        .flags = { .buff_dma = true, .swap_bytes = false },
    };
    lv_display_t *display = lvgl_port_add_disp(&display_cfg);
    (void)display;

    if (lvgl_port_lock(0)) {
        trev_init(lv_screen_active());
        trev_app_register(&CHARACTER_APP);
        trev_set_home_app(0);
        trev_open(0);
        lvgl_port_unlock();
    }

    bsp_display_backlight_on();
    bsp_buttons_init(on_btn, NULL);
    ESP_LOGI(TAG, "Character Case MVP up: 320x170 landscape, case rotation");
}
