#ifndef MEG_CLIP_H
#define MEG_CLIP_H

#include "fight.h"

int  meg_clip_init(void);
void meg_clip_fini(void);
int  meg_clip_ok(int ch);
void meg_clip_tick(Fighter *f, float dt);
void meg_clip_draw(const Fighter *f, float parallax);

#endif
