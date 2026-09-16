#include "fight.h"
#include "binds.h"
#include "roster.h"
#include "clip.h"
#include "game.h"

#include <stdlib.h>

#define GROUND 200.0f
#define GRAVITY 900.0f
#define LEFT_WALL 8.0f
#define RIGHT_WALL 392.0f
#define FALL_MAX 400.0f

enum { MV_LIGHT = 0, MV_HEAVY, MV_LLL, MV_LLH, MV_LH, MV_RANGED };

static const int STARTUP[]  = { 5, 8, 4, 5, 6, 8 };
static const int ACTIVE[]   = { 4, 5, 5, 5, 6, 4 };
static const int RECOVERY[] = { 10, 14, 12, 14, 16, 16 };
/* _damage * 15: combo1=5, heavy=6, YYY=14, YYX/YX=6, ranged=6 */
static const int DAMAGE[]   = { 75, 90, 210, 90, 90, 90 };
static const float REACH[]  = { 22.0f, 28.0f, 24.0f, 30.0f, 32.0f, 0.0f };
/* Flash _atkdir: X pelo face, Y<0 = juggle. combo3 UP, downatk DOWN, resto push. */
static const float KB_X[]   = { 90.0f, 130.0f, 40.0f, 150.0f, 180.0f, 0.0f };
static const float KB_Y[]   = { 0.0f,   0.0f, -260.0f, -140.0f, 20.0f, 0.0f };
static const int HITSTUN[]  = { 12, 16, 24, 18, 20, 10 };

/* SPRITE_RUNSPEED * 18 px/s */
static const float WALK_SPD[CH_COUNT] = {
	144.0f, 162.0f, 144.0f, 144.0f,
	126.0f, 144.0f, 144.0f, 252.0f
};
/* SPRITE_JUMPSPEED * 20 */
static const float JUMP_SPD[CH_COUNT] = {
	360.0f, 360.0f, 320.0f, 380.0f,
	320.0f, 360.0f, 360.0f, 400.0f
};
/* SPRITE_MAXDASHFUEL — Zim 0 = sem dash */
static const int DASH_FUEL_CH[CH_COUNT] = {
	10, 12, 10, 10, 8, 10, 10, 10
};
/* X no idle: 0 melee  1 gelo  2 bolha  3 butch  4 beam  5 boomer */
static const int SHOT_KIND[CH_COUNT] = {
	1, 2, 3, 5, 4, 5, 0, 5
};

#define BASE_DAMAGE 15
#define FREEZE_TIME 70
#define SHOT_SPD 260.0f
#define SHOT_W   28.0f
#define SHOT_H   8.0f
#define DASH_MAX     300
#define DASH_COST    100
#define DASH_SPEED   280.0f
#define DASH_REGEN   30.0f
#define TAP_WINDOW   12

int fight_is_blossom(const Fighter *f) { return f->ch == CH_BLOSSOM; }
int fight_is_bubbles(const Fighter *f) { return f->ch == CH_BUBBLES; }
int fight_is_buttercup(const Fighter *f) { return f->ch == CH_BUTTERCUP; }

static int char_shot(const Fighter *f)
{
	if (f->ch < 0 || f->ch >= CH_COUNT)
		return 0;
	return SHOT_KIND[f->ch];
}

static int can_shoot(const Fighter *f)
{
	return !f->shot_on && char_shot(f) != 0;
}

static int shot_dmg(const Fighter *att)
{
	switch (att->shot_kind) {
	case 2: return 3 * BASE_DAMAGE;
	case 3: return (int)(0.8f * BASE_DAMAGE);
	case 5: return 2 * BASE_DAMAGE;
	default: return 6 * BASE_DAMAGE;
	}
}

static float shot_spd(int kind)
{
	if (kind == 2)
		return 180.0f;
	if (kind == 3)
		return 200.0f;
	if (kind == 5)
		return 220.0f;
	return SHOT_SPD;
}

static void start_dash(Fighter *p, int dir);

