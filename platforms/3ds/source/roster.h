#ifndef MEG_ROSTER_H
#define MEG_ROSTER_H

/* frame_1479 — Shira e Zim são unlock (grelha final 3x2). */
enum {
	CH_BLOSSOM = 0,
	CH_BUBBLES,
	CH_BUTTERCUP,
	CH_BELL,
	CH_DEXTER,
	CH_ROWDY,
	CH_ZIM,
	CH_SHIRA,
	CH_COUNT
};

static const char *const CH_NAME[CH_COUNT] = {
	"BLOSSOM", "BUBBLES", "BUTTERCUP", "BELL",
	"DEXTER", "ROWDY", "ZIM", "SHIRA"
};

#define SEL_COLS 4
#define SEL_ROWS 2

#endif
