#include "../qcommon/qcommon.h"
#include "g_shared.h"

turretInfo_s turretInfoStore[MAX_TURRETS];

/*
==============
G_InitTurrets
==============
*/
void G_InitTurrets()
{
	for ( int i = 0; i < MAX_TURRETS; i++ )
	{
		turretInfoStore[i].inuse = qfalse;
	}
}

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
target_position does the same thing
*/
void SP_info_notnull( gentity_t *self )
{
	G_SetOrigin( self, self->r.currentOrigin );
}

/*
==============
turret_use
==============
*/
void turret_use( gentity_t *self, gentity_t *owner, gentity_t *activator ) // no activator for cod2
{
	playerState_t *ps;
	turretInfo_s *pTurretInfo;

	assert(self);

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	assert(owner);
	assert(owner->client);
	assert(owner->s.number < level.maxclients);

	ps = &owner->client->ps;
	assert(ps);

	owner->active = qtrue;
	self->active = qtrue;

	self->r.ownerNum = owner->s.number;

	ps->viewlocked = qtrue;
	ps->viewlocked_entNum = self->s.number;

	pTurretInfo->flags |= 0x800u;

	VectorCopy(owner->r.currentOrigin, pTurretInfo->userOrigin);

	owner->s.otherEntityNum = self->s.number;
	self->s.otherEntityNum = owner->s.number;

	if ( ps->pm_flags & PMF_PRONE )
		pTurretInfo->prevStance = TURRET_STANCE_PRONE;
	else
		pTurretInfo->prevStance = ps->pm_flags & PMF_DUCKED;

	if ( pTurretInfo->stance == TURRET_STANCE_PRONE )
	{
		ps->eFlags |= EF_TURRET_PRONE;
		ps->eFlags &= ~EF_TURRET_DUCK;
	}
	else if ( pTurretInfo->stance == TURRET_STANCE_CROUCH )
	{
		ps->eFlags |= EF_TURRET_DUCK;
		ps->eFlags &= ~EF_TURRET_PRONE;
	}
	else
	{
		ps->eFlags |= EF_TURRET_ACTIVE;
	}

	// Compensating for the difference between the players view at the time of use and the start angles that the gun object has
	self->s.angles2[PITCH] = AngleSubtract(ps->viewangles[PITCH], self->r.currentAngles[PITCH]);
	self->s.angles2[PITCH] = I_fclamp(self->s.angles2[PITCH], pTurretInfo->arcmin[PITCH], pTurretInfo->arcmax[PITCH]);

	self->s.angles2[YAW] = AngleSubtract(ps->viewangles[YAW], self->r.currentAngles[YAW]);
	self->s.angles2[YAW] = I_fclamp(self->s.angles2[YAW], pTurretInfo->arcmin[YAW], pTurretInfo->arcmax[YAW]);

	self->s.angles2[ROLL] = 0;

	ps->viewAngleClampRange[PITCH] = AngleSubtract(pTurretInfo->arcmax[PITCH], pTurretInfo->arcmin[PITCH]) * 0.5;
	ps->viewAngleClampBase[PITCH] = self->r.currentAngles[PITCH] + pTurretInfo->arcmax[PITCH];
	ps->viewAngleClampBase[PITCH] = AngleNormalize360Accurate(ps->viewAngleClampBase[PITCH] - ps->viewAngleClampRange[PITCH]);

	ps->viewAngleClampRange[YAW] = AngleSubtract(pTurretInfo->arcmax[YAW], pTurretInfo->arcmin[YAW]) * 0.5;
	ps->viewAngleClampBase[YAW] = self->r.currentAngles[YAW] + pTurretInfo->arcmax[YAW];
	ps->viewAngleClampBase[YAW] = AngleNormalize360Accurate(ps->viewAngleClampBase[YAW] - ps->viewAngleClampRange[YAW]);
}

/*
==============
G_IsTurretUsable
==============
*/
qboolean G_IsTurretUsable( gentity_t *self, gentity_t *owner )
{
	if ( self->active || !self->pTurretInfo )
	{
		return qfalse;
	}

	if ( !turret_behind(self, owner) )
	{
		return qfalse;
	}

	if ( owner->client->ps.grenadeTimeLeft )
	{
		return qfalse;
	}

	if ( owner->client->ps.groundEntityNum == ENTITYNUM_NONE )
	{
		return qfalse;
	}

	return qtrue;
}

