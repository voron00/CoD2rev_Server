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

#ifdef TESTING_LIBRARY
#define entityHandlers ((entityHandler_t*)( 0x08167880 ))
#else
const entityHandler_t entityHandlers[] =
{

};
#endif

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

unsigned short g_HitLocConstNames[19];
float g_fHitLocDamageMult[19];

float G_GetWeaponHitLocationMultiplier(hitLocation_t hitLoc, int weapon)
{
	WeaponDef *weaponDef;

	if ( weapon )
	{
		weaponDef = BG_GetWeaponDef(weapon);

		if ( weaponDef && !weaponDef->weaponType )
			return weaponDef->locationDamageMultipliers[hitLoc];
		else
			return g_fHitLocDamageMult[hitLoc];
	}
	else
	{
		return g_fHitLocDamageMult[hitLoc];
	}
}

unsigned short G_GetHitLocationString(hitLocation_t hitLoc)
{
	return g_HitLocConstNames[hitLoc];
}

int G_GetHitLocationIndexFromString(unsigned short sString)
{
	int i;

	for ( i = 0; i < 19; ++i )
	{
		if ( g_HitLocConstNames[i] == sString )
			return i;
	}

	return 0;
}

void G_DamageClient(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, const float *vDir, const float *vPoint, int damage, int dflags, int meansOfDeath, hitLocation_t hitLoc, int timeOffset)
{
	int iWeapon;
	int dmg;

	if ( self->takedamage && !self->client->noclip && !self->client->ufo && self->client->sess.connected == CS_CONNECTED )
	{
		if ( inflictor )
		{
			iWeapon = inflictor->s.weapon;
		}
		else if ( attacker )
		{
			iWeapon = attacker->s.weapon;
		}
		else
		{
			iWeapon = 0;
		}

		dmg = (int)(G_GetWeaponHitLocationMultiplier(hitLoc, iWeapon) * (float)damage);

		if ( dmg > 0 )
			Scr_PlayerDamage(self, inflictor, attacker, dmg, dflags, meansOfDeath, iWeapon, vPoint, vDir, hitLoc, timeOffset);
	}
}

extern dvar_t *g_debugDamage;
void G_Damage(gentity_s *self, gentity_s *inflictor, gentity_s *ent, const float *vDir,const float *vPoint, int value, int dflags, int meansOfDeath, hitLocation_t hitLoc, int timeOffset)
{
	void (*pain)(struct gentity_s *, struct gentity_s *, int, const float *, const int, const float *, int);
	void (*die)(struct gentity_s *, struct gentity_s *, struct gentity_s *, int, int, const int, const float *, int, int);
	vec3_t localdir;

	if ( self->client )
	{
		G_DamageClient(self, inflictor, ent, vDir, vPoint, value, dflags, meansOfDeath, hitLoc, timeOffset);
	}
	else if ( self->takedamage )
	{
		if ( !inflictor )
			inflictor = &g_entities[1022];

		if ( !ent )
			ent = &g_entities[1022];

		Vec3NormalizeTo(vDir, localdir);

		if ( (self->flags & 1) == 0 )
		{
			if ( value <= 0 )
				value = 1;

			if ( (self->flags & 2) != 0 && self->health - value <= 0 )
				value = self->health - 1;

			if ( g_debugDamage->current.boolean )
				Com_Printf("target:%i health:%i damage:%i\n", self->s.number, self->health, value);

			self->health -= value;
			Scr_AddEntity(ent);
			Scr_AddInt(value);
			Scr_Notify(self, scr_const.damage, 2);

			if ( self->health > 0 )
			{
				pain = entityHandlers[self->handler].pain;

				if ( pain )
					pain(self, ent, value, vPoint, meansOfDeath, localdir, hitLoc);
			}
			else
			{
				if ( self->health < -999 )
					self->health = -999;

				Scr_AddEntity(ent);
				Scr_Notify(self, scr_const.death, 1);

				die = entityHandlers[self->handler].die;

				if ( die )
					die(self, inflictor, ent, value, meansOfDeath, inflictor->s.weapon, localdir, hitLoc, timeOffset);
			}
		}
	}
}

