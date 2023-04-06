#include "../qcommon/qcommon.h"
#include "g_shared.h"

const char *hintStrings[] =
{
	"",
	"HINT_NONE",
	"HINT_ACTIVATE",
	"HINT_HEALTH",
	"HINT_FRIENDLY",
};

void GScr_SetCursorHint(scr_entref_t entref)
{
	const char *pszHint;
	gentity_s *pEnt;
	int i;
	int j;

	pEnt = GetEntity(entref);
	pszHint = Scr_GetString(0);

	if ( (pEnt->classname == scr_const.trigger_use || pEnt->classname == scr_const.trigger_use_touch)
	        && !I_stricmp(pszHint, "HINT_INHERIT") )
	{
		pEnt->s.dmgFlags = -1;
	}
	else
	{
		for ( i = 1; i < COUNT_OF(hintStrings) && hintStrings[i]; ++i )
		{
			if ( !I_stricmp(pszHint, hintStrings[i]) )
			{
				pEnt->s.dmgFlags = i;
				return;
			}
		}

		Com_Printf("List of valid hint type strings\n");

		if ( pEnt->classname == scr_const.trigger_use || pEnt->classname == scr_const.trigger_use_touch )
			Com_Printf("HINT_INHERIT (for trigger_use or trigger_use_touch entities only)\n");

		for ( j = 1; j < COUNT_OF(hintStrings) && hintStrings[j]; ++j )
			Com_Printf("%s\n", hintStrings[j]);

		Scr_Error(va("%s is not a valid hint type. See above for list of valid hint types\n", pszHint));
	}
}

void GScr_SetHintString(scr_entref_t entref)
{
	const char *string;
	unsigned int paramNum;
	char szHint[1024];
	gentity_s *pEnt;
	int type;
	int i;

	pEnt = GetEntity(entref);

	if ( pEnt->classname != scr_const.trigger_use && pEnt->classname != scr_const.trigger_use_touch )
		Scr_Error("The setHintString command only works on trigger_use or trigger_use_touch entities.\n");

	type = Scr_GetType(0);

	if ( type != VAR_STRING || (string = Scr_GetString(0), I_stricmp(string, "")) )
	{
		paramNum = Scr_GetNumParam();
		Scr_ConstructMessageString(0, paramNum - 1, "Hint String", szHint, 1024);

		if ( !G_GetHintStringIndex(&i, szHint) )
		{
			Scr_Error(va("Too many different hintstring values. Max allowed is %i different strings", 32));
		}

		pEnt->s.scale = (unsigned char)i;
	}
	else
	{
		pEnt->s.scale = 255;
	}
}

void trigger_use_shared(gentity_s *self)
{
	char szConfigString[1024];
	const char *cursorhint;
	int i;

	SV_SetBrushModel(self);
	SV_LinkEntity(self);
	self->trigger.singleUserEntIndex = 1023;
	self->s.pos.trType = TR_STATIONARY;
	VectorCopy(self->r.currentOrigin, self->s.pos.trBase);
	self->r.contents = 0x200000;
	self->r.svFlags = 1;
	self->handler = 18;
	self->s.dmgFlags = 2;

	if ( G_SpawnString("cursorhint", "", &cursorhint) )
	{
		if ( I_stricmp(cursorhint, "HINT_INHERIT") )
		{
			for ( i = 1; i < COUNT_OF(hintStrings); ++i )
			{
				if ( !I_stricmp(cursorhint, hintStrings[i]) )
				{
					self->s.dmgFlags = i;
					break;
				}
			}
		}
		else
		{
			self->s.dmgFlags = -1;
		}
	}

	self->s.scale = 255;

	if ( G_SpawnString("hintstring", "", &cursorhint) )
	{
		for ( i = 0; i < 32; ++i )
		{
			SV_GetConfigstring(i + 1278, szConfigString, 1024);

			if ( !szConfigString[0] )
			{
				SV_SetConfigstring(i + 1278, cursorhint);
				self->s.scale = (unsigned char)i;
				break;
			}

			if ( !strcmp(cursorhint, szConfigString) )
			{
				self->s.scale = (unsigned char)i;
				break;
			}
		}

		if ( i == 32 )
			Com_Error(ERR_DROP, "Too many different hintstring key values on trigger_use entities.", 32);
	}
}