/*
==============
turret_controller
==============
*/
void turret_controller( gentity_t *self, int *partBits )
{
	DObj *obj;
	vec3_t angles;

	angles[1] = self->s.angles2[1];
	angles[0] = self->s.angles2[0];
	angles[2] = 0;

	obj = Com_GetServerDObj(self->s.number);

	DObjSetControlTagAngles(obj, partBits, scr_const.tag_aim, angles);
	DObjSetControlTagAngles(obj, partBits, scr_const.tag_aim_animated, angles);

	angles[0] = self->s.angles2[2];
	angles[1] = 0;

	DObjSetControlTagAngles(obj, partBits, scr_const.tag_flash, angles);
}

/*
 * @brief So level designers can toggle them on/off
 * @param[in,out] ent
 * @param other - unused
 * @param activator - unused
 */
void use_corona( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
	if (ent->r.linked)
	{
		SV_UnlinkEntity(ent);
	}
	else
	{
		ent->active = qfalse;
		SV_LinkEntity(ent);
	}
}

/*
=================================================================================

TELEPORTERS

=================================================================================
*/
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles )
{
	assert(player->client);
	assert(player->client->sess.connected != CON_DISCONNECTED);

	// unlink to make sure it can't possibly interfere with G_KillBox
	SV_UnlinkEntity( player );

	VectorCopy(origin, player->client->ps.origin);
	player->client->ps.origin[2] += 1;

	// toggle the teleport bit so the client knows to not lerp
	player->client->ps.eFlags ^= EF_TELEPORT_BIT;

	// set angles
	SetClientViewAngle( player, angles );

	// save results of pmove
	BG_PlayerStateToEntityState(&player->client->ps, &player->s, qtrue, PMOVE_HANDLER_SERVER);

	// use the precise origin for linking
	VectorCopy(player->client->ps.origin, player->r.currentOrigin);

	if ( player->r.linked )
	{
		SV_LinkEntity( player );
	}
}

/*
==============
G_ClientStopUsingTurret
==============
*/
void G_ClientStopUsingTurret( gentity_t *self )
{
	turretInfo_s *pTurretInfo;
	gentity_t *owner;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	owner = &g_entities[self->r.ownerNum];
	assert(owner->client);

	pTurretInfo->fireSndDelay = 0;
	self->s.loopSound = 0;

	if ( pTurretInfo->prevStance != TURRET_STANCE_INVALID )
	{
		if ( pTurretInfo->prevStance == TURRET_STANCE_PRONE )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_PRONE, 0);
		}
		else if ( pTurretInfo->prevStance == TURRET_STANCE_CROUCH )
		{
			G_AddEvent(owner, EV_STANCE_FORCE_CROUCH, 0);
		}
		else
		{
			G_AddEvent(owner, EV_STANCE_FORCE_STAND, 0);
		}

		pTurretInfo->prevStance = TURRET_STANCE_INVALID;
	}

	TeleportPlayer(owner, pTurretInfo->userOrigin, owner->r.currentAngles);

	owner->client->ps.eFlags &= ~EF_TURRET_ACTIVE;

	owner->client->ps.viewlocked = 0;
	owner->client->ps.viewlocked_entNum = ENTITYNUM_NONE;

	owner->active = qfalse;
	owner->s.otherEntityNum = 0;

	self->active = qfalse;
	self->r.ownerNum = ENTITYNUM_NONE;

	pTurretInfo->flags &= ~0x800u;
}

/*
==============
G_FreeTurret
==============
*/
void G_FreeTurret( gentity_t *self )
{
	if ( g_entities[self->r.ownerNum].client )
	{
		G_ClientStopUsingTurret(self);
	}

	self->active = qfalse;

	assert(self->pTurretInfo);
	self->pTurretInfo->inuse = qfalse;
	self->pTurretInfo = NULL;
}

/*QUAKED corona (0 1 0) (-4 -4 -4) (4 4 4) START_OFF
Use color picker to set color or key "color".  values are 0.0-1.0 for each color (rgb).
"scale" will designate a multiplier to the default size.  (so 2.0 is 2xdefault size, 0.5 is half)
*/
/*
==============
SP_corona
==============
*/
void SP_corona( gentity_t *self )
{
	G_FreeEntity( self );
}

//===========================================================

/*QUAKED misc_model (1 0 0) (-16 -16 -16) (16 16 16)
"model"		arbitrary .md3 file to display
"modelscale"	scale multiplier (defaults to 1x)
"modelscale_vec"	scale multiplier (defaults to 1 1 1, scales each axis as requested)

"modelscale_vec" - Set scale per-axis.  Overrides "modelscale", so if you have both, the "modelscale" is ignored
*/
void SP_misc_model( gentity_t *self )
{
	G_FreeEntity( self );
}

