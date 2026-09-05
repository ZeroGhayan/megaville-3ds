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
	FIGHT_HIT
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
	int ch; /* 0 blossom 1 bubbles 2 buttercup */
} Fighter;

void fight_reset(Fighter *a, Fighter *b);
void fight_control(Fighter *p, const Fighter *opp);
void fight_physics(Fighter *p, float dt);
void fight_hits(Fighter *a, Fighter *b);

#endif
