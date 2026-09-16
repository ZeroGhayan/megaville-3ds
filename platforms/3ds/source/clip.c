#include "clip.h"
#include "clip_table.h"
#include "roster.h"

#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if MEG_DEX_CLIP
#include "clip_off.h"

enum {
	DEX_IDLE = 0,
	DEX_LAND,
	DEX_JUMP,
	DEX_FORWARD,
	DEX_DASH,
	DEX_SHIELD,
	DEX_COMBO1,
	DEX_COMBO2,
	DEX_COMBO3,
	DEX_RANGED,
	DEX_AIR,
	DEX_DOWNATK,
	DEX_FALL,
	DEX_FALLEN,
	DEX_RECOVER,
	DEX_WIN,
	DEX_TELEPORT,
	DEX_DAMAGE,
	DEX_CLIP_N
};

typedef struct {
	const char *prefix;
	int count;
	int loop;
	int chunk;
} DexClip;

static const DexClip DEX_CLIP[] = { DEX_CLIP_DATA };

#define MAX_SHEETS 192

static C2D_SpriteSheet g_sh[MAX_SHEETS];
static int g_key[MAX_SHEETS];
static int g_nsh;
static int g_ok;

static int phase_to_clip(const Fighter *f)
{
	if (f->hp <= 0)
		return DEX_FALLEN;
	switch (f->phase) {
	case FIGHT_HIT:
	case FIGHT_FROZEN:
		return DEX_DAMAGE;
	case FIGHT_DASH:
		return DEX_DASH;
	case FIGHT_JUMP:
		return DEX_JUMP;
	case FIGHT_GUARD:
		return DEX_SHIELD;
	case FIGHT_STARTUP:
	case FIGHT_ACTIVE:
	case FIGHT_RECOVERY:
		if (f->move == 5)
			return DEX_RANGED;
		if (f->move == 1 || f->move == 3)
			return DEX_COMBO3;
		if (f->move == 2 || f->move == 4)
			return DEX_COMBO2;
		return DEX_COMBO1;
	case FIGHT_WALK:
		return DEX_FORWARD;
	default:
		if (!f->grounded)
			return DEX_JUMP;
		return DEX_IDLE;
	}
}

static int sheet_key(int id, int sheet, int twin)
{
	return (twin ? 0x8000 : 0) | (id * 256 + sheet);
}

static C2D_SpriteSheet find_sheet(int key)
{
	int i;

	for (i = 0; i < g_nsh; ++i)
		if (g_key[i] == key)
			return g_sh[i];
	return NULL;
}

static void load_one(int id, int sheet, int twin)
{
	char path[80];
	int key;

	if (g_nsh >= MAX_SHEETS)
		return;
	key = sheet_key(id, sheet, twin);
	if (find_sheet(key))
		return;
	if (twin)
		snprintf(path, sizeof path, "%s_t_%d.t3x",
		         DEX_CLIP[id].prefix, sheet);
	else
		snprintf(path, sizeof path, "%s_%d.t3x",
		         DEX_CLIP[id].prefix, sheet);
	g_sh[g_nsh] = C2D_SpriteSheetLoad(path);
	if (!g_sh[g_nsh])
		return;
	g_key[g_nsh] = key;
	g_nsh++;
}

int meg_clip_init(void)
{
	int id, s, n, chunk, last;

	g_ok = 0;
	g_nsh = 0;
	last = DEX_DAMAGE; /* extra/win nao no preload */
	if (last >= (int)(sizeof DEX_CLIP / sizeof DEX_CLIP[0]))
		last = (int)(sizeof DEX_CLIP / sizeof DEX_CLIP[0]) - 1;
	for (id = 0; id <= last; ++id) {
		n = DEX_CLIP[id].count;
		chunk = DEX_CLIP[id].chunk;
		if (n <= 0 || chunk < 1)
			continue;
		for (s = 0; s < (n + chunk - 1) / chunk; ++s) {
			load_one(id, s, 0);
			load_one(id, s, 1);
		}
	}
	g_ok = g_nsh > 0;
	return g_ok;
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
	g_ok = 0;
}

int meg_clip_ok(int ch)
{
	return g_ok && ch == CH_DEXTER;
}

void meg_clip_tick(Fighter *f, float dt)
{
	int id, n, loop;

	if (!meg_clip_ok(f->ch))
		return;
	id = phase_to_clip(f);
	if (id != f->clip_id) {
		f->clip_id = id;
		f->clip_f = 0;
		f->clip_t = 0.0f;
	}
	n = DEX_CLIP[id].count;
	if (n <= 0)
		return;
	f->clip_t += dt;
	if (f->clip_t < 0.04f)
		return;
	f->clip_t = 0.0f;
	f->clip_f++;
	loop = DEX_CLIP[id].loop;
	if (f->clip_f >= n)
		f->clip_f = (loop < 0) ? (n - 1) : loop;
}

void meg_clip_draw(const Fighter *f, float parallax)
{
	C2D_SpriteSheet sh;
	C2D_Image img;
	float x, y, sx, rx, ry;
	int id, fr, n, chunk, local, idx, ox, oy;

	id = f->clip_id;
	if (id < 0 || id >= DEX_CLIP_N)
		id = DEX_IDLE;
	n = DEX_CLIP[id].count;
	fr = f->clip_f;
	if (fr < 0)
		fr = 0;
	if (n > 0 && fr >= n)
		fr = n - 1;
	chunk = DEX_CLIP[id].chunk;
	if (chunk < 1)
		chunk = 1;
	local = fr % chunk;
	sh = find_sheet(sheet_key(id, fr / chunk, f->twin));
	if (!sh)
		sh = find_sheet(sheet_key(id, fr / chunk, 0));
	if (!sh)
		return;
	img = C2D_SpriteSheetGetImage(sh, local);
	if (!img.subtex)
		return;
	idx = DEX_OFF_BASE[id] + fr;
	ox = DEX_OX[idx];
	oy = DEX_OY[idx];
	rx = f->x + f->w * 0.5f + parallax;
	ry = f->y + f->h;
	sx = f->face >= 0 ? -1.0f : 1.0f;
	if (sx >= 0.0f)
		x = rx - (float)ox;
	else
		x = rx + (float)ox;
	y = ry - (float)oy;
	C2D_DrawImageAt(img, x, y, 0.5f, NULL, sx, 1.0f);
}

#else

int meg_clip_init(void) { return 0; }
void meg_clip_fini(void) {}
int meg_clip_ok(int ch) { (void)ch; return 0; }
void meg_clip_tick(Fighter *f, float dt) { (void)f; (void)dt; }
void meg_clip_draw(const Fighter *f, float parallax) { (void)f; (void)parallax; }

#endif
