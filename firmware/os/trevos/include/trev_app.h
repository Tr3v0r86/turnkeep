// trev_app.h — the TrevOS app contract (board-neutral).
//
// Every TrevOS app implements this and registers at boot. The contract is the
// frozen seam between an app and the OS: the OS owns the screen and input; the app
// builds its UI under a root and reacts to two semantic inputs. A board maps its
// own hardware to those inputs (wheel + touch on the puck; two buttons on the T3),
// so apps never see a pin.
#pragma once
#include <stdint.h>
#include "lvgl.h"

#define TREV_APP_API_VERSION_V1 1
#define TREV_APP_API_VERSION 2

typedef struct trev_app trev_app_t;
typedef enum { TREV_TURN_PREV = -1, TREV_TURN_NEXT = 1 } trev_turn_t;

typedef struct {
    uint32_t api_version;
    const char *id;
    const char *name;
    void (*on_start)(trev_app_t *app, lv_obj_t *root);   // build UI under root
    void (*on_stop)(trev_app_t *app);                    // drop widget pointers
    void (*on_tick)(trev_app_t *app, uint32_t now_ms);   // optional, may be NULL
    void (*on_turn)(trev_app_t *app, trev_turn_t dir);   // navigate / change a value
    void (*on_commit)(trev_app_t *app);                  // the commit action
    void (*on_back)(trev_app_t *app);                    // optional nested navigation
} trev_app_def_t;

struct trev_app {
    const trev_app_def_t *def;
    lv_obj_t *root;
    void     *state;   // app-private; the app allocates in on_start, frees in on_stop
};
