#include "../qcommon/qcommon.h"
#include "g_shared.h"

turretInfo_s turretInfo[32];

qboolean turret_behind(gentity_s *self, gentity_s *player)
{
	float yaw;
	float clamped;
	float absmin;
	float angle;
	float dot;
	float centerYaw;
	float yawSpan;
	float minYaw;
	vec3_t forward;
	vec3_t dir;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	minYaw = self->r.currentAngles[1] + pTurretInfo->arcmin[1];
	absmin = I_fabs(pTurretInfo->arcmin[1]);
	yawSpan = (I_fabs(pTurretInfo->arcmax[1]) + absmin) * 0.5;
	yaw = minYaw + yawSpan;
	centerYaw = AngleNormalize180(yaw);
	YawVectors(centerYaw, forward, 0);
	Vec3Normalize(forward);
	VectorSubtract(self->r.currentOrigin, player->r.currentOrigin, dir);
	dir[2] = 0.0;
	Vec3Normalize(dir);
	dot = DotProduct(forward, dir);
	clamped = I_fclamp(dot, -1.0, 1.0);
	angle = Q_acos(clamped) * DEGINRAD;

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

void G_SpawnTurret(gentity_s *ent, const char *weaponName)
{
	int i;
	turretInfo_s *turret;
	WeaponDef *weaponDef;

	turret = 0;

	for ( i = 0; i < 32; ++i )
	{
		turret = &turretInfo[i];
		if ( !turret->inuse )
			break;
	}

	if ( i == 32 )
		Com_Error(ERR_DROP, "G_SpawnTurret: max number of turrets (%d) exceeded", 32);

	memset(turret, 0, sizeof(turretInfo_s));
	ent->pTurretInfo = turret;
	turret->inuse = 1;
	ent->s.weapon = G_GetWeaponIndexForName(weaponName);

	if ( !ent->s.weapon )
		Com_Error(ERR_DROP, "bad weaponinfo '%s' specified for turret", weaponName);

	weaponDef = BG_GetWeaponDef(ent->s.weapon);

	if ( weaponDef->weaponClass != WEAPCLASS_TURRET )
	{
		Scr_Error(va("G_SpawnTurret: weapon '%s' isn't a turret. This usually indicates that the weapon failed to load.", weaponName));
	}

	if ( !level.initializing && !IsItemRegistered(ent->s.weapon) )
	{
		Scr_Error(va("turret '%s' not precached", weaponName));
	}

	turret->fireTime = 0;
	turret->stance = weaponDef->stance;
	turret->prevStance = -1;
	turret->fireSndDelay = 0;

	if ( weaponDef->loopFireSound && *weaponDef->loopFireSound )
		turret->fireSnd = G_SoundAliasIndex(weaponDef->loopFireSound);
	else
		turret->fireSnd = 0;

	if ( weaponDef->loopFireSoundPlayer && *weaponDef->loopFireSoundPlayer )
		turret->fireSndPlayer = G_SoundAliasIndex(weaponDef->loopFireSoundPlayer);
	else
		turret->fireSndPlayer = 0;

	if ( weaponDef->stopFireSound && *weaponDef->stopFireSound )
		turret->stopSnd = G_SoundAliasIndex(weaponDef->stopFireSound);
	else
		turret->stopSnd = 0;

	if ( weaponDef->stopFireSoundPlayer && *weaponDef->stopFireSoundPlayer )
		turret->stopSndPlayer = G_SoundAliasIndex(weaponDef->stopFireSoundPlayer);
	else
		turret->stopSndPlayer = 0;

	if ( !level.spawnVars.spawnVarsValid || !G_SpawnFloat("rightarc", "", &turret->arcmin[1]) )
		turret->arcmin[1] = weaponDef->rightArc;

	turret->arcmin[1] = -turret->arcmin[1];

	if ( turret->arcmin[1] > 0.0 )
		turret->arcmin[1] = 0.0;

	if ( !level.spawnVars.spawnVarsValid || !G_SpawnFloat("leftarc", "", &turret->arcmax[1]) )
		turret->arcmax[1] = weaponDef->leftArc;

	if ( turret->arcmax[1] < 0.0 )
		turret->arcmax[1] = 0.0;

	if ( !level.spawnVars.spawnVarsValid || !G_SpawnFloat("toparc", "", turret->arcmin) )
		turret->arcmin[0] = weaponDef->topArc;

	turret->arcmin[0] = -turret->arcmin[0];

	if ( turret->arcmin[0] > 0.0 )
		turret->arcmin[0] = 0.0;

	if ( !level.spawnVars.spawnVarsValid || !G_SpawnFloat("bottomarc", "", turret->arcmax) )
		turret->arcmax[0] = weaponDef->bottomArc;

	if ( turret->arcmax[0] < 0.0 )
		turret->arcmax[0] = 0.0;

	turret->dropPitch = -90.0;

	if ( !ent->health )
		ent->health = 100;

	if ( !level.spawnVars.spawnVarsValid || !G_SpawnInt("damage", "0", &ent->dmg) )
		ent->dmg = weaponDef->damage;

	if ( ent->dmg < 0 )
		ent->dmg = 0;

	if ( !level.spawnVars.spawnVarsValid || !G_SpawnFloat("playerSpread", "1", &turret->playerSpread) )
		turret->playerSpread = weaponDef->playerSpread;

	if ( turret->playerSpread < 0.0 )
		turret->playerSpread = 0.0;

	turret->flags = 3;
	ent->clipmask = 1;
	ent->r.contents = 2097156;
	ent->r.svFlags = 0;
	ent->s.eType = ET_TURRET;
	ent->flags |= 0x1000u;
	G_DObjUpdate(ent);
	VectorSet(ent->r.mins, -32.0, -32.0, 0.0);
	VectorSet(ent->r.maxs, 32.0, 32.0, 56.0);
	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);
	VectorClear(ent->s.angles2);
	ent->handler = 13;
	ent->nextthink = level.time + 50;
	ent->s.apos.trType = TR_LINEAR_STOP;
	ent->takedamage = 0;

	SV_LinkEntity(ent);
}

