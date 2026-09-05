#include "exo/platform.h"
#include "exo/render.h"
#include "binds.h"

#include <citro2d.h>
#include <stdio.h>

#define GROUND_Y 200.0f
#define BOX_W    28.0f
#define BOX_H    40.0f
#define WALK     140.0f

enum {
	UI_PLAY = 0,
	UI_MENU,
	UI_CAPTURE
};

#define ROW_ACT0   0
#define ROW_ACTN   MEG_ACT_COUNT
#define ROW_JUMP_A MEG_ACT_COUNT
#define ROW_RESET  (MEG_ACT_COUNT + 1)
#define ROW_BACK   (MEG_ACT_COUNT + 2)
#define ROW_MAX    (ROW_BACK + 1)

static float g_x = 80.0f;
static float g_y = GROUND_Y - BOX_H;
static int   g_face = 1;
static int   g_paused;
static int   g_ui = UI_PLAY;
static int   g_row;
static int   g_saved_ok = 1;

static void draw_eye(ExoEye eye)
{
	float px;
	u32 col;

	exo_render_eye(eye, C2D_Color32(28, 36, 56, 255));

	px = exo_parallax(4.0f, eye);
	C2D_DrawRectSolid(0.0f + px, GROUND_Y, 0.4f, 400.0f, 40.0f,
	                  C2D_Color32(46, 72, 58, 255));

	px = exo_parallax(8.0f, eye);
	if (meg_held(MEG_ACT_GUARD))
		col = C2D_Color32(80, 140, 220, 255);
	else if (meg_held(MEG_ACT_LIGHT))
		col = C2D_Color32(240, 220, 80, 255);
	else if (meg_held(MEG_ACT_HEAVY))
		col = C2D_Color32(220, 80, 70, 255);
	else if (g_paused || g_ui != UI_PLAY)
		col = C2D_Color32(120, 120, 120, 255);
	else
		col = C2D_Color32(220, 196, 72, 255);
	C2D_DrawRectSolid(g_x + px, g_y, 0.5f, BOX_W, BOX_H, col);

	exo_top_text(200.0f, 8.0f, 0.52f, C2D_Color32(240, 240, 240, 255),
	             "BATTLE IN MEGAVILLE 3D");
	exo_top_text(200.0f, 28.0f, 0.40f, C2D_Color32(180, 200, 220, 255),
	             "SELECT = controls");
}

static void draw_play_hud(void)
{
	char buf[48];
	char jl[16], ll[16], hl[16];

	meg_mask_label(meg_binds()->mask[MEG_ACT_JUMP], jl, sizeof jl);
	meg_mask_label(meg_binds()->mask[MEG_ACT_LIGHT], ll, sizeof ll);
	meg_mask_label(meg_binds()->mask[MEG_ACT_HEAVY], hl, sizeof hl);

	exo_render_bottom(C2D_Color32(16, 16, 24, 255));
	exo_text_begin();
	exo_text(8, 8, 0.5f, C2D_Color32(255, 255, 255, 255), "PLAY");
	snprintf(buf, sizeof buf, "L %s  H %s  J %s", ll, hl, jl);
	exo_text(8, 32, 0.45f, C2D_Color32(200, 220, 255, 255), buf);
	snprintf(buf, sizeof buf, "L=%d H=%d J=%d G=%d",
	         meg_held(MEG_ACT_LIGHT), meg_held(MEG_ACT_HEAVY),
	         meg_held(MEG_ACT_JUMP), meg_held(MEG_ACT_GUARD));
	exo_text(8, 56, 0.5f, C2D_Color32(255, 220, 120, 255), buf);
	exo_text(8, 88, 0.5f, C2D_Color32(160, 160, 180, 255),
	         g_paused ? "PAUSED" : "MOVE THE BOX");
	exo_text(8, 210, 0.4f, C2D_Color32(120, 120, 140, 255),
	         "SELECT options   HOME exit");
}

