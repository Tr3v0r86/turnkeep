#include "character_app.h"
#include "character_core.h"
#include "character_pack_generated.h"
#include "trevos_theme.h"
#include "nvs.h"
#include <stdio.h>

#define C_NIGHT  lv_color_hex(0x111C2E)
#define C_DEEP   lv_color_hex(0x1B2B42)
#define C_BONE   lv_color_hex(0xF0E4C2)
#define C_BRASS  lv_color_hex(0xD5A94E)
#define C_BLOOD  lv_color_hex(0xA83D3A)
#define C_ARCANE lv_color_hex(0x55B8C6)
#define C_SMOKE  lv_color_hex(0xA8B4C2)

typedef enum {
    VIEW_HOME,
    VIEW_HEALTH,
    VIEW_ACTIONS,
    VIEW_SPELLS,
    VIEW_CHECKS,
    VIEW_RESOURCES,
    VIEW_SHEET,
    VIEW_DETAIL,
    VIEW_EDIT_DAMAGE,
    VIEW_EDIT_HEAL,
    VIEW_EDIT_TEMP,
} view_t;

typedef character_pack_entry_t entry_t;

static const char *HOME[] = { "Health", "Actions", "Spells", "Checks", "Resources", "Sheet" };
static const entry_t HEALTH[] = {
    { "Damage", "Subtract HP; temp HP is consumed first" },
    { "Heal", "Restore HP up to maximum" },
    { "Temp HP", "Replace the current temporary HP" },
    { "Undo", "Undo the last saved change" },
};
static const entry_t RESOURCES[] = {
    { "Use spell slot", "Spend one level-1 slot" },
#if CHARACTER_PACK_RESOURCE_MAX > 0
    { "Use " CHARACTER_PACK_RESOURCE_NAME, "Spend one " CHARACTER_PACK_RESOURCE_NAME " use" },
#endif
    { "Long rest", "Restore HP, slots and class resource" },
    { "Undo", "Undo the last saved change" },
};

static character_core_t s_core;
static bool s_loaded;
static bool s_save_error;
static view_t s_view = VIEW_HOME;
static view_t s_detail_parent;
static int s_cursor;
static int s_amount = 1;
static const entry_t *s_detail;
static lv_obj_t *s_root;

static bool persist(void)
{
    nvs_handle_t h;
    if (nvs_open("character", NVS_READWRITE, &h) != ESP_OK) return false;
    const character_state_t *s = &s_core.state;
    bool ok = nvs_set_u32(h, "pack", CHARACTER_PACK_GENERATION) == ESP_OK &&
              nvs_set_i32(h, "hp", s->hp) == ESP_OK &&
              nvs_set_i32(h, "temp", s->temp_hp) == ESP_OK &&
              nvs_set_u8(h, "slots", s->spell_slots) == ESP_OK &&
              nvs_set_u8(h, "resource", s->resource) == ESP_OK &&
              nvs_commit(h) == ESP_OK;
    nvs_close(h);
    return ok;
}

static void load(void)
{
    if (s_loaded) return;
    character_state_t initial = {
        .hp = CHARACTER_PACK_MAX_HP, .hp_max = CHARACTER_PACK_MAX_HP, .temp_hp = 0,
        .spell_slots = CHARACTER_PACK_SLOTS_MAX,
        .spell_slots_max = CHARACTER_PACK_SLOTS_MAX,
        .resource = CHARACTER_PACK_RESOURCE_MAX,
        .resource_max = CHARACTER_PACK_RESOURCE_MAX,
    };
    nvs_handle_t h;
    if (nvs_open("character", NVS_READONLY, &h) == ESP_OK) {
        int32_t hp, temp;
        uint8_t slots, resource;
        uint32_t pack;
        if (nvs_get_u32(h, "pack", &pack) == ESP_OK && pack == CHARACTER_PACK_GENERATION) {
            if (nvs_get_i32(h, "hp", &hp) == ESP_OK && hp >= 0 && hp <= initial.hp_max)
                initial.hp = (int16_t)hp;
            if (nvs_get_i32(h, "temp", &temp) == ESP_OK && temp >= 0 && temp <= 9999)
                initial.temp_hp = (int16_t)temp;
            if (nvs_get_u8(h, "slots", &slots) == ESP_OK && slots <= initial.spell_slots_max)
                initial.spell_slots = slots;
            if (nvs_get_u8(h, "resource", &resource) == ESP_OK && resource <= initial.resource_max)
                initial.resource = resource;
        }
        nvs_close(h);
    }
    character_core_init(&s_core, &initial);
    s_loaded = true;
}

static lv_obj_t *plain(lv_obj_t *parent)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    return o;
}