void G_InitTurrets()
{
	int i;

	for ( i = 0; i < 32; ++i )
		turretInfo[i].inuse = 0;
}

void SP_turret(gentity_s *ent)
{
	const char *weaponName;

	if ( !G_SpawnString("weaponinfo", "", &weaponName) )
		Com_Error(ERR_DROP, "no weaponinfo specified for turret");

	G_SpawnTurret(ent, weaponName);
}

void G_PlayerTurretPositionAndBlend(gentity_s *ent, gentity_s *pTurretEnt)
{
	float rotYaw;
	gclient_s *client;
	float *pCurrentOrigin;
	float *pOrigin;
	float *origin;
	float fHeightRatio;
	unsigned int iPrevBlend;
	float fPrevTransZ;
	int numVertChildren;
	float trans2[3];
	float yaw;
	float trans[3];
	float start[3];
	float end[3];
	int iBlend;
	DObjAnimMat *tagMat;
	unsigned int heightAnim;
	float fDelta;
	float fPrevBlend;
	float rot[2];
	float tagAxis[3][3];
	float localAxis[4][3];
	unsigned int leafAnim1;
	trace_t trace;
	float endpos[3];
	int numHorChildren;
	clientInfo_t *ci;
	float tagHeight;
	int i;
	unsigned int baseAnim;
	int clientNum;
	lerpFrame_t *pLerpAnim;
	WeaponDef *weapDef;
	float fBlend;
	float axis[4][3];
	XAnimTree_s *pAnimTree;
	XAnim_s *pXAnims;
	unsigned int leafAnim2;
	float localYaw;
	float turretAxis[4][3];
	float vDelta[3];

	clientNum = ent->s.clientNum;
	ci = &level_bgs.clientinfo[clientNum];
	pLerpAnim = &ci->legs;

	if ( ci->legs.animationNumber && pLerpAnim->animation && (pLerpAnim->animation->flags & 4) != 0 )
	{
		tagMat = G_DObjGetLocalTagMatrix(pTurretEnt, scr_const.tag_weapon);

		if ( tagMat )
		{
			weapDef = BG_GetWeaponDef(pTurretEnt->s.weapon);
			pAnimTree = ci->pXAnimTree;
			pXAnims = level_bgs.animData.animScriptData.animTree.anims;
			baseAnim = pLerpAnim->animationNumber & 0xFFFFFDFF;
			ConvertQuatToMat(tagMat, tagAxis);
			localYaw = vectosignedyaw(tagAxis[0]);
			AnglesToAxis(pTurretEnt->r.currentAngles, turretAxis);
			turretAxis[3][0] = pTurretEnt->r.currentOrigin[0];
			turretAxis[3][1] = pTurretEnt->r.currentOrigin[1];
			turretAxis[3][2] = pTurretEnt->r.currentOrigin[2];
			vDelta[0] = ent->r.currentOrigin[0] - turretAxis[3][0];
			vDelta[1] = ent->r.currentOrigin[1] - turretAxis[3][1];
			vDelta[2] = ent->r.currentOrigin[2] - turretAxis[3][2];
			tagHeight = (float)((float)(vDelta[0] * turretAxis[2][0]) + (float)(vDelta[1] * turretAxis[2][1]))
			            + (float)(vDelta[2] * turretAxis[2][2]);
			fDelta = tagHeight - tagMat->trans[2];
			numVertChildren = XAnimGetNumChildren(pXAnims, baseAnim);
			fPrevTransZ = 0.0;
			fPrevBlend = 0.0;
			iPrevBlend = 0;
			leafAnim2 = 0;

			if ( !numVertChildren )
			{
				Com_Error(ERR_DROP, "Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, baseAnim));
			}

			i = 0;

			do
			{
				heightAnim = XAnimGetChildAt(pXAnims, baseAnim, numVertChildren - 1 - i);
				numHorChildren = XAnimGetNumChildren(pXAnims, heightAnim);

				if ( !numHorChildren )
				{
					Com_Error(ERR_DROP, "Player anim '%s' has no children", XAnimGetAnimDebugName(pXAnims, heightAnim));
				}

				fBlend = (float)((float)numHorChildren * 0.5) + (float)(localYaw / weapDef->animHorRotateInc);

				if ( fBlend >= 0.0 )
				{
					if ( fBlend >= (float)(numHorChildren - 1) )
						fBlend = (float)(numHorChildren - 1);
				}
				else
				{
					fBlend = 0.0;
				}

				iBlend = (int)fBlend;
				fBlend = fBlend - (float)(int)fBlend;
				leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iBlend);
				XAnimGetAbsDelta(pXAnims, leafAnim1, rot, trans, 0.0);

				if ( fBlend != 0.0 )
				{
					leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iBlend + 1);
					XAnimGetAbsDelta(pXAnims, leafAnim2, rot, trans2, 0.0);
					Vec3Lerp(trans, trans2, fBlend, trans);
				}

				if ( trans[2] >= fDelta )
					break;

				fPrevTransZ = trans[2];
				iPrevBlend = iBlend;
				fPrevBlend = fBlend;

				++i;
			}
			while ( i < numVertChildren );

			XAnimClearTreeGoalWeightsStrict(pAnimTree, baseAnim, 0.2);
			XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fBlend, 0.0, 1.0, 0, 0, 0);

			if ( fBlend != 0.0 )
				XAnimSetGoalWeight(pAnimTree, leafAnim2, fBlend, 0.0, 1.0, 0, 0, 0);

			if ( i && i != numVertChildren )
			{
				fHeightRatio = (float)(fDelta - fPrevTransZ) / (float)(trans[2] - fPrevTransZ);
				XAnimSetGoalWeight(pAnimTree, heightAnim, fHeightRatio, 0.0, 1.0, 0, 0, 0);
				heightAnim = XAnimGetChildAt(pXAnims, baseAnim, numVertChildren - i);
				XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0 - fHeightRatio, 0.0, 1.0, 0, 0, 0);
				leafAnim1 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend);
				XAnimSetGoalWeight(pAnimTree, leafAnim1, 1.0 - fPrevBlend, 0.0, 1.0, 0, 0, 0);

				if ( fPrevBlend != 0.0 )
				{
					leafAnim2 = XAnimGetChildAt(pXAnims, heightAnim, iPrevBlend + 1);
					XAnimSetGoalWeight(pAnimTree, leafAnim2, fPrevBlend, 0.0, 1.0, 0, 0, 0);
				}
			}
			else
			{
				XAnimSetGoalWeight(pAnimTree, heightAnim, 1.0, 0.0, 1.0, 0, 0, 0);
			}

			XAnimCalcAbsDelta(pAnimTree, baseAnim, rot, trans);
			VectorAngleMultiply(trans, localYaw);
			localAxis[3][0] = trans[0] + tagMat->trans[0];
			localAxis[3][1] = trans[1] + tagMat->trans[1];
			localAxis[3][2] = tagHeight;
			rotYaw = RotationToYaw(rot);
			yaw = rotYaw + localYaw;
			YawToAxis(yaw, localAxis);
			MatrixMultiply43(localAxis, turretAxis, axis);
			origin = ent->client->ps.origin;
			*origin = axis[3][0];
			origin[1] = axis[3][1];
			origin[2] = axis[3][2];
			pOrigin = ent->client->ps.origin;
			start[0] = *pOrigin;
			start[1] = pOrigin[1];
			start[2] = pOrigin[2];
			client = ent->client;
			end[0] = client->ps.origin[0];
			end[1] = client->ps.origin[1];
			end[2] = client->ps.origin[2];
			start[2] = start[2] + ent->client->ps.viewHeightCurrent;
			end[2] = end[2] - 100.0;

			G_TraceCapsule(
			    &trace,
			    start,
			    vec3_origin,
			    vec3_origin,
			    end,
			    ent->s.number,
			    42008593);

			if ( trace.fraction < 1.0 && !trace.allsolid && !trace.startsolid )
			{
				Vec3Lerp(start, end, trace.fraction, endpos);
				ent->r.currentOrigin[2] = endpos[2];
				ent->client->ps.origin[2] = endpos[2];
			}

			BG_PlayerStateToEntityState(&ent->client->ps, &ent->s, 1, 1u);
			pCurrentOrigin = ent->client->ps.origin;
			ent->r.currentOrigin[0] = *pCurrentOrigin;
			ent->r.currentOrigin[1] = pCurrentOrigin[1];
			ent->r.currentOrigin[2] = pCurrentOrigin[2];
			AxisToAngles(axis, ent->r.currentAngles);
			SV_LinkEntity(ent);
		}
		else
		{
			Com_Printf("WARNING: aborting player positioning on turret since 'tag_weapon' does not exist\n");
		}
	}
}

