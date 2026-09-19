// trevos_ui.c — TrevOS shared UI kit implementation. See trevos_ui.h.
#include "trevos_ui.h"

// lv_color_hex() is not a constant expression, so the skins can't be file-scope
// const initialisers; build them at first use so the token macros stay the source.
static bool s_skins_ready;
static tt_skin_t s_paper, s_carbon;
static void skins_init(void)
{
    if (s_skins_ready) return;
    // btn_bg/btn_fg: the primary pill's fill + text (tt_pill kind 0). Carbon's pill has no
    // real fill (bg == its own ground, so the "filled" primary reads as transparent) and
    // amber text - the break face's only pill is the mid one, so carbon's secondary/outline
    // look never actually renders today.
    s_paper  = (tt_skin_t){ .ground = TT_PAPER,  .ink = TT_INK,       .muted = TT_MUTED,
                            .rule = TT_RULE,      .accent = TT_SLATE,
                            .btn_bg = TT_INK,     .btn_fg = TT_PAPER,
                            .btn_line = TT_RULE,  .btn_fg2 = TT_MUTED };
    s_carbon = (tt_skin_t){ .ground = TT_CARBON, .ink = TT_CARBON_TX, .muted = TT_CARBON_MU,
                            .rule = lv_color_hex(0x2A2724), .accent = TT_AMBER,
                            .btn_bg = TT_CARBON,  .btn_fg = TT_AMBER,
                            .btn_line = lv_color_hex(0x2A2724), .btn_fg2 = TT_CARBON_MU };
    s_skins_ready = true;
}
const tt_skin_t *tt_skin_paper(void)  { skins_init(); return &s_paper; }
const tt_skin_t *tt_skin_carbon(void) { skins_init(); return &s_carbon; }

static lv_obj_t *plain_box(lv_obj_t *parent)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
    return o;
}

#ifndef TT_ROUND_DISPLAY
#define TT_ROUND_DISPLAY 0
#endif

#if TT_ROUND_DISPLAY
#include <math.h>
// Round glass (ADR-0012). A bar that spans the full width flush to the rim is mostly NOT on
// the device: the chord at the very top of a circle has zero width, so the app name, the
// clock and the outer two action labels all render in the square sim and then vanish behind
// the bezel. Measured on the first roundsim shots, before this existed: 13.8% of the focus
// face's ink fell outside the circle, including every status-bar pixel and two of the three
// action labels, whose tap zones stayed live while their labels were invisible.
//
// The fix lives in the shared kit, not in a board, because every face calls these two helpers
// and a per-board copy would have to be written once per round board (thesis.md).
//
// Drop the bar away from the rim by TT_ROUND_INSET, then clamp its width to the chord at its
// OUTERMOST row, which is the narrowest row it occupies. Every pixel it draws is then on glass.
// TT_ROUND_INSET is the tuning knob: larger gives a wider bar and eats more vertical space.
#ifndef TT_ROUND_INSET
#define TT_ROUND_INSET 40
#endif

// tt_actionbar's round branch only (glass fixes round 2, Fix 4): how far a flank pill's
// drawing offset moves in from its zone's exact third-centre so its outer cap clears the
// rim ring instead of sitting on top of it. Tuning knob, not a hit-test boundary - see the
// call site.
#ifndef TT_PILL_RING_CLEAR
#define TT_PILL_RING_CLEAR 8
#endif

// Width of the glass `inset` px in from the top (or, symmetrically, the bottom) of the circle.
// Chord available to a bar dropped `inset` from the rim.
//
// Measured against the radius INSIDE the rim ring, not against the glass. The ring owns the
// outermost band, and a bar clamped to the glass chord is wider than the ring at its own
// height, so its ends cross straight through it. Subtracting the ring allowance once, here,
// keeps every chord-clamped thing (both bars AND tt_zone_at) inside the ring by construction
// rather than by four separate hand-tuned numbers.
static int tt_round_chord_w(int inset)
{
    lv_display_t *d = lv_display_get_default();
    int w = d ? (int)lv_display_get_horizontal_resolution(d) : 360;
    int h = d ? (int)lv_display_get_vertical_resolution(d) : 360;
    int Rg = (w < h ? w : h) / 2;      // the glass
    int Rs = Rg - TT_ROUND_RING;       // inside the ring
    int k = Rg - inset;                // vertical distance from the centre to that row
    if (k < 0) k = 0;
    if (k >= Rs) return 0;             // that row is entirely under the ring
    return 2 * (int)sqrt((double)(Rs * Rs - k * k));
}
#endif

