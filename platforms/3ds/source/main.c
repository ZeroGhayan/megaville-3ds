#include "exo/platform.h"
#include "exo/render.h"

#include <citro2d.h>
#include <stdio.h>

#define GROUND_Y 200.0f
#define BOX_W    28.0f
#define BOX_H    40.0f
#define WALK     140.0f

static float g_x = 80.0f;
static float g_y = GROUND_Y - BOX_H;
static int   g_face = 1;
static int   g_paused;

static void draw_eye(ExoEye eye)
{
	char line[64];
	float px;

	exo_render_eye(eye, C2D_Color32(28, 36, 56, 255));

	px = exo_parallax(4.0f, eye);
	C2D_DrawRectSolid(0.0f + px, GROUND_Y, 0.4f, 400.0f, 40.0f,
	                  C2D_Color32(46, 72, 58, 255));

	px = exo_parallax(8.0f, eye);
	C2D_DrawRectSolid(g_x + px, g_y, 0.5f, BOX_W, BOX_H,
	                  g_paused ? C2D_Color32(120, 120, 120, 255)
	                           : C2D_Color32(220, 196, 72, 255));

	exo_top_text(200.0f, 8.0f, 0.55f, C2D_Color32(240, 240, 240, 255),
	             "BATTLE IN MEGAVILLE 3D");
	snprintf(line, sizeof line, "Y light  X heavy  START pause");
	exo_top_text(200.0f, 28.0f, 0.42f, C2D_Color32(180, 200, 220, 255), line);
}

static void draw_bottom(void)
{
	const ExoInput *in = exo_input();
	char buf[48];

	exo_render_bottom(C2D_Color32(16, 16, 24, 255));
	exo_text_begin();
	exo_text(8, 8, 0.5f, C2D_Color32(255, 255, 255, 255), "HUD / P2 = CPU later");
	snprintf(buf, sizeof buf, "PAD %+.2f %+.2f", in->stick_x, in->stick_y);
	exo_text(8, 32, 0.5f, C2D_Color32(200, 220, 255, 255), buf);
	snprintf(buf, sizeof buf, "Y=%d X=%d START=%d",
	         exo_held(EXO_BTN_Y) ? 1 : 0,
	         exo_held(EXO_BTN_X) ? 1 : 0,
	         exo_held(EXO_BTN_START) ? 1 : 0);
	exo_text(8, 56, 0.5f, C2D_Color32(255, 220, 120, 255), buf);
	exo_text(8, 88, 0.5f, C2D_Color32(160, 160, 180, 255),
	         g_paused ? "PAUSED" : "MOVE THE BOX");
	exo_text(8, 210, 0.45f, C2D_Color32(120, 120, 140, 255),
	         "HOME to exit");
}

int main(void)
{
	if (!exo_init())
		return 1;

	while (exo_frame_begin()) {
		const ExoInput *in = exo_input();
		float dt = exo_dt();

		if (exo_down(EXO_BTN_START))
			g_paused ^= 1;

		if (!g_paused) {
			float ax = in->stick_x;
			if (exo_held(EXO_BTN_LEFT))
				ax = -1.0f;
			if (exo_held(EXO_BTN_RIGHT))
				ax = 1.0f;
			g_x += ax * WALK * dt;
			if (ax < -0.2f)
				g_face = -1;
			if (ax > 0.2f)
				g_face = 1;
			if (g_x < 8.0f)
				g_x = 8.0f;
			if (g_x > 400.0f - BOX_W - 8.0f)
				g_x = 400.0f - BOX_W - 8.0f;
			(void)g_face;
		}

		exo_render_begin();
		draw_eye(EXO_EYE_LEFT);
		draw_eye(EXO_EYE_RIGHT);
		draw_bottom();
		exo_render_end();
		exo_frame_end();
	}

	exo_shutdown();
	return 0;
}
