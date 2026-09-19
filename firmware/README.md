# Turnkeep firmware

A dedicated character companion for the **non-touch LilyGo T-Display-S3** (ESP32-S3, 16 MB flash, 8 MB octal PSRAM, 170×320 ST7789 panel used in 320×170 landscape). Other T-Display variants are not supported by this pin map. The portable character reducer and UI use the included TrevOS app contract; only the display and button support is board-specific.

## Build and install

Install [ESP-IDF v5.4](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/get-started/index.html) with ESP32-S3 tools and activate its `export.sh`. Commands below run from `firmware/`. The first build downloads pinned public components, so it needs internet access. `dependencies.lock` records their resolved versions.

```sh
idf.py -C boards/tdisplay-s3 -B build-character build
# Only when ready to replace the firmware on your connected board:
idf.py -C boards/tdisplay-s3 -B build-character -p YOUR_SERIAL_PORT flash monitor
```

This is the sole build profile; no `CHARACTER_CASE` switch is needed. No credentials, Wi-Fi setup or cloud service are required. Back up any firmware/data you want to retain before flashing your own device; this repository contains no device backups. The partition table allocates a 3 MiB factory application and NVS for saved character counters.

## Controls and scope

KEY advances; holding KEY moves back. BOOT selects/confirms; holding BOOT returns. Holding both buttons and releasing the chord saves state and enters deep sleep; either button wakes the board. The case orientation is fixed in `main/main_character.c`. HP, temporary HP, spell slots and one named resource persist in NVS. Damage consumes temporary HP first; undo restores the previous in-memory transaction. Reference lists show actions, spells, checks and sheet notes.

The included Mara Vale fixture is synthetic demonstration data, not a rules compendium or an official character sheet. Content import is **compile-time JSON → generated C header → rebuild/flash**. Runtime JSON loading, phone import, Bluetooth upload, an encounter tracker and multi-character selection are not implemented. There is one spell-slot pool and one class-resource pool; short rest rules and class mechanics are not automated. Pack text accepts printable ASCII and the middle dot only, matching the supplied font repertoire. Control characters and broader Unicode text are rejected. Battery percentage/current, endurance and thermal performance have not been measured for this public build. The simulator uses volatile in-memory NVS and cannot prove physical sleep, battery behavior or retention across process restarts.

## Edit content and verify

Python 3 uses only the standard library. `schemas/character/character-pack-v1.schema.json` documents the bounded format; `dev.py` is the executable validator. Change the character ID when replacing a character to start fresh counters; the ID hashes to the saved-state generation key. Editing content while keeping the same ID preserves compatible saved counters.

```sh
python3 tools/character/dev.py doctor
python3 tools/character/dev.py validate fixtures/character/synthetic-mara-vale.json
python3 tools/character/dev.py generate fixtures/character/synthetic-mara-vale.json apps/character/include/character_pack_generated.h
python3 -m unittest tools/character/tests/test_pack.py
cc -std=c11 -Wall -Wextra -Werror -Iapps/character/core/include apps/character/core/test_character_core.c apps/character/core/character_core.c -o /tmp/turnkeep-core
/tmp/turnkeep-core
cc -std=c11 -Wall -Wextra -Werror -Iboards/tdisplay-s3/main boards/tdisplay-s3/test/test_button_filter.c boards/tdisplay-s3/main/button_filter.c -o /tmp/turnkeep-buttons
/tmp/turnkeep-buttons
```

The tracked generated header is intentional: firmware builds need no Python pack-generation step, and the drift test checks it matches the fixture. Keep only content you have permission to redistribute in public packs.

## Desktop simulator and source

Requires CMake, a C compiler, POSIX threads and SDL2 development files. Build the device once to populate managed LVGL sources, or supply `-DLVGL_DIR=/path/to/lvgl-9.6.0`. Install SDL2 through your package manager; if CMake cannot discover it, supply `-DCMAKE_PREFIX_PATH=/path/to/sdl2` (on Apple Silicon Homebrew, `/opt/homebrew/opt/sdl2`).

```sh
cmake -S sim -B sim/build
cmake --build sim/build --target charactersim -j
./sim/build/charactersim
# Headless smoke run and native-size PPM screenshot:
SIM_DISP=null SIM_TICKS=120 SIM_SHOT=/tmp/turnkeep.ppm ./sim/build/charactersim
```

Type `KEY`, `KEY_LONG`, `BOOT`, `BOOT_LONG`, `CHORD`, or `QUIT` on stdin. UI, character core, shell and board entry point are the device sources; display, buttons and storage are host seams. SDL2 is still a build dependency for headless runs. [THIRD_PARTY.md](THIRD_PARTY.md) covers dependencies, font source and licenses; [PROVENANCE.md](PROVENANCE.md) records the extraction boundary.
