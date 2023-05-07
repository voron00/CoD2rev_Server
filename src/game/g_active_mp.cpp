#include "../qcommon/qcommon.h"
#include "g_shared.h"

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

#ifdef LIBCOD
extern dvar_t *g_playerCollision;
#endif

void G_UpdatePlayerContents(gentity_s *ent)
{
#ifdef LIBCOD
	if (!g_playerCollision->current.boolean)
		return;
#endif
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
#ifndef DEDICATED // VoroN: this code runs on client only. Running it on server breaks bots aim.
	vec3_t kickAVel;

	if ( event >= EV_FIRE_WEAPON && (event <= EV_FIRE_WEAPON_LASTSHOT || event == EV_FIRE_WEAPON_MG42) )
		BG_WeaponFireRecoil(&g_entities[clientNum].client->ps, g_entities[clientNum].client->recoilSpeed, kickAVel);
#endif
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
		entities = CM_AreaEntities(mins, maxs, entityList, 1024, 1079771144);
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

qboolean G_ClientCanSpectateTeam(gclient_s *client, int team)
{
	return (client->sess.noSpectate & (1 << team)) == 0;
}

void SpectatorThink(gentity_s *ent, usercmd_s *ucmd)
{
	gclient_s *client;
	pmove_t pm;

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

	if ( (client->buttons & 1) != 0 && (client->oldbuttons & 1) == 0 )
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

		memset(&pm, 0, sizeof(pm));
		pm.ps = &client->ps;
		memcpy(&pm.cmd, ucmd, sizeof(pm.cmd));
		pm.tracemask = 8390673;
		pm.handler = 1;
		Pmove(&pm);
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
	pmove_t pm;
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
				memset(&pm, 0, sizeof(pm));
				pm.ps = &client->ps;
				memcpy(&pm.cmd, ucmd, sizeof(pm.cmd));
				pm.oldcmd = client->sess.oldcmd;

				if ( client->ps.pm_type <= 5 )
					pm.tracemask = 42008593;
				else
					pm.tracemask = 8454161;

				pm.handler = 1;
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

				Pmove(&pm);

				if ( pm.mantleStarted )
					G_AddPlayerMantleBlockage(pm.mantleEndPos, pm.mantleDuration, &pm);

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
				VectorCopy(pm.mins, ent->r.mins);
				VectorCopy(pm.maxs, ent->r.maxs);
				ClientEvents(ent, eventSequence);
				SV_LinkEntity(ent);

				if ( !ent->client->noclip )
					G_TouchTriggers(ent);

				VectorCopy(ent->client->ps.origin, ent->r.currentOrigin);
				VectorClear(ent->r.currentAngles);
				ent->r.currentAngles[1] = ent->client->ps.viewangles[1];
				ClientImpacts(ent, &pm);

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

void G_PlayerController(gentity_s *self, int *partBits)
{
	DObj_s *obj;
	clientInfo_t *ci;

	ci = &level_bgs.clientinfo[self->s.clientNum];
	bgs = &level_bgs;
	obj = Com_GetServerDObj(self->s.number);

	BG_Player_DoControllers(obj, self, partBits, ci, level.frameTime);
}

extern dvar_t *g_friendlyNameDist;
extern dvar_t *g_friendlyfireDist;
void Player_UpdateLookAtEntity(gentity_s *ent)
{
	char *priorityMap;
	vec3_t vPos;
	vec3_t vForward;
	vec3_t vEnd;
	vec3_t vOrigin;
	trace_t trace;
	gentity_s *lookAtEnt;
	gclient_s *client;
	WeaponDef *weaponDef;

	client = ent->client;
	client->ps.pm_flags &= 0xFFCFFFFF;
	ent->client->pLookatEnt = 0;
	G_GetPlayerViewOrigin(ent, vOrigin);
	G_GetPlayerViewDirection(ent, vForward, 0, 0);

	if ( (client->ps.eFlags & 0x300) != 0 )
		weaponDef = BG_GetWeaponDef(g_entities[client->ps.viewlocked_entNum].s.weapon);
	else
		weaponDef = BG_GetWeaponDef(ent->client->ps.weapon);

	if ( ent->client->ps.weapon && weaponDef->rifleBullet )
		priorityMap = riflePriorityMap;
	else
		priorityMap = bulletPriorityMap;

	VectorMA(vOrigin, 15000.0, vForward, vEnd);
	lookAtEnt = G_FX_VisibilityTrace(&trace, vOrigin, vEnd, ent->s.number, 578824193, priorityMap, vForward);

	if ( lookAtEnt )
	{
		if ( lookAtEnt->classname != scr_const.trigger_lookat
		        || (ent->client->pLookatEnt = lookAtEnt,
		            G_Trigger(lookAtEnt, ent),
		            (lookAtEnt = G_FX_VisibilityTrace(&trace, vOrigin, vEnd, ent->s.number, 41953281, priorityMap, vForward)) != 0) )
		{
			if ( lookAtEnt->s.eType == ET_PLAYER && (trace.surfaceFlags & 0x10) == 0 )
			{
				VectorSubtract(lookAtEnt->r.currentOrigin, vOrigin, vPos);

				if ( lookAtEnt->client->sess.state.team == ent->client->sess.state.team && ent->client->sess.state.team )
				{
					if ( Square(g_friendlyNameDist->current.decimal) > VectorLengthSquared(vPos) && !ent->client->pLookatEnt )
						ent->client->pLookatEnt = lookAtEnt;

					if ( Square(g_friendlyfireDist->current.decimal) > VectorLengthSquared(vPos) )
						client->ps.pm_flags |= 0x100000u;
				}
				else
				{
					if ( Square(weaponDef->enemyCrosshairRange) > VectorLengthSquared(vPos) )
					{
						if ( !ent->client->pLookatEnt )
							ent->client->pLookatEnt = lookAtEnt;

						client->ps.pm_flags |= 0x200000u;
					}
				}
			}
		}
	}
}

void P_DamageFeedback(gentity_s *player)
{
	float viewaxis[3][3];
	float kick;
	vec3_t angles;
	int damage;
	gclient_s *client;

	client = player->client;

	if ( client->ps.pm_type <= PM_INTERMISSION )
	{
		client->ps.damageCount = 0;
		damage = client->damage_blood;

		if ( damage > 0 && client->sess.maxHealth > 0 )
		{
			damage = 100 * damage / client->sess.maxHealth;

			if ( damage > 127 )
				damage = 127;

			client->ps.aimSpreadScale = (long double)damage + client->ps.aimSpreadScale;

			if ( client->ps.aimSpreadScale > 255.0 )
				client->ps.aimSpreadScale = 255.0;

			kick = (long double)damage * 0.2;

			if ( kick >= 5.0 )
			{
				if ( kick > 90.0 )
					kick = 90.0;
			}
			else
			{
				kick = 5.0;
			}

			if ( client->damage_fromWorld )
			{
				client->v_dmg_roll = 0.0;
				client->v_dmg_pitch = -kick;
				client->ps.damagePitch = 255;
				client->ps.damageYaw = 255;
				client->damage_fromWorld = 0;
			}
			else
			{
				vectoangles(client->damage_from, angles);
				AnglesToAxis(client->ps.viewangles, viewaxis);

				client->v_dmg_roll = DotProduct(client->damage_from, viewaxis[1]) * -kick;
				client->v_dmg_pitch = DotProduct(client->damage_from, viewaxis[0]) * kick;

				client->ps.damagePitch = (int)(angles[0] / 360.0 * 256.0);
				client->ps.damageYaw = (int)(angles[1] / 360.0 * 256.0);
			}

			++client->ps.damageEvent;
			client->damageTime = level.time - 20;
			client->ps.damageCount = damage;
			client->damage_blood = 0;
		}
	}
}

void Player_ResetLoopSound(gentity_s *ent)
{
	ent->s.loopSound = 0;
}

void IntermissionClientEndFrame(gentity_s *ent)
{
	gclient_s *client;

	client = ent->client;
	ent->r.svFlags &= ~2u;
	ent->r.svFlags |= 1u;
	ent->takedamage = 0;
	ent->r.contents = 0;
	client->ps.pm_flags &= 0xFC7FFFFF;
	client->ps.pm_type = PM_INTERMISSION;
	client->ps.eFlags &= ~0x200000u;
	client->ps.eFlags &= ~0x40u;
	client->ps.viewmodelIndex = 0;
	ent->s.eType = ET_INVISIBLE;

	SV_SetConfigstring(5u, va("%i", level.teamScores[1]));
	SV_SetConfigstring(6u, va("%i", level.teamScores[2]));
}

void SpectatorClientEndFrame(gentity_s *ent)
{
	int pArchiveTime;
	gclient_s *client;
	clientState_t cstate;
	playerState_t pstate;
	int clientNum;
	int flags;

	client = ent->client;
	ent->r.svFlags &= ~2u;
	ent->r.svFlags |= 1u;
	ent->takedamage = 0;
	ent->r.contents = 0;
	client->ps.pm_flags &= ~0x800000u;
	ent->s.eType = ET_INVISIBLE;
	client->ps.viewmodelIndex = 0;
	client->fGunPitch = 0.0;
	client->fGunYaw = 0.0;

	if ( client->sess.forceSpectatorClient < 0 )
	{
update:
		if ( client->spectatorClient < 0 && !G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
			Cmd_FollowCycle_f(ent, 1);

		clientNum = client->spectatorClient;

		if ( clientNum < 0
		        || (pArchiveTime = client->sess.archiveTime + client->sess.psOffsetTime,
		            !SV_GetArchivedClientInfo(clientNum, &pArchiveTime, &pstate, &cstate))
		        || !G_ClientCanSpectateTeam(client, cstate.team) )
		{
			StopFollowing(ent);
			client->ps.pm_flags &= ~0x2000000u;

			if ( G_ClientCanSpectateTeam(client, TEAM_ALLIES)
			        || G_ClientCanSpectateTeam(client, TEAM_AXIS)
			        || G_ClientCanSpectateTeam(client, TEAM_FREE) )
			{
				client->ps.pm_flags |= 0x1000000u;
			}
			else
			{
				client->ps.pm_flags &= ~0x1000000u;
			}
			return;
		}
	}
	else
	{
		clientNum = client->sess.forceSpectatorClient;
		client->spectatorClient = clientNum;

		while ( 1 )
		{
			if ( client->sess.archiveTime < 0 )
				client->sess.archiveTime = 0;

			pArchiveTime = client->sess.archiveTime - client->sess.psOffsetTime;

			if ( SV_GetArchivedClientInfo(client->sess.forceSpectatorClient, &pArchiveTime, &pstate, &cstate) )
			{
				if ( G_ClientCanSpectateTeam(client, cstate.team) )
					break;
			}

			if ( !client->sess.archiveTime )
			{
				client->sess.forceSpectatorClient = -1;
				client->spectatorClient = -1;
				goto update;
			}

			client->sess.archiveTime -= 50;
		}
	}

	flags = pstate.eFlags & 0xFFEFFFFF | client->ps.eFlags & 0x100000;

	Com_Memcpy(client, &pstate, sizeof(playerState_t));
	HudElem_UpdateClient(client, ent->s.number, HUDELEM_UPDATE_CURRENT);

	client->ps.eFlags = flags;
	client->ps.pm_flags &= ~0x800000u;
	client->ps.pm_flags |= 0x400000u;

	if ( client->sess.forceSpectatorClient < 0 )
	{
		client->ps.pm_flags |= 0x1000000u;

		if ( G_ClientCanSpectateTeam(client, TEAM_NUM_TEAMS) )
			flags = client->ps.pm_flags | 0x2000000;
		else
			flags = client->ps.pm_flags & 0xFDFFFFFF;

		client->ps.pm_flags = flags;
	}
	else
	{
		client->ps.pm_flags &= 0xFCFFFFFF;
	}
}

#ifdef LIBCOD
extern dvar_t *g_playerEject;
#endif

extern dvar_t *g_playerCollisionEjectSpeed;
int StuckInClient(gentity_s *self)
{
	float speed;
	float ejectSpeed;
	float *pVel;
	float *velocity;
	float fDist;
	gentity_s *hit;
	int i;
	float selfSpeed;
	float hitSpeed;
	float vDelta[2];
	int iPushTime;

#ifdef LIBCOD
	if (!g_playerEject->current.boolean)
		return 0;
#endif

	iPushTime = 300;

	if ( (self->client->ps.pm_flags & 0x800000) == 0 )
		return 0;

	if ( self->client->sess.sessionState )
		return 0;

	if ( self->r.contents != 0x2000000 && self->r.contents != 0x4000000 )
		return 0;

	hit = g_entities;

	for ( i = 0; ; ++i )
	{
		if ( i >= level.maxclients )
			return 0;

		if ( hit->r.inuse
		        && (hit->client->ps.pm_flags & 0x800000) != 0
		        && hit->client->sess.sessionState == STATE_PLAYING
		        && hit != self
		        && hit->client
		        && hit->health > 0
		        && (hit->r.contents == 0x2000000 || hit->r.contents == 0x4000000)
		        && hit->r.absmin[0] <= self->r.absmax[0]
		        && self->r.absmin[0] <= hit->r.absmax[0]
		        && hit->r.absmin[1] <= self->r.absmax[1]
		        && self->r.absmin[1] <= hit->r.absmax[1]
		        && hit->r.absmin[2] <= self->r.absmax[2]
		        && self->r.absmin[2] <= hit->r.absmax[2] )
		{
			vDelta[0] = hit->r.currentOrigin[0] - self->r.currentOrigin[0];
			vDelta[1] = hit->r.currentOrigin[1] - self->r.currentOrigin[1];

			fDist = self->r.maxs[0] + hit->r.maxs[0];

			if ( (float)((float)(vDelta[0] * vDelta[0]) + (float)(vDelta[1] * vDelta[1])) <= (float)(fDist * fDist) )
				break;
		}

		++hit;
	}

	vDelta[0] = hit->r.currentOrigin[0] - self->r.currentOrigin[0];
	vDelta[1] = hit->r.currentOrigin[1] - self->r.currentOrigin[1];

	vDelta[0] = G_crandom() + vDelta[0];
	vDelta[1] = G_crandom() + vDelta[1];

	Vec2Normalize(vDelta);

	if ( Vec2Length(hit->client->ps.velocity) <= 0.0 )
		ejectSpeed = 0.0;
	else
		ejectSpeed = (float)g_playerCollisionEjectSpeed->current.integer;

	hitSpeed = ejectSpeed;

	if ( Vec2Length(self->client->ps.velocity) <= 0.0 )
		speed = 0.0;
	else
		speed = (float)g_playerCollisionEjectSpeed->current.integer;

	selfSpeed = speed;

	if ( ejectSpeed < 0.000099999997 && speed < 0.000099999997 )
	{
		hitSpeed = (float)hit->client->ps.speed;
		selfSpeed = (float)self->client->ps.speed;
	}

	velocity = hit->client->ps.velocity;

	velocity[0] = hitSpeed * vDelta[0];
	velocity[1] = hitSpeed * vDelta[1];

	hit->client->ps.pm_time = 300;
	hit->client->ps.pm_flags |= 0x200u;

	pVel = self->client->ps.velocity;

	pVel[0] = -selfSpeed * vDelta[0];
	pVel[1] = -selfSpeed * vDelta[1];

	self->client->ps.pm_time = 300;
	self->client->ps.pm_flags |= 0x200u;

	return 1;
}

unsigned int G_GetNonPVSPlayerInfo(gentity_s *pSelf, float *vPosition, int iLastUpdateEnt)
{
	unsigned int index;
	int iNext;
	int iNum;
	int team;
	gentity_s *pEnt;
	float fScale;
	float fScale_4;
	vec2_t fPos;
	int iPos;
	int iPos_4;
	int num;
	int iCurrentEnt;
	int iEntCount;
	int iBaseEnt;

	team = pSelf->client->sess.state.team;

	if ( team == TEAM_FREE || team == TEAM_SPECTATOR )
		return 0;

	if ( iLastUpdateEnt == 1023 )
		iBaseEnt = 0;
	else
		iBaseEnt = iLastUpdateEnt + 1;

	for ( iEntCount = 0; ; ++iEntCount )
	{
		if ( iEntCount > 63 )
			return 0;

		iNum = iEntCount + iBaseEnt;
		iNext = iEntCount + iBaseEnt + (iEntCount + iBaseEnt < 0 ? 0x3F : 0);
		iCurrentEnt = (iEntCount + iBaseEnt) % 64;
		pEnt = &g_entities[iCurrentEnt];

		if ( pEnt->r.inuse )
		{
			if ( pEnt->client
			        && pEnt->client->sess.sessionState == STATE_PLAYING
			        && pEnt->client->sess.state.team == team
			        && pSelf != pEnt
			        && !SV_inSnapshot(vPosition, pEnt->s.number) )
			{
				break;
			}
		}
	}

	num = pEnt->s.number;
	Vector2Subtract(pEnt->r.currentOrigin, vPosition, fPos);

	iPos = (int)(fPos[0] + 0.5);
	iPos_4 = (int)(fPos[1] + 0.5);

	fScale = 1.0;
	fScale_4 = 1.0;

	if ( iPos <= 1024 )
	{
		if ( iPos < -1022 )
			fScale = -1022.0 / iPos;
	}
	else
	{
		fScale = 1024.0 / iPos;
	}

	if ( iPos_4 <= 1024 )
	{
		if ( iPos_4 < -1022 )
			fScale_4 = -1022.0 / iPos_4;
	}
	else
	{
		fScale_4 = 1024.0 / iPos_4;
	}

	if ( fScale < 1.0 || fScale_4 < 1.0 )
	{
		if ( fScale_4 <= fScale )
		{
			if ( fScale > fScale_4 )
				iPos = (int)(iPos * fScale_4);
		}
		else
		{
			iPos_4 = (int)(iPos_4 * fScale);
		}
	}

	if ( iPos <= 1024 )
	{
		if ( iPos < -1022 )
			iPos = -1022;
	}
	else
	{
		iPos = 1024;
	}

	if ( iPos_4 <= 1024 )
	{
		if ( iPos_4 < -1022 )
			iPos_4 = -1022;
	}
	else
	{
		iPos_4 = 1024;
	}

	index = num & 0xFFFF803F;
	num = num & 0xFF00003F | ((((unsigned short)((iPos + 2) / 4) + 255) & 0x1FF) << 6) & 0x7FFF | ((((unsigned short)((iPos_4 + 2) / 4) + 255) & 0x1FF) << 15);

	return (index | ((((unsigned short)((iPos + 2) / 4) + 255) & 0x1FF) << 6)) & 0x7FFF | ((((unsigned short)((iPos_4 + 2) / 4) + 255) & 0x1FF) << 15) & 0xFFFFFF | ((unsigned char)(int)(g_entities[iNum - (iNext >> 6 << 6)].r.currentAngles[1] * 0.71111113) << 24);
}

#if COMPILE_PLAYER == 1
int player_g_speed[MAX_CLIENTS] = {0};
int player_g_gravity[MAX_CLIENTS] = {0};
#endif

extern dvar_t *g_gravity;
extern dvar_t *g_speed;
extern dvar_t *g_debugLocDamage;
void ClientEndFrame(gentity_s *entity)
{
	int flags;
	byte handler;
	int pm_type;
	DObj_s *pDObj;
	vec3_t angles;
	vec3_t origin;
	gclient_s *client;
	qboolean modelsUpdated;
	int clientNum;
	clientInfo_t *ci;
	vec3_t viewOrigin;

	client = entity->client;
	entity->handler = 10;
	client->ps.deltaTime = 0;
	modelsUpdated = G_UpdateClientInfo(entity);

	if ( client->sess.connected == CON_CONNECTED )
	{
		if ( client->sess.sessionState == STATE_INTERMISSION )
		{
			IntermissionClientEndFrame(entity);
			entity->client->buttonsSinceLastFrame = 0;
		}
		else if ( client->sess.sessionState == STATE_SPECTATOR )
		{
			SpectatorClientEndFrame(entity);
			entity->client->buttonsSinceLastFrame = 0;
		}
		else if ( client->ps.clientNum == entity->s.number )
		{
			entity->r.svFlags |= 2u;
			entity->r.svFlags &= ~1u;
			entity->takedamage = 1;
			client->ps.pm_flags |= 0x800000u;
			client->ps.pm_flags &= 0xFCFFFFFF;
			client->ps.viewmodelIndex = client->sess.viewmodelIndex;
			G_UpdatePlayerContents(entity);
			client->dropWeaponTime = 0;

			if ( client->compassPingTime <= level.time )
				client->ps.eFlags &= ~0x400000u;

			if ( client->noclip )
			{
				client->ps.pm_type = PM_NOCLIP;
			}
			else if ( client->ufo )
			{
				client->ps.pm_type = PM_UFO;
			}
			else if ( client->sess.sessionState == STATE_DEAD )
			{
				if ( entity->tagInfo )
					pm_type = PM_DEAD_LINKED;
				else
					pm_type = PM_DEAD;

				client->ps.pm_type = pm_type;
				entity->r.svFlags |= 1u;
				entity->r.svFlags &= ~2u;
				entity->takedamage = 0;
			}
			else
			{
				client->ps.pm_type = entity->tagInfo != 0;
			}

			client->ps.gravity = (int)g_gravity->current.decimal;
			client->ps.speed = g_speed->current.integer;
#if COMPILE_PLAYER == 1
			int num = client->ps.clientNum;
			if (player_g_gravity[num] > 0)
				client->ps.gravity = player_g_gravity[num];

			if (player_g_speed[num] > 0)
				client->ps.speed = player_g_speed[num];
#endif
			client->currentAimSpreadScale = client->ps.aimSpreadScale / 255.0;

			Player_UpdateLookAtEntity(entity);
			Player_UpdateCursorHints(entity);
			P_DamageFeedback(entity);

			if ( level.time - client->lastCmdTime <= 1000 )
				flags = entity->s.eFlags & 0xFFFFFF7F;
			else
				flags = entity->s.eFlags | 0x80;

			entity->s.eFlags = flags;
			client->ps.stats[0] = entity->health;

			Player_ResetLoopSound(entity);

			if ( g_smoothClients->current.boolean )
				G_PlayerStateToEntityStateExtrapolate(&client->ps, &entity->s, client->ps.commandTime, 1);
			else
				BG_PlayerStateToEntityState(&client->ps, &entity->s, 1, 1u);

			if ( entity->health > 0 && StuckInClient(entity) )
				entity->r.contents = 0x4000000;

			G_GetPlayerViewOrigin(entity, viewOrigin);
			client->ps.iCompassFriendInfo = G_GetNonPVSPlayerInfo(entity, viewOrigin, client->iLastCompassFriendlyInfoEnt);

			if ( client->ps.iCompassFriendInfo )
			{
				client->iLastCompassFriendlyInfoEnt = client->ps.iCompassFriendInfo & 0x3F;

				if ( (g_entities[client->iLastCompassFriendlyInfoEnt].s.eFlags & 0x400000) != 0 )
					flags = client->ps.eFlags | 0x800000;
				else
					flags = client->ps.eFlags & 0xFF7FFFFF;

				client->ps.eFlags = flags;
			}
			else
			{
				client->iLastCompassFriendlyInfoEnt = 1023;
			}

			if ( entity->s.eType == ET_PLAYER )
			{
				if ( entity->health <= 0 )
					handler = 11;
				else
					handler = 9;

				entity->handler = handler;
				clientNum = entity->s.clientNum;
				ci = &level_bgs.clientinfo[clientNum];
				ci->lerpMoveDir = entity->s.angles2[1];
				ci->lerpLean = entity->s.leanf;
				VectorCopy(client->ps.viewangles, ci->playerAngles);

				if ( modelsUpdated )
					G_SafeDObjFree(entity);

				pDObj = Com_GetServerDObj(entity->s.number);
				BG_UpdatePlayerDObj(pDObj, &entity->s, ci, entity->attachIgnoreCollision);
				BG_PlayerAnimation(pDObj, &entity->s, ci);

				if ( (client->ps.pm_flags & 0x800000) != 0 && (client->ps.eFlags & 0x300) != 0 )
					turret_think_client(&level.gentities[client->ps.viewlocked_entNum]);

				if ( g_debugLocDamage->current.boolean )
				{
					if ( SV_DObjExists(entity) )
					{
						G_DObjCalcPose(entity);
						SV_XModelDebugBoxes();
					}
				}

				entity->client->buttonsSinceLastFrame = 0;
			}
			else
			{
				entity->client->buttonsSinceLastFrame = 0;
			}
		}
		else
		{
			VectorCopy(client->ps.origin, origin);
			VectorSet(angles, 0.0, client->ps.viewangles[1], 0.0);
			ClientSpawn(entity, origin, angles);
			entity->client->buttonsSinceLastFrame = 0;
		}
	}
	else
	{
		entity->client->buttonsSinceLastFrame = 0;
	}
}

void G_RunClient(gentity_s *ent)
{
	int pm_type;

	if ( g_synchronousClients->current.boolean )
	{
		ent->client->sess.cmd.serverTime = level.time;
		ClientThink_real(ent, &ent->client->sess.cmd);
	}

	if ( !ent->client->noclip )
	{
		if ( ent->tagInfo )
		{
			if ( ent->client->sess.sessionState == STATE_DEAD )
				pm_type = PM_DEAD_LINKED;
			else
				pm_type = PM_NORMAL_LINKED;

			ent->client->ps.pm_type = pm_type;
			G_SetFixedLink(ent, 2);
			G_SetOrigin(ent, ent->r.currentOrigin);
			G_SetAngle(ent, ent->r.currentAngles);
			ent->s.pos.trType = TR_INTERPOLATE;
			ent->s.apos.trType = TR_INTERPOLATE;
			SV_LinkEntity(ent);
			VectorCopy(ent->r.currentOrigin, ent->client->ps.origin);
		}
		else if ( ent->client->ps.pm_type == PM_NORMAL_LINKED || ent->client->ps.pm_type == PM_DEAD_LINKED )
		{
			--ent->client->ps.pm_type;
		}
	}
}