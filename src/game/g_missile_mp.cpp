#include "../qcommon/qcommon.h"
#include "g_shared.h"

/*
=================
fire_rocket
=================
*/
gentity_t *fire_rocket( gentity_t *self, vec3_t start, vec3_t dir )
{
	gentity_t *bolt;
	WeaponDef *weapDef;

	Vec3Normalize( dir );
	weapDef = BG_GetWeaponDef( self->s.weapon );

	bolt = G_Spawn();
	Scr_SetString(&bolt->classname, scr_const.rocket);
	bolt->nextthink = level.time + 30000;    // push it out a little
	bolt->handler = ENT_HANDLER_ROCKET;
	bolt->s.eType = ET_MISSILE;
	bolt->s.eFlags |= EF_PROJECTILE;
	bolt->r.svFlags = SVF_BROADCAST;

	//DHM - Nerve :: Use the correct weapon in multiplayer
	bolt->s.weapon = self->s.weapon;

	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->dmg = weapDef->damage; // JPW NERVE
	bolt->clipmask = MASK_MISSILESHOT & ~CONTENTS_WATER;

	bolt->s.time = level.time + MISSILE_PRESTEP_TIME;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;      // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, weapDef->projectileSpeed, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta);          // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	vectoangles(dir, bolt->r.currentAngles);
	G_SetAngle(bolt, bolt->r.currentAngles);

	bolt->missile.time = (float)weapDef->destabilizeDistance / (float)weapDef->projectileSpeed * 1000.0;
	bolt->flags |= self->flags & 0x20000;

	return bolt;
}

/*
=================
fire_grenade

	NOTE!!!! NOTE!!!!!

	This accepts a /non-normalized/ direction vector to allow specification
	of how hard it's thrown.  Please scale the vector before calling.

=================
*/
gentity_t *fire_grenade( gentity_t *self, vec3_t start, vec3_t dir, int grenadeWPID, int iTime )
{
	gentity_t *bolt;
	WeaponDef *weapDef;

	bolt = G_Spawn();

	// no self->client for shooter_grenade's
	if ( self->client && self->client->ps.grenadeTimeLeft )
	{
		bolt->nextthink = level.time + self->client->ps.grenadeTimeLeft;
		self->client->ps.grenadeTimeLeft = 0;
	}
	else
	{
		bolt->nextthink = level.time + iTime;
	}

	if ( self->client )
	{
		self->client->ps.grenadeTimeLeft = 0;
	}

	bolt->handler = ENT_HANDLER_GRENADE;
	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_BROADCAST;
	bolt->s.weapon = grenadeWPID;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;

	weapDef = BG_GetWeaponDef(grenadeWPID);
	Scr_SetString(&bolt->classname, scr_const.grenade);

	bolt->dmg = weapDef->damage;
	bolt->s.eFlags = EF_BOUNCE;
	bolt->clipmask = MASK_MISSILESHOT & ~CONTENTS_WATER;

	bolt->s.time = level.time + MISSILE_PRESTEP_TIME;     // move a bit on the very first frame
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time;

	VectorCopy(start, bolt->s.pos.trBase);
	VectorCopy(dir, bolt->s.pos.trDelta);

	SnapVector(bolt->s.pos.trDelta);          // save net bandwidth

	bolt->s.apos.trType = TR_LINEAR;
	bolt->s.apos.trTime = level.time;
	vectoangles(dir, bolt->s.apos.trBase);

	bolt->s.apos.trBase[0] = AngleNormalize360(bolt->s.apos.trBase[0] - 120);

	bolt->s.apos.trDelta[0] = flrand(-45, 45) + 720;
	bolt->s.apos.trDelta[1] = 0;
	bolt->s.apos.trDelta[2] = flrand(-45, 45) + 360;

	VectorCopy(start, bolt->r.currentOrigin);
	VectorCopy(bolt->s.apos.trBase, bolt->r.currentAngles);

#ifdef LIBCOD
	extern int codecallback_fire_grenade;
	if (codecallback_fire_grenade)
	{
		stackPushString(weapDef->szInternalName);
		stackPushEntity(bolt);
		short ret = Scr_ExecEntThread(self, codecallback_fire_grenade, 2);
		Scr_FreeThread(ret);
	}
#endif

	return bolt;
}

