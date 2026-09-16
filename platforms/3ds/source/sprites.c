#include "sprites.h"
#include "roster.h"
#include "clip.h"
#include "spr_off.h"

#include <citro2d.h>
#include <string.h>

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

static const char *SHEET_PATH[CH_COUNT] = {
	"romfs:/gfx/blossom.t3x",
	"romfs:/gfx/bubbles.t3x",
	"romfs:/gfx/buttercup.t3x",
	"romfs:/gfx/bell.t3x",
	"romfs:/gfx/dexter.t3x",
	"romfs:/gfx/rowdy.t3x",
	"romfs:/gfx/zim.t3x",
	"romfs:/gfx/shira.t3x"
};

static const char *SHEET_TWIN[CH_COUNT] = {
	"romfs:/gfx/blossom_t.t3x",
	"romfs:/gfx/bubbles_t.t3x",
	"romfs:/gfx/buttercup_t.t3x",
	"romfs:/gfx/bell_t.t3x",
	"romfs:/gfx/dexter_t.t3x",
	"romfs:/gfx/rowdy_t.t3x",
	"romfs:/gfx/zim_t.t3x",
	"romfs:/gfx/shira_t.t3x"
};

static C2D_SpriteSheet g_sheet[CH_COUNT];
static C2D_SpriteSheet g_sheet_t[CH_COUNT];
static C2D_Image g_img[CH_COUNT][SPR_COUNT];
static C2D_Image g_img_t[CH_COUNT][SPR_COUNT];
static C2D_SpriteSheet g_pic;
static C2D_SpriteSheet g_picf;
static C2D_Image g_picimg[CH_COUNT];
static C2D_Image g_pictwin[CH_COUNT];
static C2D_Image g_picflip[CH_COUNT];
static int g_ok[CH_COUNT];
static int g_ok_t[CH_COUNT];
static int g_any;
static int g_picok;
static int g_picfok;

/* ajuste fino depois do crop manual */
static float g_offx[CH_COUNT];
static float g_offy[CH_COUNT];

int meg_sprites_init(void)
{
	int c, i;

	g_any = 0;
	for (c = 0; c < CH_COUNT; ++c) {
		g_sheet[c] = NULL;
		g_sheet_t[c] = NULL;
		g_ok[c] = 0;
		g_ok_t[c] = 0;
		g_sheet[c] = C2D_SpriteSheetLoad(SHEET_PATH[c]);
		if (!g_sheet[c])
			continue;
		for (i = 0; i < SPR_COUNT; ++i)
			g_img[c][i] = C2D_SpriteSheetGetImage(g_sheet[c], i);
		g_ok[c] = 1;
		g_any = 1;
		g_sheet_t[c] = C2D_SpriteSheetLoad(SHEET_TWIN[c]);
		g_ok_t[c] = 0;
		if (g_sheet_t[c]) {
			for (i = 0; i < SPR_COUNT; ++i)
				g_img_t[c][i] = C2D_SpriteSheetGetImage(g_sheet_t[c], i);
			g_ok_t[c] = 1;
		}
	}
	g_pic = C2D_SpriteSheetLoad("romfs:/gfx/pic.t3x");
	g_picf = C2D_SpriteSheetLoad("romfs:/gfx/pic_f.t3x");
	g_picok = 0;
	g_picfok = 0;
	if (g_pic) {
		for (c = 0; c < CH_COUNT; ++c) {
			g_picimg[c] = C2D_SpriteSheetGetImage(g_pic, c);
			g_pictwin[c] = C2D_SpriteSheetGetImage(g_pic, c + CH_COUNT);
		}
		g_picok = 1;
	}
	if (g_picf) {
		for (c = 0; c < CH_COUNT; ++c)
			g_picflip[c] = C2D_SpriteSheetGetImage(g_picf, c);
		g_picfok = 1;
	}
	return g_any;
}

void meg_sprites_fini(void)
{
	int c;

	for (c = 0; c < CH_COUNT; ++c) {
		if (g_sheet[c])
			C2D_SpriteSheetFree(g_sheet[c]);
		if (g_sheet_t[c])
			C2D_SpriteSheetFree(g_sheet_t[c]);
		g_sheet[c] = NULL;
		g_sheet_t[c] = NULL;
		g_ok[c] = 0;
		g_ok_t[c] = 0;
	}
	g_any = 0;
	if (g_pic)
		C2D_SpriteSheetFree(g_pic);
	if (g_picf)
		C2D_SpriteSheetFree(g_picf);
	g_pic = NULL;
	g_picf = NULL;
	g_picok = 0;
	g_picfok = 0;
}

