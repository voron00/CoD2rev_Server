#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

void G_Trigger(gentity_s *self, gentity_s *other)
{
	trigger_info_t *trigger_info;

	if ( Scr_IsSystemActive() )
	{
		if ( level.pendingTriggerListSize == 256 )
		{
			Scr_AddEntity(other);
			Scr_Notify(self, scr_const.trigger, 1);
		}
		else
		{
			trigger_info = &level.pendingTriggerList[level.pendingTriggerListSize++];
			trigger_info->entnum = self->s.number;
			trigger_info->otherEntnum = other->s.number;
			trigger_info->useCount = self->useCount;
			trigger_info->otherUseCount = other->useCount;
		}
	}
}

qboolean Respond_trigger_damage(gentity_s *pEnt, int iMOD)
{
	if ( (pEnt->spawnflags & 1) != 0 && iMOD == MOD_PISTOL_BULLET )
		return 0;

	if ( (pEnt->spawnflags & 2) != 0 && iMOD == MOD_RIFLE_BULLET )
		return 0;

	if ( (pEnt->spawnflags & 4) != 0 && iMOD >= MOD_GRENADE && iMOD <= MOD_PROJECTILE_SPLASH )
		return 0;

	if ( (pEnt->spawnflags & 8) != 0 && iMOD >= MOD_GRENADE && (iMOD <= MOD_PROJECTILE_SPLASH || iMOD == MOD_EXPLOSIVE) )
		return 0;

	if ( (pEnt->spawnflags & 0x10) != 0 && (iMOD == MOD_GRENADE_SPLASH || iMOD == MOD_PROJECTILE_SPLASH) )
		return 0;

	if ( (pEnt->spawnflags & 0x20) != 0 && iMOD == MOD_MELEE )
		return 0;

	return (pEnt->spawnflags & 0x100) == 0 || iMOD && (iMOD <= MOD_HEAD_SHOT || iMOD > MOD_TRIGGER_HURT);
}

void Activate_trigger_damage(gentity_s *pEnt, gentity_s *pOther, int iDamage, int iMOD)
{
	if ( pEnt->trigger.threshold <= 0 || iDamage >= pEnt->trigger.threshold )
	{
		if ( Respond_trigger_damage(pEnt, iMOD) )
		{
			pEnt->health -= iDamage;

			if ( !pEnt->trigger.accumulate || 32000 - pEnt->health >= pEnt->trigger.accumulate )
			{
				if ( iMOD != -1 )
					G_Trigger(pEnt, pOther);

				pEnt->health = 32000;

				if ( (pEnt->spawnflags & 0x200) != 0 )
					G_FreeEntityDelay(pEnt);
			}
		}
	}
}

void G_GrenadeTouchTriggerDamage(gentity_s *pActivator, float *vStart, float *vEnd, int iDamage, int iMOD)
{
	gentity_s *pEnt;
	vec3_t maxs;
	vec3_t mins;
	int entityList[1024];
	int entities;
	int i;

	VectorCopy(vStart, mins);
	VectorCopy(vStart, maxs);
	AddPointToBounds(vEnd, mins, maxs);

	entities = CM_AreaEntities(mins, maxs, entityList, 1024, 0x400000);

	for ( i = 0; i < entities; ++i )
	{
		pEnt = &g_entities[entityList[i]];

		if ( pEnt->classname == scr_const.trigger_damage
		        && (pEnt->flags & 0x4000) != 0
		        && SV_SightTraceToEntity(vStart, vec3_origin, vec3_origin, vEnd, pEnt->s.number, -1) )
		{
			Scr_AddEntity(pActivator);
			Scr_AddInt(iDamage);
			Scr_Notify(pEnt, scr_const.damage, 2u);
			Activate_trigger_damage(pEnt, pActivator, iDamage, iMOD);

			if ( !pEnt->trigger.accumulate )
				pEnt->health = 32000;
		}
	}
}

