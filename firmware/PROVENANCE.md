# Firmware provenance

Extracted from the author’s `esp-devwork` commit `f3aa75b8f6ce97cb6ea94e7bc5e375f03e8ac9ba` as a new standalone source tree, without importing its Git history.

Included: `apps/character` (portable reducer, UI, tests); the two-button T-Display-S3 character entry point, display and input support; the TrevOS app contract, shell, shared UI and required font sources; a character-only exact-source desktop simulator; CharacterPack schema, standard-library Python generator/validator and tests; synthetic Mara Vale fixture and a newly regenerated header.

Excluded: all personal character fixtures and their generated header/review; all unrelated boards and applications; Todoist, Wi-Fi credentials and sync integrations; device identifiers, private notes, firmware backups, old screenshots and build products; touch/round simulator targets; screenshot streaming service; LED-strip dependencies. Downloaded managed components and local build/configuration output are ignored.

Standalone adaptations: only the Character app is registered; CMake lists only needed components; managed GUI dependencies are pinned; SDL2 is discovered through CMake rather than a machine-specific prefix; required null values and text outside the font repertoire are rejected; personal-fixture tests are removed and the generated-header drift test targets Mara Vale. Device flashing was not performed during extraction.

## Extraction verification (2026-09-19)

On macOS: Python standard-library pack suite, 9 tests passed; C11 character reducer and button-filter tests passed with `-Wall -Wextra -Werror`. ESP-IDF 5.4 built the dedicated target from fresh sdkconfig defaults using esp_lvgl_port 2.9.0 and LVGL 9.6.0~1. The app image is 666,768 bytes with 79% of its 3 MiB partition free. SDL2 simulator compiled, rendered the synthetic Mara overview at native 320×170, and completed KEY/BOOT/Back/sleep-chord input with no deadlock. Physical flashing, current draw and endurance are not verification claims for this extracted build.