void Turret_FillWeaponParms(gentity_s *ent, gentity_s *activator, weaponParms *wp)
{
	float diff[3];
	float playerPos[3];
	float len;
	float flashTag[4][3];

	if ( !G_DObjGetWorldTagMatrix(ent, scr_const.tag_flash, flashTag) )
	{
		Com_Error(ERR_DROP, "Couldn't find %s on turret (entity %d, classname '%s').\n", "tag_flash", ent->s.number, SL_ConvertToString(ent->classname));
	}

	G_GetPlayerViewOrigin(activator, playerPos);
	G_GetPlayerViewDirection(activator, wp->forward, wp->right, wp->up);

	wp->gunForward[0] = wp->forward[0];
	wp->gunForward[1] = wp->forward[1];
	wp->gunForward[2] = wp->forward[2];

	diff[0] = flashTag[3][0] - playerPos[0];
	diff[1] = flashTag[3][1] - playerPos[1];
	diff[2] = flashTag[3][2] - playerPos[2];

	len = Vec3Normalize(diff);

	wp->muzzleTrace[0] = (float)(len * wp->forward[0]) + playerPos[0];
	wp->muzzleTrace[1] = (float)(len * wp->forward[1]) + playerPos[1];
	wp->muzzleTrace[2] = (float)(len * wp->forward[2]) + playerPos[2];
}