static lv_obj_t *label(lv_obj_t *parent, const char *text, const lv_font_t *font,
                       lv_color_t color)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, color, 0);
    return l;
}

static lv_obj_t *block(lv_obj_t *parent, lv_color_t color)
{
    lv_obj_t *o = plain(parent);
    lv_obj_set_style_bg_color(o, color, 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    return o;
}

static void frame(void)
{
    lv_obj_set_style_bg_color(s_root, C_NIGHT, 0);
    lv_obj_set_style_bg_opa(s_root, LV_OPA_COVER, 0);
    lv_obj_t *spine = block(s_root, C_BRASS);
    lv_obj_set_size(spine, 4, LV_PCT(100));
    lv_obj_align(spine, LV_ALIGN_LEFT_MID, 0, 0);
}

static void titlebar(const char *title, int page, int count)
{
    lv_obj_t *l = label(s_root, title, &plex_sans_b_16, C_BONE);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 12, 6);

    char pos[32];
    snprintf(pos, sizeof(pos), "%d / %d", page + 1, count);
    l = label(s_root, pos, &plex_mono_sb_11, C_BRASS);
    lv_obj_align(l, LV_ALIGN_TOP_RIGHT, -10, 8);

    if (s_save_error) {
        l = label(s_root, "NOT SAVED", &plex_sans_b_12, C_BLOOD);
        lv_obj_align(l, LV_ALIGN_TOP_MID, 0, 8);
    }
}

static void footer(const char *key, const char *boot)
{
    lv_obj_t *band = block(s_root, C_DEEP);
    lv_obj_set_size(band, 316, 30);
    lv_obj_align(band, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_t *l = label(band, key, &plex_sans_b_12, C_SMOKE);
    lv_obj_align(l, LV_ALIGN_LEFT_MID, 9, 0);
    l = label(band, boot, &plex_sans_b_12, C_BONE);
    lv_obj_align(l, LV_ALIGN_RIGHT_MID, -9, 0);
}

static void render_home(void)
{
    lv_obj_t *l = label(s_root, CHARACTER_PACK_NAME, &plex_sans_b_16, C_BONE);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 12, 5);
    char identity[96];
    snprintf(identity, sizeof(identity), "%s  /  %s %d", CHARACTER_PACK_ANCESTRY,
             CHARACTER_PACK_CLASS, CHARACTER_PACK_LEVEL);
    l = label(s_root, identity, &plex_mono_sb_10, C_ARCANE);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 12, 25);

    char hp[8];
    snprintf(hp, sizeof(hp), "%d", s_core.state.hp);
    l = label(s_root, hp, &plex_mono_b_72, C_BONE);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 8, 36);
    char max_hp[12];
    snprintf(max_hp, sizeof(max_hp), "of %d", s_core.state.hp_max);
    l = label(s_root, max_hp, &plex_sans_b_22, C_SMOKE);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 95, 78);
    l = label(s_root, s_core.state.temp_hp ? "HIT POINTS + TEMP" : "HIT POINTS",
              &plex_sans_b_12, s_core.state.temp_hp ? C_ARCANE : C_SMOKE);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 13, 112);

    char stat[64];
    snprintf(stat, sizeof(stat), "AC  %d", CHARACTER_PACK_AC);
    l = label(s_root, stat, &plex_sans_b_28, C_BRASS);
    lv_obj_align(l, LV_ALIGN_TOP_RIGHT, -10, 42);
    snprintf(stat, sizeof(stat), "SLOTS  %u / %u", s_core.state.spell_slots,
             s_core.state.spell_slots_max);
    l = label(s_root, stat, &plex_sans_b_16, C_BONE);
    lv_obj_align(l, LV_ALIGN_TOP_RIGHT, -11, 79);
#if CHARACTER_PACK_RESOURCE_MAX > 0
    snprintf(stat, sizeof(stat), "%s  %u / %u", CHARACTER_PACK_RESOURCE_NAME,
             s_core.state.resource, s_core.state.resource_max);
    l = label(s_root, stat, &plex_sans_b_16, C_BONE);
    lv_obj_align(l, LV_ALIGN_TOP_RIGHT, -11, 101);
#endif

    lv_obj_t *nav = block(s_root, C_DEEP);
    lv_obj_set_size(nav, 316, 34);
    lv_obj_align(nav, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    l = label(nav, HOME[s_cursor], &plex_sans_b_22, C_BONE);
    lv_obj_align(l, LV_ALIGN_LEFT_MID, 9, -1);
    l = label(nav, "KEY next", &plex_sans_b_12, C_SMOKE);
    lv_obj_align(l, LV_ALIGN_RIGHT_MID, -132, 0);
    l = label(nav, "BOOT open", &plex_sans_b_12, C_BRASS);
    lv_obj_align(l, LV_ALIGN_RIGHT_MID, -8, 0);
}

