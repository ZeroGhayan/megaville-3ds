#include "sprites.h"

#include <citro2d.h>
#include <stdio.h>

enum {
	SPR_IDLE = 0,
	SPR_LAND,
	SPR_JUMP,
	SPR_WALK,
	SPR_DASH,
	SPR_HEAVY,
	SPR_LIGHT,
	SPR_LIGHT2,
	SPR_HIT,
	SPR_KO,
	SPR_COUNT
};

#define NCHAR 3

static const char *SHEET_PATH[NCHAR] = {
	"romfs:/gfx/blossom.t3x",
	"romfs:/gfx/bubbles.t3x",
	"romfs:/gfx/buttercup.t3x"
};

static C2D_SpriteSheet g_sheet[NCHAR];
static C2D_Image g_img[NCHAR][SPR_COUNT];
static int g_ok[NCHAR];
static int g_any;

int meg_sprites_init(void)
{
	int c, i;

	g_any = 0;
	for (c = 0; c < NCHAR; ++c) {
		g_sheet[c] = C2D_SpriteSheetLoad(SHEET_PATH[c]);
		g_ok[c] = 0;
		if (!g_sheet[c])
			continue;
		for (i = 0; i < SPR_COUNT; ++i)
			g_img[c][i] = C2D_SpriteSheetGetImage(g_sheet[c], i);
		g_ok[c] = 1;
		g_any = 1;
	}
	return g_any;
}

void meg_sprites_fini(void)
{
	int c;

	for (c = 0; c < NCHAR; ++c) {
		if (g_sheet[c])
			C2D_SpriteSheetFree(g_sheet[c]);
		g_sheet[c] = NULL;
		g_ok[c] = 0;
	}
	g_any = 0;
}

int meg_sprites_ok(void)
{
	return g_any;
}

int meg_sprites_ok_ch(int ch)
{
	if (ch < 0 || ch >= NCHAR)
		return 0;
	return g_ok[ch];
}

static int pick(const Fighter *f)
{
	if (f->hp <= 0)
		return SPR_KO;
	if (f->phase == FIGHT_HIT)
		return SPR_HIT;
	if (f->phase == FIGHT_JUMP)
		return SPR_JUMP;
	if (f->phase == FIGHT_GUARD)
		return SPR_LAND;
	if (f->phase == FIGHT_STARTUP || f->phase == FIGHT_ACTIVE ||
	    f->phase == FIGHT_RECOVERY) {
		if (f->move == 1 || f->move == 3)
			return SPR_HEAVY;
		if (f->move == 2 || f->move == 4)
			return SPR_LIGHT2;
		return SPR_LIGHT;
	}
	if (f->vx > 20.0f || f->vx < -20.0f)
		return SPR_WALK;
	return SPR_IDLE;
}

static int clamp_ch(int ch)
{
	if (ch < 0 || ch >= NCHAR || !g_ok[ch])
		return g_ok[0] ? 0 : (g_ok[1] ? 1 : 2);
	return ch;
}

void meg_draw_fighter(const Fighter *f, float parallax)
{
	C2D_Image img;
	float w, h, x, y, sx;
	int ch;

	if (!g_any)
		return;
	ch = clamp_ch(f->ch);
	if (!g_ok[ch])
		return;
	img = g_img[ch][pick(f)];
	if (!img.subtex)
		return;
	w = img.subtex->width;
	h = img.subtex->height;
	sx = f->face >= 0 ? -1.0f : 1.0f;
	x = f->x + parallax + (f->w - w) * 0.5f;
	y = f->y + f->h - h;
	if (sx < 0.0f)
		x += w;
	C2D_DrawImageAt(img, x, y, 0.5f, NULL, sx, 1.0f);
}

void meg_draw_idle(int ch, float x, float y, float scale)
{
	C2D_Image img;
	float w, h;

	ch = clamp_ch(ch);
	if (!g_ok[ch])
		return;
	img = g_img[ch][SPR_IDLE];
	if (!img.subtex)
		return;
	w = img.subtex->width * scale;
	h = img.subtex->height * scale;
	C2D_DrawImageAt(img, x - w * 0.5f, y - h, 0.5f, NULL, -scale, scale);
}