void Fire_Lead(gentity_s *ent, gentity_s *activator)
{
	gentity_s *attacker;
	weaponParms wp;

	if ( activator == &g_entities[1023] )
		attacker = &g_entities[1022];
	else
		attacker = activator;

	Turret_FillWeaponParms(ent, attacker, &wp);
	wp.weapDef = BG_GetWeaponDef(ent->s.weapon);

	if ( wp.weapDef->weaponType )
		Weapon_RocketLauncher_Fire(ent, 0.0, &wp);
	else
		Bullet_Fire(attacker, ent->pTurretInfo->playerSpread, &wp, ent, level.time);

	G_AddEvent(ent, EV_FIRE_WEAPON_MG42, attacker->s.number);
}

void turret_shoot_internal(gentity_s *self, gentity_s *other)
{
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	pTurretInfo->fireSndDelay = 3 * BG_GetWeaponDef(self->s.weapon)->fireTime;

	if ( other->client )
	{
		Fire_Lead(self, other);
		other->client->ps.viewlocked = 2;
	}
}

void turret_clientaim(gentity_s *self, gentity_s *other)
{
	gclient_s *client;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	client = other->client;
	client->ps.viewlocked = 1;
	client->ps.viewlocked_entNum = self->s.number;

	self->s.angles2[0] = AngleSubtract(client->ps.viewangles[0], self->r.currentAngles[0]);
	self->s.angles2[0] = I_fclamp(self->s.angles2[0], pTurretInfo->arcmin[0], pTurretInfo->arcmax[0]);
	self->s.angles2[1] = AngleSubtract(client->ps.viewangles[1], self->r.currentAngles[1]);
	self->s.angles2[1] = I_fclamp(self->s.angles2[1], pTurretInfo->arcmin[1], pTurretInfo->arcmax[1]);
	self->s.angles2[2] = 0.0;

	if ( (pTurretInfo->flags & 0x800) != 0 )
	{
		pTurretInfo->flags &= ~0x800u;
		self->s.eFlags ^= 2u;
	}
}

