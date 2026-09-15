#ifndef MEG_GAME_H
#define MEG_GAME_H

enum {
	MODE_STORY = 0,
	MODE_VERSUS,
	MODE_SURVIVAL,
	MODE_DEMO,
	MODE_TUTORIAL
};

#define MAX_DIFFICULTY 10

typedef struct {
	int mode;
	int difficulty; /* 1..10, Flash _difficulty */
	int menudiff;   /* last picker value */
	int zim_survival;
	int unlock_zim;
	int unlock_zim_surv;
	int unlock_shira;
	int unlock_extra;
	int dual_ctrl;
} MegGame;

MegGame *meg_game(void);
void meg_game_init(void);
const char *meg_mode_name(int mode);
const char *meg_diff_name(int d);

#endif
