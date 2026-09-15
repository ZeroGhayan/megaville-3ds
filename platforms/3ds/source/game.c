#include "game.h"
#include "roster.h"

static MegGame g;

/* frame_1479 SPRITE_OPPONENTS — 0-based fight index */
static const int OPP[CH_COUNT][NUMOF_OPPONENTS] = {
	/* Blossom    */ { CH_BUBBLES, CH_BUTTERCUP, CH_BLOSSOM, CH_DEXTER,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Bubbles    */ { CH_BUTTERCUP, CH_BLOSSOM, CH_BUBBLES, CH_DEXTER,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Buttercup  */ { CH_BLOSSOM, CH_BUBBLES, CH_BUTTERCUP, CH_DEXTER,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Bell       */ { CH_BLOSSOM, CH_BUBBLES, CH_BUTTERCUP, CH_DEXTER,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Dexter     */ { CH_DEXTER, CH_BUBBLES, CH_BUTTERCUP, CH_BLOSSOM,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Rowdy      */ { CH_ROWDY, CH_BUBBLES, CH_BUTTERCUP, CH_BLOSSOM,
			   CH_DEXTER, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Zim        */ { CH_BLOSSOM, CH_BUBBLES, CH_BUTTERCUP, CH_DEXTER,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA },
	/* Shira      */ { CH_BLOSSOM, CH_BUBBLES, CH_BUTTERCUP, CH_DEXTER,
			   CH_ROWDY, CH_ZIM, CH_BELL, CH_SHIRA }
};

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
	g.story_level = 0;
	g.story_p1 = 0;
	g.numcontinues = 0;
	g.used_continue = 0;
}

void meg_story_begin(int p1)
{
	g.story_p1 = p1;
	g.story_level = 0;
	g.numcontinues = 0;
	g.used_continue = 0;
}

int meg_story_cpu(int p1, int level)
{
	if (p1 < 0 || p1 >= CH_COUNT)
		p1 = 0;
	if (level < 0)
		level = 0;
	if (level >= NUMOF_OPPONENTS)
		level = NUMOF_OPPONENTS - 1;
	return OPP[p1][level];
}

int meg_story_len(int diff)
{
	return (diff <= 5) ? 7 : 8;
}

int meg_story_done(void)
{
	int n = meg_story_len(g.difficulty);

	return g.story_level >= n;
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