void turret_track(gentity_s *self, gentity_s *other)
{
	WeaponDef *weaponDef;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	turret_clientaim(self, other);
	G_PlayerTurretPositionAndBlend(other, self);
	weaponDef = BG_GetWeaponDef(self->s.weapon);
	other->client->ps.viewlocked = 1;
	self->s.eFlags &= ~0x40u;
	pTurretInfo->fireTime -= 50;

	if ( pTurretInfo->fireTime <= 0 )
	{
		pTurretInfo->fireTime = 0;
		if ( (other->client->buttons & 1) != 0 )
		{
			pTurretInfo->fireTime = weaponDef->fireTime;
			turret_shoot_internal(self, other);
			self->s.eFlags |= 0x40u;
		}
	}
}

void turret_UpdateSound(gentity_s *self)
{
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	self->s.loopSound = 0;

	if ( pTurretInfo->fireSndDelay > 0 )
	{
		self->s.loopSound = (unsigned char)pTurretInfo->fireSnd;
		pTurretInfo->fireSndDelay -= 50;

		if ( pTurretInfo->fireSndDelay <= 0 )
		{
			if ( pTurretInfo->stopSnd )
			{
				self->s.loopSound = 0;
				G_PlaySoundAlias(self, pTurretInfo->stopSnd);
			}
		}
	}
}

void turret_think_client(gentity_s *self)
{
	gentity_s *owner;

	owner = &g_entities[self->r.ownerNum];

	if ( owner->active != 1 || owner->client->sess.sessionState )
	{
		G_ClientStopUsingTurret(self);
	}
	else
	{
		turret_track(self, owner);
		turret_UpdateSound(self);
	}
}