lv_obj_t *tt_face_ground(lv_obj_t *root, const tt_skin_t *skin)
{
    lv_obj_set_style_bg_color(root, skin->ground, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    return root;
}

lv_obj_t *tt_label(lv_obj_t *parent, const char *txt, const lv_font_t *font, lv_color_t col, int track)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, col, 0);
    if (track) lv_obj_set_style_text_letter_space(l, track, 0);
    return l;
}

lv_obj_t *tt_eyebrow(lv_obj_t *parent, const char *txt, lv_color_t col)
{
    return tt_label(parent, txt, TT_F_LABEL, col, TT_TRACK_WIDE);
}

lv_obj_t *tt_title(lv_obj_t *parent, const char *txt, lv_color_t col)
{
    lv_obj_t *l = tt_label(parent, txt, TT_F_TITLE, col, 0);
    lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
    return l;
}

lv_obj_t *tt_numerals(lv_obj_t *parent, lv_color_t col)
{
    return tt_label(parent, "00:00", TT_F_NUM, col, 0);
}

lv_obj_t *tt_statusbar(lv_obj_t *root, const char *app, const tt_skin_t *skin, lv_obj_t **clock_out)
{
    lv_obj_t *bar = plain_box(root);
#if TT_ROUND_DISPLAY
    lv_obj_set_size(bar, tt_round_chord_w(TT_ROUND_INSET), TT_BAR_STATUS);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, TT_ROUND_INSET);
#else
    lv_obj_set_size(bar, LV_PCT(100), TT_BAR_STATUS);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
#endif

    lv_obj_t *name = tt_label(bar, app, TT_F_STATUS, skin->muted, TT_TRACK_WIDE);
    lv_obj_align(name, LV_ALIGN_LEFT_MID, TT_PAD, 0);

    lv_obj_t *clk = tt_label(bar, "--:--", TT_F_STATUS, skin->muted, 0);
    lv_obj_align(clk, LV_ALIGN_RIGHT_MID, -TT_PAD, 0);
    if (clock_out) *clock_out = clk;
    return bar;
}

lv_obj_t *tt_hintbar(lv_obj_t *root, const char *key_verb, const char *boot_verb, const tt_skin_t *skin)
{
    lv_obj_t *bar = plain_box(root);
#if TT_ROUND_DISPLAY
    lv_obj_set_size(bar, tt_round_chord_w(TT_ROUND_INSET), TT_BAR_HINT);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -TT_ROUND_INSET);
#else
    lv_obj_set_size(bar, LV_PCT(100), TT_BAR_HINT);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, 0);
#endif
    lv_obj_set_style_border_color(bar, skin->rule, 0);   // hairline rule above the hint bar
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_opa(bar, LV_OPA_70, 0);

    char buf[96];
    lv_snprintf(buf, sizeof(buf), "KEY %s %s    |    BOOT %s %s",
                TT_GLYPH_CHEVRON, key_verb, TT_GLYPH_BULLET, boot_verb);
    lv_obj_t *l = tt_label(bar, buf, TT_F_LABEL, skin->muted, TT_TRACK);
    lv_obj_align(l, LV_ALIGN_CENTER, 0, 0);
    return bar;
}

