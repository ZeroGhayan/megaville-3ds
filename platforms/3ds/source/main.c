#include "exo/platform.h"
#include "exo/render.h"
#include "binds.h"
#include "sprites.h"

#include <citro2d.h>
#include <stdio.h>

#define GROUND_Y 200.0f
#define INTRO_LEN 240

enum {
	UI_SELECT = 0,
	UI_PLAY,
	UI_MENU,
	UI_CAPTURE
};

#define ROW_ACTN   MEG_ACT_COUNT
#define ROW_JUMP_A MEG_ACT_COUNT
#define ROW_RESET  (MEG_ACT_COUNT + 1)
#define ROW_BACK   (MEG_ACT_COUNT + 2)
#define ROW_MAX    (ROW_BACK + 1)

static Fighter g_p1, g_p2;
static int g_paused;
static int g_ui = UI_SELECT;
static int g_row;
static int g_saved_ok = 1;
static int g_intro = INTRO_LEN;
static int g_pick = 0;

static const char *CHAR_NAME[3] = { "BLOSSOM", "BUBBLES", "BUTTERCUP" };

static void apply_chars(void)
{
	g_p1.ch = g_pick;
	g_p2.ch = (g_pick + 1) % 3;
}

static void round_reset(void)
{
	fight_reset(&g_p1, &g_p2);
	apply_chars();
	g_intro = INTRO_LEN;
	g_paused = 0;
}

static u32 phase_col(const Fighter *f)
{
	if (f->phase == FIGHT_GUARD)
		return C2D_Color32(80, 140, 220, 255);
	if (f->phase == FIGHT_ACTIVE)
		return C2D_Color32(240, 220, 80, 255);
	if (f->phase == FIGHT_STARTUP)
		return C2D_Color32(220, 160, 60, 255);
	if (f->phase == FIGHT_HIT)
		return C2D_Color32(220, 80, 70, 255);
	if (f->hp <= 0)
		return C2D_Color32(40, 40, 40, 255);
	return f->ai ? C2D_Color32(180, 90, 160, 255)
	             : C2D_Color32(220, 196, 72, 255);
}

static void draw_fighter(const Fighter *f, ExoEye eye)
{
	float px = exo_parallax(8.0f, eye);

	if (meg_sprites_ok()) {
		meg_draw_fighter(f, px);
		return;
	}
	C2D_DrawRectSolid(f->x + px, f->y, 0.5f, f->w, f->h, phase_col(f));
	if (f->phase == FIGHT_ACTIVE) {
		float hx = f->face > 0 ? f->x + f->w : f->x - 22.0f;
		C2D_DrawRectSolid(hx + px, f->y + 8.0f, 0.6f, 22.0f, 16.0f,
		                  C2D_Color32(255, 255, 255, 180));
	}
}

static void draw_eye(ExoEye eye)
{
	float px;

	exo_render_eye(eye, C2D_Color32(28, 36, 56, 255));
	px = exo_parallax(4.0f, eye);
	C2D_DrawRectSolid(0.0f + px, GROUND_Y, 0.4f, 400.0f, 40.0f,
	                  C2D_Color32(46, 72, 58, 255));
	if (g_ui != UI_SELECT) {
		draw_fighter(&g_p1, eye);
		draw_fighter(&g_p2, eye);
	}
	exo_top_text(200.0f, 8.0f, 0.50f, C2D_Color32(240, 240, 240, 255),
	             "BATTLE IN MEGAVILLE 3D");
	if (g_ui == UI_SELECT)
		exo_top_text(200.0f, 100.0f, 0.7f, C2D_Color32(255, 220, 80, 255),
		             "SELECT");
	else if (g_intro > 180)
		exo_top_text(200.0f, 100.0f, 1.4f, C2D_Color32(255, 220, 80, 255), "3");
	else if (g_intro > 120)
		exo_top_text(200.0f, 100.0f, 1.4f, C2D_Color32(255, 220, 80, 255), "2");
	else if (g_intro > 60)
		exo_top_text(200.0f, 100.0f, 1.4f, C2D_Color32(255, 220, 80, 255), "1");
	else if (g_intro > 0)
		exo_top_text(200.0f, 100.0f, 0.9f, C2D_Color32(255, 80, 80, 255), "FIGHT");
	else
		exo_top_text(200.0f, 26.0f, 0.38f, C2D_Color32(180, 200, 220, 255),
		             "Y light  X heavy  SELECT binds");
}

