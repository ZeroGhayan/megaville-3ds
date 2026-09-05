#include "fight.h"
#include "binds.h"

#define GROUND 200.0f
#define GRAVITY 900.0f
#define WALK 140.0f
#define JUMP_V 280.0f
#define LEFT_WALL 8.0f
#define RIGHT_WALL 392.0f

/* Shared chains: L,L,L  L,L,H  L,H  — same for every character. */
enum { MV_LIGHT = 0, MV_HEAVY, MV_LLL, MV_LLH, MV_LH };

static const int STARTUP[]  = { 5, 8, 4, 5, 6 };
static const int ACTIVE[]   = { 4, 5, 5, 5, 6 };
static const int RECOVERY[] = { 10, 14, 12, 14, 16 };
static const int DAMAGE[]   = { 8, 14, 10, 16, 18 };
static const float REACH[]  = { 22.0f, 28.0f, 24.0f, 30.0f, 32.0f };

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
		start_move(f, MV_HEAVY);
		f->combo = 0;
	} else {
		start_move(f, MV_LIGHT);
		f->combo = 1;
	}
}

static void ai_tick(Fighter *f, const Fighter *opp)
{
	float dx = opp->x - f->x;

	if (!can_act(f))
		return;
	if (dx > 50.0f) {
		f->vx = WALK;
		f->face = 1;
	} else if (dx < -50.0f) {
		f->vx = -WALK;
		f->face = -1;
	} else {
		f->vx = 0.0f;
		if ((opp->x + opp->w * 0.5f > f->x) == (f->face > 0))
			try_attack(f, 0);
	}
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
	a->hp = b->hp = 100;
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
}

static void start_dash(Fighter *p, int dir)
{
	if (p->dashed || p->dashes < DASH_COST)
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
				p->vx = -WALK;
				p->face = -1;
			}
			if (meg_held(MEG_ACT_RIGHT)) {
				p->vx = WALK;
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

	if (p->timer > 0)
		p->timer--;

	if (p->phase == FIGHT_STARTUP && p->timer <= 0) {
		p->phase = FIGHT_ACTIVE;
		p->timer = ACTIVE[p->move];
		p->hit_done = 0;
	} else if (p->phase == FIGHT_ACTIVE && p->timer <= 0) {
		p->phase = FIGHT_RECOVERY;
		p->timer = RECOVERY[p->move];
	} else if (p->phase == FIGHT_RECOVERY && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
		p->combo = 0;
	} else if (p->phase == FIGHT_HIT && p->timer <= 0) {
		p->phase = FIGHT_IDLE;
	}
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
			vic->hp -= DAMAGE[att->move];
			if (vic->hp < 0)
				vic->hp = 0;
			vic->phase = FIGHT_HIT;
			vic->timer = 14;
			vic->vx = att->face * 80.0f;
			vic->combo = 0;
		}
	}
}

void fight_hits(Fighter *a, Fighter *b)
{
	one_hit(a, b);
	one_hit(b, a);
}