void G_CheckHitTriggerDamage(gentity_s *pActivator, float *vStart, float *vEnd, int iDamage, unsigned int iMOD)
{
	gentity_s *pEnt;
	vec3_t absmax;
	vec3_t absmin;
	vec3_t maxs;
	vec3_t mins;
	int entityList[1024];
	int entities;
	int i;

	VectorCopy(vStart, mins);
	VectorCopy(vStart, maxs);

	AddPointToBounds(vEnd, mins, maxs);
	entities = CM_AreaEntities(mins, maxs, entityList, 1024, 0x400000);

	for ( i = 0; i < entities; ++i )
	{
		pEnt = &g_entities[entityList[i]];

		if ( pEnt->classname == scr_const.trigger_damage )
		{
			VectorAdd(pEnt->r.currentOrigin, pEnt->r.mins, absmin);
			VectorAdd(pEnt->r.currentOrigin, pEnt->r.maxs, absmax);

			if ( SV_SightTraceToEntity(vStart, vec3_origin, vec3_origin, vEnd, pEnt->s.number, -1) )
			{
				Scr_AddEntity(pActivator);
				Scr_AddInt(iDamage);
				Scr_Notify(pEnt, scr_const.damage, 2u);

				Activate_trigger_damage(pEnt, pActivator, iDamage, iMOD);

				if ( !pEnt->trigger.accumulate )
					pEnt->health = 32000;
			}
		}
	}
}

void InitTriggerWait(gentity_s *ent, int spawnflag)
{
	float wait;

	if ( level.spawnVars.spawnVarsValid && G_SpawnFloat("wait", "", &wait) && wait <= 0.0 )
		ent->spawnflags |= spawnflag;
}

void InitTrigger(gentity_s *self)
{
	SV_SetBrushModel(self);
	self->r.contents = 1079771144;
	self->r.svFlags = 1;
	self->s.eFlags |= 1u;
}

void InitSentientTrigger(gentity_s *self)
{
	self->r.contents = 0;

	if ( (self->spawnflags & 8) == 0 )
		self->r.contents |= 0x40000000u;
	if ( (self->spawnflags & 1) != 0 )
		self->r.contents |= 0x40000u;
	if ( (self->spawnflags & 2) != 0 )
		self->r.contents |= 0x80000u;
	if ( (self->spawnflags & 4) != 0 )
		self->r.contents |= 0x100000u;
}

void SP_trigger_multiple(gentity_s *self)
{
	self->handler = 1;
	InitTriggerWait(self, 16);
	InitTrigger(self);
	InitSentientTrigger(self);
	SV_LinkEntity(self);
}

void SP_trigger_radius(gentity_s *ent)
{
	float height;
	float radius;

	if ( level.spawnVars.spawnVarsValid )
	{
		if ( !G_SpawnFloat("radius", "", &radius) )
		{
			Com_Error(ERR_DROP, va("radius not specified for trigger_radius at (%g %g %g)",  ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]));
		}

		if ( !G_SpawnFloat("height", "", &height) )
		{
			Com_Error(ERR_DROP, va("height not specified for trigger_radius at (%g %g %g)", ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]));
		}
	}
	else
	{
		if ( Scr_GetNumParam() <= 4 )
			Scr_Error("USAGE: spawn( \"trigger_radius\", <origin>, <spawnflags>, <radius>, <height> )");

		radius = Scr_GetFloat(3u);
		height = Scr_GetFloat(4u);
	}

	ent->handler = 1;

	ent->r.mins[0] = -radius;
	ent->r.mins[1] = -radius;
	ent->r.mins[2] = 0.0;
	ent->r.maxs[0] = radius;
	ent->r.maxs[1] = radius;
	ent->r.maxs[2] = height;

	ent->r.svFlags = 33;
	InitTriggerWait(ent, 16);
	InitSentientTrigger(ent);
	SV_LinkEntity(ent);
}

void SP_trigger_disk(gentity_s *ent)
{
	float radius;

	if ( !G_SpawnFloat("radius", "", &radius) )
	{
		Com_Error(ERR_DROP, va("radius not specified for trigger_radius at (%g %g %g)", ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2]));
	}

	ent->handler = 1;
	radius = radius + 64.0;

	ent->r.mins[0] = -radius;
	ent->r.mins[1] = -radius;
	ent->r.mins[2] = -100000.0;
	ent->r.maxs[0] = radius;
	ent->r.maxs[1] = radius;
	ent->r.maxs[2] = 100000.0;

	ent->r.svFlags = 65;
	InitTriggerWait(ent, 16);
	InitSentientTrigger(ent);
	SV_LinkEntity(ent);
}

