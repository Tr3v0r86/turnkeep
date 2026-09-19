// trevos.c — minimal TrevOS runtime: app registry, lifecycle, input routing, tick.
//
// Deliberately bare: it owns the mechanism (mount an app on the screen, route turn /
// commit / home, pump tick), not the look. The home screen is an un-styled stub; the
// styled home and the app faces are the design agent's domain.
#include "trevos.h"
#include "trevos_theme.h"
#include "esp_log.h"
#include <string.h>

#define TREV_MAX_APPS 8

static const char *TAG = "trevos";
static const trev_app_def_t *s_defs[TREV_MAX_APPS];
static int s_count;
static lv_obj_t *s_screen;
static trev_app_t s_active;          // .def == NULL means we are on home
static int s_home_app = -1;          // a registered app to use as home, or -1 for the stub

static void clear_screen(void)
{
    if (s_active.def && s_active.def->on_stop) s_active.def->on_stop(&s_active);
    s_active.def = NULL;
    s_active.root = NULL;
    s_active.state = NULL;
    lv_obj_clean(s_screen);          // deletes every child (home stub or app root)
}

static void show_home(void)
{
    clear_screen();
    lv_obj_t *home = lv_obj_create(s_screen);
    lv_obj_remove_style_all(home);
    lv_obj_set_size(home, lv_pct(100), lv_pct(100));
    lv_obj_t *l = lv_label_create(home);
    lv_label_set_text(l, "TrevOS");
    lv_obj_set_style_text_color(l, lv_color_hex(0xF2F2F2), 0);
    lv_obj_center(l);
}

static void tick_cb(lv_timer_t *t)
{
    (void)t;
    if (s_active.def && s_active.def->on_tick) s_active.def->on_tick(&s_active, lv_tick_get());
}

void trev_init(lv_obj_t *screen)
{
    s_screen = screen;
    // Own an opaque background so transparent home / app roots do not show white.
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101418), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    s_active.def = NULL;
    s_active.root = NULL;
    show_home();
    lv_timer_create(tick_cb, 100, NULL);
    ESP_LOGI(TAG, "TrevOS up (%d apps)", s_count);
}

void trev_app_register(const trev_app_def_t *def)
{
    if (s_count < TREV_MAX_APPS && def &&
        (def->api_version == TREV_APP_API_VERSION_V1 || def->api_version == TREV_APP_API_VERSION)) {
        s_defs[s_count++] = def;
    }
}

int trev_app_count(void) { return s_count; }

const trev_app_def_t *trev_app_def(int index)
{
    return (index >= 0 && index < s_count) ? s_defs[index] : NULL;
}

void trev_set_home_app(int index) { s_home_app = (index >= 0 && index < s_count) ? index : -1; }


void trev_open(int index)
{
    if (index < 0 || index >= s_count) return;
    clear_screen();
    s_active.def = s_defs[index];
    s_active.root = lv_obj_create(s_screen);
    lv_obj_remove_style_all(s_active.root);
    lv_obj_set_size(s_active.root, lv_pct(100), lv_pct(100));
    if (s_active.def->on_start) s_active.def->on_start(&s_active, s_active.root);
    ESP_LOGI(TAG, "open %s", s_active.def->id);
}

void trev_input_turn(trev_turn_t dir)
{
    if (s_active.def && s_active.def->on_turn) s_active.def->on_turn(&s_active, dir);
}

void trev_input_commit(void)
{
    if (s_active.def && s_active.def->on_commit) s_active.def->on_commit(&s_active);
}

void trev_input_back(void)
{
    if (s_active.def && s_active.def->api_version >= TREV_APP_API_VERSION && s_active.def->on_back)
        s_active.def->on_back(&s_active);
    else trev_input_home();
}

void trev_input_home(void) { if (s_home_app >= 0) trev_open(s_home_app); else show_home(); }
