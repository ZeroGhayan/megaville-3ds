#include "binds.h"

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define CFG_DIR  "sdmc:/3ds/megaville"
#define CFG_PATH "sdmc:/3ds/megaville/binds.cfg"

static MegBinds g;

static const uint32_t BTN_ORDER[] = {
	EXO_BTN_A, EXO_BTN_B, EXO_BTN_X, EXO_BTN_Y,
	EXO_BTN_L, EXO_BTN_R, EXO_BTN_ZL, EXO_BTN_ZR,
	EXO_BTN_START, EXO_BTN_SELECT,
	EXO_BTN_UP, EXO_BTN_DOWN, EXO_BTN_LEFT, EXO_BTN_RIGHT
};

static const char *BTN_NAME[] = {
	"A", "B", "X", "Y",
	"L", "R", "ZL", "ZR",
	"START", "SELECT",
	"UP", "DOWN", "LEFT", "RIGHT"
};

static const char *ACT_NAME[MEG_ACT_COUNT] = {
	"LEFT", "RIGHT", "JUMP", "GUARD", "LIGHT", "HEAVY", "PAUSE"
};

static void apply_jump_a(void)
{
	if (g.jump_on_a)
		g.mask[MEG_ACT_JUMP] |= EXO_BTN_A;
	else
		g.mask[MEG_ACT_JUMP] &= ~EXO_BTN_A;
}

void meg_binds_reset(void)
{
	memset(&g, 0, sizeof g);
	g.mask[MEG_ACT_LEFT]  = EXO_BTN_LEFT;
	g.mask[MEG_ACT_RIGHT] = EXO_BTN_RIGHT;
	g.mask[MEG_ACT_JUMP]  = EXO_BTN_UP;
	g.mask[MEG_ACT_GUARD] = EXO_BTN_DOWN;
	g.mask[MEG_ACT_LIGHT] = EXO_BTN_Y;
	g.mask[MEG_ACT_HEAVY] = EXO_BTN_X;
	g.mask[MEG_ACT_PAUSE] = EXO_BTN_START;
	g.jump_on_up    = 1;
	g.jump_on_a     = 0;
	g.guard_on_down = 1;
}

void meg_binds_init(void)
{
	meg_binds_reset();
	if (!meg_binds_load())
		meg_binds_save();
}

MegBinds *meg_binds(void)
{
	return &g;
}

const char *meg_act_name(MegAct a)
{
	if ((unsigned)a >= MEG_ACT_COUNT)
		return "?";
	return ACT_NAME[a];
}

void meg_mask_label(uint32_t mask, char *out, unsigned n)
{
	unsigned i, used = 0;

	if (!out || n == 0)
		return;
	out[0] = 0;
	if (!mask) {
		snprintf(out, n, "-");
		return;
	}
	for (i = 0; i < sizeof BTN_ORDER / sizeof BTN_ORDER[0]; ++i) {
		if (mask & BTN_ORDER[i]) {
			if (used)
				strncat(out, "+", n - strlen(out) - 1);
			strncat(out, BTN_NAME[i], n - strlen(out) - 1);
			used = 1;
		}
	}
	if (!used)
		snprintf(out, n, "%08X", (unsigned)mask);
}

int meg_held(MegAct a)
{
	const ExoInput *in = exo_input();

	if (in->held & g.mask[a])
		return 1;
	if (a == MEG_ACT_LEFT && in->stick_x < -0.40f)
		return 1;
	if (a == MEG_ACT_RIGHT && in->stick_x > 0.40f)
		return 1;
	if (a == MEG_ACT_JUMP && g.jump_on_up && in->stick_y > 0.50f)
		return 1;
	if (a == MEG_ACT_GUARD && g.guard_on_down && in->stick_y < -0.50f)
		return 1;
	return 0;
}

int meg_down(MegAct a)
{
	const ExoInput *in = exo_input();

	if (in->down & g.mask[a])
		return 1;
	if (a == MEG_ACT_JUMP && g.jump_on_up &&
	    in->stick_y > 0.50f && !(in->held & EXO_BTN_UP)) {
		/* analog edge is noisy; digital mask covers D-Pad */
	}
	return 0;
}

