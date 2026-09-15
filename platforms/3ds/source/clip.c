#include "clip.h"
#include "clip_table.h"
#include "roster.h"

#include <citro2d.h>
#include <stdio.h>
#include <string.h>

#if MEG_DEX_CLIP

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
	DEX_DAMAGE,
	DEX_EXTRA,
	DEX_CLIP_N
};

#define CACHE 4

typedef struct {
	const char *prefix;
	int count;
	int loop;
	int chunk;
} DexClip;

static const DexClip DEX_CLIP[] = { DEX_CLIP_DATA };

static C2D_SpriteSheet g_sh[CACHE];
static int g_id[CACHE];
static int g_age[CACHE];
static int g_tick;
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

static C2D_SpriteSheet load_clip(int id, int fr)
{
	char path[64];
	int i, worst, wage, chunk, sheet;

	if (id < 0 || id >= DEX_CLIP_N)
		id = 0;
	if (id >= (int)(sizeof DEX_CLIP / sizeof DEX_CLIP[0]))
		id = 0;
	chunk = DEX_CLIP[id].chunk;
	if (chunk < 1)
		chunk = 1;
	sheet = fr / chunk;
	snprintf(path, sizeof path, "%s_%d.t3x", DEX_CLIP[id].prefix, sheet);
	for (i = 0; i < CACHE; ++i) {
		if (g_id[i] == id * 256 + sheet && g_sh[i]) {
			g_age[i] = ++g_tick;
			return g_sh[i];
		}
	}
	worst = 0;
	wage = g_age[0];
	for (i = 1; i < CACHE; ++i) {
		if (g_age[i] < wage) {
			wage = g_age[i];
			worst = i;
		}
	}
	if (g_sh[worst])
		C2D_SpriteSheetFree(g_sh[worst]);
	g_sh[worst] = C2D_SpriteSheetLoad(path);
	g_id[worst] = id * 256 + sheet;
	g_age[worst] = ++g_tick;
	return g_sh[worst];
}

int meg_clip_init(void)
{
	int i;

	g_ok = 0;
	g_tick = 1;
	for (i = 0; i < CACHE; ++i) {
		g_sh[i] = NULL;
		g_id[i] = -1;
		g_age[i] = 0;
	}
	if (load_clip(DEX_IDLE, 0))
		g_ok = 1;
	return g_ok;
}

void meg_clip_fini(void)
{
	int i;

	for (i = 0; i < CACHE; ++i) {
		if (g_sh[i])
			C2D_SpriteSheetFree(g_sh[i]);
		g_sh[i] = NULL;
		g_id[i] = -1;
	}
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
	while (f->clip_t >= 0.04f) {
		f->clip_t -= 0.04f;
		f->clip_f++;
		loop = DEX_CLIP[id].loop;
		if (f->clip_f >= n) {
			if (loop < 0)
				f->clip_f = n - 1;
			else
				f->clip_f = loop;
		}
	}
}

void meg_clip_draw(const Fighter *f, float parallax)
{
	C2D_SpriteSheet sh;
	C2D_Image img;
	C2D_ImageTint tint;
	float w, h, x, y, sx;
	int id, fr, n, chunk, local;

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
	sh = load_clip(id, fr);
	if (!sh)
		return;
	img = C2D_SpriteSheetGetImage(sh, local);
	if (!img.subtex)
		return;
	w = img.subtex->width;
	h = img.subtex->height;
	sx = f->face >= 0 ? -1.0f : 1.0f;
	x = f->x + parallax + (f->w - w) * 0.5f;
	y = f->y + f->h - h;
	if (sx < 0.0f)
		x += w;
	if (f->twin) {
		C2D_PlainImageTint(&tint, C2D_Color32(80, 200, 255, 255), 0.55f);
		C2D_DrawImageAt(img, x, y, 0.5f, &tint, sx, 1.0f);
	} else {
		C2D_DrawImageAt(img, x, y, 0.5f, NULL, sx, 1.0f);
	}
}

#else

int meg_clip_init(void) { return 0; }
void meg_clip_fini(void) {}
int meg_clip_ok(int ch) { (void)ch; return 0; }
void meg_clip_tick(Fighter *f, float dt) { (void)f; (void)dt; }
void meg_clip_draw(const Fighter *f, float parallax) { (void)f; (void)parallax; }

#endif
