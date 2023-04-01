#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

gentity_s* fire_grenade(gentity_s *parent, float *start, float *dir, int grenadeWPID, int time)
{
	float speed;
	WeaponDef *weaponDef;
	gentity_s *grenade;

	grenade = G_Spawn();

	if ( parent->client && parent->client->ps.grenadeTimeLeft )
	{
		grenade->nextthink = level.time + parent->client->ps.grenadeTimeLeft;
		parent->client->ps.grenadeTimeLeft = 0;
	}
	else
	{
		grenade->nextthink = level.time + time;
	}

	if ( parent->client )
		parent->client->ps.grenadeTimeLeft = 0;

	grenade->handler = 7;
	grenade->s.eType = ET_MISSILE;
	grenade->r.svFlags = 8;
	grenade->s.weapon = grenadeWPID;
	grenade->r.ownerNum = parent->s.number;
	grenade->parent = parent;

	weaponDef = BG_GetWeaponDef(grenadeWPID);
	Scr_SetString(&grenade->classname, scr_const.grenade);

	grenade->dmg = weaponDef->damage;
	grenade->s.eFlags = 0x1000000;
	grenade->clipmask = 41953425;
	grenade->s.time = level.time + 50;
	grenade->s.pos.trType = TR_GRAVITY;
	grenade->s.pos.trTime = level.time;

	VectorCopy(start, grenade->s.pos.trBase);
	VectorCopy(dir, grenade->s.pos.trDelta);

	SnapVector(grenade->s.pos.trDelta);

	grenade->s.apos.trType = TR_LINEAR;
	grenade->s.apos.trTime = level.time;

	vectoangles(dir, grenade->s.apos.trBase);
	speed = grenade->s.apos.trBase[0] - 120.0;

	grenade->s.apos.trBase[0] = AngleNormalize360(speed);
	grenade->s.apos.trDelta[0] = flrand(-45.0, 45.0) + 720.0;
	grenade->s.apos.trDelta[1] = 0.0;
	grenade->s.apos.trDelta[2] = flrand(-45.0, 45.0) + 360.0;

	VectorCopy(start, grenade->r.currentOrigin);
	VectorCopy(grenade->s.apos.trBase, grenade->r.currentAngles);

	return grenade;
}

gentity_s* fire_rocket(gentity_s *parent, float *start, float *dir)
{
	float speed;
	WeaponDef *weaponDef;
	gentity_s *rocket;

	Vec3Normalize(dir);
	weaponDef = BG_GetWeaponDef(parent->s.weapon);
	rocket = G_Spawn();
	Scr_SetString(&rocket->classname, scr_const.rocket);
	rocket->nextthink = level.time + 30000;
	rocket->handler = 8;
	rocket->s.eType = ET_MISSILE;
	rocket->s.eFlags |= 0x400u;
	rocket->r.svFlags = 8;
	rocket->s.weapon = parent->s.weapon;
	rocket->r.ownerNum = parent->s.number;
	rocket->parent = parent;
	rocket->dmg = weaponDef->damage;
	rocket->clipmask = 41953425;
	rocket->s.time = level.time + 50;
	rocket->s.pos.trType = TR_LINEAR;
	rocket->s.pos.trTime = level.time - 50;
	VectorCopy(start, rocket->s.pos.trBase);
	speed = (float)weaponDef->projectileSpeed;
	VectorScale(dir, speed, rocket->s.pos.trDelta);

	SnapVector(rocket->s.pos.trDelta);

	VectorCopy(start, rocket->r.currentOrigin);
	vectoangles(dir, rocket->r.currentAngles);
	G_SetAngle(rocket, rocket->r.currentAngles);
	rocket->missile.time = (long double)weaponDef->destabilizeDistance
	                       / (long double)weaponDef->projectileSpeed
	                       * 1000.0;
	rocket->flags |= parent->flags & 0x20000;
	return rocket;
}