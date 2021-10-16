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


   modeset_common.c: Common parts of the video settings menus

*/

#include <stdbool.h>
#include <stdio.h>
#include "osd.h"
#include "portdefs.h"
#include "modeset_common.h"

video_mode_t modeset_mode;

int modeset_get_slprofile(void) {
  return video_settings[modeset_mode] & VIDEOIF_SET_SLPROFILE_MASK;
}

bool modeset_set_slprofile(int value) {
  video_settings[modeset_mode] =
    (video_settings[modeset_mode] & ~VIDEOIF_SET_SLPROFILE_MASK) | value;

  if (current_videomode == modeset_mode)
    VIDEOIF->settings = video_settings[modeset_mode] | video_settings_global;
  return true;
}

valueitem_t modeset_value_slprofile = {
  VALTYPE_SLPROFILEOFF, false, {{ modeset_get_slprofile, modeset_set_slprofile }}
};

valueitem_t modeset_value_sleven      = { VALTYPE_EVENODD, true,
                                          { .field = { NULL, VIDEOIF_BIT_SL_EVEN,      0, VIFLAG_MODESET }} };
valueitem_t modeset_value_slalt       = { VALTYPE_BOOL, true,
                                          { .field = { NULL, VIDEOIF_BIT_SL_ALTERNATE, 0, VIFLAG_MODESET }} };
valueitem_t modeset_value_linedoubler = { VALTYPE_BOOL, true,
                                          { .field = { NULL, VIDEOIF_BIT_LD_ENABLE,    0, VIFLAG_MODESET | VIFLAG_REDRAW }} };

void modeset_draw(menu_t *menu) {
  /* header */
  osd_setattr(true, false);

  if (modeset_mode == VIDMODE_NONSTANDARD) {
    osd_gotoxy(menu->xpos + 8, menu->ypos + 1);
    osd_puts("NonStd Settings");

    menu->items[MENUITEM_SLPROFILE].flags = MENU_FLAG_DISABLED;
    menu->items[MENUITEM_SLEVEN   ].flags = MENU_FLAG_DISABLED;
    menu->items[MENUITEM_SLALT    ].flags = MENU_FLAG_DISABLED;
  } else {
    osd_gotoxy(menu->xpos + 9, menu->ypos + 1);
    printf("%s Settings", mode_names[modeset_mode]);

    /* update the item-enable flags based on current settings */
    if (modeset_mode <= VIDMODE_576i && !(video_settings[modeset_mode] & VIDEOIF_SET_LD_ENABLE)) {
      /* no scanlines in non-doubled 240p/288p */
      menu->items[MENUITEM_SLPROFILE].flags = MENU_FLAG_DISABLED;
      menu->items[MENUITEM_SLEVEN   ].flags = MENU_FLAG_DISABLED;
      menu->items[MENUITEM_SLALT    ].flags = MENU_FLAG_DISABLED;

    } else {
      menu->items[MENUITEM_SLPROFILE].flags = 0;

      if (video_settings[modeset_mode] & VIDEOIF_SET_SLPROFILE_MASK) {
        menu->items[MENUITEM_SLEVEN].flags = 0;
        menu->items[MENUITEM_SLALT ].flags = 0;
      } else {
        menu->items[MENUITEM_SLEVEN].flags = MENU_FLAG_DISABLED;
        menu->items[MENUITEM_SLALT ].flags = MENU_FLAG_DISABLED;
      }
    }

    if (modeset_mode != VIDMODE_480i &&
        modeset_mode != VIDMODE_576i &&
        menu->items[MENUITEM_SLALT].flags == 0)
      /* alternating needs a valid field flag and thus works only in interlaced modes */
      menu->items[MENUITEM_SLALT].flags = MENU_FLAG_DISABLED;
  }

  if (modeset_mode >= VIDMODE_480p)
    menu->items[MENUITEM_LINEDOUBLER].flags = MENU_FLAG_DISABLED;
  else
    menu->items[MENUITEM_LINEDOUBLER].flags = 0;
}
