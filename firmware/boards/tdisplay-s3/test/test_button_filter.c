#include "button_filter.h"
#include <assert.h>
#include <stdio.h>

static button_event_t sample_for(button_filter_t *f, bool boot, bool key, int ms)
{
    button_event_t result = BUTTON_EVENT_NONE;
    for (int elapsed = 0; elapsed < ms; elapsed += 20) {
        button_event_t event = button_filter_sample(f, boot, key, 20);
        if (event != BUTTON_EVENT_NONE) result = event;
    }
    return result;
}

static void arm(button_filter_t *f)
{
    button_filter_init(f);
    assert(sample_for(f, false, false, 100) == BUTTON_EVENT_NONE);
}

static void short_and_long_are_distinct(void)
{
    button_filter_t f;
    arm(&f);
    assert(sample_for(&f, true, false, 100) == BUTTON_EVENT_NONE);
    assert(button_filter_sample(&f, false, false, 20) == BUTTON_EVENT_BOOT_PRESS);

    assert(sample_for(&f, false, true, 600) == BUTTON_EVENT_KEY_LONG);
    assert(button_filter_sample(&f, false, false, 20) == BUTTON_EVENT_NONE);
}

static void paired_hold_emits_only_sleep(void)
{
    button_filter_t f;
    arm(&f);
    assert(sample_for(&f, true, false, 60) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, true, true, 2000) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, false, false, 80) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, false, false, 20) == BUTTON_EVENT_SLEEP_CHORD);
}

static void late_second_press_suppresses_both_buttons(void)
{
    button_filter_t f;
    arm(&f);
    assert(sample_for(&f, true, false, 120) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, true, true, 2200) == BUTTON_EVENT_NONE);
    assert(button_filter_sample(&f, false, false, 20) == BUTTON_EVENT_NONE);
}

static void wake_hold_is_ignored_until_stable_release(void)
{
    button_filter_t f;
    button_filter_init(&f);
    assert(sample_for(&f, true, false, 1000) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, false, false, 80) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, false, false, 20) == BUTTON_EVENT_NONE);
    assert(sample_for(&f, false, true, 100) == BUTTON_EVENT_NONE);
    assert(button_filter_sample(&f, false, false, 20) == BUTTON_EVENT_KEY_PRESS);
}

int main(void)
{
    short_and_long_are_distinct();
    paired_hold_emits_only_sleep();
    late_second_press_suppresses_both_buttons();
    wake_hold_is_ignored_until_stable_release();
    puts("button filter: ok");
    return 0;
}
