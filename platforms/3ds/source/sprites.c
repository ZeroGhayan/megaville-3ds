#include "sprites.h"
#include "roster.h"
#include "clip.h"
#include "spr_off.h"

#include <citro2d.h>
#include <tex3ds.h>
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

static const char *SHEET_FLIP[CH_COUNT] = {
	"romfs:/gfx/blossom_f.t3x",
	"romfs:/gfx/bubbles_f.t3x",
	"romfs:/gfx/buttercup_f.t3x",
	"romfs:/gfx/bell_f.t3x",
	"romfs:/gfx/dexter_f.t3x",
	"romfs:/gfx/rowdy_f.t3x",
	"romfs:/gfx/zim_f.t3x",
	"romfs:/gfx/shira_f.t3x"
};

static C2D_SpriteSheet g_sheet[CH_COUNT];
static C2D_SpriteSheet g_sheet_t[CH_COUNT];
static C2D_SpriteSheet g_sheet_f[CH_COUNT];
static C2D_Image g_img[CH_COUNT][SPR_COUNT];
static C2D_Image g_img_t[CH_COUNT][SPR_COUNT];
static C2D_Image g_img_f[CH_COUNT][SPR_COUNT];
static C2D_SpriteSheet g_pic;
static C2D_SpriteSheet g_picf;
static C2D_SpriteSheet g_sky;
static C2D_SpriteSheet g_terrain;
static C2D_SpriteSheet g_lamp;
static C2D_Image g_picimg[CH_COUNT];
static C2D_Image g_pictwin[CH_COUNT];
static C2D_Image g_picflip[CH_COUNT];
static int g_ok[CH_COUNT];
static int g_ok_t[CH_COUNT];
static int g_ok_f[CH_COUNT];
static int g_any;
static int g_picok;
static int g_picfok;

int meg_sprites_init(void)
{
	int c, i;

	g_any = 0;
	for (c = 0; c < CH_COUNT; ++c) {
		g_sheet[c] = NULL;
		g_sheet_t[c] = NULL;
		g_sheet_f[c] = NULL;
		g_ok[c] = 0;
		g_ok_t[c] = 0;
		g_ok_f[c] = 0;
		g_sheet[c] = C2D_SpriteSheetLoad(SHEET_PATH[c]);
		if (!g_sheet[c])
			continue;
		for (i = 0; i < SPR_COUNT; ++i)
			g_img[c][i] = C2D_SpriteSheetGetImage(g_sheet[c], i);
		g_ok[c] = 1;
		g_any = 1;
		g_sheet_t[c] = C2D_SpriteSheetLoad(SHEET_TWIN[c]);
		if (g_sheet_t[c]) {
			for (i = 0; i < SPR_COUNT; ++i)
				g_img_t[c][i] = C2D_SpriteSheetGetImage(g_sheet_t[c], i);
			g_ok_t[c] = 1;
		}
		g_sheet_f[c] = C2D_SpriteSheetLoad(SHEET_FLIP[c]);
		if (g_sheet_f[c]) {
			for (i = 0; i < SPR_COUNT; ++i)
				g_img_f[c][i] = C2D_SpriteSheetGetImage(g_sheet_f[c], i);
			g_ok_f[c] = 1;
		}
	}
	g_pic = C2D_SpriteSheetLoad("romfs:/gfx/pic.t3x");
	g_picf = C2D_SpriteSheetLoad("romfs:/gfx/pic_f.t3x");
	g_sky = C2D_SpriteSheetLoad("romfs:/gfx/sky.t3x");
	g_terrain = C2D_SpriteSheetLoad("romfs:/gfx/terrain.t3x");
	g_lamp = C2D_SpriteSheetLoad("romfs:/gfx/lamp.t3x");
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
		if (g_sheet_f[c])
			C2D_SpriteSheetFree(g_sheet_f[c]);
		g_sheet[c] = NULL;
		g_sheet_t[c] = NULL;
		g_sheet_f[c] = NULL;
		g_ok[c] = 0;
		g_ok_t[c] = 0;
		g_ok_f[c] = 0;
	}
	g_any = 0;
	if (g_pic)
		C2D_SpriteSheetFree(g_pic);
	if (g_picf)
		C2D_SpriteSheetFree(g_picf);
	if (g_sky)
		C2D_SpriteSheetFree(g_sky);
	if (g_terrain)
		C2D_SpriteSheetFree(g_terrain);
	if (g_lamp)
		C2D_SpriteSheetFree(g_lamp);
	g_pic = NULL;
	g_picf = NULL;
	g_sky = NULL;
	g_terrain = NULL;
	g_lamp = NULL;
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

void meg_blit(C2D_Image img, float rx, float ry, float ox, float oy,
              int face, float sc)
{
	float x, y;

	if (!img.subtex)
		return;
	if (sc < 0.25f)
		sc = 0.25f;
	if (sc > 2.5f)
		sc = 2.5f;
	y = ry - oy * sc;
	/* Fonte olha à esquerda. face>0 → scaleX negativo, pés em rx. */
	if (face > 0) {
		x = rx + ox * sc;
		C2D_DrawImageAt(img, x, y, 0.5f, NULL, -sc, sc);
	} else {
		x = rx - ox * sc;
		C2D_DrawImageAt(img, x, y, 0.5f, NULL, sc, sc);
	}
}

void meg_draw_fighter(const Fighter *f, float parallax)
{
	C2D_Image img;
	float w, h, rx, ry, ox, oy, sc;
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
	memset(&img, 0, sizeof img);
	if (f->twin && g_ok_t[ch] && g_img_t[ch][fr].subtex)
		img = g_img_t[ch][fr];
	else
		img = g_img[ch][fr];
	if (!img.subtex)
		return;
	w = img.subtex->width;
	h = img.subtex->height;
	/* Pés = fundo da pose. spr_off do sliced estava inconsistente. */
	ox = w * 0.5f;
	oy = h;
	rx = f->x + f->w * 0.5f + parallax;
	ry = f->y + f->h;
	sc = (oy > 1.0f) ? (f->h / oy) : 1.0f;
	meg_blit(img, rx, ry, ox, oy, f->face, sc);
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

void meg_draw_stage(float parallax)
{
	C2D_Image img;

	if (g_sky) {
		img = C2D_SpriteSheetGetImage(g_sky, 0);
		if (img.subtex && img.subtex->width >= 200 &&
		    img.subtex->height >= 80)
			C2D_DrawImageAt(img, parallax * 0.15f, 0.0f, 0.15f,
			                NULL, 1.0f, 1.0f);
	}
	/* city.t3x no dump é um rect preto — não desenhar. */
	if (g_terrain) {
		img = C2D_SpriteSheetGetImage(g_terrain, 0);
		if (img.subtex && img.subtex->width >= 200)
			C2D_DrawImageAt(img, parallax, 200.0f - img.subtex->height + 8.0f,
			                0.3f, NULL, 1.0f, 1.0f);
		else
			C2D_DrawRectSolid(parallax, 200.0f, 0.3f, 400.0f, 40.0f,
			                  C2D_Color32(46, 72, 58, 255));
	} else {
		C2D_DrawRectSolid(parallax, 200.0f, 0.3f, 400.0f, 40.0f,
		                  C2D_Color32(46, 72, 58, 255));
	}
	if (g_lamp) {
		img = C2D_SpriteSheetGetImage(g_lamp, 0);
		if (img.subtex)
			C2D_DrawImageAt(img, 320.0f + parallax,
			                200.0f - img.subtex->height,
			                0.35f, NULL, 1.0f, 1.0f);
	}
}