/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) nonlinear angle negative_spot negative_point q3map_non-dynamic
Non-displayed light.
"light" overrides the default 300 intensity.
Nonlinear checkbox gives inverse square falloff instead of linear
Angle adds light:surface angle calculations (only valid for "Linear" lights) (wolf)
Lights pointed at a target will be spotlights.
"radius" overrides the default 64 unit radius of a spotlight at the target point.
"fade" falloff/radius adjustment value. multiply the run of the slope by "fade" (1.0f default) (only valid for "Linear" lights) (wolf)
"q3map_non-dynamic" specifies that this light should not contribute to the world's 'light grid' and therefore will not light dynamic models in the game.(wolf)
*/
void SP_light( gentity_t *self )
{
	G_FreeEntity( self );
}

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
target_position does the same thing
*/
void SP_info_null( gentity_t *self )
{
	G_FreeEntity( self );
}

/*
==============
G_SpawnTurret
==============
*/
void G_SpawnTurret( gentity_t *self, const char *weaponinfoname )
{
	int i;
	turretInfo_s *turretInfo = NULL;
	WeaponDef *weapDef;

	for ( i = 0; i < MAX_TURRETS; i++ )
	{
		turretInfo = &turretInfoStore[i];

		if ( !turretInfo->inuse )
		{
			break;
		}
	}

	if ( i == MAX_TURRETS )
	{
		Com_Error(ERR_DROP, "G_SpawnTurret: max number of turrets (%d) exceeded", MAX_TURRETS);
	}

	memset(turretInfo, 0, sizeof(*turretInfo));
	self->pTurretInfo = turretInfo;
	turretInfo->inuse = qtrue;

	self->s.weapon = G_GetWeaponIndexForName(weaponinfoname);

	if ( !self->s.weapon )
	{
		Com_Error(ERR_DROP, "bad weaponinfo '%s' specified for turret", weaponinfoname);
	}

	weapDef = BG_GetWeaponDef(self->s.weapon);

	if ( weapDef->weaponClass != WEAPCLASS_TURRET )
	{
		Scr_Error(va("G_SpawnTurret: weapon '%s' isn't a turret. This usually indicates that the weapon failed to load.", weaponinfoname));
	}

	if ( !level.initializing && !IsItemRegistered(self->s.weapon) )
	{
		Scr_Error(va("turret '%s' not precached", weaponinfoname));
	}

	turretInfo->fireTime = 0;
	turretInfo->stance = weapDef->stance;
	turretInfo->prevStance = TURRET_STANCE_INVALID;
	turretInfo->fireSndDelay = 0;

	if ( weapDef->loopFireSound && *weapDef->loopFireSound )
		turretInfo->fireSnd = G_SoundAliasIndex(weapDef->loopFireSound);
	else
		turretInfo->fireSnd = 0;

	if ( weapDef->loopFireSoundPlayer && *weapDef->loopFireSoundPlayer )
		turretInfo->fireSndPlayer = G_SoundAliasIndex(weapDef->loopFireSoundPlayer);
	else
		turretInfo->fireSndPlayer = 0;

	if ( weapDef->stopFireSound && *weapDef->stopFireSound )
		turretInfo->stopSnd = G_SoundAliasIndex(weapDef->stopFireSound);
	else
		turretInfo->stopSnd = 0;

	if ( weapDef->stopFireSoundPlayer && *weapDef->stopFireSoundPlayer )
		turretInfo->stopSndPlayer = G_SoundAliasIndex(weapDef->stopFireSoundPlayer);
	else
		turretInfo->stopSndPlayer = 0;

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("rightarc", "", &turretInfo->arcmin[1]) )
	{
		turretInfo->arcmin[1] = weapDef->rightArc;
	}

	turretInfo->arcmin[1] = -turretInfo->arcmin[1];

	if ( turretInfo->arcmin[1] > 0 )
	{
		turretInfo->arcmin[1] = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("leftarc", "", &turretInfo->arcmax[1]) )
	{
		turretInfo->arcmax[1] = weapDef->leftArc;
	}

	if ( turretInfo->arcmax[1] < 0 )
	{
		turretInfo->arcmax[1] = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("toparc", "", turretInfo->arcmin) )
	{
		turretInfo->arcmin[0] = weapDef->topArc;
	}

	turretInfo->arcmin[0] = -turretInfo->arcmin[0];

	if ( turretInfo->arcmin[0] > 0 )
	{
		turretInfo->arcmin[0] = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("bottomarc", "", turretInfo->arcmax) )
	{
		turretInfo->arcmax[0] = weapDef->bottomArc;
	}

	if ( turretInfo->arcmax[0] < 0 )
	{
		turretInfo->arcmax[0] = 0;
	}

	turretInfo->dropPitch = -90;

	if ( !self->health )
	{
		self->health = 100;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnInt("damage", "0", &self->dmg) )
	{
		self->dmg = weapDef->damage;
	}

	if ( self->dmg < 0 )
	{
		self->dmg = 0;
	}

	if ( !level.spawnVar.spawnVarsValid || !G_SpawnFloat("playerSpread", "1", &turretInfo->playerSpread) )
	{
		turretInfo->playerSpread = weapDef->playerSpread;
	}

	if ( turretInfo->playerSpread < 0 )
	{
		turretInfo->playerSpread = 0;
	}

	turretInfo->flags = 3;

	self->clipmask = CONTENTS_SOLID;
	self->r.contents = CONTENTS_NONCOLLIDING | CONTENTS_DONOTENTER;
	self->r.svFlags = 0;
	self->s.eType = ET_TURRET;
	self->flags |= 0x1000u;

	G_DObjUpdate(self);

	VectorSet(self->r.mins, -32, -32, 0);
	VectorSet(self->r.maxs, 32, 32, 56);

	G_SetOrigin(self, self->r.currentOrigin);
	G_SetAngle(self, self->r.currentAngles);

	VectorClear(self->s.angles2);

	self->handler = ENT_HANDLER_TURRET_INIT;
	self->nextthink = level.time + 50;

	self->s.apos.trType = TR_LINEAR_STOP;
	self->takedamage = qfalse;

	SV_LinkEntity(self);
}

