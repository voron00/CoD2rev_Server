#include "../qcommon/qcommon.h"
#include "g_shared.h"

const char *modNames[] =
{
	"MOD_UNKNOWN",
	"MOD_PISTOL_BULLET",
	"MOD_RIFLE_BULLET",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_PROJECTILE",
	"MOD_PROJECTILE_SPLASH",
	"MOD_MELEE",
	"MOD_HEAD_SHOT",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
};

const char *g_HitLocNames[] =
{
	"none",
	"helmet",
	"head",
	"neck",
	"torso_upper",
	"torso_lower",
	"right_arm_upper",
	"left_arm_upper",
	"right_arm_lower",
	"left_arm_lower",
	"right_hand",
	"left_hand",
	"right_leg_upper",
	"left_leg_upper",
	"right_leg_lower",
	"left_leg_lower",
	"right_foot",
	"left_foot",
	"gun",
};
#define HITLOC_NUM ARRAY_COUNT(g_HitLocNames)

unsigned short g_HitLocConstNames[HITLOC_NUM];
float g_fHitLocDamageMult[HITLOC_NUM];

/*
===============
G_GetHitLocationIndexFromString
===============
*/
int G_GetHitLocationIndexFromString( unsigned short sString )
{
	for ( int i = 0; i < HITLOC_NUM; i++ )
	{
		if ( g_HitLocConstNames[i] == sString )
		{
			return i;
		}
	}

	return 0;
}

/*
===============
G_GetHitLocationString
===============
*/
unsigned short G_GetHitLocationString( int hitLoc )
{
	assert((unsigned)hitLoc < HITLOC_NUM);
	return g_HitLocConstNames[hitLoc];
}

/*
============
AddScore

Adds score to the client
============
*/
void AddScore( gentity_t *ent, int score )
{
	if ( !ent->client )
	{
		return;
	}

	ent->client->sess.score += score;
	CalculateRanks();
}