// Touch action bar (design task D1). On a board whose only input is a three-zone tap layer,
// naming "KEY" and "BOOT" describes hardware that is not there — and centring one string
// hides WHERE to press. This lays the three verbs over their own zones instead: each label
// is a third of the bar wide with centred text, so its midpoint sits on the midpoint of the
// tap zone that fires it (thirds of H_RES, same split as bsp_touch.c's apply_tap).
// Pass NULL or "" for a zone with no action. Rectangular glass creates a fixed left/mid/right
// label triple (children 0..2, empty zones included, so index == zone). Round glass instead
// skips any empty zone entirely and creates one pill container per non-empty verb, so a
// child's index tracks how many earlier zones were filled, not which zone it is; there is no
// re-label-by-index API on either branch, callers pass the final text at creation time.
void tt_actionbar_geom(int *x0, int *w)
{
    lv_display_t *d = lv_display_get_default();
    int dw = d ? (int)lv_display_get_horizontal_resolution(d) : 240;
#if TT_ROUND_DISPLAY
    int bw = tt_round_chord_w(TT_ROUND_INSET);
#else
    int bw = dw;                       // rectangular glass: the bar spans the whole width
#endif
    if (bw > dw) bw = dw;
    if (bw < 1)  bw = 1;
    if (x0) *x0 = (dw - bw) / 2;       // the bar is always centred
    if (w)  *w  = bw;
}

int tt_zone_at(int x)
{
    int x0, w;
    tt_actionbar_geom(&x0, &w);
    if (x <  x0 + w / 3)      return -1;
    if (x >= x0 + 2 * w / 3)  return  1;
    return 0;
}

lv_obj_t *tt_actionbar(lv_obj_t *root, const char *left, const char *mid, const char *right,
                       const tt_skin_t *skin)
{
    lv_obj_t *bar = plain_box(root);
#if TT_ROUND_DISPLAY
    int bw; tt_actionbar_geom(NULL, &bw);   // one source of truth: tt_zone_at hit-tests these
    lv_obj_set_size(bar, bw, TT_BAR_HINT);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -TT_ROUND_INSET);
    // No top rule here: a drawn pill is the affordance now, where the bare label needed the
    // hairline above it to read as "buttons live here".

    int third = bw / 3;
    const char *txt[3] = { left, mid, right };
    for (int i = 0; i < 3; i++) {
        if (!txt[i] || !txt[i][0]) continue;   // no pill drawn for an unused zone
        // Mid is the primary action (filled skin->btn_bg/btn_fg); flanks are secondary
        // (outline skin->btn_line, skin->btn_fg2 text) - each skin owns its own secondary
        // look now instead of the flanks borrowing skin->rule/skin->muted.
        int kind = (i == 1) ? 0 : 1;
        lv_color_t fg = (i == 1) ? skin->btn_fg : skin->btn_fg2;
        lv_obj_t *p = tt_pill(bar, txt[i], skin->btn_bg, fg, skin->btn_line, true, kind);
        // A pill is drawn ON its zone; tt_zone_at() still hit-tests the full third, so the
        // firing area is unchanged even though the visible pill hugs its text.
        int off = -bw / 2 + i * third + third / 2;
        // Flank pills only, drawing offset only (tt_zone_at/tt_actionbar_geom untouched): at
        // the exact third-centre a flank pill's outer cap lands on top of the rim ring
        // (measured ~1px overlap on the focus/picker/padlano shots - break draws its own
        // SKIP pill directly and never goes through here). Pull each flank a few px toward
        // the mid pill so its cap clears the ring with visible daylight.
        if (kind == 1) off += (i == 0) ? TT_PILL_RING_CLEAR : -TT_PILL_RING_CLEAR;
        lv_obj_align(p, LV_ALIGN_CENTER, off, 0);
    }
#else
    lv_obj_set_size(bar, LV_PCT(100), TT_BAR_HINT);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_border_color(bar, skin->rule, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_opa(bar, LV_OPA_70, 0);

    static const lv_align_t AT[3] = { LV_ALIGN_LEFT_MID, LV_ALIGN_CENTER, LV_ALIGN_RIGHT_MID };
    const char *txt[3] = { left, mid, right };
    for (int i = 0; i < 3; i++) {
        // The centre verb is the primary action, so it carries ink; the flanks are muted.
        lv_obj_t *l = tt_label(bar, txt[i] ? txt[i] : "", TT_F_LABEL,
                               i == 1 ? skin->ink : skin->muted, TT_TRACK);
        lv_obj_set_width(l, LV_PCT(33));
        lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(l, AT[i], 0, 0);
    }
#endif
    return bar;
}