/*QUAKED misc_mg42 (1 0 0) (-16 -16 -24) (16 16 24) HIGH NOTRIPOD
harc = horizonal fire arc Default is 57.5 (left and right)
varc = vertical fire arc Default is 45 (upwards and 22.5 down)
health = how much damage can it take default is 50
damage = determines how much the weapon will inflict if a non player uses it
"accuracy" all guns are 100% accurate an entry of 0.5 would make it 50%
*/
void SP_turret( gentity_t *self )
{
	const char *weaponinfoname;

	if ( !G_SpawnString("weaponinfo", "", &weaponinfoname) )
	{
		Com_Error( ERR_DROP, "no weaponinfo specified for turret" );
	}

	G_SpawnTurret( self, weaponinfoname );
}

/*
==============
turret_think
==============
*/
void turret_think( gentity_t *self )
{
	self->nextthink = level.time + 50;

	if ( self->tagInfo )
	{
		G_GeneralLink(self);
	}

	if ( !g_entities[self->r.ownerNum].client )
	{
		turret_UpdateSound(self);
		self->s.eFlags &= ~EF_FIRING;
		turret_ReturnToDefaultPos(self, qfalse);
	}
}

/*
==============
turret_think_init
==============
*/
void turret_think_init( gentity_t *self )
{
	int i;
	trace_t trace;
	vec3_t end;
	vec3_t start;
	vec3_t transDir;
	vec3_t dir;
	vec3_t angles;
	float mtx[3][3];
	float baseMtx[4][3];
	DObjAnimMat *weaponMtx;
	DObjAnimMat *aimMtx;
	turretInfo_s *pTurretInfo;
	int numSteps = 30;

	assert(self);
	assert(self->handler == ENT_HANDLER_TURRET_INIT);

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	self->handler = ENT_HANDLER_TURRET;
	self->nextthink = level.time + 50;

	aimMtx = G_DObjGetLocalTagMatrix(self, scr_const.tag_aim);

	if ( !aimMtx )
	{
		return;
	}

	weaponMtx = G_DObjGetLocalTagMatrix(self, scr_const.tag_butt);

	if ( !weaponMtx )
	{
		return;
	}

	AnglesToAxis(self->r.currentAngles, baseMtx);
	VectorCopy(self->r.currentOrigin, baseMtx[3]);
	VectorSubtract(weaponMtx->trans, aimMtx->trans, dir);
	MatrixTransformVector43(aimMtx->trans, baseMtx, start);

	for ( i = 0; i <= numSteps; i++ )
	{
		angles[0] = (float)(-90.0 / 30.0) * (float)i;
		angles[1] = 0;
		angles[2] = 0;

		AnglesToAxis(angles, mtx);
		MatrixTransformVector(dir, mtx, transDir);
		VectorAdd(aimMtx->trans, transDir, transDir);
		MatrixTransformVector43(transDir, baseMtx, end);

		G_LocationalTrace(&trace, start, end, self->s.number, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY, bulletPriorityMap);

		if ( trace.fraction < 1.0 )
		{
			pTurretInfo->dropPitch = angles[0];
			return;
		}
	}
}