static void render_list(const char *title, const entry_t *entries, int count)
{
    if (count == 0) {
        titlebar(title, -1, 0);
        lv_obj_t *empty = label(s_root, "Nothing here", &plex_sans_b_28, C_SMOKE);
        lv_obj_align(empty, LV_ALIGN_TOP_LEFT, 12, 50);
        footer("", "BOOT hold  back");
        return;
    }
    titlebar(title, s_cursor, count);
    lv_obj_t *l = label(s_root, entries[s_cursor].name, &plex_sans_b_28, C_BONE);
    lv_obj_set_width(l, 294);
    lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 12, 37);

    l = label(s_root, entries[s_cursor].detail, &plex_sans_r_14, C_SMOKE);
    lv_obj_set_width(l, 292);
    lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_line_space(l, 3, 0);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 13, 76);
    footer("KEY  next / hold previous", "BOOT  open");
}

static void render_detail(void)
{
    titlebar("DETAIL", 0, 1);
    lv_obj_t *l = label(s_root, s_detail ? s_detail->name : "", &plex_sans_b_28, C_BRASS);
    lv_obj_set_width(l, 294);
    lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 12, 37);
    l = label(s_root, s_detail ? s_detail->detail : "", &plex_sans_r_14, C_BONE);
    lv_obj_set_width(l, 292);
    lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_line_space(l, 4, 0);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 13, 77);
    footer("", "BOOT hold  back");
}

static void render_editor(void)
{
    const char *verb = s_view == VIEW_EDIT_DAMAGE ? "DAMAGE" :
                       s_view == VIEW_EDIT_HEAL ? "HEAL" : "TEMP HP";
    lv_color_t accent = s_view == VIEW_EDIT_DAMAGE ? C_BLOOD :
                        s_view == VIEW_EDIT_HEAL ? C_ARCANE : C_BRASS;
    lv_obj_t *l = label(s_root, verb, &plex_sans_b_22, accent);
    lv_obj_align(l, LV_ALIGN_TOP_LEFT, 13, 8);
    char amount[12];
    snprintf(amount, sizeof(amount), "%d", s_amount);
    l = label(s_root, amount, &plex_mono_b_72, C_BONE);
    lv_obj_align(l, LV_ALIGN_CENTER, 0, -4);
    footer("KEY  +1 / hold -1", "BOOT  apply");
}

static void render(void)
{
    if (!s_root) return;
    lv_obj_clean(s_root);
    frame();
    if (s_view == VIEW_HOME) render_home();
    else if (s_view == VIEW_HEALTH) render_list("HEALTH", HEALTH, 4);
    else if (s_view == VIEW_ACTIONS) render_list("ACTIONS", CHARACTER_PACK_ACTIONS,
                                                  CHARACTER_PACK_ACTIONS_COUNT);
    else if (s_view == VIEW_SPELLS) render_list("SPELLS", CHARACTER_PACK_SPELLS,
                                                 CHARACTER_PACK_SPELLS_COUNT);
    else if (s_view == VIEW_CHECKS) render_list("CHECKS", CHARACTER_PACK_CHECKS,
                                                 CHARACTER_PACK_CHECKS_COUNT);
    else if (s_view == VIEW_RESOURCES)
        render_list("RESOURCES", RESOURCES, sizeof(RESOURCES) / sizeof(RESOURCES[0]));
    else if (s_view == VIEW_SHEET) render_list("SHEET", CHARACTER_PACK_SHEET,
                                                CHARACTER_PACK_SHEET_COUNT);
    else if (s_view == VIEW_DETAIL) render_detail();
    else render_editor();
}

static int view_count(view_t view)
{
    if (view == VIEW_HOME) return 6;
    if (view == VIEW_HEALTH) return 4;
    if (view == VIEW_RESOURCES) return sizeof(RESOURCES) / sizeof(RESOURCES[0]);
    if (view == VIEW_ACTIONS) return CHARACTER_PACK_ACTIONS_COUNT;
    if (view == VIEW_SPELLS) return CHARACTER_PACK_SPELLS_COUNT;
    if (view == VIEW_CHECKS) return CHARACTER_PACK_CHECKS_COUNT;
    if (view == VIEW_SHEET) return CHARACTER_PACK_SHEET_COUNT;
    return 0;
}