/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent )
{
	WeaponDef *weapDef;
	trace_t trace;
	vec3_t origin;

	assert(ent);
	assert(ent->s.weapon);

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);

	if ( weapDef->offhandClass == OFFHAND_CLASS_SMOKE_GRENADE && ent->s.groundEntityNum == ENTITYNUM_NONE )
	{
		ent->nextthink = 50;
		return;
	}

	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	SnapVector(origin);
	G_SetOrigin(ent, origin);

	ent->s.eType = ET_GENERAL;
	ent->s.eFlags |= EF_NODRAW;
	ent->flags |= FL_NODRAW;
	ent->r.svFlags |= SVF_BROADCAST;

	VectorCopy(ent->r.currentOrigin, origin);

	//bani - #560
	origin[2] -= 16;

	G_TraceCapsule(&trace, ent->r.currentOrigin, vec3_origin, vec3_origin, origin, ent->s.number, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY);

	if ( weapDef->projExplosionType == WEAPPROJEXP_NONE )
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

	if ( weapDef->projExplosionEffect && weapDef->projExplosionEffect[0] )
	{
		ent->s.eFlags |= EF_UNKNOWN;
		//Server_SwitchToValidFxScheduler();
		//fx = FX_RegisterEffect(weapDef->projExplosionEffect);
		ent->s.time = level.time;
		//ent->s.time2 = level.time + (int)(FX_GetEffectLength(fx) + 1.0);
		//VoroN: HAX
		if ( weapDef->offhandClass == OFFHAND_CLASS_SMOKE_GRENADE )
			ent->s.time2 = level.time + 60000;
		else
			ent->s.time2 = level.time + 50;
	}
	else
	{
		ent->freeAfterEvent = qtrue;
	}

	if ( weapDef->explosionInnerDamage )
	{
		G_RadiusDamage(ent->r.currentOrigin, ent, ent->parent, weapDef->explosionInnerDamage, weapDef->explosionOuterDamage, weapDef->explosionRadius, ent, entityHandlers[ent->handler].splashMethodOfDeath);
	}

	SV_LinkEntity(ent);
}