static int can_act(const Fighter *f)
{
	return f->phase == FIGHT_IDLE || f->phase == FIGHT_WALK ||
	       f->phase == FIGHT_GUARD;
}

static void start_move(Fighter *f, int mv)
{
	f->move = mv;
	f->phase = FIGHT_STARTUP;
	f->timer = STARTUP[mv];
	f->hit_done = 0;
	if (f->grounded)
		f->vx = 0.0f;
}

static void try_attack(Fighter *f, int heavy)
{
	if (f->phase == FIGHT_STARTUP || f->phase == FIGHT_HIT ||
	    f->phase == FIGHT_FROZEN)
		return;

	if (f->phase == FIGHT_ACTIVE || f->phase == FIGHT_RECOVERY) {
		if (f->move == MV_LIGHT && f->combo == 1 && !heavy) {
			start_move(f, MV_LIGHT);
			f->combo = 2;
			return;
		}
		if (f->move == MV_LIGHT && f->combo == 2 && !heavy) {
			start_move(f, MV_LLL);
			f->combo = 0;
			return;
		}
		if (f->move == MV_LIGHT && f->combo == 2 && heavy) {
			start_move(f, MV_LLH);
			f->combo = 0;
			return;
		}
		if (f->move == MV_LIGHT && f->combo == 1 && heavy) {
			start_move(f, MV_LH);
			f->combo = 0;
			return;
		}
		return;
	}

	if (!can_act(f) && f->phase != FIGHT_JUMP)
		return;

	if (heavy) {
		if (can_shoot(f)) {
			start_move(f, MV_RANGED);
			f->combo = 0;
			return;
		}
		start_move(f, MV_HEAVY);
		f->combo = 0;
	} else {
		if (!f->grounded && f->ch == CH_DEXTER) {
			start_move(f, MV_LLL);
			f->combo = 0;
			return;
		}
		start_move(f, MV_LIGHT);
		f->combo = 1;
	}
}

static int ai_roll(int pct)
{
	return (rand() % 100) < pct;
}

static void ai_tick(Fighter *f, const Fighter *opp)
{
	float dx = opp->x - f->x;
	float adx = dx < 0.0f ? -dx : dx;

	if (f->phase == FIGHT_ACTIVE || f->phase == FIGHT_RECOVERY) {
		if (ai_roll(8))
			try_attack(f, ai_roll(20));
		return;
	}
	if (!can_act(f))
		return;

	if (dx > 4.0f)
		f->face = 1;
	else if (dx < -4.0f)
		f->face = -1;

	if (can_shoot(f) && adx > 140.0f && ai_roll(28)) {
		f->vx = 0.0f;
		try_attack(f, 1);
		return;
	}

	if (adx > 52.0f) {
		f->vx = f->face > 0 ? WALK_SPD[f->ch] : -WALK_SPD[f->ch];
		if (adx > 180.0f && f->dashes >= DASH_COST && ai_roll(3))
			start_dash(f, f->face);
		return;
	}

	f->vx = 0.0f;
	if (opp->phase == FIGHT_FROZEN) {
		if (ai_roll(30))
			try_attack(f, 0);
		return;
	}
	if (ai_roll(45))
		try_attack(f, ai_roll(18));
}

