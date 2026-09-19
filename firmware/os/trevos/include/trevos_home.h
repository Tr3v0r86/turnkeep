// trevos_home.h — the round-native home face (TrevOS app). Round builds only: the whole
// implementation is #if TT_ROUND_DISPLAY, so referencing TREV_HOME_ROUND on a rectangular
// board is a link error by design (those boards have their own launchers).
#pragma once
#include "trev_app.h"

extern const trev_app_def_t TREV_HOME_ROUND;