static void mutate(character_result_t result, character_state_t before, bool had_undo)
{
    if (result != CHARACTER_OK) return;
    if (!persist()) {
        s_core.state = before;
        s_core.can_undo = had_undo;
        s_save_error = true;
    } else {
        s_save_error = false;
    }
}

static void on_start(trev_app_t *app, lv_obj_t *root)
{
    (void)app;
    load();
    s_root = root;
    s_view = VIEW_HOME;
    s_cursor = 0;
    render();
}

static void on_stop(trev_app_t *app) { (void)app; s_root = NULL; }

static void on_turn(trev_app_t *app, trev_turn_t dir)
{
    (void)app;
    if (s_view == VIEW_DETAIL) return;
    if (s_view >= VIEW_EDIT_DAMAGE) {
        s_amount += dir == TREV_TURN_NEXT ? 1 : -1;
        if (s_amount < 0) s_amount = 99;
        if (s_amount > 99) s_amount = 0;
    } else {
        int count = view_count(s_view);
        if (count == 0) return;
        s_cursor = (s_cursor + (dir == TREV_TURN_NEXT ? 1 : count - 1)) % count;
    }
    render();
}

static const entry_t *entries_for(view_t view)
{
    if (view == VIEW_ACTIONS) return CHARACTER_PACK_ACTIONS;
    if (view == VIEW_SPELLS) return CHARACTER_PACK_SPELLS;
    if (view == VIEW_CHECKS) return CHARACTER_PACK_CHECKS;
    if (view == VIEW_SHEET) return CHARACTER_PACK_SHEET;
    return NULL;
}

static void on_commit(trev_app_t *app)
{
    (void)app;
    if (s_view == VIEW_HOME) {
        static const view_t destinations[] = {
            VIEW_HEALTH, VIEW_ACTIONS, VIEW_SPELLS, VIEW_CHECKS, VIEW_RESOURCES, VIEW_SHEET
        };
        s_view = destinations[s_cursor];
        s_cursor = 0;
    } else if (s_view == VIEW_HEALTH) {
        if (s_cursor < 3) {
            s_view = (view_t)(VIEW_EDIT_DAMAGE + s_cursor);
            s_amount = 1;
        } else {
            character_state_t before = s_core.state;
            bool had_undo = s_core.can_undo;
            mutate(character_undo(&s_core), before, had_undo);
        }
    } else if (s_view == VIEW_RESOURCES) {
        character_state_t before = s_core.state;
        bool had_undo = s_core.can_undo;
#if CHARACTER_PACK_RESOURCE_MAX > 0
        character_result_t result = s_cursor == 0 ? character_use_slot(&s_core) :
                                    s_cursor == 1 ? character_use_resource(&s_core) :
                                    s_cursor == 2 ? character_long_rest(&s_core) :
                                                    character_undo(&s_core);
#else
        character_result_t result = s_cursor == 0 ? character_use_slot(&s_core) :
                                    s_cursor == 1 ? character_long_rest(&s_core) :
                                                    character_undo(&s_core);
#endif
        mutate(result, before, had_undo);
    } else if (s_view >= VIEW_EDIT_DAMAGE) {
        character_state_t before = s_core.state;
        bool had_undo = s_core.can_undo;
        character_result_t result = s_view == VIEW_EDIT_DAMAGE ? character_damage(&s_core, s_amount) :
                                    s_view == VIEW_EDIT_HEAL ? character_heal(&s_core, s_amount) :
                                    character_set_temp_hp(&s_core, s_amount);
        mutate(result, before, had_undo);
        s_view = VIEW_HOME;
        s_cursor = 0;
    } else if (s_view != VIEW_DETAIL) {
        const entry_t *entries = entries_for(s_view);
        if (entries && view_count(s_view) > 0) {
            s_detail_parent = s_view;
            s_detail = &entries[s_cursor];
            s_view = VIEW_DETAIL;
        }
    }
    render();
}

static void on_back(trev_app_t *app)
{
    (void)app;
    if (s_view == VIEW_HOME) return;
    if (s_view == VIEW_DETAIL) s_view = s_detail_parent;
    else if (s_view >= VIEW_EDIT_DAMAGE) s_view = VIEW_HEALTH;
    else s_view = VIEW_HOME;
    s_cursor = 0;
    render();
}

bool character_app_prepare_sleep(void)
{
    load();
    s_save_error = !persist();
    if (s_root) render();
    return !s_save_error;
}

const trev_app_def_t CHARACTER_APP = {
    .api_version = TREV_APP_API_VERSION,
    .id = "character",
    .name = "Character",
    .on_start = on_start,
    .on_stop = on_stop,
    .on_turn = on_turn,
    .on_commit = on_commit,
    .on_back = on_back,
};
