#include "../qcommon/qcommon.h"
#include "g_shared.h"

gentity_s* fire_grenade(gentity_s *self, float *start, float *dir, int grenadeWPID, int time)
{
	WeaponDef *weaponDef;
	gentity_s *grenade;

	grenade = G_Spawn();

	if ( self->client && self->client->ps.grenadeTimeLeft )
	{
		grenade->nextthink = level.time + self->client->ps.grenadeTimeLeft;
		self->client->ps.grenadeTimeLeft = 0;
	}
	else
	{
		grenade->nextthink = level.time + time;
	}

	if ( self->client )
		self->client->ps.grenadeTimeLeft = 0;

	grenade->handler = ENT_HANDLER_GRENADE;
	grenade->s.eType = ET_MISSILE;
	grenade->r.svFlags = SVF_BROADCAST;
	grenade->s.weapon = grenadeWPID;
	grenade->r.ownerNum = self->s.number;
	grenade->parent = self;
	weaponDef = BG_GetWeaponDef(grenadeWPID);
	Scr_SetString(&grenade->classname, scr_const.grenade);
	grenade->dmg = weaponDef->damage;
	grenade->s.eFlags = 0x1000000;
	grenade->clipmask = 41953425;
	grenade->s.time = level.time + 50;
	grenade->s.pos.trType = TR_GRAVITY;
	grenade->s.pos.trTime = level.time;
	VectorCopy(start, grenade->s.pos.trBase);
	VectorCopy(dir, grenade->s.pos.trDelta);
	SnapVector(grenade->s.pos.trDelta);
	grenade->s.apos.trType = TR_LINEAR;
	grenade->s.apos.trTime = level.time;
	vectoangles(dir, grenade->s.apos.trBase);
	grenade->s.apos.trBase[0] = AngleNormalize360(grenade->s.apos.trBase[0] - 120.0);
	grenade->s.apos.trDelta[0] = flrand(-45.0, 45.0) + 720.0;
	grenade->s.apos.trDelta[1] = 0.0;
	grenade->s.apos.trDelta[2] = flrand(-45.0, 45.0) + 360.0;
	VectorCopy(start, grenade->r.currentOrigin);
	VectorCopy(grenade->s.apos.trBase, grenade->r.currentAngles);
#ifdef LIBCOD
	extern int codecallback_fire_grenade;
	if (codecallback_fire_grenade)
	{
		stackPushString(weaponDef->szInternalName);
		stackPushEntity(grenade);
		short ret = Scr_ExecEntThread(self, codecallback_fire_grenade, 2);
		Scr_FreeThread(ret);
	}
#endif
	return grenade;
}

gentity_s* fire_rocket(gentity_s *self, float *start, float *dir)
{
	WeaponDef *weaponDef;
	gentity_s *rocket;

	Vec3Normalize(dir);
	weaponDef = BG_GetWeaponDef(self->s.weapon);
	rocket = G_Spawn();
	Scr_SetString(&rocket->classname, scr_const.rocket);
	rocket->nextthink = level.time + 30000;
	rocket->handler = ENT_HANDLER_ROCKET;
	rocket->s.eType = ET_MISSILE;
	rocket->s.eFlags |= 0x400u;
	rocket->r.svFlags = SVF_BROADCAST;
	rocket->s.weapon = self->s.weapon;
	rocket->r.ownerNum = self->s.number;
	rocket->parent = self;
	rocket->dmg = weaponDef->damage;
	rocket->clipmask = 41953425;
	rocket->s.time = level.time + 50;
	rocket->s.pos.trType = TR_LINEAR;
	rocket->s.pos.trTime = level.time - 50;
	VectorCopy(start, rocket->s.pos.trBase);
	VectorScale(dir, (float)weaponDef->projectileSpeed, rocket->s.pos.trDelta);
	SnapVector(rocket->s.pos.trDelta);
	VectorCopy(start, rocket->r.currentOrigin);
	vectoangles(dir, rocket->r.currentAngles);
	G_SetAngle(rocket, rocket->r.currentAngles);
	rocket->missile.time = (float)weaponDef->destabilizeDistance / (float)weaponDef->projectileSpeed * 1000.0;
	rocket->flags |= self->flags & 0x20000;

	return rocket;
}