void SP_trigger_hurt(gentity_s *self)
{
	const char *sound;

	InitTrigger(self);
	G_SpawnString("sound", "world_hurt_me", &sound);

	if ( !self->dmg )
		self->dmg = 5;

	self->r.contents = 1079771144;

	if ( ((LOBYTE(self->spawnflags) ^ 1) & 1) != 0 )
		self->handler = 3;
	else
		self->handler = 2;
}

void SP_trigger_once(gentity_s *ent)
{
	ent->handler = 1;
	ent->spawnflags |= 0x10u;
	InitTrigger(ent);
	InitSentientTrigger(ent);
	SV_LinkEntity(ent);
}

void SP_trigger_use(gentity_s *self)
{
	trigger_use_shared(self);
}

void SP_trigger_use_touch(gentity_s *self)
{
	trigger_use_shared(self);
}

void SP_trigger_damage(gentity_s *ent)
{
	G_SpawnInt("accumulate", "0", &ent->trigger.accumulate);
	G_SpawnInt("threshold", "0", &ent->trigger.threshold);
	ent->health = 32000;
	ent->takedamage = 1;
	ent->handler = 4;
	InitTriggerWait(ent, 512);
	InitTrigger(ent);
	SV_LinkEntity(ent);
}

void SP_trigger_lookat(gentity_s *ent)
{
	SV_SetBrushModel(ent);
	ent->r.contents = 0x20000000;
	ent->r.svFlags = 1;
	ent->s.eFlags |= 1u;
	SV_LinkEntity(ent);
}

void multi_trigger(gentity_s *self)
{
	if ( (self->spawnflags & 0x10) != 0 )
		G_FreeEntityDelay(self);
}

void Touch_Multi(gentity_s *self, gentity_s *other, int touched)
{
	G_Trigger(self, other);
	multi_trigger(self);
}

void hurt_use(gentity_s *self, gentity_s *ent, gentity_s *other)
{
	if ( self->handler == 3 )
		self->handler = 2;
	else
		self->handler = 3;
}

void hurt_touch(gentity_s *self, gentity_s *other, int touched)
{
	int dflags;

	if ( other->takedamage && self->trigger.timestamp <= level.time )
	{
		G_Trigger(self, other);

		if ( (self->spawnflags & 0x10) != 0 )
			self->trigger.timestamp = level.time + 1000;
		else
			self->trigger.timestamp = level.time + 50;

		if ( (self->spawnflags & 8) != 0 )
			dflags = 16;
		else
			dflags = 0;

		G_Damage(other, self, self, 0, 0, self->dmg, dflags, 13, HITLOC_NONE, 0);

		if ( (self->spawnflags & 0x20) != 0 )
			self->handler = 2;
	}
}

void Use_trigger_damage(gentity_s *pEnt, gentity_s *pOther, gentity_s *pActivator)
{
	Activate_trigger_damage(pEnt, pOther, pEnt->trigger.accumulate + 1, -1);
}

void Pain_trigger_damage(gentity_s *pSelf, gentity_s *pAttacker, int iDamage, const float *vPoint, int iMod, const float *vDir, int hitLoc)
{
	Activate_trigger_damage(pSelf, pAttacker, iDamage, iMod);

	if ( !pSelf->trigger.accumulate )
		pSelf->health = 32000;
}

void Die_trigger_damage(gentity_s *pSelf, gentity_s *pInflictor, gentity_s *pAttacker, int iDamage, int iMod, int iWeapon, const float *vDir, int hitLoc, int psTimeOffset)
{
	Activate_trigger_damage(pSelf, pAttacker, iDamage, iMod);

	if ( !pSelf->trigger.accumulate )
		pSelf->health = 32000;
}

void use_trigger_use(gentity_s *pSelf, gentity_s *pEnt, gentity_s *pOther)
{
	;
}