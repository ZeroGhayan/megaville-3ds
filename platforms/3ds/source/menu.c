#include "menu.h"
#include "game.h"
#include "binds.h"

#include "exo/input.h"
#include "exo/render.h"

#include <citro2d.h>
#include <stdio.h>

static int g_scr = SCR_TITLE;
static int g_row;
static int g_howto;
static int g_blink;
static char g_note[48];

int meg_screen(void)
{
	return g_scr;
}

void meg_set_screen(int s)
{
	g_scr = s;
	g_row = 0;
	g_note[0] = 0;
	if (s == SCR_DIFF)
		g_row = meg_game()->menudiff - 1;
}

static u32 hi(int on)
{
	return on ? C2D_Color32(255, 220, 90, 255)
	          : C2D_Color32(200, 205, 215, 255);
}

static int main_count(void)
{
	return meg_game()->unlock_extra ? 6 : 5;
}

static void go_diff_or_select(int mode)
{
	MegGame *g = meg_game();

	g->mode = mode;
	if (mode == MODE_SURVIVAL) {
		g->zim_survival = 0;
		g->difficulty = 1;
		if (exo_held(EXO_BTN_START) && g->unlock_zim_surv) {
			g->zim_survival = 1;
			g->difficulty = 10;
		}
		meg_set_screen(SCR_SELECT);
		return;
	}
	g->difficulty = g->menudiff;
	meg_set_screen(SCR_DIFF);
}

static void tick_title(void)
{
	MegGame *g = meg_game();

	g_blink++;
	if (exo_held(EXO_BTN_L) && exo_held(EXO_BTN_R) &&
	    meg_raw_down(EXO_BTN_Y)) {
		g->unlock_extra = 1;
		g->unlock_zim = 1;
		g->unlock_zim_surv = 1;
		g->unlock_shira = 1;
		snprintf(g_note, sizeof g_note, "EXTRA UNLOCKED");
	}
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_START))
		meg_set_screen(SCR_MAIN);
}

static void tick_main(void)
{
	int n = main_count();

	if (meg_raw_down(EXO_BTN_UP) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_DOWN) && g_row < n - 1)
		g_row++;
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_TITLE);
	if (!meg_raw_down(EXO_BTN_A) && !meg_raw_down(EXO_BTN_START))
		return;
	switch (g_row) {
	case 0:
		go_diff_or_select(MODE_STORY);
		break;
	case 1:
		go_diff_or_select(MODE_VERSUS);
		break;
	case 2:
		go_diff_or_select(MODE_SURVIVAL);
		break;
	case 3:
		meg_set_screen(SCR_OPTIONS);
		break;
	case 4:
		g_howto = 0;
		meg_set_screen(SCR_HOWTO);
		break;
	default:
		meg_set_screen(SCR_EXTRA);
		break;
	}
}

static void tick_diff(void)
{
	MegGame *g = meg_game();

	if (meg_raw_down(EXO_BTN_LEFT) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_RIGHT) && g_row < 9)
		g_row++;
	if (meg_raw_down(EXO_BTN_UP) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_DOWN) && g_row < 9)
		g_row++;
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_MAIN);
	if (meg_raw_down(EXO_BTN_A) || meg_raw_down(EXO_BTN_START)) {
		g->difficulty = g_row + 1;
		g->menudiff = g->difficulty;
		meg_set_screen(SCR_SELECT);
	}
}

static void tick_options(void)
{
	int n = meg_game()->unlock_extra ? 4 : 3;

	if (meg_raw_down(EXO_BTN_UP) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_DOWN) && g_row < n - 1)
		g_row++;
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_MAIN);
	if (!meg_raw_down(EXO_BTN_A))
		return;
	if (g_row == 0)
		meg_set_screen(SCR_BINDS);
	else if (g_row == 1)
		snprintf(g_note, sizeof g_note, "GFX: 3DS native");
	else if (g_row == 2 && meg_game()->unlock_extra)
		meg_set_screen(SCR_EXTRA);
	else
		meg_set_screen(SCR_MAIN);
}

