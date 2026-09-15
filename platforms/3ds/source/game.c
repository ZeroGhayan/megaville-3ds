#include "game.h"

static MegGame g;

MegGame *meg_game(void)
{
	return &g;
}

void meg_game_init(void)
{
	g.mode = MODE_STORY;
	g.difficulty = 5;
	g.menudiff = 5;
	g.zim_survival = 0;
	g.unlock_zim = 0;
	g.unlock_zim_surv = 0;
	g.unlock_shira = 0;
	g.unlock_extra = 0;
	g.dual_ctrl = 0;
}

const char *meg_mode_name(int mode)
{
	switch (mode) {
	case MODE_STORY:
		return "STORY";
	case MODE_VERSUS:
		return "VERSUS";
	case MODE_SURVIVAL:
		return "SURVIVAL";
	case MODE_DEMO:
		return "DEMO";
	default:
		return "TUTORIAL";
	}
}

const char *meg_diff_name(int d)
{
	static const char *n[11] = {
		"", "1 VERY EASY", "2 EASY", "3 NORMAL", "4 HARD",
		"5 HARDER", "6 TOUGH", "7 EXPERT", "8 INSANE",
		"9 NIGHTMARE", "10 MAX"
	};
	if (d < 1)
		d = 1;
	if (d > 10)
		d = 10;
	return n[d];
}