void fight_reset(Fighter *a, Fighter *b)
{
	a->x = 100.0f - 10.0f;
	b->x = 300.0f - 10.0f;
	a->w = b->w = 20.0f;
	a->h = b->h = 36.0f;
	a->y = b->y = 200.0f - 36.0f;
	a->vx = b->vx = 0.0f;
	a->vy = b->vy = 0.0f;
	a->face = 1;
	b->face = -1;
	a->grounded = b->grounded = 1;
	a->phase = b->phase = FIGHT_IDLE;
	a->timer = b->timer = 0;
	a->hp = b->hp = 1000;
	a->combo = b->combo = 0;
	a->move = b->move = 0;
	a->hit_done = b->hit_done = 0;
	a->ai = 0;
	b->ai = 0;
	if (meg_game()->mode == MODE_STORY)
		b->ai = 1;
	else if (meg_game()->mode == MODE_VERSUS)
		b->ai = meg_game()->vs_cpu && !meg_game()->dual_ctrl;
	else if (meg_game()->mode == MODE_SURVIVAL)
		b->ai = 1;
	a->dashes = b->dashes = DASH_MAX;
	a->dash_acc = b->dash_acc = 0.0f;
	a->dash_fuel = b->dash_fuel = 0;
	a->dashed = b->dashed = 0;
	a->tap_dir = b->tap_dir = 0;
	a->tap_age = b->tap_age = 0;
	a->shot_on = b->shot_on = 0;
	a->shot_hit = b->shot_hit = 0;
	a->twin = b->twin = 0;
	a->clip_id = b->clip_id = -1;
	a->clip_f = b->clip_f = 0;
	a->clip_t = b->clip_t = 0.0f;
	a->airj = b->airj = 0;
	a->jlock = b->jlock = 0;
}

static void start_dash(Fighter *p, int dir)
{
	int fuel;

	if (p->dashed || p->dashes < DASH_COST)
		return;
	fuel = (p->ch >= 0 && p->ch < CH_COUNT) ? DASH_FUEL_CH[p->ch] : 10;
	if (p->ch == CH_ZIM)
		return;
	if (!p->grounded)
		return;
	if (!can_act(p) && p->phase != FIGHT_WALK)
		return;
	p->face = dir;
	p->phase = FIGHT_DASH;
	p->dash_fuel = fuel;
	p->dashed = 1;
	p->dashes -= DASH_COST;
	p->vx = (float)dir * DASH_SPEED;
}

static void tap_dash(Fighter *p, int dir)
{
	if (p->tap_dir == dir && p->tap_age > 0 && p->tap_age <= TAP_WINDOW)
		start_dash(p, dir);
	p->tap_dir = dir;
	p->tap_age = 1;
}

void fight_control(Fighter *p, const Fighter *opp)
{
	if (p->phase == FIGHT_FROZEN || p->phase == FIGHT_HIT)
		return;
	if (p->tap_age > 0 && p->tap_age < 60)
		p->tap_age++;

	if (p->phase == FIGHT_DASH)
		return;

	if (p->ai)
		ai_tick(p, opp);
	else {
		p->vx = 0.0f;
		if (can_act(p) || p->phase == FIGHT_JUMP) {
			if (meg_down(MEG_ACT_LEFT))
				tap_dash(p, -1);
			if (meg_down(MEG_ACT_RIGHT))
				tap_dash(p, 1);
			if (p->phase == FIGHT_DASH)
				return;
			if (meg_held(MEG_ACT_LEFT)) {
				p->vx = -WALK_SPD[p->ch];
				p->face = -1;
			}
			if (meg_held(MEG_ACT_RIGHT)) {
				p->vx = WALK_SPD[p->ch];
				p->face = 1;
			}
		}
		if (!meg_held(MEG_ACT_LEFT) && !meg_held(MEG_ACT_RIGHT))
			p->dashed = 0;
		if (p->jlock > 0)
			; /* atraso após o takeoff */
		else if (meg_down(MEG_ACT_JUMP)) {
			if (p->grounded && can_act(p)) {
				p->vy = -JUMP_SPD[p->ch];
				p->grounded = 0;
				p->airj = 0;
				p->jlock = 12;
				p->phase = FIGHT_JUMP;
			} else if (!p->grounded && p->ch == CH_DEXTER &&
			           !p->airj && p->phase == FIGHT_JUMP) {
				p->vy = -JUMP_SPD[p->ch];
				p->airj = 1;
				p->jlock = 12;
				p->phase = FIGHT_JUMP;
			}
		}
		if (can_act(p) && p->grounded && meg_held(MEG_ACT_GUARD))
			p->phase = FIGHT_GUARD;
		else if (p->phase == FIGHT_GUARD)
			p->phase = FIGHT_IDLE;
		if (meg_down(MEG_ACT_LIGHT))
			try_attack(p, 0);
		if (meg_down(MEG_ACT_HEAVY))
			try_attack(p, 1);
		if (p->phase != FIGHT_JUMP && can_act(p) && p->grounded &&
		    p->phase != FIGHT_GUARD) {
			if (p->vx > 20.0f || p->vx < -20.0f)
				p->phase = FIGHT_WALK;
			else
				p->phase = FIGHT_IDLE;
		}
	}

	if (p->phase == FIGHT_GUARD)
		p->vx = 0.0f;
}

