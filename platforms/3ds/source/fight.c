#include "fight.h"
#include "binds.h"
#include "roster.h"

#include <stdlib.h>

#define GROUND 200.0f
#define GRAVITY 900.0f
#define WALK 140.0f
#define JUMP_V 280.0f
#define JUMP_V 280.0f
#define LEFT_WALL 8.0f
#define RIGHT_WALL 392.0f

/* Shared chains: L,L,L  L,L,H  L,H  — same for every character. */
enum { MV_LIGHT = 0, MV_HEAVY, MV_LLL, MV_LLH, MV_LH, MV_RANGED };

static const int STARTUP[]  = { 5, 8, 4, 5, 6, 8 };
static const int ACTIVE[]   = { 4, 5, 5, 5, 6, 4 };
static const int RECOVERY[] = { 10, 14, 12, 14, 16, 16 };
/* Flash _damage * BASE_DAMAGE (15): combo1=4, heavy/upper=6 */
static const int DAMAGE[]   = { 60, 90, 60, 90, 90, 90 };
static const float REACH[]  = { 22.0f, 28.0f, 24.0f, 30.0f, 32.0f, 0.0f };

/* SPRITE_RUNSPEED * 18 (Flash px/frame @ ~30fps) */
static const float WALK_SPD[CH_COUNT] = {
	144.0f, 162.0f, 144.0f, 144.0f,
	126.0f, 144.0f, 144.0f, 252.0f
};

#define BASE_DAMAGE 15 /* frame_254 DEFAULT_BASE_DAMAGE */
#define BUBBLE_MUL  3.0f
#define BUTCH_MUL   0.8f
#define BLOSSOM_SHOT_MUL 6.0f
#define FREEZE_TIME 70
#define SHOT_SPD 260.0f

static int shot_dmg(const Fighter *att)
{
	if (att->shot_kind == 2)
		return (int)(BUBBLE_MUL * BASE_DAMAGE); /* 45 */
	if (att->shot_kind == 3)
		return (int)(BUTCH_MUL * BASE_DAMAGE); /* 12 */
	return (int)(BLOSSOM_SHOT_MUL * BASE_DAMAGE); /* 90 */
}
#define SHOT_SPD 260.0f
#define SHOT_W   28.0f
#define SHOT_H   8.0f

int fight_is_blossom(const Fighter *f)
{
	return f->ch == 0;
}

int fight_is_bubbles(const Fighter *f)
{
	return f->ch == 1;
}

int fight_is_buttercup(const Fighter *f)
{
	return f->ch == 2;
}

static int can_shoot(const Fighter *f)
{
	return !f->shot_on && (fight_is_blossom(f) || fight_is_bubbles(f) ||
	                       fight_is_buttercup(f));
}

static void start_dash(Fighter *p, int dir);

#define DASH_MAX     300
#define DASH_COST    100
#define DASH_FUEL    10
#define DASH_SPEED   280.0f
#define DASH_REGEN   30.0f /* per second; Flash +1 @ 30fps */
#define TAP_WINDOW   12

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
	f->vx = 0.0f;
}

static void try_attack(Fighter *f, int heavy)
{
	if (f->phase == FIGHT_STARTUP || f->phase == FIGHT_HIT)
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

	if (!can_act(f) || !f->grounded)
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

	if ((fight_is_blossom(f) || fight_is_bubbles(f) || fight_is_buttercup(f)) &&
	    adx > 140.0f && !f->shot_on && ai_roll(28)) {
		f->vx = 0.0f;
		try_attack(f, 1);
		return;
	}

	if (adx > 52.0f) {
		f->vx = f->face > 0 ? WALK_SPD[f->ch] : -WALK_SPD[f->ch];
		if (adx > 180.0f && f->dashes >= 100 && ai_roll(3))
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
	a->x = 100.0f;
	b->x = 300.0f;
	a->y = b->y = GROUND - 40.0f;
	a->w = b->w = 28.0f;
	a->h = b->h = 40.0f;
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
	b->ai = 1;
	a->dashes = b->dashes = DASH_MAX;
	a->dash_acc = b->dash_acc = 0.0f;
	a->dash_fuel = b->dash_fuel = 0;
	a->dashed = b->dashed = 0;
	a->tap_dir = b->tap_dir = 0;
	a->tap_age = b->tap_age = 0;
	a->shot_on = b->shot_on = 0;
	a->shot_hit = b->shot_hit = 0;
}

static void start_dash(Fighter *p, int dir)
{
	if (p->dashed || p->dashes < DASH_COST)
		return;
	if (p->ch == CH_ZIM)
		return;
	if (!can_act(p) && p->phase != FIGHT_WALK)
		return;
	p->face = dir;
	p->phase = FIGHT_DASH;
	p->dash_fuel = DASH_FUEL;
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
		if (can_act(p) && p->grounded && meg_down(MEG_ACT_JUMP)) {
			p->vy = -JUMP_V;
			p->grounded = 0;
			p->phase = FIGHT_JUMP;
		}
		if (can_act(p) && p->grounded && meg_held(MEG_ACT_GUARD))
			p->phase = FIGHT_GUARD;
		else if (p->phase == FIGHT_GUARD)
			p->phase = FIGHT_IDLE;
		if (meg_down(MEG_ACT_LIGHT))
			try_attack(p, 0);
		if (meg_down(MEG_ACT_HEAVY))
			try_attack(p, 1);
	}

	if (p->phase == FIGHT_GUARD)
		p->vx = 0.0f;
}