uint32_t meg_capture_button(void)
{
	const ExoInput *in = exo_input();
	unsigned i;

	for (i = 0; i < sizeof BTN_ORDER / sizeof BTN_ORDER[0]; ++i)
		if (in->down & BTN_ORDER[i])
			return BTN_ORDER[i];
	return 0;
}

void meg_set_bind(MegAct a, uint32_t button)
{
	unsigned i;

	if (!button || (unsigned)a >= MEG_ACT_COUNT)
		return;
	g.mask[a] = button;
	for (i = 0; i < MEG_ACT_COUNT; ++i)
		if (i != (unsigned)a)
			g.mask[i] &= ~button;
	g.jump_on_a = (g.mask[MEG_ACT_JUMP] & EXO_BTN_A) ? 1 : 0;
}

void meg_set_jump_on_a(int on)
{
	g.jump_on_a = on ? 1 : 0;
	apply_jump_a();
	if (g.jump_on_a) {
		unsigned i;
		for (i = 0; i < MEG_ACT_COUNT; ++i)
			if (i != MEG_ACT_JUMP)
				g.mask[i] &= ~EXO_BTN_A;
	}
}

static void ensure_dir(void)
{
	mkdir("sdmc:/3ds", 0777);
	mkdir(CFG_DIR, 0777);
}

int meg_binds_save(void)
{
	FILE *f;

	ensure_dir();
	f = fopen(CFG_PATH, "w");
	if (!f)
		return 0;
	fprintf(f, "jump_on_up=%d\n", g.jump_on_up);
	fprintf(f, "jump_on_a=%d\n", g.jump_on_a);
	fprintf(f, "guard_on_down=%d\n", g.guard_on_down);
	fprintf(f, "left=%u\n",  (unsigned)g.mask[MEG_ACT_LEFT]);
	fprintf(f, "right=%u\n", (unsigned)g.mask[MEG_ACT_RIGHT]);
	fprintf(f, "jump=%u\n",  (unsigned)g.mask[MEG_ACT_JUMP]);
	fprintf(f, "guard=%u\n", (unsigned)g.mask[MEG_ACT_GUARD]);
	fprintf(f, "light=%u\n", (unsigned)g.mask[MEG_ACT_LIGHT]);
	fprintf(f, "heavy=%u\n", (unsigned)g.mask[MEG_ACT_HEAVY]);
	fprintf(f, "pause=%u\n", (unsigned)g.mask[MEG_ACT_PAUSE]);
	fclose(f);
	return 1;
}

int meg_binds_load(void)
{
	FILE *f;
	char key[32];
	unsigned val;
	int flag;

	f = fopen(CFG_PATH, "r");
	if (!f)
		return 0;
	while (fscanf(f, "%31[^=]=", key) == 1) {
		if (!strcmp(key, "jump_on_up") ||
		    !strcmp(key, "jump_on_a") ||
		    !strcmp(key, "guard_on_down")) {
			if (fscanf(f, "%d", &flag) == 1) {
				if (!strcmp(key, "jump_on_up"))
					g.jump_on_up = flag ? 1 : 0;
				else if (!strcmp(key, "jump_on_a"))
					g.jump_on_a = flag ? 1 : 0;
				else
					g.guard_on_down = flag ? 1 : 0;
			}
		} else if (fscanf(f, "%u", &val) == 1) {
			if (!strcmp(key, "left"))
				g.mask[MEG_ACT_LEFT] = val;
			else if (!strcmp(key, "right"))
				g.mask[MEG_ACT_RIGHT] = val;
			else if (!strcmp(key, "jump"))
				g.mask[MEG_ACT_JUMP] = val;
			else if (!strcmp(key, "guard"))
				g.mask[MEG_ACT_GUARD] = val;
			else if (!strcmp(key, "light"))
				g.mask[MEG_ACT_LIGHT] = val;
			else if (!strcmp(key, "heavy"))
				g.mask[MEG_ACT_HEAVY] = val;
			else if (!strcmp(key, "pause"))
				g.mask[MEG_ACT_PAUSE] = val;
		}
		fscanf(f, "%*[^\n]");
		fscanf(f, "\n");
	}
	fclose(f);
	apply_jump_a();
	return 1;
}
