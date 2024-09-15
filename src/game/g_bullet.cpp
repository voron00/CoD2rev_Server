#include "../qcommon/qcommon.h"
#include "g_shared.h"

int Bullet_GetDamage(const weaponParms *wp, float dist)
{
	float damage;
	float minDamage;
	float maxDamage;
	float range;

	if ( wp->weapDef->maxDamageRange > dist )
		return wp->weapDef->damage;

	if ( wp->weapDef->minDamageRange <= dist )
		return wp->weapDef->minDamage;

	range = wp->weapDef->minDamageRange - wp->weapDef->maxDamageRange;

	if ( range == 0.0 )
		return wp->weapDef->damage;

	maxDamage = (dist - wp->weapDef->maxDamageRange) / range;
	minDamage = (float)wp->weapDef->minDamage;
	damage = (float)wp->weapDef->damage;

	return (int)lerp(damage, minDamage, maxDamage);
}

#define MAX_BULLET_RESURSIONS 12
void Bullet_Fire_Extended(const gentity_s *inflictor, gentity_s *attacker, float *start, float *end, float dmgScale, int resursion, const weaponParms *wp, const gentity_s *weaponEnt, int gameTime)
{
	float scale;
	int event;
	float dot;
	vec3_t dir;
	vec3_t origin;
	int meansOfDeath;
	int dflags;
	int damage;
	float dist;
	vec3_t temp;
	gentity_s *hitEnt;
	gentity_s *bulletEffectTempEnt;
	trace_t trace;

	dflags = 0;

	if ( resursion > MAX_BULLET_RESURSIONS )
	{
		Com_DPrintf("Bullet_Fire_Extended: Too many resursions, bullet aborted\n");
		return;
	}

	if ( wp->weapDef->rifleBullet )
	{
		meansOfDeath = MOD_RIFLE_BULLET;
		dflags = DFLAGS_PASSTHRU;
	}
	else
	{
		meansOfDeath = MOD_PISTOL_BULLET;
	}

	if ( wp->weapDef->armorPiercing )
		dflags |= DFLAGS_NO_ARMOR;

	if ( wp->weapDef->rifleBullet )
		G_LocationalTrace(&trace, start, end, inflictor->s.number, 41953329, riflePriorityMap);
	else
		G_LocationalTrace(&trace, start, end, inflictor->s.number, 41953329, bulletPriorityMap);

	Vec3Lerp(start, end, trace.fraction, origin);
	G_CheckHitTriggerDamage(attacker, start, origin, wp->weapDef->damage, meansOfDeath);
	hitEnt = &g_entities[trace.entityNum];
	VectorSubtract(end, start, dir);
	Vec3Normalize(dir);
	dot = DotProduct(dir, trace.normal) * -2.0;
	VectorMA(dir, dot, trace.normal, dir);

	if ( (trace.surfaceFlags & SURF_SKY) == 0 && !hitEnt->client && trace.fraction < 1.0 )
	{
		if ( wp->weapDef->weaponClass == WEAPCLASS_SPREAD )
		{
			event = EV_BULLET_HIT_SMALL;
		}
		else if ( wp->weapDef->rifleBullet )
		{
			event = EV_SHOTGUN_HIT;
		}
		else
		{
			event = EV_BULLET_HIT_LARGE;
		}

		if ( wp->weapDef->rifleBullet )
			event = EV_SHOTGUN_HIT;
		else
			event = EV_BULLET_HIT_LARGE;

		bulletEffectTempEnt = G_TempEntity(origin, event);
		bulletEffectTempEnt->s.eventParm = DirToByte(trace.normal);
		bulletEffectTempEnt->s.scale = DirToByte(dir);

		if ( hitEnt->s.eType == ET_PLAYER_CORPSE )
			bulletEffectTempEnt->s.surfType = SURF_TYPE_FLESH;
		else
			bulletEffectTempEnt->s.surfType = SURF_TYPEINDEX(trace.surfaceFlags);

		bulletEffectTempEnt->s.otherEntityNum = weaponEnt->s.number;
	}

	if ( (trace.contents & CONTENTS_GLASS) != 0 )
	{
		VectorSubtract(end, start, dir);
		Vec3Normalize(dir);
		dot = DotProduct(trace.normal, dir);

		if ( -dot < 0.125 )
			scale = 0.0;
		else
			scale = 0.25 / -dot;

		VectorMA(origin, scale, dir, start);
		Bullet_Fire_Extended(inflictor, attacker, start, end, dmgScale, resursion + 1, wp, weaponEnt, gameTime);
	}
	else if ( hitEnt->takedamage )
	{
		if ( hitEnt != attacker )
		{
			VectorSubtract(start, origin, temp);
			dist = VectorLength(temp);
			damage = (int)((float)Bullet_GetDamage(wp, dist) * dmgScale);

			G_Damage(hitEnt, attacker, attacker, wp->forward, origin, damage, dflags, meansOfDeath, trace.partGroup, level.time - gameTime);

			if ( hitEnt->client )
			{
				if ( (dflags & DFLAGS_PASSTHRU) != 0 && (Dvar_GetInt("scr_friendlyfire") || !OnSameTeam(hitEnt, attacker)) )
				{
					Bullet_Fire_Extended(hitEnt, attacker, origin, end, dmgScale * 0.5, resursion + 1, wp, weaponEnt, gameTime);
				}
			}
		}
	}
}

