#include "exo/platform.h"
#include "exo/render.h"
#include "binds.h"
#include "roster.h"
#include "menu.h"
#include "game.h"
#include "sprites.h"

#include <citro2d.h>
#include <stdio.h>

#define GROUND_Y 200.0f
#define INTRO_LEN 240

#define ROW_ACTN   MEG_ACT_COUNT
#define ROW_JUMP_A MEG_ACT_COUNT
#define ROW_RESET  (MEG_ACT_COUNT + 1)
#define ROW_BACK   (MEG_ACT_COUNT + 2)
#define ROW_MAX    (ROW_BACK + 1)

static Fighter g_p1, g_p2;
static int g_paused;
static int g_row;
static int g_saved_ok = 1;
static int g_intro = INTRO_LEN;
static int g_pick = 0;
static int g_binds_ingame;
static int g_vs_lock;
static int g_cont_n;
static int g_cont_t;
static int g_cont_row;

static void apply_chars(void)
{
	MegGame *g = meg_game();

	if (g->mode == MODE_STORY) {
		g_p1.ch = g->story_p1;
		g_p2.ch = meg_story_cpu(g->story_p1, g->story_level);
	} else {
		g_p1.ch = g_pick;
		g_p2.ch = (g_pick + 1) % CH_COUNT;
	}
	g_p1.twin = 0;
	g_p2.twin = (g_p1.ch == g_p2.ch);
}

static void round_reset(void)
{
	fight_reset(&g_p1, &g_p2);
	apply_chars();
	g_intro = INTRO_LEN;
	g_paused = 0;
}

static void goto_vs(void)
{
	apply_chars();
	g_vs_lock = 25;
	meg_set_screen(SCR_VS);
}

static void draw_vs_top(void)
{
	exo_top_text(110.0f, 8.0f, 0.55f, C2D_Color32(255, 255, 255, 255),
	             CH_NAME[g_p1.ch]);
	exo_top_text(290.0f, 8.0f, 0.55f, C2D_Color32(255, 255, 255, 255),
	             CH_NAME[g_p2.ch]);
	C2D_DrawRectSolid(20.0f, 42.0f, 0.2f, 360.0f, 168.0f,
	                  C2D_Color32(8, 18, 80, 255));
	meg_draw_pic(g_p1.ch, 110.0f, 200.0f, 2.6f, 1, 0);
	meg_draw_pic(g_p2.ch, 290.0f, 200.0f, 2.6f, -1,
	             g_p2.twin ? C2D_Color32(80, 200, 255, 255) : 0);
	exo_top_text(200.0f, 100.0f, 1.1f, C2D_Color32(255, 255, 255, 255), "VS");
}

