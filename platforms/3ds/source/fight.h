#ifndef MEG_FIGHT_H
#define MEG_FIGHT_H

typedef enum {
	FIGHT_IDLE = 0,
	FIGHT_WALK,
	FIGHT_JUMP,
	FIGHT_GUARD,
	FIGHT_STARTUP,
	FIGHT_ACTIVE,
	FIGHT_RECOVERY,
	FIGHT_HIT,
	FIGHT_DASH,
	FIGHT_FROZEN
} FightPhase;

typedef struct {
	float x, y, vx, vy;
	float w, h;
	int face; /* +1 right */
	int grounded;
	FightPhase phase;
	int timer;
	int hp;
	int combo; /* 0 none, 1 L, 2 LL, 3 LLL/LLH/LH done */
	int move;  /* 0 light 1 heavy 2 lll 3 llh 4 lh */
	int hit_done;
	int ai;
	int ch;
	int dashes;
	float dash_acc;
	int dash_fuel;
	int dashed;     /* already dashed this tap */
	int tap_dir;    /* last tap -1/0/+1 */
	int tap_age;
	int shot_on;
	int shot_hit;
	int shot_kind; /* 1 blossom freeze  2 bubbles */
	float shot_x, shot_y, shot_vx;
} Fighter;

int fight_is_blossom(const Fighter *f);
int fight_is_bubbles(const Fighter *f);

void fight_reset(Fighter *a, Fighter *b);
void fight_control(Fighter *p, const Fighter *opp);
void fight_physics(Fighter *p, float dt);
void fight_hits(Fighter *a, Fighter *b);

#endif
