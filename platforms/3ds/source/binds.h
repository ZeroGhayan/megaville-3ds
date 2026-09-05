#ifndef MEG_BINDS_H
#define MEG_BINDS_H

#include "exo/input.h"

typedef enum {
	MEG_ACT_LEFT = 0,
	MEG_ACT_RIGHT,
	MEG_ACT_JUMP,
	MEG_ACT_GUARD,
	MEG_ACT_LIGHT,
	MEG_ACT_HEAVY,
	MEG_ACT_PAUSE,
	MEG_ACT_COUNT
} MegAct;

typedef struct {
	uint32_t mask[MEG_ACT_COUNT];
	int jump_on_up;   /* stick / D-Pad up also jumps (default 1) */
	int jump_on_a;    /* A also jumps (default 0) */
	int guard_on_down;
} MegBinds;

void       meg_binds_init(void);
void       meg_binds_reset(void);
int        meg_binds_load(void);
int        meg_binds_save(void);
MegBinds  *meg_binds(void);

const char *meg_act_name(MegAct a);
void        meg_mask_label(uint32_t mask, char *out, unsigned n);

int  meg_held(MegAct a);
int  meg_down(MegAct a);

/* first face/shoulder/start/select/dpad pressed this frame, else 0 */
uint32_t meg_capture_button(void);

void meg_set_bind(MegAct a, uint32_t button);
void meg_set_jump_on_a(int on);

#endif
