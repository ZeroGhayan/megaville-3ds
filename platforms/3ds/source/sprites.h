#ifndef MEG_SPRITES_H
#define MEG_SPRITES_H

#include "fight.h"

int  meg_sprites_init(void);
void meg_sprites_fini(void);
int  meg_sprites_ok(void);
int  meg_sprites_ok_ch(int ch);
void meg_draw_fighter(const Fighter *f, float parallax);
void meg_draw_idle(int ch, float x, float y, float scale);

#endif