static void draw_select(void)
{
	int i;
	u32 col;

	exo_render_bottom(C2D_Color32(16, 16, 28, 255));
	exo_text_begin();
	exo_text(8, 8, 0.5f, C2D_Color32(255, 255, 255, 255), "CHOOSE FIGHTER");
	for (i = 0; i < 3; ++i) {
		col = (i == g_pick) ? C2D_Color32(255, 220, 90, 255)
		                    : C2D_Color32(160, 170, 180, 255);
		exo_text(16.0f + (float)i * 100.0f, 40, 0.42f, col, CHAR_NAME[i]);
		if (meg_sprites_ok_ch(i))
			meg_draw_idle(i, 48.0f + (float)i * 100.0f, 150.0f, 1.4f);
		else
			exo_bot_rect(24.0f + (float)i * 100.0f, 100, 40, 50,
			             i == g_pick ? C2D_Color32(220, 196, 72, 255)
			                         : C2D_Color32(80, 80, 90, 255));
	}
	exo_text(8, 210, 0.4f, C2D_Color32(120, 120, 140, 255),
	         "LEFT/RIGHT  A fight");
}

static void tick_select(void)
{
	if (meg_raw_down(EXO_BTN_LEFT) && g_pick > 0)
		g_pick--;
	if (meg_raw_down(EXO_BTN_RIGHT) && g_pick < 2)
		g_pick++;
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_START)) {
		round_reset();
		g_ui = UI_PLAY;
	}
}

static void draw_play_hud(void)
{
	char buf[48];

	exo_render_bottom(C2D_Color32(16, 16, 24, 255));
	exo_text_begin();
	snprintf(buf, sizeof buf, "P1 %d   CPU %d", g_p1.hp, g_p2.hp);
	exo_text(8, 8, 0.5f, C2D_Color32(255, 255, 255, 255), buf);
	exo_bot_rect(8, 32, (float)g_p1.hp * 1.4f, 10,
	             C2D_Color32(220, 196, 72, 255));
	exo_bot_rect(8, 48, (float)g_p2.hp * 1.4f, 10,
	             C2D_Color32(180, 90, 160, 255));
	{
		int i;
		for (i = 0; i < 3; ++i) {
			u32 c = g_p1.dashes >= (i + 1) * 100
			            ? C2D_Color32(80, 200, 255, 255)
			            : C2D_Color32(40, 50, 60, 255);
			exo_bot_rect(8.0f + (float)i * 22.0f, 64, 18, 8, c);
		}
	}
	exo_text(8, 72, 0.45f, C2D_Color32(200, 220, 255, 255),
	         g_intro > 0 ? "GET READY" : (g_paused ? "PAUSED" : "LLL  LLH  LH"));
	exo_text(8, 210, 0.4f, C2D_Color32(120, 120, 140, 255),
	         (g_p1.hp <= 0 || g_p2.hp <= 0)
	             ? "Y rematch  B roster"
	             : "SELECT options   HOME exit");
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
	if (meg_down(MEG_ACT_PAUSE) && g_intro <= 0)
		g_paused ^= 1;
	if (g_paused)
		return;
	if (g_intro > 0) {
		g_intro--;
		return;
	}
	if (g_p1.hp <= 0 || g_p2.hp <= 0) {
		if (meg_down(MEG_ACT_LIGHT) || meg_down(MEG_ACT_HEAVY))
			round_reset();
		if (meg_raw_down(EXO_BTN_B))
			g_ui = UI_SELECT;
		return;
	}
	fight_control(&g_p1, &g_p2);
	fight_control(&g_p2, &g_p1);
	fight_physics(&g_p1, dt);
	fight_physics(&g_p2, dt);
	fight_hits(&g_p1, &g_p2);
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
	round_reset();
	meg_sprites_init();

	while (exo_frame_begin()) {
		float dt = exo_dt();

		meg_binds_poll();

		if (g_ui == UI_SELECT) {
			tick_select();
		} else if (g_ui == UI_PLAY && meg_raw_down(EXO_BTN_SELECT)) {
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
		if (g_ui == UI_SELECT)
			draw_select();
		else if (g_ui == UI_PLAY)
			draw_play_hud();
		else
			draw_menu();
		exo_render_end();
		exo_frame_end();
	}

	meg_sprites_fini();
	exo_shutdown();
	return 0;
}
