#ifndef MEG_MENU_H
#define MEG_MENU_H

enum {
	SCR_TITLE = 0,
	SCR_MAIN,
	SCR_DIFF,
	SCR_OPTIONS,
	SCR_HOWTO,
	SCR_EXTRA,
	SCR_SELECT,
	SCR_BINDS,
	SCR_CAPTURE,
	SCR_PLAY
};

int meg_screen(void);
void meg_set_screen(int s);
void meg_menu_tick(void);
void meg_menu_draw_top(void);
void meg_menu_draw_bot(void);

#endif
