#include "mgos.h"
#include "led_master.h"

static void mgos_intern_jumping_rings_loop(mgos_rgbleds* leds)
{
    uint32_t num_rows = leds->panel_height;
    uint32_t num_cols = leds->panel_width;
    tools_rgb_data out_pix;
    int run = leds->internal_loops;
    run = run <= 0 ? 1 : run;

    int rings = mgos_sys_config_get_ledeffects_jumping_rings_count();

    while (run--) {
        mgos_rgbleds_clear(leds);
        for (int col = 0; col < num_cols; col++) {
            for (int row = leds->pix_pos; row < (leds->pix_pos + rings); row++) {
                int step = (num_cols * rings) ? (256 / (num_cols * rings)) : 0;
                int pix_pos = (leds->pix_pos + col * step) % 256;
                out_pix = tools_color_wheel(((row * 256 / num_rows) + pix_pos) & 255, 255.0);
                LOG(LL_VERBOSE_DEBUG, ("ledeffects_jumping_rings:\tR: 0x%.02X\tG: 0x%.02X\tB: 0x%.02X", out_pix.r, out_pix.g, out_pix.b));
                mgos_rgbleds_plot_pixel(leds, col, row, out_pix, true);
            }
        }
        mgos_rgbleds_show(leds);
        leds->pix_pos = (leds->pix_pos + 1) % (num_rows - rings);
    }
}

void mgos_ledeffects_jumping_rings(void* param, mgos_rgbleds_action action)
{
    static bool do_time = false;
    static uint32_t max_time = 0;
    uint32_t time = (mgos_uptime_micros() / 1000);
    mgos_rgbleds* leds = (mgos_rgbleds*)param;

    switch (action) {
    case MGOS_RGBLEDS_ACT_INIT:
      leds->timeout = mgos_sys_config_get_ledeffects_jumping_rings_timeout();
      leds->dim_all = mgos_sys_config_get_ledeffects_jumping_rings_dim_all();
      LOG(LL_DEBUG, ("mgos_jumping_rings: called (init)"));
      break;
    case MGOS_RGBLEDS_ACT_EXIT:
        LOG(LL_DEBUG, ("mgos_jumping_rings: called (exit)"));
        break;
    case MGOS_RGBLEDS_ACT_LOOP:
        mgos_intern_jumping_rings_loop(leds);
        if (do_time) {
            time = (mgos_uptime_micros() /1000) - time;
            max_time = (time > max_time) ? time : max_time;
            LOG(LL_VERBOSE_DEBUG, ("Rainbow loop duration: %d milliseconds, max: %d ...", time / 1000, max_time / 1000));
        }
        break;
    }
}

bool mgos_jumping_rings_init(void) {
  LOG(LL_INFO, ("mgos_jumping_rings_init ..."));
  ledmaster_add_effect("ANIM_JUMPING_RINGS", mgos_ledeffects_jumping_rings);
  return true;
}
