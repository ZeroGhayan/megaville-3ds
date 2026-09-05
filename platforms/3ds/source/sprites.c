#include "sprites.h"

#include <citro2d.h>

enum {
	SPR_IDLE = 0,
	SPR_LAND,
	SPR_JUMP,
	SPR_WALK,
	SPR_DASH,
	SPR_HEAVY,
	SPR_LIGHT,
	SPR_LIGHT2,
	SPR_COUNT
};

static C2D_SpriteSheet g_sheet;
static C2D_Image g_img[SPR_COUNT];
static int g_ok;

int meg_sprites_init(void)
{
	int i;

	g_sheet = C2D_SpriteSheetLoad("romfs:/gfx/blossom.t3x");
	if (!g_sheet)
		return 0;
	for (i = 0; i < SPR_COUNT; ++i)
		g_img[i] = C2D_SpriteSheetGetImage(g_sheet, i);
	g_ok = 1;
	return 1;
}

void meg_sprites_fini(void)
{
	if (g_sheet)
		C2D_SpriteSheetFree(g_sheet);
	g_sheet = NULL;
	g_ok = 0;
}

int meg_sprites_ok(void)
{
	return g_ok;
}

static int pick(const Fighter *f)
{
	if (f->phase == FIGHT_JUMP)
		return SPR_JUMP;
	if (f->phase == FIGHT_GUARD)
		return SPR_LAND;
	if (f->phase == FIGHT_HIT)
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

void meg_draw_fighter(const Fighter *f, float parallax)
{
	C2D_Image img;
	float w, h, x, y, sx;

	if (!g_ok)
		return;
	img = g_img[pick(f)];
	w = img.subtex->width;
	h = img.subtex->height;
	sx = f->face >= 0 ? 1.0f : -1.0f;
	x = f->x + parallax + (f->w - w) * 0.5f;
	y = f->y + f->h - h;
	if (sx < 0.0f)
		x += w;
	C2D_DrawImageAt(img, x, y, 0.5f, NULL, sx, 1.0f);
}