static void draw_continue_top(void)
{
	char n[4];

	if (g_cont_n < 0) {
		meg_draw_pic(g_p1.ch, 110.0f, 210.0f, 2.8f, 1,
		             C2D_Color32(40, 255, 90, 255));
		exo_top_text(250.0f, 100.0f, 0.7f, C2D_Color32(255, 255, 255, 255),
		             "GAME OVER");
		return;
	}
	exo_top_text(200.0f, 8.0f, 0.65f, C2D_Color32(255, 255, 255, 255),
	             "CONTINUE?");
	meg_draw_pic(g_p1.ch, 110.0f, 210.0f, 2.8f, 1, 0);
	snprintf(n, sizeof n, "%d", g_cont_n);
	exo_top_text(270.0f, 90.0f, 1.6f, C2D_Color32(255, 255, 255, 255), n);
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

static void draw_shot(const Fighter *f, float px)
{
	float w = 28.0f, h = 8.0f;
	u32 col = C2D_Color32(80, 200, 255, 230);

	if (!f->shot_on)
		return;
	if (f->shot_kind == 2) {
		w = h = 14.0f;
		col = C2D_Color32(255, 180, 230, 230);
	} else if (f->shot_kind == 3) {
		w = 22.0f;
		h = 10.0f;
		col = C2D_Color32(80, 220, 90, 230);
	}
	C2D_DrawRectSolid(f->shot_x + px, f->shot_y, 0.55f, w, h, col);
}

static void draw_fighter(const Fighter *f, ExoEye eye)
{
	float px = exo_parallax(8.0f, eye);

	if (meg_sprites_ok()) {
		meg_draw_fighter(f, px);
		if (f->phase == FIGHT_FROZEN)
			C2D_DrawRectSolid(f->x + px, f->y, 0.7f, f->w, f->h,
			                  C2D_Color32(120, 200, 255, 110));
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
	int scr = meg_screen();

	if (scr == SCR_VS || scr == SCR_CONTINUE) {
		exo_render_eye(eye, C2D_Color32(0, 0, 0, 255));
		if (scr == SCR_VS)
			draw_vs_top();
		else
			draw_continue_top();
		return;
	}
	if (scr != SCR_PLAY && scr != SCR_SELECT &&
	    scr != SCR_BINDS && scr != SCR_CAPTURE) {
		exo_render_eye(eye, C2D_Color32(18, 8, 28, 255));
		meg_menu_draw_top();
		return;
	}

	exo_render_eye(eye, C2D_Color32(28, 36, 56, 255));
	px = exo_parallax(4.0f, eye);
	C2D_DrawRectSolid(0.0f + px, GROUND_Y, 0.4f, 400.0f, 40.0f,
	                  C2D_Color32(46, 72, 58, 255));
	if (scr == SCR_PLAY) {
		draw_fighter(&g_p1, eye);
		draw_fighter(&g_p2, eye);
		draw_shot(&g_p1, px);
	}
	exo_top_text(200.0f, 8.0f, 0.50f, C2D_Color32(240, 240, 240, 255),
	             "BATTLE IN MEGAVILLE 3D");
	if (meg_screen() == SCR_SELECT)
		exo_top_text(200.0f, 100.0f, 0.7f, C2D_Color32(255, 220, 80, 255),
		             meg_mode_name(meg_game()->mode));
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
	int i, col, row;
	u32 tint;
	float x, y;

	exo_render_bottom(C2D_Color32(16, 16, 28, 255));
	exo_text_begin();
	exo_text(8, 6, 0.48f, C2D_Color32(255, 255, 255, 255), "SELECT");
	for (i = 0; i < CH_COUNT; ++i) {
		col = i % SEL_COLS;
		row = i / SEL_COLS;
		x = 12.0f + (float)col * 78.0f;
		y = 28.0f + (float)row * 88.0f;
		tint = (i == g_pick) ? C2D_Color32(255, 220, 90, 255)
		                     : C2D_Color32(140, 150, 160, 255);
		exo_bot_rect(x, y, 72, 80,
		             i == g_pick ? C2D_Color32(50, 48, 28, 255)
		                         : C2D_Color32(28, 30, 40, 255));
		exo_text(x + 4, y + 4, 0.32f, tint, CH_NAME[i]);
		if (meg_sprites_ok_ch(i))
			meg_draw_idle(i, x + 36.0f, y + 72.0f, 1.0f);
	}
	exo_text(8, 214, 0.38f, C2D_Color32(120, 120, 140, 255),
	         meg_game()->mode == MODE_STORY
	             ? "A start   B menu   (Zim n/a)"
	             : "PAD move   A fight   B menu");
}

static void tick_select(void)
{
	if (meg_raw_down(EXO_BTN_LEFT) && (g_pick % SEL_COLS) > 0)
		g_pick--;
	if (meg_raw_down(EXO_BTN_RIGHT) && (g_pick % SEL_COLS) < SEL_COLS - 1)
		g_pick++;
	if (meg_raw_down(EXO_BTN_UP) && g_pick >= SEL_COLS)
		g_pick -= SEL_COLS;
	if (meg_raw_down(EXO_BTN_DOWN) && g_pick + SEL_COLS < CH_COUNT)
		g_pick += SEL_COLS;
	if (meg_game()->mode == MODE_STORY && g_pick == CH_ZIM)
		g_pick = CH_SHIRA;
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_MAIN);
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_START)) {
		if (meg_game()->mode == MODE_STORY)
			meg_story_begin(g_pick);
		goto_vs();
	}
}