/*
==============
turret_think_client
==============
*/
void turret_think_client( gentity_t *self )
{
	gentity_t *owner;

	owner = &g_entities[self->r.ownerNum];
	assert(owner->client);

	if ( owner->active != qtrue || owner->client->sess.sessionState != SESS_STATE_PLAYING )
	{
		G_ClientStopUsingTurret(self);
		return;
	}

	turret_track(self, owner);
	turret_UpdateSound(self);
}

/*
==============
turret_UpdateTargetAngles
==============
*/
int turret_UpdateTargetAngles( gentity_t *self, const vec3_t desiredAngles, qboolean bManned )
{
	vec2_t downAngles;
	qboolean bComplete;
	turretInfo_s *pTurretInfo;
	float fDelta;
	vec2_t fSpeed;
	int i;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	bComplete = qtrue;

	downAngles[1] = self->s.angles2[0];
	self->s.angles2[0] = downAngles[1] + self->s.angles2[2];

	if ( bManned )
	{
		fSpeed[0] = BG_GetWeaponDef(self->s.weapon)->maxVertTurnSpeed;
		fSpeed[1] = BG_GetWeaponDef(self->s.weapon)->maxHorTurnSpeed;
	}
	else
	{
		fSpeed[0] = 200;
		fSpeed[1] = 200;
	}

	if ( pTurretInfo->flags & 0x200 && pTurretInfo->flags & 0x100 && fSpeed[0] < 360 )
	{
		fSpeed[0] = 360;
	}

	for ( i = 0; i < 2; i++ )
	{
		fSpeed[i] = fSpeed[i] * 0.05f;
		assert(fSpeed[i] >= 0);

		fDelta = AngleSubtract(desiredAngles[i], self->s.angles2[i]);

		if ( fDelta > fSpeed[i] )
		{
			bComplete = qfalse;
			fDelta = fSpeed[i];
		}
		else if ( -fSpeed[i] > fDelta )
		{
			bComplete = qfalse;
			fDelta = -fSpeed[i];
		}

		self->s.angles2[i] = self->s.angles2[i] + fDelta;
	}

	downAngles[0] = self->s.angles2[0];
	self->s.angles2[2] = downAngles[0];

	if ( pTurretInfo->flags & 0x200 )
	{
		if ( pTurretInfo->flags & 0x400 )
		{
			if ( pTurretInfo->triggerDown > self->s.angles2[0] )
				downAngles[0] = pTurretInfo->triggerDown;
			else
				pTurretInfo->flags &= ~0x100u;
		}
		else if ( self->s.angles2[0] > pTurretInfo->triggerDown )
		{
			downAngles[0] = pTurretInfo->triggerDown;
		}
		else
		{
			pTurretInfo->flags &= ~0x100u;
		}
	}

	fDelta = AngleSubtract(downAngles[0], downAngles[1]);

	if ( fDelta > fSpeed[0] )
	{
		bComplete = qfalse;
		fDelta = fSpeed[0];
	}
	else if ( -fSpeed[0] > fDelta )
	{
		bComplete = qfalse;
		fDelta = -fSpeed[0];
	}

	self->s.angles2[0] = downAngles[1] + fDelta;
	self->s.angles2[2] = self->s.angles2[2] - self->s.angles2[0];

	return bComplete;
}

/*
==============
turret_UpdateSound
==============
*/
void turret_UpdateSound( gentity_t *self )
{
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	self->s.loopSound = 0;

	if ( pTurretInfo->fireSndDelay <= 0 )
	{
		return;
	}

	self->s.loopSound = (unsigned char)pTurretInfo->fireSnd;
	pTurretInfo->fireSndDelay -= 50;

	if ( pTurretInfo->fireSndDelay > 0 )
	{
		return;
	}

	if ( !pTurretInfo->stopSnd )
	{
		return;
	}

	self->s.loopSound = 0;
	G_PlaySoundAlias(self, pTurretInfo->stopSnd);
}

/*
==============
turret_ReturnToDefaultPos
==============
*/
int turret_ReturnToDefaultPos( gentity_t *self, qboolean bManned )
{
	float dropPitch;
	vec2_t desiredAngles;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;

	if ( bManned )
		dropPitch = 0;
	else
		dropPitch = pTurretInfo->dropPitch;

	desiredAngles[0] = dropPitch;
	desiredAngles[1] = 0;

	return turret_UpdateTargetAngles( self, desiredAngles, bManned );
}

