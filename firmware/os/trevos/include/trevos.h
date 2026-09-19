// trevos.h — TrevOS runtime API.
//
// The OS owns one LVGL screen and routes semantic input to the active app. A board
// brings up LVGL, calls trev_init() with the screen, registers apps, and feeds
// trev_input_* from its own input glue. Input and open/home calls must be made
// under the LVGL lock (they touch widgets); on_tick runs inside the LVGL task.
#pragma once
#include "lvgl.h"
#include "trev_app.h"

void trev_init(lv_obj_t *screen);
void trev_app_register(const trev_app_def_t *def);
int  trev_app_count(void);
const trev_app_def_t *trev_app_def(int index);   // registered app metadata, or NULL if out of range

void trev_open(int index);            // mount app[index] on the screen

// Designate a registered app as the home face (a styled launcher). When set,
// trev_input_home() opens it instead of the bare built-in stub. -1 (default) = stub.
void trev_set_home_app(int index);

void trev_input_turn(trev_turn_t dir);
void trev_input_commit(void);
void trev_input_back(void);
void trev_input_home(void);           // open the home app (if set) or the stub