static void draw_continue(void)
{
	u32 a = (g_cont_row == 0) ? C2D_Color32(255, 220, 90, 255)
	                          : C2D_Color32(220, 220, 230, 255);
	u32 b = (g_cont_row == 1) ? C2D_Color32(255, 220, 90, 255)
	                          : C2D_Color32(220, 220, 230, 255);

	exo_render_bottom(C2D_Color32(0, 0, 0, 255));
	exo_text_begin();
	if (g_cont_n < 0) {
		exo_text(16, 100, 0.5f, C2D_Color32(200, 200, 210, 255),
		         "A / B   menu");
		return;
	}
	exo_text(24, 100, 0.55f, a, "CONTINUE");
	exo_text(160, 100, 0.55f, b, "BACK TO MENU");
}

static void tick_continue(void)
{
	MegGame *g = meg_game();

	if (g_vs_lock > 0)
		g_vs_lock--;
	if (g_cont_n < 0) {
		if (g_vs_lock == 0 &&
		    (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_B) ||
		     meg_raw_down(EXO_BTN_START)))
			meg_set_screen(SCR_MAIN);
		return;
	}
	g_cont_t++;
	if (g_cont_t >= 60) {
		g_cont_t = 0;
		g_cont_n--;
		if (g_cont_n < 0)
			g_vs_lock = 12;
	}
	if (g_vs_lock > 0)
		return;
	if (meg_raw_down(EXO_BTN_LEFT))
		g_cont_row = 0;
	if (meg_raw_down(EXO_BTN_RIGHT))
		g_cont_row = 1;
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_START)) {
		if (g_cont_row == 0) {
			g->used_continue = 1;
			g->numcontinues++;
			goto_vs();
		} else {
			meg_set_screen(SCR_MAIN);
		}
	}
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_MAIN);
}

static void tick_vs(void)
{
	if (g_vs_lock > 0) {
		g_vs_lock--;
		return;
	}
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_B) ||
	    meg_raw_down(EXO_BTN_START) || meg_down(MEG_ACT_LIGHT) ||
	    meg_down(MEG_ACT_HEAVY)) {
		round_reset();
		meg_set_screen(SCR_PLAY);
	}
}

static void draw_vs_bot(void)
{
	exo_render_bottom(C2D_Color32(0, 0, 0, 255));
	exo_text_begin();
	exo_text(16, 110, 0.42f, C2D_Color32(180, 180, 190, 255),
	         "Y / X / A   fight");
}

static void draw_storyend(void)
{
	char buf[48];
	MegGame *g = meg_game();

	exo_render_bottom(C2D_Color32(12, 16, 28, 255));
	exo_text_begin();
	exo_text(16, 24, 0.6f, C2D_Color32(255, 220, 90, 255), "STORY CLEAR");
	snprintf(buf, sizeof buf, "DIFF %d   CONTINUES %d",
	         g->difficulty, g->numcontinues);
	exo_text(16, 70, 0.42f, C2D_Color32(230, 230, 240, 255), buf);
	if (g->unlock_zim_surv)
		exo_text(16, 100, 0.4f, C2D_Color32(80, 220, 120, 255),
		         "ZIM SURVIVAL UNLOCKED");
	exo_text(16, 200, 0.38f, C2D_Color32(140, 140, 160, 255), "A  menu");
}

static void tick_storyend(void)
{
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_START) ||
	    meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_MAIN);
}