/*
==============
turret_clientaim
==============
*/
void turret_clientaim( gentity_t *self, gentity_t *other )
{
	playerState_t *ps;
	turretInfo_s *pTurretInfo;

	assert(self);

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);

	assert(other);
	assert(other->client);

	ps = &other->client->ps;
	assert(ps);
	assert(self->active);

	ps->viewlocked = qtrue;
	ps->viewlocked_entNum = self->s.number;

	self->s.angles2[0] = AngleSubtract(ps->viewangles[0], self->r.currentAngles[0]);
	self->s.angles2[0] = I_fclamp(self->s.angles2[0], pTurretInfo->arcmin[0], pTurretInfo->arcmax[0]);

	self->s.angles2[1] = AngleSubtract(ps->viewangles[1], self->r.currentAngles[1]);
	self->s.angles2[1] = I_fclamp(self->s.angles2[1], pTurretInfo->arcmin[1], pTurretInfo->arcmax[1]);

	self->s.angles2[2] = 0;

	if ( pTurretInfo->flags & 0x800 )
	{
		pTurretInfo->flags &= ~0x800u;
		self->s.eFlags ^= EF_TELEPORT_BIT;
	}
}

/*
==============
turret_behind
==============
*/
qboolean turret_behind( gentity_t *self, gentity_t *other )
{
	float angle;
	float dot;
	float centerYaw;
	float yawSpan;
	float minYaw;
	vec3_t forward;
	vec3_t dir;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	assert(pTurretInfo);
	assert(other->client);

	minYaw = self->r.currentAngles[1] + pTurretInfo->arcmin[1];
	yawSpan = (I_fabs(pTurretInfo->arcmax[1]) + I_fabs(pTurretInfo->arcmin[1])) * 0.5;
	centerYaw = AngleNormalize180(minYaw + yawSpan);

	YawVectors(centerYaw, forward, 0);
	Vec3Normalize(forward);

	VectorSubtract(self->r.currentOrigin, other->r.currentOrigin, dir);
	dir[2] = 0;

	Vec3Normalize(dir);

	dot = DotProduct(forward, dir);
	angle = Q_acos(I_fclamp(dot, -1.0, 1.0)) * RAD_TO_DEG;

	return yawSpan >= angle;
}

/*
==============
Turret_FillWeaponParms
==============
*/
void Turret_FillWeaponParms( gentity_t *ent, gentity_t *activator, weaponParms *wp )
{
	vec3_t diff;
	vec3_t playerPos;
	float flashTag[4][3];

	if ( !G_DObjGetWorldTagMatrix(ent, scr_const.tag_flash, flashTag) )
	{
		Com_Error(ERR_DROP, "Couldn't find %s on turret (entity %d, classname '%s').\n", "tag_flash", ent->s.number, SL_ConvertToString(ent->classname));
	}

	G_GetPlayerViewOrigin(activator, playerPos);
	G_GetPlayerViewDirection(activator, wp->forward, wp->right, wp->up);

	VectorCopy(wp->forward, wp->gunForward);
	VectorSubtract(flashTag[3], playerPos, diff);

	VectorMA(playerPos, Vec3Normalize(diff), wp->forward, wp->muzzleTrace);
}