void fight_physics(Fighter *p, float dt)
{
	int kind;
	float spd;

	if (p->phase == FIGHT_FROZEN)
		p->vx = 0.0f;

	if (p->jlock > 0)
		p->jlock--;

	p->x += p->vx * dt;
	p->vy += GRAVITY * dt;
	if (p->vy > FALL_MAX)
		p->vy = FALL_MAX;
	p->y += p->vy * dt;
	/* Só aterra se estiver a descer — senão o pulo some no mesmo frame. */
	if (p->y + p->h >= GROUND && p->vy >= 0.0f) {
		p->y = GROUND - p->h;
		p->vy = 0.0f;
		if (!p->grounded && p->phase == FIGHT_JUMP)
			p->phase = FIGHT_IDLE;
		p->grounded = 1;
		p->airj = 0;
	} else {
		p->grounded = 0;
		if (p->y + p->h > GROUND && p->vy < 0.0f)
			p->y = GROUND - p->h - 0.5f;
	}
	if (p->x < LEFT_WALL)
		p->x = LEFT_WALL;
	if (p->x + p->w > RIGHT_WALL)
		p->x = RIGHT_WALL - p->w;

	if (p->dashes < DASH_MAX && p->phase != FIGHT_DASH) {
		p->dash_acc += DASH_REGEN * dt;
		while (p->dash_acc >= 1.0f && p->dashes < DASH_MAX) {
			p->dashes++;
			p->dash_acc -= 1.0f;
		}
	}

	if (p->phase == FIGHT_DASH) {
		p->vx = (float)p->face * DASH_SPEED;
		p->dash_fuel--;
		if (p->dash_fuel <= 0) {
			p->phase = FIGHT_IDLE;
			p->vx = 0.0f;
		}
	}

	if (p->shot_on) {
		p->shot_x += p->shot_vx * dt;
		if (p->shot_x < -40.0f || p->shot_x > 440.0f)
			p->shot_on = 0;
	}

	if (p->timer > 0)
		p->timer--;

	if (p->phase == FIGHT_STARTUP && p->timer <= 0) {
		p->phase = FIGHT_ACTIVE;
		p->timer = ACTIVE[p->move];
		p->hit_done = 0;
		if (p->move == MV_RANGED && !p->shot_on) {
			kind = char_shot(p);
			p->shot_on = 1;
			p->shot_hit = 0;
			p->shot_kind = kind;
			p->shot_y = (kind == 3) ? (GROUND - 18.0f)
			                       : (p->y + 16.0f);
			spd = shot_spd(kind);
			if (p->face > 0) {
				p->shot_x = p->x + p->w;
				p->shot_vx = spd;
			} else {
				p->shot_x = p->x - SHOT_W;
				p->shot_vx = -spd;
			}
		}
	} else if (p->phase == FIGHT_ACTIVE && p->timer <= 0) {
		p->phase = FIGHT_RECOVERY;
		p->timer = RECOVERY[p->move];
	} else if (p->phase == FIGHT_RECOVERY && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
		p->combo = 0;
	} else if (p->phase == FIGHT_HIT && p->timer <= 0) {
		p->phase = p->grounded ? FIGHT_IDLE : FIGHT_JUMP;
	} else if (p->phase == FIGHT_FROZEN && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
		p->vx = 0.0f;
	}
	meg_clip_tick(p, dt);
}

static int apply_hp(Fighter *vic, int raw)
{
	int d = raw;

	if (vic->ch == CH_SHIRA)
		d *= 2;
	else if (vic->ch == CH_ZIM) {
		d = (int)(d * 0.8f) - 5;
		if (d < 1)
			d = 1;
	}
	vic->hp -= d;
	if (vic->hp < 0)
		vic->hp = 0;
	if (vic->hp <= 0)
		vic->shot_on = 0;
	return d;
}