/*
================
G_RunMissile
================
*/
void G_RunMissile( gentity_t *ent )
{
	WeaponDef *weapDef;
	vec3_t endpos;
	int mod;
	vec3_t vOldOrigin;
	trace_t trDown;
	trace_t tr;
	vec3_t dir;
	vec3_t origin;

	assert(ent);

	if ( ent->s.pos.trType == TR_STATIONARY && ent->s.groundEntityNum != ENTITYNUM_WORLD )
	{
		VectorCopy(ent->r.currentOrigin, origin);
		origin[2] -= 1.5;
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);

		if ( tr.fraction == 1.0 )
		{
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
			ent->s.pos.trDuration = 0;

			VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
			VectorClear(ent->s.pos.trDelta);
		}
	}

	VectorCopy(ent->r.currentOrigin, vOldOrigin);

	// get current position
	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	VectorSubtract(origin, ent->r.currentOrigin, dir);

	if ( Vec3Normalize(dir) < TRACE_EPSILON )
	{
		G_RunThink(ent);
		return;
	}

	// trace a line from the previous position to the current position,
	// ignoring interactions with the missile owner
	if ( I_fabs(ent->s.pos.trDelta[2]) <= 30 || SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER) )
	{
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	}
	else
	{
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask | CONTENTS_WATER);
	}

	if ( SURF_TYPEINDEX( tr.surfaceFlags ) == SURF_TYPE_WATER )
	{
		G_RunMissile_CreateWaterSplash(ent, &tr);
		G_MissileTrace(&tr, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	}

	mod = entityHandlers[ent->handler].methodOfDeath;

	if ( mod == MOD_GRENADE && g_entities[tr.entityNum].flags & FL_GRENADE_TOUCH_DAMAGE )
	{
		G_MissileTrace_IgnoreEntity(&tr, tr.entityNum, ent, origin);
	}

	Vec3Lerp(ent->r.currentOrigin, origin, tr.fraction, endpos);
	VectorCopy(endpos, ent->r.currentOrigin);

	if ( ent->s.eFlags & EF_BOUNCE )
	{
		if ( tr.fraction == 1.0 || tr.fraction < 1.0 && tr.normal[2] > MIN_WALK_NORMAL )
		{
			VectorCopy(ent->r.currentOrigin, origin);
			origin[2] -= 1.5;

			G_MissileTrace(&trDown, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);

			if ( trDown.fraction != 1.0 && trDown.entityNum == ENTITYNUM_WORLD )
			{
				tr = trDown;
				Vec3Lerp(ent->r.currentOrigin, origin, trDown.fraction, endpos);

				ent->s.pos.trBase[2] = endpos[2] + 1.5 - ent->r.currentOrigin[2] + ent->s.pos.trBase[2];

				VectorCopy(endpos, ent->r.currentOrigin);
				ent->r.currentOrigin[2] = ent->r.currentOrigin[2] + 1.5;
			}
		}
	}

	SV_LinkEntity(ent);

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);

	if ( mod == MOD_GRENADE )
	{
		G_GrenadeTouchTriggerDamage(ent, vOldOrigin, ent->r.currentOrigin, weapDef->explosionInnerDamage, MOD_GRENADE);
	}

	if ( tr.fraction == 1.0 )
	{
		if ( VectorLength(ent->s.pos.trDelta) != 0 )
		{
			ent->s.groundEntityNum = ENTITYNUM_NONE;

			if ( weapDef->weaponType == WEAPTYPE_PROJECTILE && !(ent->flags & FL_STABLE_MISSILES) )
			{
				G_RunMissile_Destabilize(ent);
			}
		}

		G_RunThink(ent);
		return;
	}

	if ( tr.surfaceFlags & SURF_NOIMPACT )
	{
		G_FreeEntity(ent);
		return;
	}

	G_MissileImpact(ent, &tr, dir, endpos);

	if ( ent->s.eType == ET_MISSILE )
	{
		G_RunThink(ent);
	}
}

/*
================
G_RunMissile_GetPerturbation
================
*/
float G_RunMissile_GetPerturbation( float destabilizationCurvatureMax )
{
	assert(destabilizationCurvatureMax < 1000000000.0f && destabilizationCurvatureMax >= 0.0f);
	return tan(destabilizationCurvatureMax * DEG_TO_RAD);
}

/*
================
G_RunMissile_Destabilize
================
*/
void G_RunMissile_Destabilize( gentity_t *ent )
{
	WeaponDef *weapDef;
	float perturbationMax;
	int i;
	vec3_t newAngleAccel;
	vec3_t newAPos;

	assert(ent);
	assert(ent->s.eType == ET_MISSILE);
	assert(!(ent->flags & FL_STABLE_MISSILES));

	if ( ent->s.pos.trTime + ent->missile.time >= level.time )
	{
		return;
	}

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);

	VectorCopy(ent->s.pos.trDelta, newAPos);
	Vec3Normalize(newAPos);

	perturbationMax = G_RunMissile_GetPerturbation(weapDef->destabilizationAngleMax);

	for ( i = 0; i < 3; i++ )
	{
		newAngleAccel[i] = flrand(-1.0, 1.0);
	}

	VectorScale(newAngleAccel, perturbationMax, newAngleAccel);
	VectorAdd(newAPos, newAngleAccel, newAPos);

	Vec3Normalize(newAPos);

	VectorScale(newAPos, weapDef->projectileSpeed, ent->s.pos.trDelta);
	VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);

	vectoangles(newAPos, ent->r.currentAngles);
	G_SetAngle(ent, ent->r.currentAngles);

	ent->s.pos.trTime = level.time;

	if ( ent->flags & FL_MISSILE_DESTABILIZED )
		ent->missile.time *= weapDef->destabilizationTimeReductionRatio;
	else
		ent->missile.time = weapDef->destabilizationBaseTime * 1000.0;

	ent->flags |= FL_MISSILE_DESTABILIZED;
}