/*
==============
G_PlayerTurretPositionAndBlend
==============
*/
void G_PlayerTurretPositionAndBlend( gentity_t *ent, gentity_t *pTurretEnt )
{
	vec3_t tagAxis[3];
	float turretAxis[4][3];
	vec3_t vDelta;

	int clientNum = ent->s.clientNum;
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);

	clientInfo_t *ci = &level_bgs.clientinfo[clientNum];
	assert(ci->infoValid);

	lerpFrame_t *pLerpAnim = &ci->legs;

	if ( !ci->legs.animationNumber || !pLerpAnim->animation || !( pLerpAnim->animation->flags & 4 ) )
	{
		return;
	}

	DObjAnimMat *tagMat = G_DObjGetLocalTagMatrix(pTurretEnt, scr_const.tag_weapon);

	if ( !tagMat )
	{
		Com_Printf("WARNING: aborting player positioning on turret since 'tag_weapon' does not exist\n");
		return;
	}

	assert(pTurretEnt->s.weapon);

	WeaponDef *weapDef = BG_GetWeaponDef(pTurretEnt->s.weapon);
	assert(weapDef->weaponClass == WEAPCLASS_TURRET);

	XAnimTree *pAnimTree = ci->pXAnimTree;
	XAnim *pXAnims = level_bgs.animData.animScriptData.animTree.anims;

	unsigned int baseAnim = pLerpAnim->animationNumber & ~ANIM_TOGGLEBIT;

	ConvertQuatToMat(tagMat, tagAxis);
	float localYaw = vectosignedyaw(tagAxis[0]);

	AnglesToAxis(pTurretEnt->r.currentAngles, turretAxis);

	VectorCopy(pTurretEnt->r.currentOrigin, turretAxis[3]);
	VectorSubtract(ent->r.currentOrigin, turretAxis[3], vDelta);

	float tagHeight = DotProduct(vDelta, turretAxis[2]);
	float fDelta = tagHeight - tagMat->trans[2];

	XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0);

	int numVertChildren = XAnimGetNumChildren(pXAnims, baseAnim);

	float fPrevTransZ = 0;
	float fPrevBlend = 0;
	int iPrevBlend = 0;

	unsigned int heightAnim;
	unsigned int leafAnim1;
	unsigned int leafAnim2 = 0;

	if ( !numVertChildren )
	{
		Com_Error(ERR_DROP, "Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, baseAnim));
	}

	int numHorChildren;
	int iBlend;
	float fBlend;

	vec2_t rot;
	vec3_t trans;

	float w;

	int i = 0;

	do
	{
		heightAnim = XAnimGetChildAt(pXAnims, baseAnim, i);
		XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0, 1.0, 1.0, 0, 0, 0);
		numHorChildren = XAnimGetNumChildren(pXAnims, heightAnim);

		if ( !numHorChildren )
		{
			Com_Error(ERR_DROP, "Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, heightAnim));
		}

		fBlend = numHorChildren * 0.5 - localYaw / weapDef->animHorRotateInc;

		if ( fBlend < 0 )
		{
			fBlend = 0;
		}
		else if ( fBlend >= numHorChildren - 1 )
		{
			fBlend = numHorChildren - 1;
		}

		iBlend = (int)fBlend;
		fBlend -= (float)(int)iBlend;

		leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iBlend);
		XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fBlend, 1.0, 1.0, 0, 0, 0);

		if ( fBlend != 0 )
		{
			leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iBlend + 1);
			XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, 1.0, 1.0, 0, 0, 0);
		}

		XAnimCalcAbsDelta(pAnimTree, heightAnim, rot, trans);

		if ( trans[2] >= fDelta )
		{
			break;
		}

		fPrevTransZ = trans[2];
		iPrevBlend = iBlend;
		fPrevBlend = fBlend;

		i++;
	}
	while ( i < numVertChildren );

	XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0);

	w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim1) - (1.0 - fBlend)) * (1000.0 / level.frameTime);

	if ( w > 0 )
		XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fBlend, 1.0 / w, 1.0, 0, 0, 0);
	else
		XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fBlend, 0, 1.0, 0, 0, 0);

	if ( fBlend != 0 )
	{
		w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim2) - fBlend) * (1000.0 / level.frameTime);

		if ( w > 0 )
			XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, 1.0 / w, 1.0, 0, 0, 0);
		else
			XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, 0, 1.0, 0, 0, 0);
	}

	float fHeightRatio;

	if ( !i || i == numVertChildren )
	{
		w = I_fabs(XAnimGetWeight(pAnimTree, heightAnim) - 1.0) * (1000.0 / level.frameTime);

		if ( w > 0 )
			XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0, 1.0 / w, 1.0, 0, 0, 0);
		else
			XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0, 0, 1.0, 0, 0, 0);
	}
	else
	{
		assert(trans[2] - fPrevTransZ);

		fHeightRatio = (fDelta - fPrevTransZ) / (trans[2] - fPrevTransZ);
		w = I_fabs(XAnimGetWeight(pAnimTree, heightAnim) - fHeightRatio) * (1000.0 / level.frameTime);

		if ( w > 0 )
			XAnimSetGoalWeight(pAnimTree, heightAnim, fHeightRatio, 1.0 / w, 1.0, 0, 0, 0);
		else
			XAnimSetGoalWeight(pAnimTree, heightAnim, fHeightRatio, 0, 1.0, 0, 0, 0);

		heightAnim = XAnimGetChildAt(pXAnims, baseAnim, i - 1);
		w = I_fabs(XAnimGetWeight(pAnimTree, heightAnim) - (1.0 - fHeightRatio)) * (1000.0 / level.frameTime);

		if ( w > 0 )
			XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0 - fHeightRatio, 1.0 / w, 1.0, 0, 0, 0);
		else
			XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0 - fHeightRatio, 0, 1.0, 0, 0, 0);

		leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend);
		w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim1) - (1.0 - fPrevBlend)) * (1000.0 / level.frameTime);

		if ( w > 0 )
			XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fPrevBlend, 1.0 / w, 1.0, 0, 0, 0);
		else
			XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fPrevBlend, 0, 1.0, 0, 0, 0);

		if ( fPrevBlend != 0 )
		{
			leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend + 1);
			w = I_fabs(XAnimGetWeight(pAnimTree, leafAnim2) - fPrevBlend) * (1000.0 / level.frameTime);

			if ( w > 0 )
				XAnimSetGoalWeight(pAnimTree, leafAnim2, fPrevBlend, 1.0 / w, 1.0, 0, 0, 0);
			else
				XAnimSetGoalWeight(pAnimTree, leafAnim2, fPrevBlend, 0, 1.0, 0, 0, 0);
		}
	}

	XAnimCalcAbsDelta(pAnimTree, baseAnim, rot, trans);

	float axis[4][3];
	float localAxis[4][3];

	vec3_t start;
	vec3_t end;
	vec3_t endpos;

	trace_t trace;

	VectorAngleMultiply(trans, localYaw);
	Vector2Add(trans, tagMat->trans, localAxis[3]);
	localAxis[3][2] = tagHeight;

	float yaw = RotationToYaw(rot) + localYaw;
	YawToAxis(yaw, localAxis);

	MatrixMultiply43(localAxis, turretAxis, axis);
	VectorCopy(axis[3], ent->client->ps.origin);

	VectorCopy(ent->client->ps.origin, start);
	VectorCopy(ent->client->ps.origin, end);

	start[2] += ent->client->ps.viewHeightCurrent;
	end[2] -= DEFAULT_VIEWHEIGHT;

	G_TraceCapsule(&trace, start, vec3_origin, vec3_origin, end, ent->s.number, MASK_PLAYERSOLID);

	if ( trace.fraction < 1.0 )
	{
		Vec3Lerp(start, end, trace.fraction, endpos);
		ent->client->ps.origin[2] = endpos[2];
	}

	BG_PlayerStateToEntityState(&ent->client->ps, &ent->s, qtrue, PMOVE_HANDLER_SERVER);
	VectorCopy(ent->client->ps.origin, ent->r.currentOrigin);

	AxisToAngles(axis, ent->r.currentAngles);
	SV_LinkEntity(ent);
}