void turret_use(gentity_s *self, gentity_s *owner, gentity_s *other)
{
	float clamp0;
	float clamp1;
	gclient_s *client;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;
	client = owner->client;
	owner->active = 1;
	self->active = 1;
	self->r.ownerNum = owner->s.number;
	client->ps.viewlocked = 1;
	client->ps.viewlocked_entNum = self->s.number;
	pTurretInfo->flags |= 0x800u;
	VectorCopy(owner->r.currentOrigin, pTurretInfo->userOrigin);
	owner->s.otherEntityNum = self->s.number;
	self->s.otherEntityNum = owner->s.number;

	if ( (client->ps.pm_flags & 1) != 0 )
		pTurretInfo->prevStance = 2;
	else
		pTurretInfo->prevStance = (client->ps.pm_flags & 2) != 0;

	if ( pTurretInfo->stance == 2 )
	{
		client->ps.eFlags |= 0x100u;
		client->ps.eFlags &= ~0x200u;
	}
	else if ( pTurretInfo->stance == 1 )
	{
		client->ps.eFlags |= 0x200u;
		client->ps.eFlags &= ~0x100u;
	}
	else
	{
		client->ps.eFlags |= 0x300u;
	}

	self->s.angles2[0] = AngleSubtract(client->ps.viewangles[0], self->r.currentAngles[0]);
	self->s.angles2[0] = I_fclamp(self->s.angles2[0], pTurretInfo->arcmin[0], pTurretInfo->arcmax[0]);
	self->s.angles2[1] = AngleSubtract(client->ps.viewangles[1], self->r.currentAngles[1]);
	self->s.angles2[1] = I_fclamp(self->s.angles2[1], pTurretInfo->arcmin[1], pTurretInfo->arcmax[1]);
	self->s.angles2[2] = 0.0;
	client->ps.viewAngleClampRange[0] = AngleSubtract(pTurretInfo->arcmax[0], pTurretInfo->arcmin[0]) * 0.5;
	client->ps.viewAngleClampBase[0] = self->r.currentAngles[0] + pTurretInfo->arcmax[0];
	clamp0 = client->ps.viewAngleClampBase[0] - client->ps.viewAngleClampRange[0];
	client->ps.viewAngleClampBase[0] = AngleNormalize360Accurate(clamp0);
	client->ps.viewAngleClampRange[1] = AngleSubtract(pTurretInfo->arcmax[1], pTurretInfo->arcmin[1]) * 0.5;
	client->ps.viewAngleClampBase[1] = self->r.currentAngles[1] + pTurretInfo->arcmax[1];
	clamp1 = client->ps.viewAngleClampBase[1] - client->ps.viewAngleClampRange[1];
	client->ps.viewAngleClampBase[1] = AngleNormalize360Accurate(clamp1);
}

void turret_controller(gentity_s *self, int *partBits)
{
	DObj_s *obj;
	vec3_t angles;

	angles[1] = self->s.angles2[1];
	angles[0] = self->s.angles2[0];
	angles[2] = 0.0;

	obj = Com_GetServerDObj(self->s.number);

	DObjSetControlTagAngles(obj, partBits, scr_const.tag_aim, angles);
	DObjSetControlTagAngles(obj, partBits, scr_const.tag_aim_animated, angles);

	angles[0] = self->s.angles2[2];
	angles[1] = 0.0;

	DObjSetControlTagAngles(obj, partBits, scr_const.tag_flash, angles);
}