int meg_sprites_ok(void)
{
	return g_any;
}

int meg_sprites_ok_ch(int ch)
{
	if (ch < 0 || ch >= CH_COUNT)
		return 0;
	return g_ok[ch];
}

void meg_tint_twin(C2D_ImageTint *t)
{
	(void)t;
}

static int pick(const Fighter *f)
{
	if (f->hp <= 0)
		return SPR_KO;
	if (f->phase == FIGHT_HIT)
		return SPR_HIT;
	if (f->phase == FIGHT_FROZEN)
		return SPR_HIT;
	if (f->phase == FIGHT_DASH)
		return SPR_DASH;
	if (f->phase == FIGHT_JUMP)
		return SPR_JUMP;
	if (f->phase == FIGHT_GUARD)
		return SPR_LAND;
	if (f->phase == FIGHT_STARTUP || f->phase == FIGHT_ACTIVE ||
	    f->phase == FIGHT_RECOVERY) {
		if (f->move == 5)
			return SPR_LIGHT2;
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
	if (ch < 0 || ch >= CH_COUNT || !g_ok[ch])
		return g_ok[0] ? 0 : (g_ok[1] ? 1 : 2);
	return ch;
}

void meg_draw_fighter(const Fighter *f, float parallax)
{
	C2D_Sprite spr;
	C2D_SpriteSheet sh;
	float rx, ry, ox, oy, w, h;
	int ch, fr;

	if (!g_any)
		return;
	if (meg_clip_ok(f->ch)) {
		meg_clip_draw(f, parallax);
		return;
	}
	ch = clamp_ch(f->ch);
	if (!g_ok[ch])
		return;
	fr = pick(f);
	if (f->twin && g_ok_t[ch] && g_sheet_t[ch])
		sh = g_sheet_t[ch];
	else
		sh = g_sheet[ch];
	if (!sh)
		return;
	C2D_SpriteFromSheet(&spr, sh, (size_t)fr);
	w = spr.image.subtex ? spr.image.subtex->width : 0;
	h = spr.image.subtex ? spr.image.subtex->height : 0;
	if (w < 1.0f || h < 1.0f)
		return;
	ox = w * 0.5f;
	oy = h;
	if (SPR_OX[ch][fr] || SPR_OY[ch][fr]) {
		ox = (float)SPR_OX[ch][fr];
		oy = (float)SPR_OY[ch][fr];
	}
	rx = f->x + f->w * 0.5f + parallax;
	ry = f->y + f->h;
	C2D_SpriteSetCenter(&spr, ox, oy);
	C2D_SpriteSetPos(&spr, rx, ry);
	C2D_SpriteSetScale(&spr, f->face >= 0 ? -1.0f : 1.0f, 1.0f);
	C2D_DrawSprite(&spr);
}

void meg_draw_idle(int ch, float x, float y, float scale)
{
	meg_draw_pic(ch, x, y, scale, 1, 0);
}

void meg_draw_pic(int ch, float x, float y, float scale, int face, int twin)
{
	C2D_Image img;
	float w, h, sx;
	int flip;

	if (ch < 0 || ch >= CH_COUNT)
		return;
	memset(&img, 0, sizeof img);
	flip = face >= 0;
	if (g_picok) {
		if (twin && g_pictwin[ch].subtex)
			img = g_pictwin[ch];
		else if (flip && g_picfok && g_picflip[ch].subtex)
			img = g_picflip[ch];
		else
			img = g_picimg[ch];
	}
	if (!img.subtex) {
		if (!g_ok[ch])
			return;
		img = g_img[ch][SPR_IDLE];
	}
	if (!img.subtex)
		return;
	w = img.subtex->width * scale;
	h = img.subtex->height * scale;
	/* flip baked in pic_f — scaleX nunca negativo (atlas UV no 3DS) */
	sx = scale;
	if (flip && !g_picfok)
		sx = -scale;
	x -= w * 0.5f;
	if (sx < 0.0f)
		x += w;
	C2D_DrawImageAt(img, x, y - h, 0.5f, NULL, sx, scale);
}