void G_ParseHitLocDmgTable()
{
	fileHandle_t f;
	int len;
	char loadBuffer[8192];
	cspField_t pFieldList[19];
	size_t n;
	const char *s;
	const char *src;
	int i;

	src = "info/mp_lochit_dmgtable";
	s = "LOCDMGTABLE";
	n = strlen("LOCDMGTABLE");

	for ( i = 0; i < 19; ++i )
	{
		g_fHitLocDamageMult[i] = 1.0;
		pFieldList[i].szName = g_HitLocNames[i];
		pFieldList[i].iOffset = 4 * i;
		pFieldList[i].iFieldType = CSPFT_FLOAT;
		g_HitLocConstNames[i] = Scr_AllocString(g_HitLocNames[i]);
	}

	len = FS_FOpenFileByMode(src, &f, FS_READ);

	if ( len <= 0 )
		Com_Error(ERR_DROP, "Could not load hitloc damage table %s", src);

	FS_Read(loadBuffer, n, f);
	loadBuffer[n] = 0;

	if ( strncmp(loadBuffer, s, n) )
		Com_Error(ERR_DROP, "\"%s\" does not appear to be a hitloc damage table", src);

	if ( (int)(len - n) > 0x1FFF )
		Com_Error(ERR_DROP, "\"%s\" Is too long of a hitloc damage table to parse", src);

	FS_Read(loadBuffer, len - n, f);
	loadBuffer[len - n] = 0;
	FS_FCloseFile(f);

	if ( !Info_Validate(loadBuffer) )
		Com_Error(ERR_DROP, "\"%s\" is not a valid hitloc damage table", src);

	if ( !ParseConfigStringToStruct(
	            (unsigned char *)g_fHitLocDamageMult,
	            pFieldList,
	            19,
	            loadBuffer,
	            0,
	            0,
	            BG_StringCopy) )
		Com_Error(ERR_DROP, "Error parsing hitloc damage table %s", src);
}

void LookAtKiller(gentity_s *self, gentity_s *inflictor, gentity_s *attacker)
{
	gclient_s *client;
	vec3_t dir;

	if ( !attacker || attacker == self )
	{
		if ( !inflictor || inflictor == self )
		{
			self->client->ps.stats[1] = (int)self->r.currentAngles[1];
			return;
		}

		VectorSubtract(inflictor->r.currentOrigin, self->r.currentOrigin, dir);
	}
	else
	{
		VectorSubtract(attacker->r.currentOrigin, self->r.currentOrigin, dir);
	}

	client = self->client;
	client->ps.stats[1] = (int)vectoyaw(dir);
	vectoyaw(dir);
}

void player_die(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, int damage, int meansOfDeath, int iWeapon, const float *vDir, hitLocation_t hitLoc, int psTimeOffset)
{
	int type;
	gentity_s *turret;
	gclient_s *client;
	vec3_t origin;
	vec3_t dir;
	int deathAnimDuration;
	int i;

	if ( Com_GetServerDObj(self->client->ps.clientNum)
	        && self->client->ps.pm_type <= 1
	        && (self->client->ps.pm_flags & 0x400000) == 0 )
	{
		bgs = &level_bgs;

		if ( attacker->s.eType == ET_TURRET && attacker->r.ownerNum != 1023 )
			attacker = &g_entities[attacker->r.ownerNum];

		Scr_AddEntity(attacker);
		Scr_Notify(self, scr_const.death, 1u);

		if ( iWeapon )
		{
			if ( attacker->client )
			{
				if ( (attacker->client->ps.eFlags & 0x300) != 0 )
				{
					turret = &g_entities[attacker->s.otherEntityNum];

					if ( turret->s.eType == ET_TURRET )
						iWeapon = turret->s.weapon;
				}
			}
		}

		if ( self->client->ps.grenadeTimeLeft )
		{
			dir[0] = G_crandom();
			dir[1] = G_crandom();
			dir[2] = G_random();
			VectorScale(dir, 160.0, dir);
			VectorCopy(self->r.currentOrigin, origin);
			origin[2] = origin[2] + 40.0;
			fire_grenade(self, origin, dir, self->client->ps.offHandIndex, self->client->ps.grenadeTimeLeft);
		}

		if ( self->client->ps.pm_type == 1 )
			type = 7;
		else
			type = 6;

		self->client->ps.pm_type = type;
		deathAnimDuration = BG_AnimScriptEvent(&self->client->ps, ANIM_ET_DEATH, 0, 1);

		Scr_PlayerKilled(
		    self,
		    inflictor,
		    attacker,
		    damage,
		    meansOfDeath,
		    iWeapon,
		    vDir,
		    hitLoc,
		    psTimeOffset,
		    deathAnimDuration);

		for ( i = 0; i < level.maxclients; ++i )
		{
			client = &level.clients[i];

			if ( client->sess.connected == CS_CONNECTED
			        && client->sess.sessionState == STATE_SPECTATOR
			        && client->spectatorClient == self->s.number )
			{
				Cmd_Score_f(&g_entities[i]);
			}
		}

		self->takedamage = 1;
		self->r.contents = 0x4000000;
		self->r.currentAngles[2] = 0.0;
		LookAtKiller(self, inflictor, attacker);
		VectorCopy(self->r.currentAngles, self->client->ps.viewangles);
		self->s.loopSound = 0;
		SV_UnlinkEntity(self);
		self->r.maxs[2] = 30.0;
		SV_LinkEntity(self);
		self->health = 0;
		self->handler = 11;
	}
}