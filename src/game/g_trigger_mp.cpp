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
	entities = CM_AreaEntities((const vec3_t *)mins, (const vec3_t *)maxs, entityList, 1024, 0x400000);

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