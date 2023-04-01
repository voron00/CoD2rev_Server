#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
extern bgs_t level_bgs;
#endif

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

int singleClientEvents[] =
{
	140,
	141,
	142,
	143,
	185,
	186,
	-1,
};

vec3_t TouchTriggersgentity_range = { 40.0, 40.0, 52.0 };

qboolean G_UpdateClientInfo(gentity_s *ent)
{
	gclient_s *client;
	qboolean updated = 0;
	int i;
	const char *tagName;
	const char *modelName;
	clientInfo_t *info;

	client = ent->client;
	info = &level_bgs.clientinfo[ent->s.clientNum];
	modelName = G_ModelName(ent->model);
	client->sess.state.modelindex = ent->model;

	if ( strcmp(info->model, modelName) )
	{
		updated = 1;
		I_strncpyz(info->model, modelName, 64);
	}

	for ( i = 0; i < 6; ++i )
	{
		if ( ent->attachModelNames[i] )
		{
			modelName = G_ModelName(ent->attachModelNames[i]);
			client->sess.state.attachModelIndex[i] = ent->attachModelNames[i];

			if ( strcmp(info->attachModelNames[i], modelName) )
			{
				updated = 1;
				I_strncpyz(info->attachModelNames[i], modelName, 64);
			}

			tagName = SL_ConvertToString(ent->attachTagNames[i]);
			client->sess.state.attachTagIndex[i] = G_TagIndex(tagName);

			if ( strcmp(info->attachTagNames[i], tagName) )
			{
				updated = 1;
				I_strncpyz(info->attachTagNames[i], tagName, 64);
			}
		}
		else
		{
			info->attachModelNames[i][0] = 0;
			info->attachTagNames[i][0] = 0;
			client->sess.state.attachModelIndex[i] = 0;
			client->sess.state.attachTagIndex[i] = 0;
		}
	}

	return updated;
}

void G_UpdatePlayerContents(gentity_s *ent)
{
	if ( ent->client->noclip )
	{
		ent->r.contents = 0;
	}
	else if ( ent->client->ufo )
	{
		ent->r.contents = 0;
	}
	else if ( ent->client->sess.sessionState == STATE_DEAD )
	{
		ent->r.contents = 0;
	}
	else
	{
		ent->r.contents = 0x2000000;
	}
}

void G_PlayerEvent(int clientNum, int event)
{
	vec3_t kickAVel;

	if ( event >= EV_FIRE_WEAPON && (event <= EV_FIRE_WEAPON_LASTSHOT || event == EV_FIRE_WEAPON_MG42) )
		BG_WeaponFireRecoil(&g_entities[clientNum].client->ps, g_entities[clientNum].client->recoilSpeed, kickAVel);
}

void G_PlayerStateToEntityStateExtrapolate(playerState_s *ps, entityState_s *s, int time, int snap)
{
	int flags;
	float lerpTime;
	byte event;
	int j;
	int i;

	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy(ps->origin, s->pos.trBase);
	VectorCopy(ps->velocity, s->pos.trDelta);
	s->pos.trTime = time;
	s->pos.trDuration = 50;
	s->apos.trType = TR_INTERPOLATE;
	VectorCopy(ps->viewangles, s->apos.trBase);
	s->angles2[1] = (float)ps->movementDir;
	s->eFlags = ps->eFlags;

	if ( ps->entityEventSequence - ps->eventSequence >= 0 )
	{
		s->eventParm = 0;
	}
	else
	{
		if ( ps->eventSequence - ps->entityEventSequence > 4 )
			ps->entityEventSequence = ps->eventSequence - 4;

		s->eventParm = LOBYTE(ps->eventParms[ps->entityEventSequence++ & 3]);
	}

	if ( ps->oldEventSequence - ps->eventSequence > 0 )
		ps->oldEventSequence = ps->eventSequence;

	for ( i = ps->oldEventSequence; i != ps->eventSequence; ++i )
	{
		event = ps->events[i & 3];
		G_PlayerEvent(s->number, event);

		for ( j = 0; singleClientEvents[j] > 0 && singleClientEvents[j] != event; ++j )
			;

		if ( singleClientEvents[j] < 0 )
		{
			s->events[s->eventSequence & 3] = event;
			s->eventParms[s->eventSequence++ & 3] = LOBYTE(ps->eventParms[i & 3]);
		}
	}

	ps->oldEventSequence = ps->eventSequence;
	s->weapon = LOBYTE(ps->weapon);
	s->groundEntityNum = LOWORD(ps->groundEntityNum);

	if ( (ps->pm_flags & 0xC00000) != 0 )
		s->eType = ET_PLAYER;
	else
		s->eType = ET_INVISIBLE;

	if ( snap )
	{
		s->pos.trBase[0] = (float)(int)s->pos.trBase[0];
		s->pos.trBase[1] = (float)(int)s->pos.trBase[1];
		s->pos.trBase[2] = (float)(int)s->pos.trBase[2];
		s->apos.trBase[0] = (float)(int)s->apos.trBase[0];
		s->apos.trBase[1] = (float)(int)s->apos.trBase[1];
		s->apos.trBase[2] = (float)(int)s->apos.trBase[2];
	}

	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum;

	if ( (ps->eFlags & 0x300) != 0 )
		s->otherEntityNum = ps->viewlocked_entNum;

	if ( ps->pm_type <= 5 )
		flags = s->eFlags & 0xFFFDFFFF;
	else
		flags = s->eFlags | 0x20000;

	s->eFlags = flags;

	if ( (ps->pm_flags & 0x40) != 0 )
		flags = s->eFlags | 0x40000;
	else
		flags = s->eFlags & 0xFFFBFFFF;

	s->eFlags = flags;
	s->leanf = ps->leanf;

	if ( PM_GetEffectiveStance(ps) == 1 )
	{
		lerpTime = (float)(ps->commandTime - ps->viewHeightLerpTime)
		           / (float)PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);

		if ( lerpTime >= 0.0 )
		{
			if ( lerpTime > 1.0 )
				lerpTime = 1.0;
		}
		else
		{
			lerpTime = 0.0;
		}

		if ( !ps->viewHeightLerpDown )
			lerpTime = 1.0 - lerpTime;

		s->fTorsoHeight = ps->fTorsoHeight * lerpTime;
		s->fTorsoPitch = AngleNormalize180(ps->fTorsoPitch) * lerpTime;
		s->fWaistPitch = AngleNormalize180(ps->fWaistPitch) * lerpTime;
	}
	else
	{
		s->fTorsoHeight = 0.0;
		s->fTorsoPitch = 0.0;
		s->fWaistPitch = 0.0;
	}
}