int turret_UpdateTargetAngles(gentity_s *self, const float *desiredAngles, int bManned)
{
	vec2_t deltaAngles;
	int bComplete;
	turretInfo_s *pTurretInfo;
	float fDelta;
	float fSpeed[2];
	int i;

	pTurretInfo = self->pTurretInfo;
	bComplete = 1;
	deltaAngles[1] = self->s.angles2[0];
	self->s.angles2[0] = deltaAngles[1] + self->s.angles2[2];

	if ( bManned )
	{
		fSpeed[0] = BG_GetWeaponDef(self->s.weapon)->maxVertTurnSpeed;
		fSpeed[1] = BG_GetWeaponDef(self->s.weapon)->maxHorTurnSpeed;
	}
	else
	{
		fSpeed[0] = 200.0;
		fSpeed[1] = 200.0;
	}

	if ( (pTurretInfo->flags & 0x200) != 0 && (pTurretInfo->flags & 0x100) != 0 && fSpeed[0] < 360.0 )
		fSpeed[0] = 360.0;

	for ( i = 0; i <= 1; ++i )
	{
		fSpeed[i] = fSpeed[i] * 0.050000001;
		fDelta = AngleSubtract(desiredAngles[i], self->s.angles2[i]);

		if ( fDelta <= (float)fSpeed[i] )
		{
			if ( -fSpeed[i] > (float)fDelta )
			{
				bComplete = 0;
				fDelta = -fSpeed[i];
			}
		}
		else
		{
			bComplete = 0;
			fDelta = fSpeed[i];
		}

		self->s.angles2[i] = self->s.angles2[i] + fDelta;
	}

	deltaAngles[0] = self->s.angles2[0];
	self->s.angles2[2] = deltaAngles[0];

	if ( (pTurretInfo->flags & 0x200) != 0 )
	{
		if ( (pTurretInfo->flags & 0x400) != 0 )
		{
			if ( pTurretInfo->triggerDown <= (float)self->s.angles2[0] )
				pTurretInfo->flags &= ~0x100u;
			else
				deltaAngles[0] = pTurretInfo->triggerDown;
		}
		else if ( self->s.angles2[0] <= (float)pTurretInfo->triggerDown )
		{
			pTurretInfo->flags &= ~0x100u;
		}
		else
		{
			deltaAngles[0] = pTurretInfo->triggerDown;
		}
	}

	fDelta = AngleSubtract(deltaAngles[0], deltaAngles[1]);

	if ( fDelta <= (float)fSpeed[0] )
	{
		if ( -fSpeed[0] > (float)fDelta )
		{
			bComplete = 0;
			fDelta = -fSpeed[0];
		}
	}
	else
	{
		bComplete = 0;
		fDelta = fSpeed[0];
	}

	self->s.angles2[0] = deltaAngles[1] + fDelta;
	self->s.angles2[2] = self->s.angles2[2] - self->s.angles2[0];

	return bComplete;
}

int turret_ReturnToDefaultPos(gentity_s *self, int bManned)
{
	float dropPitch;
	vec2_t desiredAngles;
	turretInfo_s *pTurretInfo;

	pTurretInfo = self->pTurretInfo;

	if ( bManned )
		dropPitch = 0.0;
	else
		dropPitch = pTurretInfo->dropPitch;

	desiredAngles[0] = dropPitch;
	desiredAngles[1] = 0.0;

	return turret_UpdateTargetAngles(self, desiredAngles, bManned);
}

void turret_think(gentity_s *self)
{
	self->nextthink = level.time + 50;

	if ( self->tagInfo )
		G_GeneralLink(self);

	if ( !g_entities[self->r.ownerNum].client )
	{
		turret_UpdateSound(self);
		self->s.eFlags &= ~0x40u;
		turret_ReturnToDefaultPos(self, 0);
	}
}

void turret_think_init(gentity_s *ent)
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
	int numSteps;

	numSteps = 30;
	pTurretInfo = ent->pTurretInfo;
	ent->handler = 14;
	ent->nextthink = level.time + 50;
	aimMtx = G_DObjGetLocalTagMatrix(ent, scr_const.tag_aim);

	if ( aimMtx )
	{
		weaponMtx = G_DObjGetLocalTagMatrix(ent, scr_const.tag_butt);

		if ( weaponMtx )
		{
			AnglesToAxis(ent->r.currentAngles, baseMtx);
			VectorCopy(ent->r.currentOrigin, baseMtx[3]);
			VectorSubtract(weaponMtx->trans, aimMtx->trans, dir);
			MatrixTransformVector43(aimMtx->trans, baseMtx, start);

			for ( i = 0; i <= numSteps; ++i )
			{
				angles[0] = (float)i * -3.0;
				angles[1] = 0.0;
				angles[2] = 0.0;

				AnglesToAxis(angles, mtx);
				MatrixTransformVector(dir, mtx, transDir);
				VectorAdd(aimMtx->trans, transDir, transDir);
				MatrixTransformVector43(transDir, baseMtx, end);
				G_LocationalTrace(&trace, start, end, ent->s.number, 2065, bulletPriorityMap);

				if ( trace.fraction < 1.0 )
				{
					pTurretInfo->dropPitch = angles[0];
					return;
				}
			}
		}
	}
}