/*
==============
Fire_Lead
==============
*/
void Fire_Lead( gentity_t *ent, gentity_t *activator )
{
	weaponParms wp;

	assert(activator);
	if ( activator == &g_entities[ENTITYNUM_NONE] )
		activator = &g_entities[ENTITYNUM_WORLD];

	Turret_FillWeaponParms(ent, activator, &wp);
	assert(ent->pTurretInfo);
	wp.weapDef = BG_GetWeaponDef(ent->s.weapon);

	if ( wp.weapDef->weaponType )
		Weapon_RocketLauncher_Fire(ent, 0, &wp);
	else
		Bullet_Fire(activator, ent->pTurretInfo->playerSpread, &wp, ent, level.time);

	G_AddEvent(ent, EV_FIRE_WEAPON_MG42, activator->s.number);
}

/*
==============
turret_shoot_internal
==============
*/
void turret_shoot_internal( gentity_t *self, gentity_t *other )
{
	assert(self->pTurretInfo);
	assert(other);

	self->pTurretInfo->fireSndDelay = 3 * BG_GetWeaponDef(self->s.weapon)->fireTime;

	if ( other->client )
	{
		Fire_Lead(self, other);
		other->client->ps.viewlocked = PLAYERVIEWLOCK_WEAPONJITTER;
	}
}

/*
==============
turret_track
==============
*/
void turret_track( gentity_t *self, gentity_t *other )
{
	WeaponDef *weapDef;
	turretInfo_s *turretInfo;

	turretInfo = self->pTurretInfo;
	assert(turretInfo);
	assert(self->active);
	assert(other->client);

	turret_clientaim(self, other);
	G_PlayerTurretPositionAndBlend(other, self);

	weapDef = BG_GetWeaponDef(self->s.weapon);

	other->client->ps.viewlocked = PLAYERVIEWLOCK_FULL;
	self->s.eFlags &= ~EF_FIRING;
	turretInfo->fireTime -= 50;

	if ( turretInfo->fireTime > 0 )
	{
		return;
	}

	turretInfo->fireTime = 0;

	if ( !(other->client->buttons & BUTTON_ATTACK) )
	{
		return;
	}

	turretInfo->fireTime = weapDef->fireTime;
	turret_shoot_internal(self, other);
	self->s.eFlags |= EF_FIRING;
}