void G_ExplodeMissile(gentity_s *ent)
{
	WeaponDef *weaponDef;
	trace_t trace;
	vec3_t vEnd;
	vec3_t vPos;

	weaponDef = BG_GetWeaponDef(ent->s.weapon);

	if ( weaponDef->offhandClass == OFFHAND_CLASS_SMOKE_GRENADE && ent->s.groundEntityNum == 1023 )
	{
		ent->nextthink = 50;
	}
	else
	{
		BG_EvaluateTrajectory(&ent->s.pos, level.time, vPos);
		SnapVector(vPos);
		G_SetOrigin(ent, vPos);

		ent->s.eType = ET_GENERAL;
		ent->s.eFlags |= 0x20u;
		ent->flags |= 0x800u;
		ent->r.svFlags |= SVF_BROADCAST;
		VectorCopy(ent->r.currentOrigin, vEnd);
		vEnd[2] = vEnd[2] - 16.0;

		G_TraceCapsule(&trace, ent->r.currentOrigin, vec3_origin, vec3_origin, vEnd, ent->s.number, 2065);

		if ( weaponDef->projExplosionType == WEAPPROJEXP_NONE )
		{
			G_AddEvent(ent, EV_CUSTOM_EXPLODE, DirToByte(trace.normal));
		}
		else
		{
			G_AddEvent(ent, EV_GRENADE_EXPLODE, DirToByte(trace.normal));
		}

		if ( SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER) )
			ent->s.surfType = SURF_TYPE_WATER;
		else
			ent->s.surfType = SURF_TYPEINDEX(trace.surfaceFlags);

		if ( weaponDef->projExplosionEffect && weaponDef->projExplosionEffect[0] )
		{
			ent->s.eFlags |= 0x10000u;
			//Server_SwitchToValidFxScheduler();
			//fx = FX_RegisterEffect(weaponDef->projExplosionEffect);
			ent->s.time = level.time;
			//ent->s.time2 = level.time + (int)(FX_GetEffectLength(fx) + 1.0);
			//VoroN: HAX
			if ( weaponDef->offhandClass == OFFHAND_CLASS_SMOKE_GRENADE )
				ent->s.time2 = level.time + 60000;
			else
				ent->s.time2 = level.time + 50;
		}
		else
		{
			ent->freeAfterEvent = 1;
		}

		if ( weaponDef->explosionInnerDamage )
		{
			G_RadiusDamage(
			    ent->r.currentOrigin,
			    ent,
			    ent->parent,
			    (float)weaponDef->explosionInnerDamage,
			    (float)weaponDef->explosionOuterDamage,
			    (float)weaponDef->explosionRadius,
			    ent,
			    entityHandlers[ent->handler].splashMethodOfDeath);
		}

		SV_LinkEntity(ent);
	}
}

void G_MissileTrace(trace_t *results, const float *start, const float *end, int passEntityNum, int contentmask)
{
	vec3_t dir;

	G_LocationalTrace(results, start, end, passEntityNum, contentmask, bulletPriorityMap);

	if ( results->startsolid )
	{
		results->fraction = 0.0;
		VectorSubtract(start, end, dir);
		Vec3NormalizeTo(dir, results->normal);
	}
}

void RunMissile_CreateWaterSplash(gentity_s *missile, trace_t *trace)
{
	gentity_s *tempEnt;
	vec3_t reflect;

	Vec3NormalizeTo(missile->s.pos.trDelta, reflect);

	if ( reflect[2] < 0.0 )
		reflect[2] = reflect[2] * -1.0;

	tempEnt = G_TempEntity(missile->r.currentOrigin, EV_BULLET_HIT_LARGE);
	tempEnt->s.eventParm = DirToByte(trace->normal);
	tempEnt->s.scale = DirToByte(reflect);
	tempEnt->s.surfType = SURF_TYPEINDEX(trace->surfaceFlags);
	tempEnt->s.otherEntityNum = missile->s.number;
}

