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

#ifdef TESTING_LIBRARY
#define builtin_methods (((scr_method_t*)( 0x08168280 )))
#else
scr_method_t builtin_methods[] =
{

};
#endif

#ifdef TESTING_LIBRARY
#define functions (((scr_function_t*)( 0x08167BC0 )))
#else
scr_function_t functions[] =
{

};
#endif

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

void Scr_PlayerConnect(gentity_s *self)
{
	unsigned short callback;

	callback = Scr_ExecEntThread(self, g_scr_data.gametype.playerconnect, 0);
	Scr_FreeThread(callback);
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

void Scr_BulletTrace()
{
	const char *surfaceName;
	vec3_t vNorm;
	vec3_t value;
	int iSurfaceTypeIndex;
	trace_t trace;
	int contentmask;
	int iIgnoreEntNum;
	gentity_s *pIgnoreEnt;
	vec3_t vEnd;
	vec3_t vStart;

	pIgnoreEnt = 0;
	iIgnoreEntNum = 1023;
	contentmask = 41953329;

	Scr_GetVector(0, vStart);
	Scr_GetVector(1, vEnd);

	if ( !Scr_GetInt(2) )
		contentmask &= ~0x2000000u;

	if ( Scr_GetType(3) == VAR_OBJECT && Scr_GetPointerType(3) == VAR_ENTITY )
	{
		pIgnoreEnt = Scr_GetEntity(3);
		iIgnoreEntNum = pIgnoreEnt->s.number;
	}

	G_LocationalTrace(&trace, vStart, vEnd, iIgnoreEntNum, contentmask, 0);
	Scr_MakeArray();
	Scr_AddFloat(trace.fraction);
	Scr_AddArrayStringIndexed(scr_const.fraction);
	Vec3Lerp(vStart, vEnd, trace.fraction, value);
	Scr_AddVector(value);
	Scr_AddArrayStringIndexed(scr_const.position);

	if ( trace.hitId == 1023 || trace.hitId == 1022 )
		Scr_AddUndefined();
	else
		Scr_AddEntity(&g_entities[trace.hitId]);

	Scr_AddArrayStringIndexed(scr_const.entity);

	if ( trace.fraction >= 1.0 )
	{
		VectorSubtract(vEnd, vStart, vNorm);
		Vec3Normalize(vNorm);
		Scr_AddVector(vNorm);
		Scr_AddArrayStringIndexed(scr_const.normal);
		Scr_AddConstString(scr_const.none);
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
	}
	else
	{
		Scr_AddVector(trace.normal);
		Scr_AddArrayStringIndexed(scr_const.normal);
		iSurfaceTypeIndex = (trace.surfaceFlags & 0x1F00000) >> 20;
		surfaceName = Com_SurfaceTypeToName(iSurfaceTypeIndex);
		Scr_AddString(surfaceName);
		Scr_AddArrayStringIndexed(scr_const.surfacetype);
	}
}

void (*BuiltIn_GetMethod(const char **pName, int *type))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

#ifdef TESTING_LIBRARY
	for ( i = 0; i < 59; ++i )
#else
	for ( i = 0; i < COUNT_OF(builtin_methods); ++i )
#endif
	{
		if ( !strcmp(name, builtin_methods[i].name) )
		{
			*pName = builtin_methods[i].name;
			*type = builtin_methods[i].developer;
			return builtin_methods[i].call;
		}
	}

	return NULL;
}

void (*Scr_GetMethod(const char **pName, int *type))(scr_entref_t)
{
	void (*scriptent_meth)(scr_entref_t);
	void (*scr_meth)(scr_entref_t);
	void (*hud_meth)(scr_entref_t);
	void (*meth)(scr_entref_t);

	*type = 0;

	meth = Player_GetMethod(pName);
	scriptent_meth = ScriptEnt_GetMethod(pName);

	if ( !meth )
		meth = scriptent_meth;

	scr_meth = BuiltIn_GetMethod(pName, type);

	if ( !meth )
		meth = scr_meth;

	hud_meth = HudElem_GetMethod(pName);

	if ( !meth )
		return hud_meth;

	return meth;
}

void (*Scr_GetFunction(const char **pName, int *type))()
{
	const char *name;
	unsigned int i;

	name = *pName;

#ifdef TESTING_LIBRARY
	for ( i = 0; i < 144; ++i )
#else
	for ( i = 0; i < COUNT_OF(builtin_methods); ++i )
#endif
	{
		if ( !strcmp(name, functions[i].name) )
		{
			*pName = functions[i].name;
			*type = functions[i].developer;
			return functions[i].call;
		}
	}

	return NULL;
}

extern dvar_t * sv_gametype;
void GScr_LoadGameTypeScript()
{
	char s[64];

	Com_sprintf(s, sizeof(s), "maps/mp/gametypes/%s", sv_gametype->current.string);

	g_scr_data.gametype.main = Scr_GetFunctionHandle(s, "main", 1);
	g_scr_data.gametype.startupgametype = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_StartGameType", 1);
	g_scr_data.gametype.playerconnect = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerConnect", 1);
	g_scr_data.gametype.playerdisconnect = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerDisconnect", 1);
	g_scr_data.gametype.playerdamage = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerDamage", 1);
	g_scr_data.gametype.playerkilled = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerKilled", 1);
}

void GScr_LoadLevelScript()
{
	dvar_t *mapname;
	char s[64];

	mapname = Dvar_RegisterString("mapname", "", 0x1044u);
	Com_sprintf(s, sizeof(s), "maps/mp/%s", mapname->current.string);
	g_scr_data.levelscript = Scr_GetFunctionHandle(s, "main", 0);
}

void GScr_LoadFields()
{
	int classnum;

	for ( classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
		Scr_SetClassMap(classnum);

	GScr_AddFieldsForEntity();
	GScr_AddFieldsForHudElems();
	GScr_AddFieldsForRadiant();
}

void GScr_LoadScripts()
{
	Scr_BeginLoadScripts();
	g_scr_data.deletestruct = Scr_GetFunctionHandle("codescripts/delete", "main", 1);
	g_scr_data.initstructs = Scr_GetFunctionHandle("codescripts/struct", "initstructs", 1);
	g_scr_data.createstruct = Scr_GetFunctionHandle("codescripts/struct", "createstruct", 1);
	GScr_LoadGameTypeScript();
	GScr_LoadLevelScript();
	Scr_PostCompileScripts();
	GScr_LoadFields();
	Scr_EndLoadScripts();
}