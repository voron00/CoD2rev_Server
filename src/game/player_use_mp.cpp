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

#ifdef TESTING_LIBRARY
#define entityHandlers ((entityHandler_t*)( 0x08167880 ))
#else
const entityHandler_t entityHandlers[] =
{

};
#endif

bool Player_ActivateCmd(gentity_s *ent)
{
	if ( !Scr_IsSystemActive() )
		return 0;

	ent->client->useHoldEntity = 1023;

	if ( ent->active )
	{
		if ( (ent->client->ps.eFlags & 0x300) != 0 )
			ent->active = 2;
		else
			ent->active = 0;

		return 1;
	}
	else if ( (ent->client->ps.pm_flags & 4) != 0 )
	{
		return 1;
	}
	else if ( ent->client->ps.cursorHintEntIndex == 1023 )
	{
		return 0;
	}
	else
	{
		ent->client->useHoldEntity = ent->client->ps.cursorHintEntIndex;
		ent->client->useHoldTime = level.time;

		return 1;
	}
}

void Player_UseEntity(gentity_s *playerEnt, gentity_s *useEnt)
{
	void (*touch)(struct gentity_s *, struct gentity_s *, int);
	void (*use)(struct gentity_s *, struct gentity_s *, struct gentity_s *);

	use = entityHandlers[useEnt->handler].use;
	touch = entityHandlers[useEnt->handler].touch;

	if ( useEnt->s.eType == ET_ITEM )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.touch, 1);
		useEnt->active = 1;

		if ( touch )
			touch(useEnt, playerEnt, 0);
	}
	else if ( useEnt->s.eType != ET_TURRET || G_IsTurretUsable(useEnt, playerEnt) )
	{
		Scr_AddEntity(playerEnt);
		Scr_Notify(useEnt, scr_const.trigger, 1);

		if ( use )
			use(useEnt, playerEnt, playerEnt);
	}

	playerEnt->client->useHoldEntity = 1023;
}

extern dvar_t *g_useholdspawndelay;
extern dvar_t *g_useholdtime;
void Player_ActivateHoldCmd(gentity_s *ent)
{
	if ( Scr_IsSystemActive()
	        && ent->client->useHoldEntity != 1023
	        && level.time - ent->client->lastSpawnTime >= g_useholdspawndelay->current.integer
	        && level.time - ent->client->useHoldTime >= g_useholdtime->current.integer )
	{
		Player_UseEntity(ent, &g_entities[ent->client->useHoldEntity]);
	}
}

void Player_UpdateActivate(gentity_s *ent)
{
	bool useSucceeded;

	ent->client->ps.pm_flags &= ~8u;

	if ( ent->client->ps.weaponstate < WEAPON_BINOCULARS_INIT || ent->client->ps.weaponstate > WEAPON_BINOCULARS_END )
	{
		useSucceeded = 0;

		if ( ent->client->useHoldEntity == 1023
		        || (ent->client->oldbuttons & 0x20) == 0
		        || (ent->client->buttons & 0x20) != 0 )
		{
			if ( (ent->client->latched_buttons & 0x28) != 0 )
				useSucceeded = Player_ActivateCmd(ent);

			if ( ent->client->useHoldEntity != 1023 || useSucceeded )
			{
				if ( (ent->client->buttons & 0x28) != 0 )
					Player_ActivateHoldCmd(ent);
			}
			else if ( (ent->client->latched_buttons & 0x20) != 0 )
			{
				ent->client->ps.pm_flags |= 8u;
			}
		}
		else
		{
			ent->client->ps.pm_flags |= 8u;
		}
	}
}