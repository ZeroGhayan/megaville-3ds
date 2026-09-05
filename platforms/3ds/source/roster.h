#ifndef MEG_ROSTER_H
#define MEG_ROSTER_H

/* NAME_* do frame_1479 — BLOSSOM=0 confirmado. */
enum {
	CH_BLOSSOM = 0,
	CH_BUBBLES,
	CH_BUTTERCUP,
	CH_BELL,
	CH_SHIRA,
	CH_DEXTER,
	CH_ZIM,
	CH_ROWDY,
	CH_COUNT
};

static const char *const CH_NAME[CH_COUNT] = {
	"BLOSSOM", "BUBBLES", "BUTTERCUP", "BELL",
	"SHIRA", "DEXTER", "ZIM", "ROWDY"
};

#define SEL_COLS 4
#define SEL_ROWS 2

#endif