/*
================
G_MissileLandAngles
================
*/
void G_MissileLandAngles( gentity_t *ent, trace_t *trace, vec3_t vAngles, qboolean bForceAlign )
{
	float fAbsAngDelta;
	float fAngleDelta;
	float fSurfacePitch;
	int hitTime;

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
	BG_EvaluateTrajectory(&ent->s.apos, hitTime, vAngles);

	if ( trace->normal[2] <= 0.1 )
	{
		if ( !bForceAlign )
		{
			ent->s.apos.trDelta[0] = AngleNormalize360((float)((rand() & 0x7F) - 63) + ent->s.apos.trDelta[0]);
		}

		return;
	}

	fSurfacePitch = PitchForYawOnNormal(vAngles[1], trace->normal);
	fAngleDelta = AngleSubtract(fSurfacePitch, vAngles[0]);
	fAbsAngDelta = I_fabs(fAngleDelta);

	if ( !bForceAlign )
	{
		VectorCopy(vAngles, ent->s.apos.trBase);
		ent->s.apos.trTime = hitTime;

		if ( fAbsAngDelta >= 80.0 )
		{
			ent->s.apos.trDelta[0] = (randomf() * 0.3f + 0.85f) * ent->s.apos.trDelta[0];
		}
		else
		{
			ent->s.apos.trDelta[0] = (randomf() * 0.3f + 0.85f) * ent->s.apos.trDelta[0] * -1.0;
		}
	}

	vAngles[0] = AngleNormalize180(vAngles[0]);

	if ( bForceAlign || fAbsAngDelta < 45.0 )
	{
		if ( I_fabs(vAngles[0]) <= 90.0 )
		{
			vAngles[0] = AngleNormalize360(fSurfacePitch);
		}
		else
		{
			vAngles[0] = AngleNormalize360(fSurfacePitch + 180.0);
		}
	}
	else
	{
		if ( fAbsAngDelta >= 80.0 )
		{
			vAngles[0] = AngleNormalize360(vAngles[0]);
		}
		else
		{
			vAngles[0] = AngleNormalize360(fAngleDelta * 0.25 + vAngles[0]);
		}
	}
}

