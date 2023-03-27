#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

void G_ClientStopUsingTurret(gentity_s *self)
{
	turretInfo_s *info;
	gentity_s *owner;

	info = self->pTurretInfo;
	owner = &g_entities[self->r.ownerNum];
	info->fireSndDelay = 0;
	self->s.loopSound = 0;

	if ( info->prevStance != -1 )
	{
		if ( info->prevStance == 2 )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_PRONE, 0);
		}
		else if ( info->prevStance == 1 )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_CROUCH, 0);
		}
		else
		{
			G_AddEvent(owner, EV_STANCE_FORCE_STAND, 0);
		}
		info->prevStance = -1;
	}

	TeleportPlayer(owner, info->userOrigin, owner->r.currentAngles);
	owner->client->ps.eFlags &= 0xFFFFFCFF;
	owner->client->ps.viewlocked = 0;
	owner->client->ps.viewlocked_entNum = 1023;
	owner->active = 0;
	owner->s.otherEntityNum = 0;
	self->active = 0;
	self->r.ownerNum = 1023;
	info->flags &= ~0x800u;
}

void G_FreeTurret(gentity_s *ent)
{
	if ( g_entities[ent->r.ownerNum].client )
		G_ClientStopUsingTurret(ent);

	ent->active = 0;
	ent->pTurretInfo->inuse = 0;
	ent->pTurretInfo = 0;
}