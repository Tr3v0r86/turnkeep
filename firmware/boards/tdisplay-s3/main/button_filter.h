#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_BOOT_PRESS,
    BUTTON_EVENT_BOOT_LONG,
    BUTTON_EVENT_KEY_PRESS,
    BUTTON_EVENT_KEY_LONG,
    BUTTON_EVENT_SLEEP_CHORD,
} button_event_t;

typedef struct {
    bool ready;
    bool boot_down;
    bool key_down;
    bool chord;
    bool chord_ready;
    bool suppress;
    bool long_sent;
    uint16_t release_ms;
    uint16_t first_ms;
    uint16_t held_ms;
} button_filter_t;

void button_filter_init(button_filter_t *filter);
button_event_t button_filter_sample(button_filter_t *filter, bool boot_down,
                                    bool key_down, uint16_t elapsed_ms);
