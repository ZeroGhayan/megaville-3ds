#include "clip.h"
#include "clip_table.h"
#include "roster.h"
#include "sprites.h"

#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if MEG_CLIP
#include "clip_off.h"

enum {
	CLIP_IDLE = 0,
	CLIP_LAND,
	CLIP_JUMP,
	CLIP_FORWARD,
	CLIP_DASH,
	CLIP_SHIELD,
	CLIP_COMBO1,
	CLIP_COMBO2,
	CLIP_COMBO3,
	CLIP_RANGED,
	CLIP_AIR,
	CLIP_DOWNATK,
	CLIP_FALL,
	CLIP_FALLEN,
	CLIP_RECOVER,
	CLIP_WIN,
	CLIP_TELEPORT,
	CLIP_DAMAGE
};

#define MAX_SHEETS 192

static C2D_SpriteSheet g_sh[MAX_SHEETS];
static int g_key[MAX_SHEETS];
static int g_nsh;
static int g_loaded[CH_COUNT];

static int phase_to_clip(const Fighter *f)
{
	if (f->hp <= 0)
		return CLIP_FALLEN;
	switch (f->phase) {
	case FIGHT_HIT:
	case FIGHT_FROZEN:
		return CLIP_DAMAGE;
	case FIGHT_DASH:
		return CLIP_DASH;
	case FIGHT_JUMP:
		return CLIP_JUMP;
	case FIGHT_GUARD:
		return CLIP_SHIELD;
	case FIGHT_STARTUP:
	case FIGHT_ACTIVE:
	case FIGHT_RECOVERY:
		if (f->move == 5)
			return CLIP_RANGED;
		if (f->move == 1 || f->move == 3)
			return CLIP_COMBO3;
		if (f->move == 2 || f->move == 4)
			return CLIP_COMBO2;
		return CLIP_COMBO1;
	case FIGHT_WALK:
		return CLIP_FORWARD;
	default:
		if (!f->grounded)
			return CLIP_JUMP;
		if (f->vx > 20.0f || f->vx < -20.0f)
			return CLIP_FORWARD;
		return CLIP_IDLE;
	}
}

static int sheet_key(int ch, int id, int sheet)
{
	return (ch << 12) | (id * 32 + sheet);
}

static C2D_SpriteSheet find_sheet(int key)
{
	int i;

	for (i = 0; i < g_nsh; ++i)
		if (g_key[i] == key)
			return g_sh[i];
	return NULL;
}

static void load_one(int ch, int id, int sheet)
{
	char path[96];
	int key;

	if (g_nsh >= MAX_SHEETS)
		return;
	if (CLIP[ch][id].count <= 0)
		return;
	key = sheet_key(ch, id, sheet);
	if (find_sheet(key))
		return;
	snprintf(path, sizeof path, "%s_%d.t3x", CLIP[ch][id].prefix, sheet);
	g_sh[g_nsh] = C2D_SpriteSheetLoad(path);
	if (!g_sh[g_nsh])
		return;
	g_key[g_nsh] = key;
	g_nsh++;
}

static void ensure_char(int ch)
{
	int id, s, n, chunk;

	if (ch < 0 || ch >= CH_COUNT || g_loaded[ch])
		return;
	for (id = 0; id < CLIP_ANIM_N; ++id) {
		n = CLIP[ch][id].count;
		chunk = CLIP[ch][id].chunk;
		if (n <= 0 || chunk < 1)
			continue;
		for (s = 0; s < (n + chunk - 1) / chunk; ++s)
			load_one(ch, id, s);
	}
	g_loaded[ch] = 1;
}

int meg_clip_init(void)
{
	memset(g_loaded, 0, sizeof g_loaded);
	g_nsh = 0;
	return 1;
}

void meg_clip_fini(void)
{
	int i;

	for (i = 0; i < g_nsh; ++i) {
		if (g_sh[i])
			C2D_SpriteSheetFree(g_sh[i]);
		g_sh[i] = NULL;
	}
	g_nsh = 0;
	memset(g_loaded, 0, sizeof g_loaded);
}

int meg_clip_ok(int ch)
{
	if (ch < 0 || ch >= CH_COUNT)
		return 0;
	if (CLIP[ch][CLIP_IDLE].count <= 0)
		return 0;
	ensure_char(ch);
	return find_sheet(sheet_key(ch, CLIP_IDLE, 0)) != NULL;
}

void meg_clip_tick(Fighter *f, float dt)
{
	int id, n, loop, ch;

	ch = f->ch;
	if (!meg_clip_ok(ch))
		return;
	ensure_char(ch);
	id = phase_to_clip(f);
	if (id != f->clip_id) {
		f->clip_id = id;
		f->clip_f = 0;
		f->clip_t = 0.0f;
	}
	n = CLIP[ch][id].count;
	if (n <= 0)
		return;
	if (id == CLIP_IDLE) {
		f->clip_f = 0;
		f->clip_t = 0.0f;
		return;
	}
	f->clip_t += dt;
	if (f->clip_t < 0.04f)
		return;
	f->clip_t = 0.0f;
	f->clip_f++;
	loop = CLIP[ch][id].loop;
	if (f->clip_f >= n)
		f->clip_f = (loop < 0) ? (n - 1) : loop;
}

void meg_clip_draw(const Fighter *f, float parallax)
{
	C2D_SpriteSheet sh;
	C2D_Image img;
	float rx, ry;
	int ch, id, fr, n, chunk, local, idx, ox, oy;

	ch = f->ch;
	if (!meg_clip_ok(ch))
		return;
	ensure_char(ch);
	id = f->clip_id;
	if (id < 0 || id >= CLIP_ANIM_N)
		id = CLIP_IDLE;
	n = CLIP[ch][id].count;
	if (n <= 0)
		return;
	fr = f->clip_f;
	if (fr < 0)
		fr = 0;
	if (fr >= n)
		fr = n - 1;
	chunk = CLIP[ch][id].chunk;
	if (chunk < 1)
		chunk = 1;
	local = fr % chunk;
	sh = find_sheet(sheet_key(ch, id, fr / chunk));
	if (!sh)
		return;
	img = C2D_SpriteSheetGetImage(sh, local);
	if (!img.subtex)
		return;
	idx = CLIP_BASE[ch][id] + fr;
	ox = CLIP_OX[ch][idx];
	oy = CLIP_OY[ch][idx];
	rx = f->x + f->w * 0.5f + parallax;
	ry = f->y + f->h;
	{
		float sc = 1.0f;

		if (oy > 8)
			sc = f->h / (float)oy;
		else if (img.subtex->height > 0)
			sc = f->h / (float)img.subtex->height;
		meg_blit(img, rx, ry, (float)ox, (float)oy, f->face, sc);
	}
}

#else

int meg_clip_init(void) { return 0; }
void meg_clip_fini(void) {}
int meg_clip_ok(int ch) { (void)ch; return 0; }
void meg_clip_tick(Fighter *f, float dt) { (void)f; (void)dt; }
void meg_clip_draw(const Fighter *f, float parallax) { (void)f; (void)parallax; }

#endif
