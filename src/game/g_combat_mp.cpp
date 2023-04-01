#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define entityHandlers ((entityHandler_t*)( 0x08167880 ))
#else
const entityHandler_t entityHandlers[] =
{

};
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