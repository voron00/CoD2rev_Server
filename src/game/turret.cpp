#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

qboolean turret_behind(gentity_s *self, gentity_s *other)
{
	float dir[3];
	float centerYaw;
	float minYaw;
	float angle;
	float forward[3];
	turretInfo_s *pTurretInfo;
	float yawSpan;
	float dot;

	pTurretInfo = self->pTurretInfo;
	minYaw = self->r.currentAngles[1] + pTurretInfo->arcmin[1];
	yawSpan = (float)(fabs(pTurretInfo->arcmax[1]) + fabs(pTurretInfo->arcmin[1]) * 0.5);
	centerYaw = AngleNormalize180(minYaw + yawSpan);
	YawVectors(centerYaw, forward, 0);
	Vec3Normalize(forward);
	dir[0] = self->r.currentOrigin[0] - other->r.currentOrigin[0];
	dir[1] = self->r.currentOrigin[1] - other->r.currentOrigin[1];
	dir[2] = 0.0;
	Vec3Normalize(dir);
	dot = (float)((float)(forward[0] * dir[0]) + (float)(forward[1] * dir[1])) + (float)(forward[2] * dir[2]);
	angle = Q_acos(dot) * 57.295776;

	return angle <= yawSpan;
}

qboolean G_IsTurretUsable(gentity_s *useEnt, gentity_s *playerEnt)
{
	if ( useEnt->active || !useEnt->pTurretInfo )
		return 0;

	if ( !turret_behind(useEnt, playerEnt) )
		return 0;

	if ( playerEnt->client->ps.grenadeTimeLeft )
		return 0;

	return playerEnt->client->ps.groundEntityNum != 1023;
}

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