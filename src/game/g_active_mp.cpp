#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define bgs (*((bgs_t**)( 0x0855A4E0 )))
#else
extern bgs_t *bgs;
#endif

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

extern dvar_t *g_antilag;
void ClientEvents(gentity_s *ent, int oldEventSequence)
{
	gclient_s *self;
	vec3_t dir;
	float dmgScale;
	gclient_s *client;
	int dmg;
	int event;
	int i;

	client = ent->client;

	if ( oldEventSequence < client->ps.eventSequence - 4 )
		oldEventSequence = client->ps.eventSequence - 4;

	for ( i = oldEventSequence; i < client->ps.eventSequence; ++i )
	{
		event = client->ps.events[i & 3];
		dmg = client->ps.eventParms[i & 3];

		if ( event < EV_LANDING_PAIN_DEFAULT || event > EV_LANDING_PAIN_ASPHALT )
		{
			switch ( event )
			{
			case EV_FIRE_WEAPON:
			case EV_FIRE_WEAPONB:
			case EV_FIRE_WEAPON_LASTSHOT:
			case EV_FIRE_WEAPON_MG42:
				if ( g_antilag->current.boolean )
					FireWeapon(ent, client->lastServerTime);
				else
					FireWeapon(ent, level.time);
				break;
			case EV_FIRE_MELEE:
				FireWeaponMelee(ent);
				break;
			case EV_USE_OFFHAND:
				G_UseOffHand(ent);
				break;
			case EV_BINOCULAR_ENTER:
				Scr_Notify(ent, scr_const.binocular_enter, 0);
				break;
			case EV_BINOCULAR_EXIT:
				Scr_Notify(ent, scr_const.binocular_exit, 0);
				break;
			case EV_BINOCULAR_FIRE:
				Scr_Notify(ent, scr_const.binocular_fire, 0);
				break;
			case EV_BINOCULAR_RELEASE:
				Scr_Notify(ent, scr_const.binocular_release, 0);
				break;
			case EV_BINOCULAR_DROP:
				Scr_Notify(ent, scr_const.binocular_drop, 0);
				break;
			case EV_GRENADE_SUICIDE:
				if ( ent->client && (ent->flags & 3) == 0 )
				{
					self = ent->client;
					ent->health = 0;
					self->ps.stats[0] = 0;
					player_die(ent, ent, ent, 100000, 12, 0, 0, HITLOC_NONE, 0);
				}
				break;
			default:
				continue;
			}
		}
		else
		{
			if ( ent->s.eType != ET_PLAYER )
				return;

			if ( dmg <= 99 )
				dmgScale = (long double)dmg * 0.0099999998;
			else
				dmgScale = 1.1;

			if ( dmgScale != 0.0 )
			{
				dmgScale = (long double)client->ps.stats[2] * dmgScale;
				VectorSet(dir, 0.0, 0.0, 1.0);
				G_Damage(ent, 0, 0, 0, 0, (int)dmgScale, 0, 11, HITLOC_NONE, 0);
			}
		}
	}
}

extern dvar_t *g_mantleBlockTimeBuffer;
void G_AddPlayerMantleBlockage(float *endPos, int duration, pmove_t *pm)
{
	gentity_s *owner;
	gentity_s *ent;

	owner = &g_entities[pm->ps->clientNum];
	ent = G_Spawn();
	ent->parent = owner;
	ent->r.ownerNum = pm->ps->clientNum;
	ent->r.contents = 0x10000;
	ent->clipmask = 0x10000;
	ent->r.svFlags = 33;
	ent->s.eType = ET_INVISIBLE;
	ent->handler = 19;
	VectorCopy(owner->r.mins, ent->r.mins);
	VectorCopy(owner->r.maxs, ent->r.maxs);
	G_SetOrigin(ent, endPos);
	SV_LinkEntity(ent);
	ent->nextthink = g_mantleBlockTimeBuffer->current.integer + level.time + duration;
}

extern dvar_t *g_inactivity;
int ClientInactivityTimer(gclient_s *client)
{
	if ( !g_inactivity->current.integer )
	{
		client->inactivityTime = level.time + 60000;
		client->inactivityWarning = 0;
		return 1;
	}

	if ( client->sess.cmd.forwardmove || client->sess.cmd.rightmove || (client->sess.cmd.buttons & 0x401) != 0 )
	{
		client->inactivityTime = level.time + 1000 * g_inactivity->current.integer;
		client->inactivityWarning = 0;
		return 1;
	}

	if ( client->sess.localClient )
		return 1;

	if ( level.time <= client->inactivityTime )
	{
		if ( level.time > client->inactivityTime - 10000 && !client->inactivityWarning )
		{
			client->inactivityWarning = 1;
			SV_GameSendServerCommand(client - level.clients, 0, va("%c \"GAME_INACTIVEDROPWARNING\"", 99));
		}

		return 1;
	}

	SV_GameDropClient(client - level.clients, "GAME_DROPPEDFORINACTIVITY");

	return 0;
}