/*
================
G_BounceMissile
================
*/
qboolean G_BounceMissile( gentity_t *ent, trace_t *trace )
{
	int surfType;
	int contents;
	float dot;
	int hitTime;
	vec3_t vAngles;
	vec3_t vDelta;
	vec3_t velocity;
	WeaponDef *weapDef;

	assert(ent->s.weapon);

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);

	contents = SV_PointContents(ent->r.currentOrigin, -1, CONTENTS_WATER);
	surfType = SURF_TYPEINDEX(trace->surfaceFlags);

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
	BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
	dot = DotProduct( velocity, trace->normal );
	VectorMA( velocity, -2 * dot, trace->normal, ent->s.pos.trDelta );
	assert(!IS_NAN((ent->s.pos.trDelta)[0]) && !IS_NAN((ent->s.pos.trDelta)[1]) && !IS_NAN((ent->s.pos.trDelta)[2]));

	// RF, record this for mover pushing
	if ( trace->normal[2] > 0.7 /*&& VectorLengthSquared( ent->s.pos.trDelta ) < SQR(40)*/ )
	{
		ent->s.groundEntityNum = trace->entityNum;
	}

	if ( ent->s.eFlags & EF_BOUNCE ) // both flags marked, do a third type of bounce
	{
		if ( VectorLength(velocity) > 0 && dot <= 0 )
		{
			VectorScale(ent->s.pos.trDelta, (weapDef->perpendicularBounce[surfType] - weapDef->parallelBounce[surfType]) * (dot / -VectorLength(velocity)) + weapDef->parallelBounce[surfType], ent->s.pos.trDelta);
		}

		// check for stop
		//%	if ( trace->plane.normal[2] > 0.2 && VectorLengthSquared( ent->s.pos.trDelta ) < SQR(40) )
		if ( trace->normal[2] > 0.7 && VectorLength(ent->s.pos.trDelta) < 20 )
		{
			G_SetOrigin(ent, ent->r.currentOrigin);
			G_MissileLandAngles(ent, trace, vAngles, qtrue); // final rotation value
			G_SetAngle(ent, vAngles);
			return qfalse;
		}
	}

	VectorScale(trace->normal, 0.1, vDelta);

	if ( vDelta[2] > 0 )
	{
		vDelta[2] = 0;
	}

	VectorAdd(ent->r.currentOrigin, vDelta, ent->r.currentOrigin);
	VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
	assert(!IS_NAN((ent->r.currentOrigin)[0]) && !IS_NAN((ent->r.currentOrigin)[1]) && !IS_NAN((ent->r.currentOrigin)[2]));

	ent->s.pos.trTime = level.time;

	G_MissileLandAngles(ent, trace, vAngles, qfalse);
	VectorCopy(vAngles, ent->s.apos.trBase);

	ent->s.apos.trTime = level.time;

	if ( contents )
	{
		return qfalse;
	}

	VectorSubtract(ent->s.pos.trDelta, velocity, velocity);
	assert(!IS_NAN((ent->s.pos.trDelta)[0]) && !IS_NAN((ent->s.pos.trDelta)[1]) && !IS_NAN((ent->s.pos.trDelta)[2]));

	return VectorLength(velocity) > 100;
}

/*
================
G_RunMissile_CreateWaterSplash
================
*/
void G_RunMissile_CreateWaterSplash( gentity_t *ent, trace_t *trace )
{
	gentity_t *tent;
	vec3_t reflect;

	assert(ent);
	assert(trace);

	Vec3NormalizeTo(ent->s.pos.trDelta, reflect);

	if ( reflect[2] < 0 )
	{
		reflect[2] *= -1.0;
	}

	tent = G_TempEntity(ent->r.currentOrigin, EV_BULLET_HIT_LARGE);

	tent->s.eventParm = DirToByte(trace->normal);
	tent->s.eventParm2 = DirToByte(reflect);
	tent->s.surfType = SURF_TYPEINDEX(trace->surfaceFlags);
	tent->s.otherEntityNum = ent->s.number;
}

/*
================
G_MissileTrace
================
*/
void G_MissileTrace( trace_t *results, const vec3_t start, const vec3_t end, int passEntityNum, int contentmask )
{
	vec3_t dir;

	G_LocationalTrace(results, start, end, passEntityNum, contentmask, bulletPriorityMap);

	if ( !results->startsolid )
	{
		return;
	}

	results->fraction = 0;

	VectorSubtract(start, end, dir);
	Vec3NormalizeTo(dir, results->normal);
}

/*
================
G_MissileTrace_IgnoreEntity
================
*/
void G_MissileTrace_IgnoreEntity( trace_t *results, int hitId, gentity_t *ent, const vec3_t origin )
{
	int prevContents = g_entities[hitId].r.contents;
	g_entities[hitId].r.contents = 0;

	G_MissileTrace(results, ent->r.currentOrigin, origin, ent->r.ownerNum, ent->clipmask);
	g_entities[hitId].r.contents = prevContents;
}