static void tick_howto(void)
{
	if (meg_raw_down(EXO_BTN_RIGHT) || meg_raw_down(EXO_BTN_A)) {
		if (g_howto < 6)
			g_howto++;
		else
			meg_set_screen(SCR_MAIN);
	}
	if (meg_raw_down(EXO_BTN_LEFT) && g_howto > 0)
		g_howto--;
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_MAIN);
}

static void tick_extra(void)
{
	MegGame *g = meg_game();

	if (meg_raw_down(EXO_BTN_UP) && g_row > 0)
		g_row--;
	if (meg_raw_down(EXO_BTN_DOWN) && g_row < 3)
		g_row++;
	if (meg_raw_down(EXO_BTN_B))
		meg_set_screen(SCR_OPTIONS);
	if (!meg_raw_down(EXO_BTN_A))
		return;
	if (g_row == 0)
		g->dual_ctrl = !g->dual_ctrl;
	else if (g_row == 1)
		g->unlock_zim_surv = 1;
	else if (g_row == 2)
		g->unlock_shira = 1;
	else
		meg_set_screen(SCR_OPTIONS);
}

void meg_menu_tick(void)
{
	switch (g_scr) {
	case SCR_TITLE:
		tick_title();
		break;
	case SCR_MAIN:
		tick_main();
		break;
	case SCR_DIFF:
		tick_diff();
		break;
	case SCR_OPTIONS:
		tick_options();
		break;
	case SCR_HOWTO:
		tick_howto();
		break;
	case SCR_EXTRA:
		tick_extra();
		break;
	default:
		break;
	}
}

void meg_menu_draw_top(void)
{
	u32 gold = C2D_Color32(255, 220, 80, 255);
	u32 pink = C2D_Color32(255, 105, 180, 255);
	u32 mute = C2D_Color32(180, 200, 220, 255);

	switch (g_scr) {
	case SCR_TITLE:
		exo_top_text(200, 70, 0.55f, mute, "THE POWERPUFF GIRLS");
		exo_top_text(200, 100, 0.72f, pink, "BATTLE IN MEGAVILLE 3D");
		if ((g_blink / 20) & 1)
			exo_top_text(200, 160, 0.5f, gold, "PRESS START");
		if (g_note[0])
			exo_top_text(200, 200, 0.4f, gold, g_note);
		break;
	case SCR_MAIN:
		exo_top_text(200, 90, 0.7f, gold, "MEGAVILLE");
		exo_top_text(200, 130, 0.4f, mute, "CHOOSE A MODE");
		break;
	case SCR_DIFF:
		exo_top_text(200, 80, 0.55f, gold, meg_mode_name(meg_game()->mode));
		exo_top_text(200, 120, 0.7f, pink, meg_diff_name(g_row + 1));
		break;
	case SCR_HOWTO:
		exo_top_text(200, 100, 0.6f, gold, "HOW TO PLAY");
		break;
	case SCR_EXTRA:
		exo_top_text(200, 100, 0.6f, pink, "EXTRA OPTIONS");
		break;
	case SCR_OPTIONS:
		exo_top_text(200, 100, 0.6f, gold, "OPTIONS");
		break;
	default:
		break;
	}
}

static void line(float y, int on, const char *s)
{
	exo_text(16, y, 0.48f, hi(on), s);
}