void ClientIntermissionThink(gentity_s *ent)
{
	gclient_s *client;

	client = ent->client;
	client->oldbuttons = client->buttons;
	client->buttons = client->sess.cmd.buttons;
	client->buttonsSinceLastFrame |= client->buttons & ~client->oldbuttons;
}

qboolean G_ClientCanSpectateTeam(gclient_s *client, team_t team)
{
	return ((unsigned char)(client->sess.noSpectate >> team) ^ 1) & 1;
}

void SpectatorThink(gentity_s *ent, usercmd_s *ucmd)
{
	gclient_s *client;
	pmove_t pmove;

	client = ent->client;
	client->oldbuttons = client->buttons;
	client->buttons = client->sess.cmd.buttons;
	client->buttonsSinceLastFrame |= client->buttons & ~client->oldbuttons;

	if ( client->sess.forceSpectatorClient < 0
	        && G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS)
	        && client->spectatorClient >= 0
	        && (client->buttons & 4) != (client->oldbuttons & 4) )
	{
		StopFollowing(ent);
	}

	if ( (client->buttons & 1) != 0 && ((LOBYTE(client->oldbuttons) ^ 1) & 1) != 0 )
	{
		Cmd_FollowCycle_f(ent, 1);
	}
	else if ( (client->buttons & 0x1000) != 0 && (client->oldbuttons & 0x1000) == 0 )
	{
		Cmd_FollowCycle_f(ent, -1);
	}
	if ( (client->ps.pm_flags & 0x400000) == 0 )
	{
		client->ps.pm_type = 4;

		if ( G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
			client->ps.speed = 400;
		else
			client->ps.speed = 0;

		memset(&pmove, 0, sizeof(pmove));
		pmove.ps = &client->ps;
		pmove.cmd = *ucmd;
		pmove.tracemask = 8390673;
		pmove.handler = 1;
		Pmove(&pmove);
		VectorCopy(client->ps.origin, ent->r.currentOrigin);
		SV_UnlinkEntity(ent);
	}
}

