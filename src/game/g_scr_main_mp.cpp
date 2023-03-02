#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define g_scr_data (*(scr_data_t*)( 0x0879C780 ))
#else
scr_data_t g_scr_data;
#endif

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

extern game_hudelem_t g_hudelems[];

gentity_t* Scr_EntityForRef(scr_entref_t entRef)
{
	if ( entRef.classnum == CLASS_NUM_ENTITY )
		return &g_entities[entRef.entnum];

	Scr_ObjectError("not an entity");
	return 0;
}

game_hudelem_t* Scr_HudElemForRef(scr_entref_t entRef)
{
	if ( entRef.classnum == CLASS_NUM_HUDELEM )
		return &g_hudelems[entRef.entnum];

	Scr_ObjectError("not a hud element");
	return 0;
}

char* Scr_GetGameTypeNameForScript(const char *pszGameTypeScript)
{
	int i;

	for ( i = 0; i < g_scr_data.gametype.iNumGameTypes; ++i )
	{
		if ( !Q_stricmp(g_scr_data.gametype.list[i].pszScript, pszGameTypeScript) )
		{
			return g_scr_data.gametype.list[i].pszName;
		}
	}

	return 0;
}

void Scr_ValidateLocalizedStringRef(int parmIndex, const char *token, int tokenLen)
{
	int charIter;

	if ( tokenLen > 1 )
	{
		for ( charIter = 0; charIter < tokenLen; ++charIter )
		{
			if ( !isalnum(token[charIter]) && token[charIter] != '_' )
			{
				Scr_ParamError(parmIndex, va("Illegal localized string reference: %s must contain only alpha-numeric characters and underscores", token));
			}
		}
	}
}

extern dvar_t *loc_warnings;
extern dvar_t *loc_warningsAsErrors;
void Scr_ConstructMessageString(int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, unsigned int stringLimit)
{
	unsigned int charIndex;
	unsigned int tokenLen;
	int type;
	gentity_t *ent;
	int parmIndex;
	const char *token;
	unsigned int stringLen;

	stringLen = 0;

	for ( parmIndex = firstParmIndex; parmIndex <= lastParmIndex; ++parmIndex )
	{
		type = Scr_GetType(parmIndex);

		if ( type == VAR_ISTRING )
		{
			token = Scr_GetIString(parmIndex);
			tokenLen = strlen(token);
			Scr_ValidateLocalizedStringRef(parmIndex, token, tokenLen);

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(parmIndex, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( stringLen )
			{
				string[stringLen++] = 20;
			}
		}
		else if ( type != VAR_OBJECT || Scr_GetPointerType(parmIndex) != VAR_ENTITY )
		{
			token = Scr_GetString(parmIndex);
			tokenLen = strlen(token);

			for ( charIndex = 0; charIndex < tokenLen; ++charIndex )
			{
				if ( token[charIndex] == 20 || token[charIndex] == 21 || token[charIndex] == 22 )
				{
					Scr_ParamError(parmIndex, va("bad escape character (%i) present in string", token[charIndex]));
				}

				if ( isalpha(token[charIndex]) )
				{
					if ( loc_warnings->current.boolean )
					{
						if ( loc_warningsAsErrors->current.boolean )
						{
							Scr_LocalizationError(parmIndex, va("non-localized %s strings are not allowed to have letters in them: \"%s\"", errorContext, token));
						}
						else
						{
							Com_Printf("WARNING: Non-localized %s string is not allowed to have letters in it. Must be changed over to a localized string: \"%s\"\n",
							           errorContext, token);
						}
					}
					break;
				}
			}

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(parmIndex, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( tokenLen )
			{
				string[stringLen++] = 21;
			}
		}
		else
		{
			ent = Scr_GetEntity(parmIndex);

			if ( !ent->client )
			{
				Scr_ParamError(parmIndex, "Entity is not a player");
			}

			token = va("%s^7", ent->client->sess.state.name);
			tokenLen = strlen(token);

			if ( stringLen + tokenLen + 1 >= stringLimit )
			{
				Scr_ParamError(parmIndex, va("%s is too long. Max length is %i\n", errorContext, stringLimit));
			}

			if ( tokenLen )
			{
				string[stringLen++] = 21;
			}
		}

		for ( charIndex = 0; charIndex < tokenLen; ++charIndex )
		{
			if ( token[charIndex] != 20 && token[charIndex] != 21 && token[charIndex] != 22 )
			{
				string[stringLen] = token[charIndex];
			}
			else
			{
				string[stringLen] = '.';
			}

			++stringLen;
		}
	}

	string[stringLen] = 0;
}

void Scr_SetOrigin(gentity_s *ent, int offset)
{
	vec3_t origin;

	Scr_GetVector(0, origin);
	G_SetOrigin(ent, origin);

	if ( ent->r.linked )
		SV_LinkEntity(ent);
}

void Scr_SetAngles(gentity_s *ent, int offset)
{
	vec3_t angles;

	Scr_GetVector(0, angles);
	G_SetAngle(ent, angles);
}

void Scr_SetHealth(gentity_s *ent, int offset)
{
	int health;

	health = Scr_GetInt(0);

	if ( ent->client )
	{
		ent->health = health;
		ent->client->ps.stats[0] = health;
	}
	else
	{
		ent->maxHealth = health;
		ent->health = health;
	}
}

unsigned int GScr_AllocString(const char *string)
{
	return Scr_AllocString(string);
}

int GScr_GetHeadIconIndex(const char *pszIcon)
{
	char dest[MAX_STRING_CHARS];
	int i;

	if ( !*pszIcon )
		return 0;

	for ( i = 0; i < 15; ++i )
	{
		SV_GetConfigstring(i + 31, dest, 1024);

		if ( !I_stricmp(dest, pszIcon) )
			return i + 1;
	}

	Scr_Error(va("Head icon '%s' was not precached\n", pszIcon));
	return 0;
}

int GScr_GetStatusIconIndex(const char *pszIcon)
{
	char dest[MAX_STRING_CHARS];
	int i;

	if ( !*pszIcon )
		return 0;

	for ( i = 0; i < 8; ++i )
	{
		SV_GetConfigstring(i + 23, dest, 1024);

		if ( !I_stricmp(dest, (char *)pszIcon) )
			return i + 1;
	}

	Scr_Error(va("Status icon '%s' was not precached\n", pszIcon));
	return 0;
}