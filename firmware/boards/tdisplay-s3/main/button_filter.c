#include "button_filter.h"
#include <string.h>

#define RELEASE_MS 100
#define PAIR_MS 100
#define LONG_MS 600
#define CHORD_MS 2000

void button_filter_init(button_filter_t *filter)
{
    memset(filter, 0, sizeof(*filter));
}

static void reset_press(button_filter_t *filter)
{
    bool ready = filter->ready;
    memset(filter, 0, sizeof(*filter));
    filter->ready = ready;
}

button_event_t button_filter_sample(button_filter_t *filter, bool boot_down,
                                    bool key_down, uint16_t elapsed_ms)
{
    if (!filter) return BUTTON_EVENT_NONE;

    if (!filter->ready) {
        if (!boot_down && !key_down) {
            filter->release_ms += elapsed_ms;
            if (filter->release_ms >= RELEASE_MS) {
                reset_press(filter);
                filter->ready = true;
            }
        } else {
            filter->release_ms = 0;
        }
        return BUTTON_EVENT_NONE;
    }

    if (filter->chord || filter->suppress) {
        if (filter->chord && boot_down && key_down && !filter->chord_ready) {
            filter->held_ms += elapsed_ms;
            if (filter->held_ms >= CHORD_MS) filter->chord_ready = true;
        }
        if (boot_down || key_down) {
            filter->release_ms = 0;
            return BUTTON_EVENT_NONE;
        }
        filter->release_ms += elapsed_ms;
        if (filter->release_ms < RELEASE_MS) return BUTTON_EVENT_NONE;
        bool sleep = filter->chord_ready;
        reset_press(filter);
        return sleep ? BUTTON_EVENT_SLEEP_CHORD : BUTTON_EVENT_NONE;
    }

    bool was_boot = filter->boot_down;
    bool was_key = filter->key_down;
    filter->boot_down = boot_down;
    filter->key_down = key_down;

    if (!was_boot && !was_key && (boot_down || key_down)) {
        filter->first_ms = elapsed_ms;
        filter->held_ms = elapsed_ms;
        if (boot_down && key_down) {
            filter->chord = true;
            filter->held_ms = elapsed_ms;
        }
        return BUTTON_EVENT_NONE;
    }

    if ((was_boot ^ was_key) && boot_down && key_down) {
        if (filter->first_ms <= PAIR_MS && !filter->long_sent) {
            filter->chord = true;
            filter->held_ms = elapsed_ms;
        } else {
            filter->suppress = true;
        }
        return BUTTON_EVENT_NONE;
    }

    if (boot_down || key_down) {
        filter->first_ms += elapsed_ms;
        filter->held_ms += elapsed_ms;
        if (!filter->long_sent && filter->held_ms >= LONG_MS) {
            filter->long_sent = true;
            return boot_down ? BUTTON_EVENT_BOOT_LONG : BUTTON_EVENT_KEY_LONG;
        }
        return BUTTON_EVENT_NONE;
    }

    if (was_boot || was_key) {
        button_event_t event = BUTTON_EVENT_NONE;
        if (!filter->long_sent)
            event = was_boot ? BUTTON_EVENT_BOOT_PRESS : BUTTON_EVENT_KEY_PRESS;
        reset_press(filter);
        return event;
    }
    return BUTTON_EVENT_NONE;
}