static void draw_menu(void)
{
	int i;
	char line[40];
	char lab[20];
	MegBinds *b = meg_binds();
	u32 col;

	exo_render_bottom(C2D_Color32(12, 14, 22, 255));
	exo_text_begin();
	exo_text(8, 4, 0.45f, C2D_Color32(255, 255, 255, 255),
	         g_ui == UI_CAPTURE ? "PRESS A BUTTON" : "CONTROLS");

	for (i = 0; i < ROW_MAX; ++i) {
		int y = 22 + i * 16;
		col = (i == g_row) ? C2D_Color32(255, 220, 90, 255)
		                   : C2D_Color32(180, 190, 200, 255);
		if (i < ROW_ACTN) {
			meg_mask_label(b->mask[i], lab, sizeof lab);
			snprintf(line, sizeof line, "%s  %s", meg_act_name((MegAct)i), lab);
		} else if (i == ROW_JUMP_A) {
			snprintf(line, sizeof line, "JUMP ON A   %s",
			         b->jump_on_a ? "ON" : "OFF");
		} else if (i == ROW_RESET) {
			snprintf(line, sizeof line, "RESET DEFAULTS");
		} else {
			snprintf(line, sizeof line, "BACK");
		}
		exo_text(8, y, 0.4f, col, line);
	}
	exo_text(8, 220, 0.35f, C2D_Color32(110, 110, 130, 255),
	         g_saved_ok ? "A edit  B back" : "SAVE FAIL");
}

static void tick_play(float dt)
{
	float ax = 0.0f;

	if (meg_down(MEG_ACT_PAUSE))
		g_paused ^= 1;

	if (g_paused)
		return;

	if (meg_held(MEG_ACT_LEFT))
		ax -= 1.0f;
	if (meg_held(MEG_ACT_RIGHT))
		ax += 1.0f;
	g_x += ax * WALK * dt;
	if (ax < -0.2f)
		g_face = -1;
	if (ax > 0.2f)
		g_face = 1;
	if (g_x < 8.0f)
		g_x = 8.0f;
	if (g_x > 400.0f - BOX_W - 8.0f)
		g_x = 400.0f - BOX_W - 8.0f;

	if (meg_held(MEG_ACT_JUMP))
		g_y = GROUND_Y - BOX_H - 18.0f;
	else
		g_y = GROUND_Y - BOX_H;
	(void)g_face;
}

static void activate_row(void)
{
	if (g_row < ROW_ACTN) {
		g_ui = UI_CAPTURE;
		return;
	}
	if (g_row == ROW_JUMP_A) {
		meg_set_jump_on_a(!meg_binds()->jump_on_a);
		g_saved_ok = meg_binds_save();
		return;
	}
	if (g_row == ROW_RESET) {
		meg_binds_reset();
		g_saved_ok = meg_binds_save();
		return;
	}
	g_ui = UI_PLAY;
}

static void tick_menu(void)
{
	uint32_t got;

	if (g_ui == UI_CAPTURE) {
		if (meg_raw_down(EXO_BTN_SELECT)) {
			g_ui = UI_MENU;
			return;
		}
		got = meg_capture_button();
		if (got && got != EXO_BTN_SELECT) {
			meg_set_bind((MegAct)g_row, got);
			g_saved_ok = meg_binds_save();
			g_ui = UI_MENU;
		}
		return;
	}

	if (meg_raw_down(EXO_BTN_UP) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_DOWN) && g_row < ROW_MAX - 1)
		g_row++;
	if (meg_raw_down(EXO_BTN_A))
		activate_row();
	if (meg_raw_down(EXO_BTN_B) || meg_raw_down(EXO_BTN_SELECT))
		g_ui = UI_PLAY;
}

int main(void)
{
	if (!exo_init())
		return 1;
	meg_binds_init();

	while (exo_frame_begin()) {
		float dt = exo_dt();

		meg_binds_poll();

		if (g_ui == UI_PLAY && meg_raw_down(EXO_BTN_SELECT)) {
			g_ui = UI_MENU;
			g_paused = 1;
		} else if (g_ui == UI_PLAY) {
			tick_play(dt);
		} else {
			tick_menu();
		}

		exo_render_begin();
		draw_eye(EXO_EYE_LEFT);
		draw_eye(EXO_EYE_RIGHT);
		if (g_ui == UI_PLAY)
			draw_play_hud();
		else
			draw_menu();
		exo_render_end();
		exo_frame_end();
	}

	exo_shutdown();
	return 0;
}