/*
================
G_MissileImpact
	impactDamage is how much damage the impact will do to func_explosives
================
*/
void G_MissileImpact( gentity_t *ent, trace_t *trace, vec3_t dir, vec3_t endpos )
{
	gentity_t *other;
	int mod;
	vec3_t velocity;
	WeaponDef *weapDef;
	qboolean hitClient = qfalse;

	assert(ent);
	assert(ent->s.weapon);

	other = &g_entities[trace->entityNum];
	ent->s.surfType = SURF_TYPEINDEX(trace->surfaceFlags);

	// check for bounce
	if ( ( !other->takedamage ) && ( ent->s.eFlags & ( EF_BOUNCE ) ) )
	{
		if ( G_BounceMissile(ent, trace) && !trace->startsolid )
		{
			G_AddEvent(ent, EV_GRENADE_BOUNCE, SURF_TYPEINDEX(trace->surfaceFlags));
		}
		return;
	}

	weapDef = BG_GetWeaponDef(ent->s.weapon);
	assert(weapDef);
	mod = entityHandlers[ent->handler].methodOfDeath;

	// impact damage
	if ( other->takedamage )
	{
		if ( ent->dmg )
		{
			if ( LogAccuracyHit( other, &g_entities[ent->r.ownerNum] ) )
			{
				hitClient = qtrue;
			}
			BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
			if ( !VectorLength( velocity ) )
			{
				velocity[2] = 1;    // stepped on a grenade
			}
			if ( ent->r.ownerNum == ENTITYNUM_NONE )
			{
				G_Damage(other, ent, NULL, velocity, ent->r.currentOrigin, ent->dmg, 0, mod, HITLOC_NONE, 0);
			}
			else
			{
				G_Damage(other, ent, &g_entities[ent->r.ownerNum], velocity, ent->r.currentOrigin, ent->dmg, 0, mod, HITLOC_NONE, 0);
			}
		}
		else // if no damage value, then this is a splash damage grenade only
		{
			if ( other->client && !trace->surfaceFlags )
			{
				trace->surfaceFlags = SURF_FLESH;
			}
			if ( G_BounceMissile(ent, trace) && !trace->startsolid )
			{
				G_AddEvent(ent, EV_GRENADE_BOUNCE, SURF_TYPEINDEX(trace->surfaceFlags));
			}
			return;
		}
	}

	// damage triggers
	if ( ent->dmg )
	{
		if ( ent->r.ownerNum == ENTITYNUM_NONE )
		{
			G_CheckHitTriggerDamage(&g_entities[ENTITYNUM_WORLD], ent->r.currentOrigin, endpos, ent->dmg, mod);
		}
		else
		{
			G_CheckHitTriggerDamage(&g_entities[ent->r.ownerNum], ent->r.currentOrigin, endpos, ent->dmg, mod);
		}
	}

	// is it cheaper in bandwidth to just remove this ent and create a new
	// one, rather than changing the missile into the explosion?

	if ( hitClient || trace->partName )
		G_AddEvent(ent, EV_ROCKET_EXPLODE_NOMARKS, DirToByte(trace->normal));
	else
		G_AddEvent(ent, EV_ROCKET_EXPLODE, DirToByte(trace->normal));

	ent->s.surfType = SURF_TYPEINDEX(trace->surfaceFlags);
	ent->freeAfterEvent = qtrue;
	ent->s.eType = ET_GENERAL;
	ent->s.eFlags ^= EF_TELEPORT_BIT;
	ent->s.eFlags |= EF_NODRAW;
	ent->flags |= FL_NODRAW;

	RoundFloatArray(endpos, ent->s.pos.trBase);
	G_SetOrigin(ent, endpos);

	// splash damage (doesn't apply to person directly hit)
	if ( weapDef->explosionInnerDamage )
	{
		G_RadiusDamage(endpos, ent, ent->parent, weapDef->explosionInnerDamage, weapDef->explosionOuterDamage, weapDef->explosionRadius, other, entityHandlers[ent->handler].splashMethodOfDeath);
	}

	SV_LinkEntity(ent);
}
