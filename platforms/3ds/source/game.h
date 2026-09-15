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
#define NUMOF_OPPONENTS 8

typedef struct {
	int mode;
	int difficulty;
	int menudiff;
	int zim_survival;
	int unlock_zim;
	int unlock_zim_surv;
	int unlock_shira;
	int unlock_extra;
	int dual_ctrl;
	int story_level;
	int story_p1;
	int numcontinues;
	int used_continue;
} MegGame;

MegGame *meg_game(void);
void meg_game_init(void);
void meg_story_begin(int p1);
int meg_story_cpu(int p1, int level);
int meg_story_len(int diff);
int meg_story_done(void);
const char *meg_mode_name(int mode);
const char *meg_diff_name(int d);

#endif
