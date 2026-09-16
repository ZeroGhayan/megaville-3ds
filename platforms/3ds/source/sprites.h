#ifndef MEG_SPRITES_H
#define MEG_SPRITES_H

#include "fight.h"
#include <stdint.h>
#include <citro2d.h>

int  meg_sprites_init(void);
void meg_sprites_fini(void);
int  meg_sprites_ok(void);
int  meg_sprites_ok_ch(int ch);
void meg_tint_twin(C2D_ImageTint *t);
void meg_draw_fighter(const Fighter *f, float parallax);
void meg_draw_idle(int ch, float x, float y, float scale);
void meg_draw_pic(int ch, float x, float y, float scale, int face, int twin);
/* face>=0 espelha via UVs (não scaleX=-1, que parte o atlas). */
void meg_blit(C2D_Image img, float rx, float ry, float ox, float oy, int face);

#endif
