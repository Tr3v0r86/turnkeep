/* sim/stubs/sim_bsp_display.c — host BSP display. No panel; the real LVGL display is
 * created in lvgl_port_add_disp. Hands back NULL opaque handles (never dereferenced). */
#include "bsp_display.h"
#include <stddef.h>

esp_err_t bsp_display_init(esp_lcd_panel_io_handle_t *ret_io, esp_lcd_panel_handle_t *ret_panel)
{
    if (ret_io)    *ret_io = NULL;
    if (ret_panel) *ret_panel = NULL;
    return ESP_OK;
}

void bsp_display_backlight_on(void) { /* no panel to light */ }
void bsp_display_power_off(esp_lcd_panel_handle_t panel) { (void)panel; }
