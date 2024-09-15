#include "../qcommon/qcommon.h"
#include "g_shared.h"

vec2_t traceOffsets[] =
{
	{ 0.000000, 0.000000},
	{ 1.000000, 1.000000},
	{ 1.000000, -1.000000},
	{ -1.000000, 1.000000},
	{ -1.000000, -1.000000},
};

qboolean LogAccuracyHit(gentity_s *target, gentity_s *attacker)
{
	if ( !target->takedamage )
		return 0;

	if ( target == attacker )
		return 0;

	if ( !target->client )
		return 0;

	if ( !attacker->client )
		return 0;

	if ( target->client->ps.pm_type <= PM_INTERMISSION )
		return OnSameTeam(target, attacker) == 0;

	return 0;
}

extern dvar_t *sv_fps;
extern dvar_t *g_antilag;

void G_AntiLagRewindClientPos(int gameTime, AntilagClientStore *antilagStore)
{
	vec3_t origin;
	//int snapshotTime;
	int clientNum;

	if ( g_antilag->current.boolean )
	{
		memset(antilagStore, 0, sizeof(AntilagClientStore));

		if ( level.time - gameTime > 1000 / sv_fps->current.integer )
		{
			for ( clientNum = 0; clientNum < level.maxclients; ++clientNum )
			{
				if ( level.clients[clientNum].sess.connected == CON_CONNECTED
				        && level.clients[clientNum].sess.sessionState == SESS_STATE_PLAYING
				        && SV_GetClientPositionsAtTime(clientNum, gameTime, origin) )
				{
					//snapshotTime = gameTime;
					memcpy(antilagStore->realClientPositions[clientNum], g_entities[clientNum].r.currentOrigin, sizeof(antilagStore->realClientPositions[clientNum]));
					SV_UnlinkEntity(&g_entities[clientNum]);
					memcpy(g_entities[clientNum].r.currentOrigin, origin, sizeof(vec3_t));
					SV_LinkEntity(&g_entities[clientNum]);
					antilagStore->clientMoved[clientNum] = 1;
				}
			}
		}
	}
}

void G_AntiLag_RestoreClientPos(AntilagClientStore *antilagStore)
{
	int clientNum;

	if ( g_antilag->current.boolean )
	{
		for ( clientNum = 0; clientNum < level.maxclients; ++clientNum )
		{
			if ( antilagStore->clientMoved[clientNum] )
			{
				SV_UnlinkEntity(&g_entities[clientNum]);
				memcpy(g_entities[clientNum].r.currentOrigin, antilagStore->realClientPositions[clientNum], sizeof(vec3_t));
				SV_LinkEntity(&g_entities[clientNum]);
			}
		}
	}
}

void Weapon_Throw_Grenade(gentity_s *ent, int grenType, weaponParms *wp)
{
	float speed;
	float fAddVel;
	vec3_t vTossVel;
	gentity_s *grenade;

	speed = (float)wp->weapDef->projectileSpeed;
	VectorScale(wp->forward, speed, vTossVel);
	vTossVel[2] = (float)wp->weapDef->projectileSpeedUp + vTossVel[2];
	grenade = fire_grenade(ent, wp->muzzleTrace, vTossVel, grenType, wp->weapDef->fuseTime);
	Vec3Normalize(vTossVel);
	fAddVel = DotProduct(ent->client->ps.velocity, vTossVel);
	VectorMA(grenade->s.pos.trDelta, fAddVel, vTossVel, grenade->s.pos.trDelta);
}

void Weapon_RocketLauncher_Fire(gentity_s *ent, float spread, weaponParms *targetOffset)
{
	float temp;
	vec3_t startPos;
	vec3_t dir;
	float fAimOffset;
	float scale;
	vec3_t kickBack;

	kickBack[1] = 16.0;
	temp = tan(spread * 0.017453292);
	fAimOffset = temp * 16.0;
	gunrandom(kickBack, &scale);
	kickBack[0] = kickBack[0] * fAimOffset;
	scale = scale * fAimOffset;
	VectorScale(targetOffset->forward, 16.0, dir);
	VectorMA(dir, kickBack[0], targetOffset->right, dir);
	VectorMA(dir, scale, targetOffset->up, dir);
	Vec3Normalize(dir);
	VectorCopy(targetOffset->muzzleTrace, startPos);
	fire_rocket(ent, startPos, dir);

	if ( ent->client )
		VectorMA(ent->client->ps.velocity, -64.0, targetOffset->forward, ent->client->ps.velocity);
}