static void launch(Fighter *att, Fighter *vic, int mv, int dmg)
{
	float kbx, kby;

	apply_hp(vic, dmg);
	vic->phase = FIGHT_HIT;
	vic->timer = HITSTUN[mv];
	kbx = KB_X[mv];
	kby = KB_Y[mv];
	if (!att->grounded && mv == MV_HEAVY) {
		kbx = 40.0f;
		kby = 220.0f; /* downatk slam */
		vic->timer = 18;
	}
	vic->vx = (float)att->face * kbx;
	vic->vy = kby;
	if (kby < 0.0f)
		vic->grounded = 0;
	vic->combo = 0;
	if (vic->hp <= 0)
		att->shot_on = 0;
}

static void one_hit(Fighter *att, Fighter *vic)
{
	float hx, hy, hw, hh;

	if (att->phase != FIGHT_ACTIVE || att->hit_done)
		return;
	if (att->move == MV_RANGED)
		return;
	hx = att->face > 0 ? att->x + att->w : att->x - REACH[att->move];
	hy = att->y + 8.0f;
	hw = REACH[att->move];
	hh = 16.0f;
	if (hx < vic->x + vic->w && hx + hw > vic->x &&
	    hy < vic->y + vic->h && hy + hh > vic->y) {
		att->hit_done = 1;
		if (vic->phase == FIGHT_GUARD) {
			vic->x += att->face * 6.0f;
		} else {
			launch(att, vic, att->move, DAMAGE[att->move]);
		}
	}
}

static void shot_hit(Fighter *att, Fighter *vic)
{
	float hx, hy, kbx, kby;

	if (!att->shot_on || att->shot_hit)
		return;
	hx = att->shot_x;
	hy = att->shot_y;
	if (hx < vic->x + vic->w && hx + SHOT_W > vic->x &&
	    hy < vic->y + vic->h && hy + SHOT_H > vic->y) {
		att->shot_hit = 1;
		att->shot_on = 0;
		if (vic->phase == FIGHT_GUARD) {
			vic->x += att->face * 4.0f;
			return;
		}
		apply_hp(vic, shot_dmg(att));
		if (att->shot_kind == 1 && vic->ch != CH_ZIM) {
			vic->phase = FIGHT_FROZEN;
			vic->timer = FREEZE_TIME;
			vic->vx = 0.0f;
			vic->vy = 0.0f;
			vic->combo = 0;
			return;
		}
		kbx = 70.0f;
		kby = 0.0f;
		if (att->shot_kind == 2) { kbx = 40.0f; }
		if (att->shot_kind == 3) { kbx = 55.0f; }
		if (att->shot_kind == 4) { kbx = 90.0f; kby = -80.0f; }
		if (att->shot_kind == 5) { kbx = 75.0f; kby = -30.0f; }
		vic->phase = FIGHT_HIT;
		vic->timer = 14;
		vic->vx = (float)att->face * kbx;
		vic->vy = kby;
		if (kby < 0.0f)
			vic->grounded = 0;
		vic->combo = 0;
	}
}

int fight_hurtbox(const Fighter *f, float *x, float *y, float *w, float *h)
{
	*x = f->x;
	*y = f->y;
	*w = f->w;
	*h = f->h;
	return 1;
}

int fight_hitbox(const Fighter *f, float *x, float *y, float *w, float *h)
{
	if (f->phase != FIGHT_ACTIVE || f->move == MV_RANGED)
		return 0;
	*w = REACH[f->move];
	*h = 16.0f;
	*x = f->face > 0 ? f->x + f->w : f->x - *w;
	*y = f->y + 8.0f;
	return 1;
}

void fight_hits(Fighter *a, Fighter *b)
{
	one_hit(a, b);
	one_hit(b, a);
	shot_hit(a, b);
	shot_hit(b, a);
}