void meg_menu_draw_bot(void)
{
	MegGame *g = meg_game();
	char buf[48];
	int i, n;

	exo_render_bottom(C2D_Color32(12, 10, 22, 255));
	exo_text_begin();

	if (g_scr == SCR_TITLE) {
		exo_text(16, 16, 0.42f, C2D_Color32(255, 105, 180, 255),
		         "BATTLE IN MEGAVILLE 3D");
		exo_text(16, 48, 0.4f, C2D_Color32(200, 200, 210, 255),
		         "A / START   enter");
		exo_text(16, 200, 0.32f, C2D_Color32(110, 110, 130, 255),
		         "L+R+Y  extra (dev)");
		return;
	}

	if (g_scr == SCR_MAIN) {
		static const char *items[] = {
			"STORY", "VERSUS", "SURVIVAL",
			"OPTIONS", "HOW TO PLAY", "EXTRA"
		};
		n = main_count();
		exo_text(16, 8, 0.45f, C2D_Color32(255, 255, 255, 255), "MENU");
		for (i = 0; i < n; ++i)
			line(36.0f + (float)i * 22.0f, i == g_row, items[i]);
		exo_text(16, 214, 0.32f, C2D_Color32(110, 110, 130, 255),
		         "A select  B title");
		return;
	}

	if (g_scr == SCR_DIFF) {
		exo_text(16, 8, 0.45f, C2D_Color32(255, 255, 255, 255),
		         "DIFFICULTY");
		for (i = 0; i < 10; ++i) {
			float x = 20.0f + (float)(i % 5) * 56.0f;
			float y = 70.0f + (float)(i / 5) * 48.0f;
			u32 c = (i == g_row) ? C2D_Color32(255, 220, 90, 255)
			                     : C2D_Color32(80, 90, 110, 255);
			exo_bot_rect(x, y, 48, 36, c);
			snprintf(buf, sizeof buf, "%d", i + 1);
			exo_text(x + 16, y + 10, 0.5f,
			         C2D_Color32(20, 20, 30, 255), buf);
		}
		exo_text(16, 180, 0.4f, C2D_Color32(220, 220, 230, 255),
		         meg_diff_name(g_row + 1));
		exo_text(16, 214, 0.32f, C2D_Color32(110, 110, 130, 255),
		         "PAD  A fight  B back");
		return;
	}

	if (g_scr == SCR_OPTIONS) {
		n = g->unlock_extra ? 4 : 3;
		exo_text(16, 8, 0.45f, C2D_Color32(255, 255, 255, 255), "OPTIONS");
		line(40, g_row == 0, "CONTROLS");
		line(64, g_row == 1, "GRAPHICS");
		if (g->unlock_extra)
			line(88, g_row == 2, "EXTRA");
		line(88.0f + (g->unlock_extra ? 24.0f : 0.0f),
		     g_row == n - 1, "BACK");
		if (g_note[0])
			exo_text(16, 180, 0.38f, C2D_Color32(255, 220, 90, 255),
			         g_note);
		return;
	}

	if (g_scr == SCR_HOWTO) {
		static const char *pg[] = {
			"Y light   X heavy   hold to charge",
			"Y Y Y  Y Y X  Y X  combo chains",
			"UP jump   DOWN guard   double tap dash",
			"Blossom freeze breath  Bubbles bubble",
			"Buttercup butch  Bell/Shira boomer",
			"Story  Versus  Survival  1-10 CPU",
			"L+R+Y on title unlocks extra"
		};
		snprintf(buf, sizeof buf, "PAGE %d / 7", g_howto + 1);
		exo_text(16, 8, 0.45f, C2D_Color32(255, 255, 255, 255), buf);
		exo_text(16, 80, 0.42f, C2D_Color32(230, 230, 240, 255),
		         pg[g_howto]);
		exo_text(16, 214, 0.32f, C2D_Color32(110, 110, 130, 255),
		         "A next  B menu");
		return;
	}

	if (g_scr == SCR_EXTRA) {
		exo_text(16, 8, 0.45f, C2D_Color32(255, 105, 180, 255), "EXTRA");
		snprintf(buf, sizeof buf, "DUAL CTRL  %s",
		         g->dual_ctrl ? "ON" : "OFF");
		line(40, g_row == 0, buf);
		snprintf(buf, sizeof buf, "ZIM SURVIVAL  %s",
		         g->unlock_zim_surv ? "YES" : "NO");
		line(64, g_row == 1, buf);
		snprintf(buf, sizeof buf, "SHIRA  %s",
		         g->unlock_shira ? "YES" : "NO");
		line(88, g_row == 2, buf);
		line(112, g_row == 3, "BACK");
		exo_text(16, 160, 0.32f, C2D_Color32(140, 140, 160, 255),
		         "Survival + hold START = Zim mode");
		return;
	}
}
