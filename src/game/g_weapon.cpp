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

vec2_t traceOffsets[] =
{
	{ 0.000000, 0.000000},
	{ 1.000000, 1.000000},
	{ 1.000000, -1.000000},
	{ -1.000000, 1.000000},
	{ -1.000000, -1.000000},
};

extern dvar_t *sv_fps;
extern dvar_t *g_antilag;

void G_AntiLagRewindClientPos(int gameTime, AntilagClientStore *antilagStore)
{
	vec3_t origin;
	int snapshotTime;
	int clientNum;

	if ( g_antilag->current.boolean )
	{
		memset(antilagStore, 0, sizeof(AntilagClientStore));

		if ( level.time - gameTime > 1000 / sv_fps->current.integer )
		{
			for ( clientNum = 0; clientNum < level.maxclients; ++clientNum )
			{
				if ( level.clients[clientNum].sess.connected == CS_CONNECTED
				        && level.clients[clientNum].sess.sessionState == STATE_PLAYING
				        && SV_GetClientPositionsAtTime(clientNum, gameTime, origin) )
				{
					snapshotTime = gameTime;
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
	fAddVel = VectorsLengthSquared(ent->client->ps.velocity, vTossVel);
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
	temp = tan(spread * 0.0174532925199433);
	fAimOffset = temp * 16.0;
	Bullet_RandomDir(kickBack, &scale);
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

extern char bulletPriorityMap[];
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
		self = &g_entities[trace.hitId];

		if ( self->client )
			tempEnt = G_TempEntity(origin, EV_MELEE_HIT);
		else
			tempEnt = G_TempEntity(origin, EV_MELEE_MISS);

		tempEnt->s.otherEntityNum = trace.hitId;
		tempEnt->s.eventParm = DirToByte(trace.normal);
		tempEnt->s.weapon = ent->s.weapon;

		if ( trace.hitId != 1022 )
		{
			if ( self->takedamage )
			{
				G_Damage(self, ent, ent, wp->forward, origin, damage + rand() % 5, 0, 7, (hitLocation_t)trace.partName, 0);
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