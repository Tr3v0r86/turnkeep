/* Turnkeep host loop; SIM_DISP=null, SIM_TICKS=N, SIM_SHOT=out.ppm. */
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


void board_app_main(void);   /* main.c's app_main, renamed at compile time */

static uint32_t now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
}

#if LV_USE_SNAPSHOT
/* SIM_SHOT=path.ppm -> dump the active screen as a binary PPM (P6). Works headless
 * (lv_snapshot renders the object tree to its own buffer, independent of the display
 * flush). Convert to PNG with: sips -s format png path.ppm --out path.png  (macOS). */
static void sim_write_ppm(const char *path)
{
    lv_draw_buf_t *snap = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);
    if (!snap) { fprintf(stderr, "[shot] snapshot failed\n"); return; }
    int w = (int)snap->header.w, h = (int)snap->header.h;
    uint32_t stride = snap->header.stride;
    FILE *f = fopen(path, "wb");
    if (!f) { fprintf(stderr, "[shot] cannot open %s\n", path); lv_draw_buf_destroy(snap); return; }
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int y = 0; y < h; y++) {
        const uint8_t *row = snap->data + (size_t)y * stride;
        for (int x = 0; x < w; x++) {
            const uint8_t *p = row + (size_t)x * 4;   /* ARGB8888 little-endian = B,G,R,A */
            fputc(p[2], f); fputc(p[1], f); fputc(p[0], f);
        }
    }
    fclose(f);
    lv_draw_buf_destroy(snap);
    fprintf(stderr, "[shot] wrote %s (%dx%d ppm)\n", path, w, h);
}
#endif

int main(void)
{
    fprintf(stderr, "[sim] lv_init\n");
    lv_init();

    fprintf(stderr, "[sim] board_app_main (device boot order)\n");
    board_app_main();


    const char *cap = getenv("SIM_TICKS");
    int ticks = cap ? atoi(cap) : -1;
    fprintf(stderr, "[sim] LVGL loop start (ticks=%s)\n", cap ? cap : "inf");

    uint32_t last = now_ms();
    while (ticks != 0) {
        lvgl_port_lock(0);
        lv_timer_handler();
        lvgl_port_unlock();

        usleep(5000);
        uint32_t t = now_ms();
        lv_tick_inc(t - last);
        last = t;
        if (ticks > 0) ticks--;
    }

#if LV_USE_SNAPSHOT
    {
        const char *shot = getenv("SIM_SHOT");
        if (shot) { lvgl_port_lock(0); sim_write_ppm(shot); lvgl_port_unlock(); }
    }
#endif

    fprintf(stderr, "[sim] clean exit\n");
    return 0;
}