void MissileLandAngles(gentity_s *ent, trace_t *trace, float *vAngles, int bForceAlign)
{
	float aNorm;
	float vNorm;
	float vPitch;
	float aPitch;
	float random;
	float aRand;
	vec3_t vAng;
	float vLen;
	float aSub;
	float fSurfacePitch;
	int hitTime;

	hitTime = level.previousTime + (int)((float)(level.time - level.previousTime) * trace->fraction);
	BG_EvaluateTrajectory(&ent->s.apos, hitTime, vAngles);
	VectorCopy(vAngles, vAng);

	if ( trace->normal[2] <= 0.1 )
	{
		if ( !bForceAlign )
		{
			random = (float)((rand() & 0x7F) - 63) + ent->s.apos.trDelta[0];
			ent->s.apos.trDelta[0] = AngleNormalize360(random);
		}
	}
	else
	{
		fSurfacePitch = PitchForYawOnNormal(vAngles[1], trace->normal);
		aSub = AngleSubtract(fSurfacePitch, *vAngles);
		vLen = I_fabs(aSub);

		if ( !bForceAlign )
		{
			VectorCopy(vAngles, ent->s.apos.trBase);
			ent->s.apos.trTime = hitTime;

			if ( vLen >= 80.0 )
			{
				ent->s.apos.trDelta[0] = (G_random() * 0.30000001 + 0.85000002) * ent->s.apos.trDelta[0];
			}
			else
			{
				aRand = (G_random() * 0.30000001 + 0.85000002) * ent->s.apos.trDelta[0];
				ent->s.apos.trDelta[0] = -aRand;
			}
		}

		*vAngles = AngleNormalize180(*vAngles);

		if ( bForceAlign || vLen < 45.0 )
		{
			if ( I_fabs(*vAngles) <= 90.0 )
			{
				aNorm = AngleNormalize360(fSurfacePitch);
			}
			else
			{
				vPitch = fSurfacePitch + 180.0;
				aNorm = AngleNormalize360(vPitch);
			}

			*vAngles = aNorm;
		}
		else
		{
			if ( vLen >= 80.0 )
			{
				vNorm = AngleNormalize360(*vAngles);
			}
			else
			{
				aPitch = aSub * 0.25 + *vAngles;
				vNorm = AngleNormalize360(aPitch);
			}

			*vAngles = vNorm;
		}
	}
}

int BounceMissile(gentity_s *ent, trace_t *trace)
{
	float dotScale;
	int surfType;
	int contents;
	float len2;
	float vScale;
	float dot;
	float vLenSq;
	float vLenSq2;
	vec3_t angles;
	vec3_t normal;
	vec3_t velocity;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ent->s.weapon);
	contents = SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER);
	surfType = SURF_TYPEINDEX(trace->surfaceFlags);

	BG_EvaluateTrajectoryDelta(
	    &ent->s.pos,
	    level.previousTime + (int)((float)(level.time - level.previousTime) * trace->fraction),
	    velocity);

	dot = DotProduct(velocity, trace->normal);
	dotScale = dot * -2.0;
	VectorMA(velocity, dotScale, trace->normal, ent->s.pos.trDelta);

	if ( trace->normal[2] > 0.7 )
		ent->s.groundEntityNum = trace->entityNum;

	if ( (ent->s.eFlags & 0x1000000) == 0 )
		goto land;

	len2 = VectorLength(velocity);

	if ( len2 > 0.0 && dot <= 0.0 )
	{
		vLenSq = dot / -len2;
		vScale = (weaponDef->perpendicularBounce[surfType] - weaponDef->parallelBounce[surfType]) * vLenSq
		         + weaponDef->parallelBounce[surfType];

		VectorScale(ent->s.pos.trDelta, vScale, ent->s.pos.trDelta);
	}

	if ( trace->normal[2] > 0.7 && VectorLength(ent->s.pos.trDelta) < 20.0 )
	{
		G_SetOrigin(ent, ent->r.currentOrigin);
		MissileLandAngles(ent, trace, angles, 1);
		G_SetAngle(ent, angles);
		return 0;
	}
	else
	{
land:
		VectorScale(trace->normal, 0.1, normal);

		if ( normal[2] > 0.0 )
			normal[2] = 0.0;

		VectorAdd(ent->r.currentOrigin, normal, ent->r.currentOrigin);
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;
		MissileLandAngles(ent, trace, angles, 0);
		VectorCopy(angles, ent->s.apos.trBase);
		ent->s.apos.trTime = level.time;

		if ( contents )
		{
			return 0;
		}
		else
		{
			VectorSubtract(ent->s.pos.trDelta, velocity, velocity);
			vLenSq2 = VectorLength(velocity);
			return vLenSq2 > 100.0;
		}
	}
}

