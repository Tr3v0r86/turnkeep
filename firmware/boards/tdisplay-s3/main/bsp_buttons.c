// bsp_buttons.c — debounced poll of the two active-low buttons.
//
// 20 ms sample task. A release before the long threshold is a short press; holding
// past LONG_MS fires a long press once. No managed-component dependency on purpose:
// two buttons do not need iot_button, and a hand-rolled poller has no API churn.
#include "bsp_buttons.h"
#include "button_filter.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_BOOT  0
#define PIN_KEY  14
#define POLL_MS  20

static bsp_btn_cb_t s_cb;
static void *s_ctx;

static void btn_task(void *arg)
{
    (void)arg;
    button_filter_t filter;
    button_filter_init(&filter);

    for (;;) {
        button_event_t event = button_filter_sample(
            &filter,
            gpio_get_level(PIN_BOOT) == 0,
            gpio_get_level(PIN_KEY) == 0,
            POLL_MS);
        if (s_cb) {
            if (event == BUTTON_EVENT_BOOT_PRESS) s_cb(BSP_BTN_BOOT, BSP_BTN_PRESS, s_ctx);
            else if (event == BUTTON_EVENT_BOOT_LONG) s_cb(BSP_BTN_BOOT, BSP_BTN_LONG, s_ctx);
            else if (event == BUTTON_EVENT_KEY_PRESS) s_cb(BSP_BTN_KEY, BSP_BTN_PRESS, s_ctx);
            else if (event == BUTTON_EVENT_KEY_LONG) s_cb(BSP_BTN_KEY, BSP_BTN_LONG, s_ctx);
            else if (event == BUTTON_EVENT_SLEEP_CHORD) s_cb(BSP_BTN_BOOT, BSP_BTN_SLEEP_CHORD, s_ctx);
        }
        vTaskDelay(pdMS_TO_TICKS(POLL_MS));
    }
}

void bsp_buttons_init(bsp_btn_cb_t cb, void *ctx)
{
    s_cb = cb;
    s_ctx = ctx;
    gpio_config_t gc = {
        .pin_bit_mask = (1ULL << PIN_BOOT) | (1ULL << PIN_KEY),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&gc);
    xTaskCreate(btn_task, "btns", 2560, NULL, 5, NULL);
}
