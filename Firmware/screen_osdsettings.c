/* GCVideo DVI Firmware

   Copyright (C) 2015-2021, Ingo Korb <ingo@akana.de>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGE.


   screen_osdsettings.c: Menu screen for OSD-related settings

*/

#include <stdbool.h>
#include <stddef.h>
#include "menu.h"
#include "osd.h"
#include "pad.h"
#include "portdefs.h"
#include "screens.h"
#include "settings.h"

enum {
  MENUITEM_RESBOX,
  MENUITEM_RESBOX_X,
  MENUITEM_RESBOX_Y,
  MENUITEM_RESBOX_TIME,
  MENUITEM_HOLD_TIME,
  MENUITEM_ALPHA,
  MENUITEM_TINTCB,
  MENUITEM_TINTCR,
  MENUITEM_IRCONFIG,
  MENUITEM_EXIT
};

/* --- valueitems --- */

static valueitem_t value_resbox      = { VALTYPE_BOOL,        true,
                                         { .field = { &resbox_enabled,  8, 24, VIFLAG_REDRAW }} };
static valueitem_t value_resbox_x    = { VALTYPE_RESBOX_X,    true,
                                         { .field = { &resbox_x,        8, 24, 0 }} };
static valueitem_t value_resbox_y    = { VALTYPE_RESBOX_Y,    true,
                                         { .field = { &resbox_y,        8, 24, 0 }} };
static valueitem_t value_resbox_time = { VALTYPE_RESBOX_TIME, true,
                                         { .field = { &resbox_time,     4, 24, 0 }} };
static valueitem_t value_hold_time   = { VALTYPE_RESBOX_TIME, true,
                                         { .field = { &resbox_time,     4, 28, 0 }} };
static valueitem_t value_alpha       = { VALTYPE_BYTE,        true,
                                         { .field = { &osdbg_settings, 32, VIDEOIF_OSDBG_ALPHA_SHIFT,  VIFLAG_UPDATE_VIDEOIF }} };
static valueitem_t value_tint_cb     = { VALTYPE_SBYTE_99,    true,
                                         { .field = { &osdbg_settings,  8, VIDEOIF_OSDBG_TINTCB_SHIFT,
                                                      VIFLAG_UPDATE_VIDEOIF | VIFLAG_SBYTE }} };
static valueitem_t value_tint_cr     = { VALTYPE_SBYTE_99,    true,
                                         { .field = { &osdbg_settings,  8, VIDEOIF_OSDBG_TINTCR_SHIFT,
                                                      VIFLAG_UPDATE_VIDEOIF | VIFLAG_SBYTE }} };

/* --- menu definition --- */

static void osdset_draw(menu_t *menu);

static menuitem_t osdset_items[] = {
  { "Mode Popup",       &value_resbox,      1, 0 }, // 0
  { " X Position",      &value_resbox_x,    2, 0 }, // 1
  { " Y Position",      &value_resbox_y,    3, 0 }, // 2
  { "Hold Time" + 4,    &value_resbox_time, 4, 0 }, // 3
  { "Hold Time",        &value_hold_time,   5, 0 }, // 4
  { "BG Transparency",  &value_alpha,       6, 0 }, // 5
  { "BG Tint Blue",     &value_tint_cb,     7, 0 }, // 6
  { "BG Tint Red",      &value_tint_cr,     8, 0 }, // 7
  { "IR Key Config...", NULL,               9, 0 }, // 8
  { "Exit",             NULL,              11, 0 }, // 9
};

static menu_t osdset_menu = {
  11, 7,
  23, 13,
  osdset_draw,
  sizeof(osdset_items) / sizeof(*osdset_items),
  osdset_items
};

static void osdset_draw(menu_t *menu) {
  if (resbox_enabled) {
    osdset_items[MENUITEM_RESBOX_X].flags    = 0;
    osdset_items[MENUITEM_RESBOX_Y].flags    = 0;
    osdset_items[MENUITEM_RESBOX_TIME].flags = 0;
  } else {
    osdset_items[MENUITEM_RESBOX_X].flags    = MENU_FLAG_DISABLED;
    osdset_items[MENUITEM_RESBOX_Y].flags    = MENU_FLAG_DISABLED;
    osdset_items[MENUITEM_RESBOX_TIME].flags = MENU_FLAG_DISABLED;
  }
}

void screen_osdsettings(void) {
  int current_item = 0;

  while (1) {
    osd_clrscr();
    menu_draw(&osdset_menu);
    current_item = menu_exec(&osdset_menu, current_item);

    switch (current_item) {
      case MENU_ABORT:
      case MENUITEM_EXIT:
      default:
        return;

      case MENUITEM_IRCONFIG:
        screen_irconfig(true);
        break;
    }
  }
}