/*
===============
G_GetWeaponHitLocationMultiplier
===============
*/
float G_GetWeaponHitLocationMultiplier( int hitLoc, int weapon )
{
	WeaponDef *weapDef;

	assert((hitLoc >= HITLOC_NONE) && (hitLoc < HITLOC_NUM));

	if ( !weapon )
	{
		return g_fHitLocDamageMult[hitLoc];
	}

	weapDef = BG_GetWeaponDef(weapon);

	if ( weapDef && weapDef->weaponType == WEAPTYPE_BULLET )
	{
		return weapDef->locationDamageMultipliers[hitLoc];
	}

	return g_fHitLocDamageMult[hitLoc];
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{
	vec3_t		dir;
	vec3_t		angles;

	if ( attacker && attacker != self )
	{
		VectorSubtract (attacker->r.currentOrigin, self->r.currentOrigin, dir);
	}
	else if ( inflictor && inflictor != self )
	{
		VectorSubtract (inflictor->r.currentOrigin, self->r.currentOrigin, dir);
	}
	else
	{
		self->client->ps.stats[STAT_DEAD_YAW] = self->r.currentAngles[YAW];
		return;
	}

	assert(self->client);
	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

	angles[YAW] = vectoyaw ( dir );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
}

/*
============
G_HitLocStrcpy
============
*/
void G_HitLocStrcpy( unsigned char *member, const char *keyValue )
{
	strcpy((char *)member, keyValue);
}

/*
==================
G_ParseHitLocDmgTable
==================
*/
void G_ParseHitLocDmgTable()
{
#define FILENAME "info/mp_lochit_dmgtable"
#define IDENT "LOCDMGTABLE"
	fileHandle_t f;
	int len;
	char loadBuffer[8192];
	cspField_t pFieldList[HITLOC_NUM];
	int n;
	int i;

	n = strlen("LOCDMGTABLE");

	for ( i = 0; i < ARRAY_COUNT(pFieldList); i++ )
	{
		g_fHitLocDamageMult[i] = 1.0f;

		pFieldList[i].szName = g_HitLocNames[i];
		pFieldList[i].iOffset = i * sizeof( intptr_t );
		pFieldList[i].iFieldType = CSPFT_FLOAT;

		g_HitLocConstNames[i] = Scr_AllocString(g_HitLocNames[i]);
	}

	len = FS_FOpenFileByMode(FILENAME, &f, FS_READ);

	if ( len <= 0 )
	{
		Com_Error(ERR_DROP, "Could not load hitloc damage table %s", FILENAME);
	}

	FS_Read(loadBuffer, n, f);
	loadBuffer[n] = 0;

	if ( strncmp(loadBuffer, IDENT, n) )
	{
		Com_Error(ERR_DROP, "\"%s\" does not appear to be a hitloc damage table", FILENAME);
	}

	if ( len - n >= sizeof(loadBuffer) )
	{
		Com_Error(ERR_DROP, "\"%s\" Is too long of a hitloc damage table to parse", FILENAME);
	}

	FS_Read(loadBuffer, len - n, f);
	loadBuffer[len - n] = 0;
	FS_FCloseFile(f);

	if ( !Info_Validate(loadBuffer) )
	{
		Com_Error(ERR_DROP, "\"%s\" is not a valid hitloc damage table", FILENAME);
	}

	if ( !ParseConfigStringToStruct((unsigned char *)g_fHitLocDamageMult, pFieldList, ARRAY_COUNT(pFieldList), loadBuffer, 0, NULL, G_HitLocStrcpy) )
	{
		Com_Error(ERR_DROP, "Error parsing hitloc damage table %s", FILENAME);
	}
}

/*
==================
G_IndexForMeansOfDeath
==================
*/
int G_IndexForMeansOfDeath( const char *name )
{
	for ( int i = 0; i < ARRAY_COUNT(modNames); i++ )
	{
		if ( !I_stricmp(name, modNames[i]) )
		{
			return i;
		}
	}

	Com_Printf("Unknown means of death string '%s'\n", name);
	return MOD_UNKNOWN;
}

/*
==================
G_DamageClient
==================
*/
void G_DamageClient( gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                     const vec3_t vDir, const vec3_t vPoint, int damage, int dflags,
                     int meansOfDeath, int hitLoc, int timeOffset )
{
	int weapon;

	if ( !self->takedamage )
	{
		return;
	}

	if ( self->client->noclip )
	{
		return;
	}

	if ( self->client->ufo )
	{
		return;
	}

	if ( self->client->sess.connected != CON_CONNECTED )
	{
		return;
	}

	if ( inflictor )
	{
		weapon = inflictor->s.weapon;
	}
	else if ( attacker )
	{
		weapon = attacker->s.weapon;
	}
	else
	{
		weapon = WP_NONE;
	}

	assert((hitLoc >= HITLOC_NONE) && (hitLoc < HITLOC_NUM));
	damage *= G_GetWeaponHitLocationMultiplier(hitLoc, weapon);

	if ( damage <= 0 )
	{
		return;
	}

	Scr_PlayerDamage(self, inflictor, attacker, damage, dflags, meansOfDeath, weapon, vPoint, vDir, hitLoc, timeOffset);
}

/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker,
                 int damage, int meansOfDeath, int iWeapon, const vec3_t vDir,
                 int hitLoc, int psTimeOffset )
{
	if ( !Com_GetServerDObj(self->client->ps.clientNum) )
	{
		return;
	}

	if ( self->client->ps.pm_type >= PM_DEAD )
	{
		return;
	}

	if ( self->client->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	if ( attacker->s.eType == ET_TURRET && attacker->r.ownerNum != ENTITYNUM_NONE )
	{
		attacker = &g_entities[attacker->r.ownerNum];
	}

	Scr_AddEntity(attacker);
	Scr_Notify(self, scr_const.death, 1);

	if ( iWeapon && attacker->client != NULL && attacker->client->ps.eFlags & EF_TURRET_ACTIVE )
	{
		assert(self->client->ps.viewlocked_entNum != ENTITYNUM_NONE);
		gentity_t *turret = &g_entities[attacker->s.otherEntityNum];

		if ( turret->s.eType == ET_TURRET )
		{
			iWeapon = turret->s.weapon;
		}
	}

	// JPW NERVE -- if player is holding ticking grenade, drop it
	if ( self->client->ps.grenadeTimeLeft )
	{
		vec3_t launchvel, launchspot;

		launchvel[0] = G_crandom();
		launchvel[1] = G_crandom();
		launchvel[2] = G_random();
		VectorScale( launchvel, 160, launchvel );
		VectorCopy( self->r.currentOrigin, launchspot );
		launchspot[2] += 40;

#ifdef LIBCOD
		if ( g_dropGrenadeOnDeath->current.boolean )
#endif
		{
			// Gordon: fixes premature grenade explosion, ta bani ;)
			gentity_t *m = fire_grenade( self, launchspot, launchvel, self->client->ps.offHandIndex, self->client->ps.grenadeTimeLeft );
			//m->damage = 0;
		}
	}

	if ( self->client->ps.pm_type == PM_NORMAL_LINKED )
		self->client->ps.pm_type = PM_DEAD_LINKED;
	else
		self->client->ps.pm_type = PM_DEAD;

	int deathAnimDuration = BG_AnimScriptEvent(&self->client->ps, ANIM_ET_DEATH, qfalse, qtrue);
	Scr_PlayerKilled(self, inflictor, attacker, damage, meansOfDeath, iWeapon, vDir, hitLoc, psTimeOffset, deathAnimDuration);

	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for ( int i = 0; i < level.maxclients; i++ )
	{
		gclient_t *client = &level.clients[i];

		if ( client->sess.connected != CON_CONNECTED )
		{
			continue;
		}
		if ( client->sess.sessionState != SESS_STATE_SPECTATOR )
		{
			continue;
		}

		if ( client->spectatorClient == self->s.number )
		{
			Cmd_Score_f( &g_entities[i] );
		}
	}

	self->takedamage = qtrue; // can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	self->r.currentAngles[ROLL] = 0;

	LookAtKiller(self, inflictor, attacker);
	VectorCopy(self->r.currentAngles, self->client->ps.viewangles);

	self->s.loopSound = 0;

	SV_UnlinkEntity(self);

	self->r.maxs[2] = 30;

	SV_LinkEntity(self);

	self->health = 0;
	self->handler = ENT_HANDLER_CLIENT_DEAD;
}

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
#define MASK_CAN_DAMAGE ( CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_MISSILECLIP | CONTENTS_SKY | CONTENTS_CLIPSHOT | CONTENTS_UNKNOWN )
float CanDamage( gentity_t *targ, const vec3_t centerPos )
{
	int hits;
	float halfHeight;
	vec3_t eyeOrigin;
	vec3_t right;
	vec3_t forward;
	int i;
	vec3_t dest[5];

	if ( targ->client )
	{
		G_GetPlayerViewOrigin(targ, eyeOrigin);

		halfHeight = (eyeOrigin[2] - targ->r.currentOrigin[2]) * 0.5;
		VectorSubtract(centerPos, targ->r.currentOrigin, forward);
		forward[2] = 0;

		Vec3Normalize(forward);

		right[0] = -forward[1];
		right[1] = forward[0];
		right[2] = forward[2];

		VectorAdd(eyeOrigin, targ->r.currentOrigin, dest[0]);

		VectorScale(dest[0], 0.5, dest[0]);
		VectorMA(dest[0], 15, right, dest[1]);

		dest[1][2] += halfHeight;
		VectorMA(dest[0], 15, right, dest[2]);

		dest[2][2] -= halfHeight;
		VectorMA(dest[0], -15, right, dest[3]);

		dest[3][2] += halfHeight;
		VectorMA(dest[0], -15, right, dest[4]);

		dest[4][2] -= halfHeight;

		for ( i = 0, hits = 0; i < 5; ++i )
		{
			if ( G_LocationalTracePassed(centerPos, dest[i], targ->s.number, MASK_CAN_DAMAGE) )
			{
				hits++;
			}
		}

		if ( !hits )
		{
			return 0;
		}

		if ( hits > 3 )
		{
			return 1.0f;
		}

		return hits / 3.0f;
	}

	// this should probably check in the plane of projection,
	// rather than in world coordinate
	VectorAdd(targ->r.absmin, targ->r.absmax, dest[0]);
	VectorScale(dest[0], 0.5, dest[0]);

	VectorCopy(dest[0], dest[1]);
	dest[1][0] += 15;
	dest[1][1] += 15;

	VectorCopy(dest[0], dest[2]);
	dest[2][0] += 15;
	dest[2][1] -= 15;

	VectorCopy(dest[0], dest[3]);
	dest[3][0] -= 15;
	dest[3][1] += 15;

	VectorCopy(dest[0], dest[4]);
	dest[4][0] -= 15;
	dest[4][1] -= 15;

	for ( i = 0; i < 5; i++ )
	{
		if ( G_LocationalTracePassed(dest[i], centerPos, targ->s.number, MASK_CAN_DAMAGE) )
		{
			return 1.0f;
		}
	}

	return 0;
}

/*
============
G_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, const vec3_t dir, const vec3_t point, int damage, int dflags, int mod, int hitLoc, int timeOffset )
{
	void (*pain)(gentity_t *, gentity_t *, int, const float *, const int, const float *, int);
	void (*die)(gentity_t *, gentity_t *, gentity_t *, int, int, const int, const float *, int, int);
	vec3_t localdir;

	if ( targ->client )
	{
		G_DamageClient(targ, inflictor, attacker, dir, point, damage, dflags, mod, hitLoc, timeOffset);
		return;
	}

	if ( !targ->takedamage )
	{
		return;
	}

	if ( !inflictor )
	{
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker )
	{
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

	Vec3NormalizeTo(dir, localdir);

	// check for godmode
	if ( targ->flags & FL_GODMODE )
	{
		return;
	}

	if ( damage < 1 )
	{
		damage = 1;
	}

	if ( targ->flags & FL_DEMI_GODMODE && targ->health - damage < 1 )
	{
		damage = targ->health - 1;
	}

	if ( g_debugDamage->current.boolean )
	{
		Com_Printf("target:%i health:%i damage:%i\n", targ->s.number, targ->health, damage);
	}

	// do the damage
	targ->health -= damage;

	Scr_AddEntity(attacker);
	Scr_AddInt(damage);
	Scr_Notify(targ, scr_const.damage, 2);

	if ( targ->health <= 0 )
	{
		if ( targ->health < -999 )
		{
			targ->health = -999;
		}

		Scr_AddEntity(attacker);
		Scr_Notify(targ, scr_const.death, 1);

		die = entityHandlers[targ->handler].die;

		if ( die )
		{
			die(targ, inflictor, attacker, damage, mod, inflictor->s.weapon, localdir, hitLoc, timeOffset);
		}
	}
	else
	{
		pain = entityHandlers[targ->handler].pain;

		if ( pain )
		{
			pain(targ, attacker, damage, point, mod, localdir, hitLoc);
		}
	}
}

/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( const vec3_t origin, gentity_t *inflictor, gentity_t *attacker, float fInnerDamage, float fOuterDamage, float radius, gentity_t *ignore, int mod )
{
	int entityList[MAX_GENTITIES];
	int numListedEntities;
	gentity_t *ent;
	qboolean hitClient = qfalse;
	vec3_t v;
	vec3_t dir;
	vec3_t mins, maxs;
	vec3_t midpoint;
	vec3_t dest;
	trace_t tr;
	float boxradius;
	float dist;
	float points;
	float damageAmount;
	int i, e;

	if ( !attacker )
	{
		return qfalse;
	}
	if ( radius < 1 )
	{
		radius = 1;
	}

	boxradius = 1.4142135 * radius; // radius * sqrt(2) for bounding box enlargement --
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for ( i = 0 ; i < 3 ; i++ )
	{
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = CM_AreaEntities( mins, maxs, entityList, MAX_GENTITIES, -1 );

	for ( e = 0 ; e < numListedEntities ; e++ )
	{
		ent = &g_entities[entityList[ e ]];

		if ( ent == ignore )
		{
			continue;
		}

		if ( !ent->takedamage )
		{
			continue;
		}

		if ( ent->r.bmodel )
		{
			for ( i = 0; i < 3; i++ )
			{
				if ( ent->r.absmin[i] > origin[i] )
				{
					v[i] = ent->r.absmin[i] - origin[i];
				}
				else if ( origin[i] > ent->r.absmax[i] )
				{
					v[i] = origin[i] - ent->r.absmax[i];
				}
				else
				{
					v[i] = 0;
				}
			}
		}
		else
		{
			VectorSubtract(ent->r.currentOrigin, origin, v);
		}

		dist = VectorLength( v );
		if ( dist >= radius )
		{
			continue;
		}
		if ( ent->client && level.bPlayerIgnoreRadiusDamage )
		{
			continue;
		}

		points = (fInnerDamage - fOuterDamage) * (1.0 - dist / radius) + fOuterDamage;
		damageAmount = CanDamage( ent, origin );

		if ( damageAmount > 0 )
		{
			if ( LogAccuracyHit( ent, attacker ) )
			{
				hitClient = qtrue;
			}
			VectorSubtract( ent->r.currentOrigin, origin, dir );
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;

			G_Damage( ent, inflictor, attacker, dir, origin, (int)(points * damageAmount), DAMAGE_RADIUS, mod, HITLOC_NONE, 0 );
		}
		else
		{
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );

			G_TraceCapsule(&tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, CONTENTS_SOLID | CONTENTS_GLASS | CONTENTS_SKY);
			if ( tr.fraction < 1.0 )
			{
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if ( dist < radius * 0.2f )   // closer than 1/4 dist
				{
					if ( LogAccuracyHit( ent, attacker ) )
					{
						hitClient = qtrue;
					}
					VectorSubtract( ent->r.currentOrigin, origin, dir );
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int)( points * 0.1f ), DAMAGE_RADIUS, mod, HITLOC_NONE, 0 );
				}
			}
		}
	}

	return hitClient;
}