void gunrandom(float *x, float *y)
{
	float v2;
	float sinT;
	float theta;
	float r;
	float cosT;

	theta = G_random() * 360.0;
	r = G_random();
	v2 = (float)(theta * 0.017453292);
	cosT = cos(v2);
	sinT = sin(v2);
	*x = r * cosT;
	*y = r * sinT;
}

extern dvar_t *g_fixedWeaponSpreads;
vec2_t fixed_spread_grid[] =
{
	{ 0.0, 0.0 },
	{ 1.0, 0.0 },
	{ -1.0, 0.0 },
	{ 0.0, 1.0 },
	{ 0.0, -1.0 },
	{ 1.0, 1.0 },
	{ -1.0, 1.0 },
	{ 1.0, -1.0 },
	{ -1.0, -1.0 },
};

void Bullet_RandomSpread(float spread, float *end, const weaponParms *wp, float maxRange, int shot)
{
	float r;
	float aimOffset;
	float up;
	float right;

	r = tan(spread * 0.017453292);
	aimOffset = r * maxRange;

	if (g_fixedWeaponSpreads->current.boolean && shot != -1 && shot < ARRAY_COUNT(fixed_spread_grid))
	{
		right = fixed_spread_grid[shot][0];
		up = fixed_spread_grid[shot][1];
	}
	else
		gunrandom(&right, &up);

	right = right * aimOffset;
	up = up * aimOffset;

	VectorMA(wp->muzzleTrace, maxRange, wp->forward, end);
	VectorMA(end, right, wp->right, end);
	VectorMA(end, up, wp->up, end);
}

void Bullet_Fire_Spread(const gentity_s *weaponEnt, gentity_s *attacker, const weaponParms *wp, int gameTime, float spread)
{
	int i;
	vec3_t start;
	vec3_t end;
	int shotCount;

	VectorCopy(wp->muzzleTrace, start);
	shotCount = wp->weapDef->shotCount;

	if (g_fixedWeaponSpreads->current.boolean)
		shotCount++; // Extra bullet for a center shot.

	for ( i = 0; i < shotCount; ++i )
	{
		Bullet_RandomSpread(spread, end, wp, wp->weapDef->minDamageRange, i);
		Bullet_Fire_Extended(weaponEnt, attacker, start, end, 1.0, 0, wp, weaponEnt, gameTime);
	}
}

void Bullet_Fire(gentity_s *attacker, float spread, weaponParms *wp, const gentity_s *ent, int gameTime)
{
	AntilagClientStore store;
	vec3_t endpos;

	G_AntiLagRewindClientPos(gameTime, &store);

	if ( wp->weapDef->weaponClass == WEAPCLASS_SPREAD )
	{
		Bullet_Fire_Spread(ent, attacker, wp, gameTime, spread);
	}
	else
	{
		Bullet_RandomSpread(spread, endpos, wp, 8192.0, -1);
		Bullet_Fire_Extended(ent, attacker, wp->muzzleTrace, endpos, 1.0, 0, wp, ent, gameTime);
	}

	G_AntiLag_RestoreClientPos(&store);
}