void G_CalcMuzzlePoints(const gentity_s *ent, weaponParms *wp)
{
	vec3_t angles;

	VectorCopy(ent->client->ps.viewangles, angles);
	angles[0] = ent->client->fGunPitch;
	angles[1] = ent->client->fGunYaw;
	AngleVectors(angles, wp->forward, wp->right, wp->up);
	G_GetPlayerViewOrigin((gentity_s *)ent, wp->muzzleTrace);
}

void G_SetEquippedOffHand(int clientNum, int offHandIndex)
{
	SV_GameSendServerCommand(clientNum, SV_CMD_RELIABLE, va("%c %i", 67, offHandIndex));
}

int G_GivePlayerWeapon(playerState_s *ps, int weapon)
{
	int offhand;
	unsigned int altWeapon;
	WeaponDef *weaponDef;
	WeaponDef *altWeaponDef;

	if ( Com_BitCheck(ps->weapons, weapon) )
		return 0;

	altWeaponDef = BG_GetWeaponDef(weapon);

	if ( altWeaponDef->weaponClass == WEAPCLASS_TURRET )
		return 0;

	if ( altWeaponDef->weaponClass == WEAPCLASS_NON_PLAYER )
		return 0;

	Com_BitSet(ps->weapons, weapon);
	Com_BitClear(ps->weaponrechamber, weapon);

	if ( altWeaponDef->weaponClass == WEAPCLASS_ITEM )
		return 1;

	if ( altWeaponDef->offhandClass )
	{
		if ( ps->offHandIndex )
		{
			if ( BG_WeaponAmmo(ps, ps->offHandIndex) <= 0 )
			{
				weaponDef = BG_GetWeaponDef(ps->offHandIndex);
				offhand = BG_GetFirstAvailableOffhand(ps, weaponDef->offhandClass);

				if ( offhand )
					ps->offHandIndex = offhand;
				else
					ps->offHandIndex = weapon;

				G_SetEquippedOffHand(ps->clientNum, ps->offHandIndex);
			}
		}
		else
		{
			ps->offHandIndex = weapon;
			G_SetEquippedOffHand(ps->clientNum, ps->offHandIndex);
		}

		return 1;
	}
	else
	{
		if ( (unsigned int)(altWeaponDef->weaponSlot - 1) <= 1 )
		{
			if ( ps->weaponslots[1] )
			{
				if ( !ps->weaponslots[2] )
					ps->weaponslots[2] = weapon;
			}
			else
			{
				ps->weaponslots[1] = weapon;
			}
		}
		for ( altWeapon = altWeaponDef->altWeaponIndex;
		        altWeapon && !Com_BitCheck(ps->weapons, altWeapon);
		        altWeapon = BG_GetWeaponDef(altWeapon)->altWeaponIndex )
		{
			Com_BitSet(ps->weapons, altWeapon);
			Com_BitClear(ps->weaponrechamber, weapon);
		}

		return 1;
	}
}

void FireWeapon(gentity_s *ent, int gametime)
{
	float maxSpread;
	float minSpread;
	float spread;
	float currentAimSpreadScale;
	weaponParms wp;

	if ( (ent->client->ps.eFlags & 0x300) == 0 || !ent->active )
	{
		wp.weapDef = BG_GetWeaponDef(ent->s.weapon);
		G_CalcMuzzlePoints(ent, &wp);
		currentAimSpreadScale = ent->client->currentAimSpreadScale;
		BG_GetSpreadForWeapon(&ent->client->ps, ent->s.weapon, &minSpread, &maxSpread);

		if ( ent->client->ps.fWeaponPosFrac == 1.0 )
			spread = (maxSpread - wp.weapDef->adsSpread) * currentAimSpreadScale + wp.weapDef->adsSpread;
		else
			spread = (maxSpread - minSpread) * currentAimSpreadScale + minSpread;

		if ( wp.weapDef->weaponType )
		{
			if ( wp.weapDef->weaponType == WEAPTYPE_GRENADE )
			{
				Weapon_Throw_Grenade(ent, ent->s.weapon, &wp);
			}
			else if (wp.weapDef->weaponType == WEAPTYPE_PROJECTILE)
			{
				Weapon_RocketLauncher_Fire(ent, spread, &wp);
			}
			else
			{
				Com_Error(ERR_DROP, "Unknown weapon type %i for %s", wp.weapDef->weaponType, wp.weapDef->szInternalName);
			}
		}
		else
		{
			Bullet_Fire(ent, spread, &wp, ent, gametime);
		}
	}
}