static void draw_play_hud(void)
{
	char buf[48];

	exo_render_bottom(C2D_Color32(16, 16, 24, 255));
	exo_text_begin();
	snprintf(buf, sizeof buf, "%s %d/%d d%d  P1 %d  CPU %d",
	         meg_mode_name(meg_game()->mode),
	         meg_game()->mode == MODE_STORY ? meg_game()->story_level + 1 : 1,
	         meg_game()->mode == MODE_STORY
	             ? meg_story_len(meg_game()->difficulty)
	             : 1,
	         meg_game()->difficulty, g_p1.hp, g_p2.hp);
	exo_text(8, 8, 0.5f, C2D_Color32(255, 255, 255, 255), buf);
	exo_bot_rect(8, 32, (float)g_p1.hp * 0.14f, 10,
	             C2D_Color32(220, 196, 72, 255));
	exo_bot_rect(8, 48, (float)g_p2.hp * 0.14f, 10,
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
	             ? (meg_game()->mode == MODE_STORY ? ""
	                                               : "Y rematch  B roster")
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
	         meg_screen() == SCR_CAPTURE ? "PRESS A BUTTON" : "CONTROLS");

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
		MegGame *g = meg_game();

		if (g->mode == MODE_STORY) {
			if (g_p2.hp <= 0 && g_p1.hp > 0) {
				g->story_level++;
				if (meg_story_done()) {
					if (!g->used_continue && g->difficulty < 6)
						g->unlock_zim_surv = 1;
					meg_set_screen(SCR_STORYEND);
				} else {
					goto_vs();
				}
			} else {
				g_cont_n = 9;
				g_cont_t = 0;
				g_cont_row = 0;
				g_vs_lock = 25;
				meg_set_screen(SCR_CONTINUE);
			}
			return;
		}
		if (meg_down(MEG_ACT_LIGHT) || meg_down(MEG_ACT_HEAVY))
			round_reset();
		if (meg_raw_down(EXO_BTN_B))
			meg_set_screen(SCR_SELECT);
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
		meg_set_screen(SCR_CAPTURE);
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
	meg_set_screen(g_binds_ingame ? SCR_PLAY : SCR_OPTIONS);
	if (g_intro > 0)
		g_paused = 0;
}

static void tick_menu(void)
{
	uint32_t got;

	if (meg_screen() == SCR_CAPTURE) {
		if (meg_raw_down(EXO_BTN_SELECT)) {
			meg_set_screen(SCR_BINDS);
			return;
		}
		got = meg_capture_button();
		if (got && got != EXO_BTN_SELECT) {
			meg_set_bind((MegAct)g_row, got);
			g_saved_ok = meg_binds_save();
			meg_set_screen(SCR_BINDS);
		}
		return;
	}

	if (meg_raw_down(EXO_BTN_UP) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_DOWN) && g_row < ROW_MAX - 1)
		g_row++;
	if (meg_raw_down(EXO_BTN_A))
		activate_row();
	if (meg_raw_down(EXO_BTN_B) || meg_raw_down(EXO_BTN_SELECT)) {
		meg_set_screen(g_binds_ingame ? SCR_PLAY : SCR_OPTIONS);
		if (g_intro > 0)
			g_paused = 0;
	}
}

int main(void)
{
	int scr;

	if (!exo_init())
		return 1;
	meg_binds_init();
	meg_game_init();
	round_reset();
	meg_sprites_init();
	meg_set_screen(SCR_TITLE);

	while (exo_frame_begin()) {
		float dt = exo_dt();

		meg_binds_poll();
		scr = meg_screen();

		if (scr == SCR_SELECT) {
			tick_select();
		} else if (scr == SCR_VS) {
			tick_vs();
		} else if (scr == SCR_CONTINUE) {
			tick_continue();
		} else if (scr == SCR_STORYEND) {
			tick_storyend();
		} else if (scr == SCR_PLAY && meg_raw_down(EXO_BTN_SELECT) &&
		           g_intro <= 0) {
			g_binds_ingame = 1;
			meg_set_screen(SCR_BINDS);
			g_paused = 1;
		} else if (scr == SCR_PLAY) {
			tick_play(dt);
		} else if (scr == SCR_BINDS || scr == SCR_CAPTURE) {
			tick_menu();
		} else {
			g_binds_ingame = 0;
			meg_menu_tick();
		}

		exo_render_begin();
		draw_eye(EXO_EYE_LEFT);
		draw_eye(EXO_EYE_RIGHT);
		scr = meg_screen();
		if (scr == SCR_SELECT)
			draw_select();
		else if (scr == SCR_PLAY)
			draw_play_hud();
		else if (scr == SCR_VS)
			draw_vs_bot();
		else if (scr == SCR_CONTINUE)
			draw_continue();
		else if (scr == SCR_STORYEND)
			draw_storyend();
		else if (scr == SCR_BINDS || scr == SCR_CAPTURE)
			draw_menu();
		else
			meg_menu_draw_bot();
		exo_render_end();
		exo_frame_end();
	}

	meg_sprites_fini();
	exo_shutdown();
	return 0;
}