void ClientImpacts(gentity_s *ent, pmove_t *pm)
{
	void (*entTouch)(struct gentity_s *, struct gentity_s *, int);
	void (*otherTouch)(struct gentity_s *, struct gentity_s *, int);
	gentity_s *other;
	int j;
	int i;

	otherTouch = entityHandlers[ent->handler].touch;

	for ( i = 0; i < pm->numtouch; ++i )
	{
		for ( j = 0; j < i && pm->touchents[j] != pm->touchents[i]; ++j )
			;

		if ( j == i )
		{
			other = &g_entities[pm->touchents[i]];

			if ( Scr_IsSystemActive() )
			{
				Scr_AddEntity(other);
				Scr_Notify(ent, scr_const.touch, 1u);
				Scr_AddEntity(ent);
				Scr_Notify(other, scr_const.touch, 1u);
			}

			entTouch = entityHandlers[other->handler].touch;

			if ( entTouch )
				entTouch(other, ent, 1);

			if ( otherTouch )
				otherTouch(ent, other, 1);
		}
	}
}

void G_TouchTriggers(gentity_s *ent)
{
	void (*entTouch)(struct gentity_s *, struct gentity_s *, int);
	void (*otherTouch)(struct gentity_s *, struct gentity_s *, int);
	vec3_t maxs;
	vec3_t mins;
	gentity_s *touch;
	int entityList[1024];
	int entities;
	int i;

	if ( ent->client->ps.pm_type <= 1 )
	{
		VectorSubtract(ent->client->ps.origin, TouchTriggersgentity_range, mins);
		VectorAdd(ent->client->ps.origin, TouchTriggersgentity_range, maxs);
		entities = CM_AreaEntities((const vec3_t *)mins, (const vec3_t *)maxs, entityList, 1024, 1079771144);
		VectorAdd(ent->client->ps.origin, ent->r.mins, mins);
		VectorAdd(ent->client->ps.origin, ent->r.maxs, maxs);
		ShrinkBoundsToHeight(mins, maxs);
		otherTouch = entityHandlers[ent->handler].touch;

		for ( i = 0; ; ++i )
		{
			if ( i >= entities )
				return;

			touch = &g_entities[entityList[i]];
			entTouch = entityHandlers[touch->handler].touch;

			if ( entTouch || otherTouch )
			{
				if ( touch->s.eType == ET_ITEM )
				{
					if ( !BG_PlayerTouchesItem(&ent->client->ps, &touch->s, level.time) )
						continue;
				}
				else if ( !SV_EntityContact(mins, maxs, touch) )
				{
					continue;
				}

				if ( Scr_IsSystemActive() )
				{
					Scr_AddEntity(ent);
					Scr_Notify(touch, scr_const.touch, 1);
					Scr_AddEntity(touch);
					Scr_Notify(ent, scr_const.touch, 1);
				}

				if ( entTouch )
					entTouch(touch, ent, 1);
			}
		}
	}
}

int GetFollowPlayerState(int clientNum, playerState_s *ps)
{
	if ( (g_entities[clientNum].client->ps.pm_flags & 0x800000) == 0 )
		return 0;

	memcpy(ps, g_entities[clientNum].client, sizeof(playerState_s));
	memset(&ps->hud, 0, sizeof(hudElemState_t) / 2);

	return 1;
}