extern dvar_t *g_smoothClients;
void ClientThink_real(gentity_s *ent, usercmd_s *ucmd)
{
	int flags;
	float vAxis3[3][3];
	float vAxis2[3][3];
	float vAxis[3][3];
	int shellshockDuration;
	float shellshockScale;
	float shellshockCalc;
	WeaponDef *weaponDef;
	vec3_t angles;
	vec3_t viewangles;
	weaponState_t ws;
	viewState_t vs;
	int frametime;
	int eventSequence;
	pmove_t pmove;
	gclient_s *client;

	client = ent->client;

	if ( client->sess.connected == CS_CONNECTED )
	{
		if ( ucmd->serverTime > level.time + 200 )
			ucmd->serverTime = level.time + 200;

		if ( ucmd->serverTime < level.time - 1000 )
			ucmd->serverTime = level.time - 1000;

		frametime = ucmd->serverTime - client->ps.commandTime;

		if ( frametime > 0 || client->ps.clientNum != ent - g_entities )
		{
			if ( frametime > 200 )
				frametime = 200;

			if ( client->bFrozen )
				flags = client->ps.pm_flags | 0x8000;
			else
				flags = client->ps.pm_flags & 0xFFFF7FFF;

			client->ps.pm_flags = flags;

			if ( client->sess.sessionState == STATE_INTERMISSION )
			{
				ClientIntermissionThink(ent);
			}
			else if ( client->sess.sessionState == STATE_SPECTATOR )
			{
				SpectatorThink(ent, ucmd);
			}
			else if ( ClientInactivityTimer(client) )
			{
				eventSequence = client->ps.eventSequence;
				memset(&pmove, 0, sizeof(pmove));
				pmove.ps = &client->ps;
				pmove.cmd = *ucmd;
				pmove.oldcmd = client->sess.oldcmd;

				if ( client->ps.pm_type <= 5 )
					pmove.tracemask = 42008593;
				else
					pmove.tracemask = 8454161;

				pmove.handler = 1;
				VectorCopy(client->ps.origin, client->oldOrigin);

				client->oldbuttons = client->buttons;
				client->buttons = client->sess.cmd.buttons;
				client->latched_buttons = client->buttons & ~client->oldbuttons;
				client->buttonsSinceLastFrame |= client->latched_buttons;

				vs.ps = &client->ps;
				vs.damageTime = client->damageTime;
				vs.time = level.time;
				vs.v_dmg_pitch = client->v_dmg_pitch;
				vs.v_dmg_roll = client->v_dmg_roll;
				vs.xyspeed = BG_GetSpeed(&client->ps, level.time);
				vs.frametime = (float)frametime * 0.001;
				vs.fLastIdleFactor = client->fLastIdleFactor;
				vs.weapIdleTime = &client->weapIdleTime;

				BG_CalculateViewMovementAngles(&vs, angles);
				VectorAdd(client->ps.viewangles, angles, viewangles);
				weaponDef = BG_GetWeaponDef(client->ps.weapon);
				shellshockDuration = client->ps.shellshockTime + client->ps.shellshockDuration - vs.time;

				if ( shellshockDuration <= 0 )
				{
					shellshockScale = 1.0;
				}
				else
				{
					shellshockCalc = 1.0;

					if ( shellshockDuration <= 2999 )
						shellshockCalc = (float)shellshockDuration / 3000.0;

					shellshockCalc = (3.0 - (shellshockCalc + shellshockCalc)) * shellshockCalc * shellshockCalc;
					shellshockScale = (weaponDef->swayShellShockScale - 1.0) * shellshockCalc + 1.0;
				}

				BG_CalculateWeaponPosition_Sway(
				    &client->ps,
				    client->swayViewAngles,
				    client->swayOffset,
				    client->swayAngles,
				    1.0,
				    frametime);

				ws.ps = vs.ps;
				ws.xyspeed = vs.xyspeed;
				ws.frametime = (float)frametime * 0.001;
				VectorCopy(client->vLastMoveAng, ws.vLastMoveAng);
				ws.fLastIdleFactor = client->fLastIdleFactor;
				ws.time = vs.time;
				ws.damageTime = client->damageTime;
				ws.v_dmg_pitch = client->v_dmg_pitch;
				ws.v_dmg_roll = client->v_dmg_roll;

				VectorCopy(client->recoilAngles, ws.recoilAngles);
				VectorCopy(client->recoilSpeed, ws.recoilSpeed);
				VectorCopy(client->swayAngles, ws.swayAngles);

				ws.weapIdleTime = &client->weapIdleTime;
				BG_CalculateWeaponMovement(&ws, angles);

				if ( BG_IsAimDownSightWeapon(ws.ps->weapon) && ws.ps->fWeaponPosFrac != 0.0 && !weaponDef->adsOverlayReticle )
				{
					AnglesToAxis(angles, vAxis);
					AnglesToAxis(viewangles, vAxis2);
					MatrixMultiply(vAxis, vAxis2, vAxis3);
					AxisToAngles(vAxis3, viewangles);
				}

				VectorCopy(ws.vLastMoveAng, client->vLastMoveAng);
				client->fLastIdleFactor = ws.fLastIdleFactor;
				VectorCopy(ws.recoilAngles, client->recoilAngles);
				VectorCopy(ws.recoilSpeed, client->recoilSpeed);
				client->fGunPitch = viewangles[0];
				client->fGunYaw = viewangles[1];

				Pmove(&pmove);

				if ( pmove.mantleStarted )
					G_AddPlayerMantleBlockage(pmove.mantleEndPos, pmove.mantleDuration, &pmove);

				ent->s.animMovetype = (ent->client->ps.pm_flags & 2) != 0;

				if ( ent->client->ps.eventSequence != eventSequence )
				{
					ent->eventTime = level.time;
					ent->r.eventTime = level.time;
				}

				if ( g_smoothClients->current.boolean )
					G_PlayerStateToEntityStateExtrapolate(&ent->client->ps, &ent->s, ent->client->ps.commandTime, 1);
				else
					BG_PlayerStateToEntityState(&ent->client->ps, &ent->s, 1, 1);

				VectorCopy(ent->s.pos.trBase, ent->r.currentOrigin);
				VectorCopy(pmove.mins, ent->r.mins);
				VectorCopy(pmove.maxs, ent->r.maxs);
				ClientEvents(ent, eventSequence);
				SV_LinkEntity(ent);

				if ( !ent->client->noclip )
					G_TouchTriggers(ent);

				VectorCopy(ent->client->ps.origin, ent->r.currentOrigin);
				VectorClear(ent->r.currentAngles);
				ent->r.currentAngles[1] = ent->client->ps.viewangles[1];
				ClientImpacts(ent, &pmove);

				if ( ent->client->ps.eventSequence != eventSequence )
					ent->eventTime = level.time;

				Player_UpdateActivate(ent);
			}
		}
	}
}

extern dvar_t *g_synchronousClients;
void ClientThink(int clientNum)
{
	gentity_s *ent;

	ent = &g_entities[clientNum];
	bgs = &level_bgs;

	memcpy(&ent->client->sess.oldcmd, &ent->client->sess.cmd, sizeof(ent->client->sess.oldcmd));
	SV_GetUsercmd(clientNum, &ent->client->sess.cmd);
	ent->client->lastCmdTime = level.time;

	if ( !g_synchronousClients->current.boolean )
		ClientThink_real(ent, &ent->client->sess.cmd);

	bgs = 0;
}

void G_SetLastServerTime(int clientNum, int lastServerTime)
{
	gentity_s *ent;

	ent = &g_entities[clientNum];

	if ( level.time - lastServerTime > 1000 )
		lastServerTime = level.time - 1000;

	if ( lastServerTime >= ent->client->lastServerTime || level.time <= lastServerTime )
		ent->client->lastServerTime = lastServerTime;
}