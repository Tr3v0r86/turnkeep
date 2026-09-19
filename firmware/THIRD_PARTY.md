# Third-party components

Original project code follows the repository root license. Third-party material keeps its own license; that root license does not replace the notices below.

| Component | Version / origin | License and delivery |
|---|---|---|
| ESP-IDF | 5.4, Espressif | Apache-2.0 for ESP-IDF; bundled third-party components retain their own notices. Installed separately, not vendored here. |
| esp_lvgl_port | 2.9.0, Espressif component registry | Apache-2.0; fetched by IDF Component Manager with its license. |
| LVGL | registry 9.6.0~1 | MIT; fetched by Component Manager with its license and bundled dependency notices. |
| SDL2 | system installation | zlib; desktop simulator only, installed separately. |
| IBM Plex Sans and Mono | IBM Corp., source TTFs in `tools/fonts/` | SIL Open Font License 1.1, copyright © 2017 IBM Corp., reserved name “Plex”; full notice in [licenses/IBMPlex-OFL.txt](licenses/IBMPlex-OFL.txt). |

`os/trevos/ui/fonts/plex_*.c` are generated glyph data derived from the included IBM Plex fonts. They retain OFL coverage. Font conversion is not required to compile: all converted C files are included. To regenerate, use upstream [lv_font_conv](https://github.com/lvgl/lv_font_conv) (MIT, separately installed); the exact font, size, range and command options are recorded at the top of each generated C file. Run those options from `tools/fonts/`, adjusting the output path to `../../os/trevos/ui/fonts/`. The extraction preserves the existing converted font data rather than claiming reproducibility across converter versions.

No proprietary book text, artwork or game logos are included. Mara Vale is synthetic example data with short numerical/mechanical summaries. Turnkeep is not affiliated with or endorsed by a game publisher, LilyGo, Espressif or IBM.