void MissileImpact(gentity_s *ent, trace_t *trace, float *dir, float *endpos)
{
	unsigned int iDir;
	float explosionInnerDamage;
	vec_t *pOririn;
	float explosionOuterDamage;
	int dmg;
	float explosionRadius;
	int event;
	int isModel;
	int meansOfDeath;
	WeaponDef *weaponDef;
	vec3_t vDir;
	int loghit;
	gentity_s *inflictor;

	loghit = 0;
	inflictor = &g_entities[trace->entityNum];
	ent->s.surfType = SURF_TYPEINDEX(trace->surfaceFlags);

	if ( inflictor->takedamage || (ent->s.eFlags & 0x1000000) == 0 )
	{
		weaponDef = BG_GetWeaponDef(ent->s.weapon);
		meansOfDeath = entityHandlers[ent->handler].methodOfDeath;

		if ( inflictor->takedamage )
		{
			if ( !ent->dmg )
			{
				if ( inflictor->client && !trace->surfaceFlags )
					trace->surfaceFlags = 7340032;

				if ( BounceMissile(ent, trace) && !trace->startsolid )
					goto explode;

				return;
			}

			if ( LogAccuracyHit(inflictor, &g_entities[ent->r.ownerNum]) )
				loghit = 1;

			BG_EvaluateTrajectoryDelta(&ent->s.pos, level.time, vDir);

			if ( VectorLength(vDir) == 0.0 )
				vDir[2] = 1.0;

			dmg = ent->dmg;
			pOririn = ent->r.currentOrigin;

			if ( ent->r.ownerNum == 1023 )
				G_Damage(inflictor, ent, 0, vDir, pOririn, dmg, 0, meansOfDeath, HITLOC_NONE, 0);
			else
				G_Damage(
				    inflictor,
				    ent,
				    &g_entities[ent->r.ownerNum],
				    vDir,
				    pOririn,
				    dmg,
				    0,
				    meansOfDeath,
				    HITLOC_NONE,
				    0);
		}

		if ( ent->dmg )
		{
			if ( ent->r.ownerNum == 1023 )
				G_CheckHitTriggerDamage(&g_entities[1022], ent->r.currentOrigin, endpos, ent->dmg, meansOfDeath);
			else
				G_CheckHitTriggerDamage(
				    &g_entities[ent->r.ownerNum],
				    ent->r.currentOrigin,
				    endpos,
				    ent->dmg,
				    meansOfDeath);
		}

		isModel = 0;

		if ( loghit || trace->partName )
			isModel = 1;

		iDir = DirToByte(trace->normal);

		if ( isModel )
			event = EV_ROCKET_EXPLODE_NOMARKS;
		else
			event = EV_ROCKET_EXPLODE;

		G_AddEvent(ent, event, iDir);
		ent->s.surfType = (trace->surfaceFlags & 0x1F00000) >> 20;
		ent->freeAfterEvent = 1;
		ent->s.eType = ET_GENERAL;
		ent->s.eFlags ^= 2u;
		ent->s.eFlags |= 0x20u;
		ent->flags |= 0x800u;
		RoundFloatArray(endpos, ent->s.pos.trBase);
		G_SetOrigin(ent, endpos);

		if ( weaponDef->explosionInnerDamage )
		{
			explosionRadius = (float)weaponDef->explosionRadius;
			explosionOuterDamage = (float)weaponDef->explosionOuterDamage;
			explosionInnerDamage = (float)weaponDef->explosionInnerDamage;
			G_RadiusDamage(
			    endpos,
			    ent,
			    ent->parent,
			    explosionInnerDamage,
			    explosionOuterDamage,
			    explosionRadius,
			    inflictor,
			    entityHandlers[ent->handler].splashMethodOfDeath);
		}

		SV_LinkEntity(ent);
		return;
	}

	if ( BounceMissile(ent, trace) && !trace->startsolid )
explode:
		G_AddEvent(ent, EV_GRENADE_BOUNCE, (trace->surfaceFlags & 0x1F00000) >> 20);
}