lv_obj_t *tt_chip(lv_obj_t *parent, const char *txt, lv_color_t bg, lv_color_t fg)
{
    lv_obj_t *chip = plain_box(parent);
    lv_obj_set_size(chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(chip, TT_R_PILL, 0);     // pill (CSS border-radius 999)
    lv_obj_set_style_bg_color(chip, bg, 0);
    lv_obj_set_style_bg_opa(chip, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(chip, 5, 0);            // CSS padding 3px 9px ÷2
    lv_obj_set_style_pad_ver(chip, 1, 0);
    lv_obj_t *l = tt_label(chip, txt, TT_F_LABEL, fg, 0);
    lv_obj_center(l);
    return chip;
}

// Pill anatomy (design task): radius 999 (full). Standard pad top/hor/bottom 11/18/10,
// small 8/6/7 - asymmetric top/bottom is the optical nudge for the font's baseline, so the
// content-sized height comes out ~38 std / ~30 small without a separate height override.
// Small pill hor pad trimmed 12 -> 6 and tracking dropped to 0 (final review finding 2):
// three small pills on the round actionbar's ~59px thirds were fusing/clipping at the old
// 12px pad + wide tracking on a 6-char word like "LENGTH". Std pill (home face) untouched.
lv_obj_t *tt_pill(lv_obj_t *parent, const char *txt, lv_color_t bg, lv_color_t fg,
                  lv_color_t border, bool small, int kind)
{
    lv_obj_t *pill = plain_box(parent);
    lv_obj_set_size(pill, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(pill, TT_R_PILL, 0);
    if (kind == 0) {
        lv_obj_set_style_bg_color(pill, bg, 0);
        lv_obj_set_style_bg_opa(pill, LV_OPA_COVER, 0);
    } else {
        lv_obj_set_style_bg_opa(pill, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_color(pill, border, 0);
        lv_obj_set_style_border_width(pill, 2, 0);
        lv_obj_set_style_border_opa(pill, LV_OPA_COVER, 0);
    }
    if (small) {
        lv_obj_set_style_pad_top(pill, 8, 0);
        lv_obj_set_style_pad_bottom(pill, 7, 0);
        lv_obj_set_style_pad_hor(pill, 6, 0);
    } else {
        lv_obj_set_style_pad_top(pill, 11, 0);
        lv_obj_set_style_pad_bottom(pill, 10, 0);
        lv_obj_set_style_pad_hor(pill, 18, 0);
    }
    lv_obj_t *l = tt_label(pill, txt, small ? TT_F_LABEL_SM : TT_F_LABEL, fg,
                          small ? 0 : TT_TRACK_WIDE);
    lv_obj_center(l);
    return pill;
}

lv_obj_t *tt_dot(lv_obj_t *parent, lv_color_t col, bool filled, int d)
{
    lv_obj_t *dot = plain_box(parent);
    lv_obj_set_size(dot, d, d);
    lv_obj_set_style_radius(dot, TT_R_PILL, 0);
    if (filled) {
        lv_obj_set_style_bg_color(dot, col, 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    } else {
        lv_obj_set_style_border_color(dot, col, 0);
        lv_obj_set_style_border_width(dot, 1, 0);
        lv_obj_set_style_border_opa(dot, LV_OPA_COVER, 0);
    }
    return dot;
}

lv_obj_t *tt_descbox(lv_obj_t *parent, const char *txt, int w, const tt_skin_t *skin, lv_color_t txt_col)
{
    lv_obj_t *box = plain_box(parent);
    lv_obj_set_size(box, w, LV_SIZE_CONTENT);
    lv_obj_set_style_border_color(box, skin->rule, 0);    // left rule (CSS #E0DBCE solid)
    lv_obj_set_style_border_width(box, 1, 0);             // CSS 2px ÷2
    lv_obj_set_style_border_side(box, LV_BORDER_SIDE_LEFT, 0);
    lv_obj_set_style_border_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(box, 6, 0);                 // CSS 11px ÷2
    lv_obj_t *l = tt_label(box, txt, TT_F_BODY, txt_col, 0);
    lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(l, w - 6);
    return box;
}

static void arc_exec(void *var, int32_t v) { lv_arc_set_value((lv_obj_t *)var, v); }

void tt_arc_anim(lv_obj_t *arc, int32_t to, uint32_t ms)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_values(&a, lv_arc_get_value(arc), to);
    lv_anim_set_time(&a, ms);
    lv_anim_set_exec_cb(&a, arc_exec);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

// ---- generic micro-interaction tweens (Phase C) -------------------------------------
// The exec callbacks below all take the target object as `var`, so each running anim is
// torn down by LVGL's object destructor (lv_obj.c calls lv_anim_delete(obj, NULL) for
// every var==obj anim) when set_view()/lv_obj_clean()/on_stop() frees the widget. That is
// the whole safety story: no scale/opa/translate callback can fire on a freed object.

// TT_NO_LAYER_FX: boards that cannot afford a composite layer set this to 1 (issue #36).
//
// `transform_scale_*` and whole-object `opa` both make LVGL render the object into an
// off-screen layer of obj_w * obj_h * 2 bytes, allocated out of the LVGL pool. When that
// allocation fails LVGL only warns "Allocating layer buffer failed. Try later" and returns
// LV_DRAW_UNIT_IDLE — but lv_refr.c's draw_buf_flush loops `while(layer->draw_task_head)`,
// so a task that can never allocate is retried forever, taskLVGL never yields, and the task
// watchdog kills the UI. That is the CYD freeze (#36), and #3 on the T3 before it. The T3
// escaped via CONFIG_LV_USE_CLIB_MALLOC into PSRAM; the CYD has no PSRAM and a 64 KB pool
// that already holds the widget tree, so it cannot.
//
// Every tween guarded below settles at the object's RESTING state (scale 1.0x, opa COVER),
// which is also its default — so skipping the tween is a visual no-op, not a missing end
// state. tt_anim_translate and tt_arc_anim are deliberately NOT guarded: neither allocates
// a layer, so both keep working and the shell keeps some motion.
// (The macro itself defaults to 0 in trevos_ui.h.)

// Uniform scale: drive transform_scale_x and _y together (LV_SCALE_NONE=256 == 1.0x).
static void scale_exec(void *var, int32_t v)
{
    lv_obj_t *o = (lv_obj_t *)var;
    lv_obj_set_style_transform_scale_x(o, v, 0);
    lv_obj_set_style_transform_scale_y(o, v, 0);
}

// Centre the scale pivot once so the pop grows about the object's middle, not its corner.
static void center_pivot(lv_obj_t *obj)
{
    lv_obj_set_style_transform_pivot_x(obj, LV_PCT(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, LV_PCT(50), 0);
}

// Shared scale-from->1.0x tween. The only thing that differs between scale_in (ease-out,
// fast-settle) and pop (overshoot, bounce-settle) is the path; everything else — centre
// pivot, immediate start-value apply, var bound to obj for auto-cleanup, settle exactly at
// LV_SCALE_NONE — is identical, so they share one body.
static void scale_anim(lv_obj_t *obj, int from_pct, uint32_t ms, lv_anim_path_cb_t path)
{
    if (!obj || TT_NO_LAYER_FX) return;   // resting state is scale 1.0x — nothing to apply
    center_pivot(obj);
    int32_t from = (int32_t)((int64_t)LV_SCALE_NONE * from_pct / 100);
    scale_exec(obj, from);                 // apply start immediately: no first-frame snap
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, from, LV_SCALE_NONE);   // settle exactly at 1.0x (no transform)
    lv_anim_set_time(&a, ms);
    lv_anim_set_exec_cb(&a, scale_exec);
    lv_anim_set_path_cb(&a, path);
    lv_anim_start(&a);
}

void tt_anim_scale_in(lv_obj_t *obj, int from_pct, uint32_t ms)
{
    scale_anim(obj, from_pct, ms, lv_anim_path_ease_out);   // ease-out: fast then settle
}

void tt_anim_pop(lv_obj_t *obj, int from_pct, uint32_t ms)
{
    scale_anim(obj, from_pct, ms, lv_anim_path_overshoot);  // overshoot: crests past 1.0x, settles
}

static void opa_exec(void *var, int32_t v) { lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)v, 0); }

void tt_anim_fade_in(lv_obj_t *obj, uint32_t ms, uint32_t delay)
{
    if (!obj || TT_NO_LAYER_FX) return;   // must return BEFORE the opa 0 below, or it stays invisible
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, 0);   // start hidden so the delay doesn't flash it
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);   // settles fully opaque (resting opa)
    lv_anim_set_time(&a, ms);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_early_apply(&a, true);             // hold opa 0 through the delay, no flash
    lv_anim_set_exec_cb(&a, opa_exec);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

static void trans_x_exec(void *var, int32_t v) { lv_obj_set_style_translate_x((lv_obj_t *)var, v, 0); }
static void trans_y_exec(void *var, int32_t v) { lv_obj_set_style_translate_y((lv_obj_t *)var, v, 0); }

void tt_anim_translate(lv_obj_t *obj, int dx, int dy, uint32_t ms)
{
    if (!obj) return;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, ms);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    if (dx) {                                   // slide X -> 0 (resting translate is 0)
        lv_obj_set_style_translate_x(obj, dx, 0);
        lv_anim_set_values(&a, dx, 0);
        lv_anim_set_exec_cb(&a, trans_x_exec);
        lv_anim_start(&a);
    }
    if (dy) {                                   // slide Y -> 0
        lv_anim_t b = a;                        // copy; lv_anim_start copies again internally
        lv_obj_set_style_translate_y(obj, dy, 0);
        lv_anim_set_values(&b, dy, 0);
        lv_anim_set_exec_cb(&b, trans_y_exec);
        lv_anim_start(&b);
    }
}

void tt_anim_root_fade(lv_obj_t *obj, uint32_t ms)
{
    if (!obj || TT_NO_LAYER_FX) return;   // before the opa 0 below: a guarded face must stay visible
    lv_anim_delete(obj, opa_exec);                 // drop any prior fade so they don't stack/fight
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, 0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(&a, ms);
    lv_anim_set_exec_cb(&a, opa_exec);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

void tt_anim_breathe(lv_obj_t *obj, int peak_pct, uint32_t period)
{
    // Worst offender when layers are unaffordable: repeats forever, so it re-requests a
    // layer every frame for the life of the face rather than once at build time.
    if (!obj || TT_NO_LAYER_FX) return;
    center_pivot(obj);
    int32_t peak = (int32_t)((int64_t)LV_SCALE_NONE * peak_pct / 100);
    // scale pulse: 1.0x -> peak -> 1.0x, infinite, ease-in-out (the in-and-out breath).
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_SCALE_NONE, peak);
    lv_anim_set_time(&a, period);
    lv_anim_set_reverse_duration(&a, period);      // play back down to 1.0x (reverse_time is legacy in 9.5)
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, scale_exec);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_start(&a);
    // matched opacity dip: full -> ~70% -> full, same period, so the dot also softens as it
    // grows. Separate anim (different prop) on the same var; both auto-delete with the obj.
    lv_anim_t o;
    lv_anim_init(&o);
    lv_anim_set_var(&o, obj);
    lv_anim_set_values(&o, LV_OPA_COVER, LV_OPA_70);
    lv_anim_set_time(&o, period);
    lv_anim_set_reverse_duration(&o, period);
    lv_anim_set_repeat_count(&o, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&o, opa_exec);
    lv_anim_set_path_cb(&o, lv_anim_path_ease_in_out);
    lv_anim_start(&o);
}

// The standard entrance (design task motion recipe): root fade, hero scale-in, rows fade
// staggered. Each sub-anim already null-checks and no-ops under TT_NO_LAYER_FX, so this is
// safe to call unconditionally from every build_* function.
void tt_face_enter(lv_obj_t *root, lv_obj_t *hero, lv_obj_t **rows, int nrows)
{
    tt_anim_root_fade(root, 160);
    tt_anim_scale_in(hero, 94, 200);
    for (int i = 0; i < nrows; i++) {
        if (rows[i]) tt_anim_fade_in(rows[i], 140, (uint32_t)(i * 40));
    }
}