bool Melee_Trace(gentity_s *ent, weaponParms *wp, int damage, float range, float width, float height, trace_t *traceResult, float *hitOrigin)
{
	float widthScale;
	float heightScale;
	int numTraces;
	int i;
	vec3_t end;

	if ( width > 0.0 || height > 0.0 )
		numTraces = 5;
	else
		numTraces = 1;

	for ( i = 0; i < numTraces; ++i )
	{
		VectorMA(wp->muzzleTrace, range, wp->forward, end);
		widthScale = width * traceOffsets[i][0];
		VectorMA(end, widthScale, wp->right, end);
		heightScale = height * traceOffsets[i][1];
		VectorMA(end, heightScale, wp->up, end);
		G_LocationalTrace(traceResult, wp->muzzleTrace, end, ent->s.number, 41953329, bulletPriorityMap);
		Vec3Lerp(wp->muzzleTrace, end, traceResult->fraction, hitOrigin);

		if ( !i )
			G_CheckHitTriggerDamage(ent, wp->muzzleTrace, hitOrigin, damage, MOD_MELEE);

		if ( (traceResult->surfaceFlags & 0x10) == 0 && traceResult->fraction != 1.0 )
			return 1;
	}

	return 0;
}

void Weapon_Melee(gentity_s *ent, weaponParms *wp, float range, float width, float height)
{
	vec3_t origin;
	int damage;
	gentity_s *tempEnt;
	gentity_s *self;
	trace_t trace;

	damage = BG_GetWeaponDef(ent->s.weapon)->meleeDamage;

	if ( Melee_Trace(ent, wp, damage, range, width, height, &trace, origin) )
	{
		self = &g_entities[trace.entityNum];

		if ( self->client )
			tempEnt = G_TempEntity(origin, EV_MELEE_HIT);
		else
			tempEnt = G_TempEntity(origin, EV_MELEE_MISS);

		tempEnt->s.otherEntityNum = trace.entityNum;
		tempEnt->s.eventParm = DirToByte(trace.normal);
		tempEnt->s.weapon = ent->s.weapon;

		if ( trace.entityNum != 1022 )
		{
			if ( self->takedamage )
			{
				G_Damage(self, ent, ent, wp->forward, origin, damage + rand() % 5, 0, 7, trace.partGroup, 0);
			}
		}
	}
}

extern dvar_t *player_meleeRange;
extern dvar_t *player_meleeWidth;
extern dvar_t *player_meleeHeight;
void FireWeaponMelee(gentity_s *ent)
{
	weaponParms wp;

	if ( (ent->client->ps.eFlags & 0x300) == 0 || !ent->active )
	{
		wp.weapDef = BG_GetWeaponDef(ent->s.weapon);
		G_GetPlayerViewOrigin(ent, wp.muzzleTrace);
		G_GetPlayerViewDirection(ent, wp.forward, wp.right, wp.up);
		Weapon_Melee(
		    ent,
		    &wp,
		    player_meleeRange->current.decimal,
		    player_meleeWidth->current.decimal,
		    player_meleeHeight->current.decimal);
	}
}

void G_UseOffHand(gentity_s *ent)
{
	weaponParms wp;

	wp.weapDef = BG_GetWeaponDef(ent->client->ps.offHandIndex);
	G_CalcMuzzlePoints(ent, &wp);
	Weapon_Throw_Grenade(ent, ent->client->ps.offHandIndex, &wp);
}

int G_GetWeaponIndexForName(const char *name)
{
	if ( level.initializing )
		return BG_GetWeaponIndexForName(name, G_RegisterWeapon);
	else
		return BG_FindWeaponIndexForName(name);
}

void G_SetupWeaponDef()
{
	Com_DPrintf("----------------------\n");
	Com_DPrintf("Game: G_SetupWeaponDef\n");

	if ( !bg_iNumWeapons )
	{
		SV_SetWeaponInfoMemory();
		ClearRegisteredItems();
		BG_ClearWeaponDef();
		BG_FillInAmmoItems(G_RegisterWeapon);
		G_GetWeaponIndexForName("defaultweapon_mp");
	}

	Com_DPrintf("----------------------\n");
}