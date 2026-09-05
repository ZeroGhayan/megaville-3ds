#ifndef MEG_SPRITES_H
#define MEG_SPRITES_H

#include "fight.h"

int  meg_sprites_init(void);
void meg_sprites_fini(void);
int  meg_sprites_ok(void);
void meg_draw_fighter(const Fighter *f, float parallax);

#endif