void Missile_TraceNoContents(trace_t *results, int entityNum, gentity_s *ent, const float *origin)
{
	int contents;

	contents = g_entities[entityNum].r.contents;
	g_entities[entityNum].r.contents = 0;
	G_MissileTrace(results, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	g_entities[entityNum].r.contents = contents;
}

void RunMissile_Destabilize(gentity_s *missile)
{
	float time;
	float speed;
	WeaponDef *weaponDef;
	float angleMax;
	int i;
	vec3_t newAngleAccel;
	vec3_t newAPos;

	if ( missile->s.pos.trTime + (int)missile->missile.time < level.time )
	{
		weaponDef = BG_GetWeaponDef(missile->s.weapon);
		VectorCopy(missile->s.pos.trDelta, newAPos);
		Vec3Normalize(newAPos);
		angleMax = convertDegreesToTan(weaponDef->destabilizationAngleMax);

		for ( i = 0; i < 3; ++i )
			newAngleAccel[i] = flrand(-1.0, 1.0);

		VectorScale(newAngleAccel, angleMax, newAngleAccel);
		VectorAdd(newAPos, newAngleAccel, newAPos);
		Vec3Normalize(newAPos);
		speed = (float)weaponDef->projectileSpeed;
		VectorScale(newAPos, speed, missile->s.pos.trDelta);
		VectorCopy(missile->r.currentOrigin, missile->s.pos.trBase);
		vectoangles(newAPos, missile->r.currentAngles);
		G_SetAngle(missile, missile->r.currentAngles);
		missile->s.pos.trTime = level.time;

		if ( (missile->flags & 0x10000) != 0 )
			time = missile->missile.time * weaponDef->destabilizationTimeReductionRatio;
		else
			time = weaponDef->destabilizationBaseTime * 1000.0;

		missile->missile.time = time;
		missile->flags |= 0x10000u;
	}
}

void G_RunMissile(gentity_s *ent)
{
	WeaponDef *weaponDef;
	vec3_t endpos;
	int modIndex;
	vec3_t vOldOrigin;
	trace_t traceDown;
	trace_t trace;
	vec3_t vDir;
	vec3_t origin;

	if ( ent->s.pos.trType == TR_STATIONARY && ent->s.groundEntityNum != 1022 )
	{
		VectorCopy(ent->r.currentOrigin, origin);
		origin[2] = origin[2] - 1.5;

		G_MissileTrace(&trace, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);

		if ( trace.fraction == 1.0 )
		{
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
			ent->s.pos.trDuration = 0;
			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorClear(ent->s.pos.trDelta);
		}
	}

	VectorCopy(ent->r.currentOrigin, vOldOrigin);
	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	VectorSubtract(origin, ent->r.currentOrigin, vDir);

	if ( Vec3Normalize(vDir) < 0.001 )
	{
		G_RunThink(ent);
		return;
	}

	if ( I_fabs(ent->s.pos.trDelta[2]) <= 30.0 || SV_PointContents(ent->r.currentOrigin, -1, 32) )
		G_MissileTrace(&trace, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	else
		G_MissileTrace(&trace, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask | 0x20);

	if ( (trace.surfaceFlags & 0x1F00000) == 20971520 )
	{
		RunMissile_CreateWaterSplash(ent, &trace);
		G_MissileTrace(&trace, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	}

	modIndex = entityHandlers[ent->handler].methodOfDeath;

	if ( modIndex == MOD_GRENADE && SLOWORD(g_entities[trace.entityNum].flags) < 0 )
		Missile_TraceNoContents(&trace, trace.entityNum, ent, origin);

	Vec3Lerp(ent->r.currentOrigin, origin, trace.fraction, endpos);
	VectorCopy(endpos, ent->r.currentOrigin);

	if ( (ent->s.eFlags & 0x1000000) != 0
	        && (trace.fraction == 1.0 || trace.fraction < 1.0 && trace.normal[2] > 0.69999999) )
	{
		VectorCopy(ent->r.currentOrigin, origin);
		origin[2] = origin[2] - 1.5;
		G_MissileTrace(&traceDown, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);

		if ( traceDown.fraction != 1.0 && traceDown.entityNum == 1022 )
		{
			trace = traceDown;
			Vec3Lerp(ent->r.currentOrigin, origin, traceDown.fraction, endpos);
			ent->s.pos.trBase[2] = endpos[2] + 1.5 - ent->r.currentOrigin[2] + ent->s.pos.trBase[2];
			VectorCopy(endpos, ent->r.currentOrigin);
			ent->r.currentOrigin[2] = ent->r.currentOrigin[2] + 1.5;
		}
	}

	SV_LinkEntity(ent);
	weaponDef = BG_GetWeaponDef(ent->s.weapon);

	if ( modIndex == MOD_GRENADE )
		G_GrenadeTouchTriggerDamage(ent, vOldOrigin, ent->r.currentOrigin, weaponDef->explosionInnerDamage, 3);

	if ( trace.fraction == 1.0 )
	{
		if ( VectorLength(ent->s.pos.trDelta) != 0.0 )
		{
			ent->s.groundEntityNum = 1023;

			if ( weaponDef->weaponType == 2 && (ent->flags & 0x20000) == 0 )
				RunMissile_Destabilize(ent);
		}

		G_RunThink(ent);
		return;
	}

	if ( (trace.surfaceFlags & 0x10) != 0 )
	{
		G_FreeEntity(ent);
		return;
	}

	MissileImpact(ent, &trace, vDir, endpos);

	if ( ent->s.eType == ET_MISSILE )
	{
		G_RunThink(ent);
		return;
	}
}