void fight_physics(Fighter *p, float dt)
{
	if (p->phase == FIGHT_FROZEN)
		p->vx = 0.0f;

	p->x += p->vx * dt;
	p->vy += GRAVITY * dt;
	p->y += p->vy * dt;
	if (p->y + p->h >= GROUND) {
		p->y = GROUND - p->h;
		p->vy = 0.0f;
		if (!p->grounded && p->phase == FIGHT_JUMP)
			p->phase = FIGHT_IDLE;
		p->grounded = 1;
	} else {
		p->grounded = 0;
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
			p->shot_on = 1;
			p->shot_hit = 0;
			if (fight_is_bubbles(p))
				p->shot_kind = 2;
			else if (fight_is_buttercup(p))
				p->shot_kind = 3;
			else
				p->shot_kind = 1;
			p->shot_y = fight_is_buttercup(p) ? (GROUND - 18.0f)
			                                 : (p->y + 16.0f);
			{
				float spd = SHOT_SPD;
				if (p->shot_kind == 2)
					spd = 180.0f;
				if (p->shot_kind == 3)
					spd = 200.0f;
				if (p->face > 0) {
					p->shot_x = p->x + p->w;
					p->shot_vx = spd;
				} else {
					p->shot_x = p->x - SHOT_W;
					p->shot_vx = -spd;
				}
			}
		}
	} else if (p->phase == FIGHT_ACTIVE && p->timer <= 0) {
		p->phase = FIGHT_RECOVERY;
		p->timer = RECOVERY[p->move];
	} else if (p->phase == FIGHT_RECOVERY && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
		p->combo = 0;
	} else if (p->phase == FIGHT_HIT && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
	} else if (p->phase == FIGHT_FROZEN && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
		p->vx = 0.0f;
	}
}

static int apply_hp(Fighter *vic, int raw)
{
	int d = raw;

	if (vic->ch == CH_SHIRA)
		d *= 2;
	else if (vic->ch == CH_ZIM)
		d = (int)(d * 0.8f);
	vic->hp -= d;
	if (vic->hp < 0)
		vic->hp = 0;
	if (vic->hp <= 0) {
		vic->shot_on = 0;
	}
	return d;
}

static void one_hit(Fighter *att, Fighter *vic)
{
	float hx, hy, hw, hh;

	if (att->phase != FIGHT_ACTIVE || att->hit_done)
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
			apply_hp(vic, DAMAGE[att->move]);
			vic->phase = FIGHT_HIT;
			vic->timer = 14;
			vic->vx = att->face * 80.0f;
			vic->combo = 0;
			if (vic->hp <= 0)
				att->shot_on = 0;
		}
	}
}

static void shot_hit(Fighter *att, Fighter *vic)
{
	float hx, hy;

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
		} else if (att->shot_kind == 1 && vic->ch != CH_ZIM) {
			apply_hp(vic, shot_dmg(att));
			vic->phase = FIGHT_FROZEN;
			vic->timer = FREEZE_TIME;
			vic->vx = 0.0f;
			vic->vy = 0.0f;
			vic->combo = 0;
		} else {
			apply_hp(vic, shot_dmg(att));
			vic->phase = FIGHT_HIT;
			vic->timer = 12;
			vic->vx = att->face * 70.0f;
			vic->combo = 0;
		}
	}
}

void fight_hits(Fighter *a, Fighter *b)
{
	one_hit(a, b);
	one_hit(b, a);
	shot_hit(a, b);
	shot_hit(b, a);
}
