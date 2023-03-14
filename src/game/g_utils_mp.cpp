#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

XModel* cached_models[MAX_MODELS];

const char* G_ModelName(int modelIndex)
{
	return SV_GetConfigstringConst(modelIndex + 334);
}

int G_FindConfigstringIndex(const char *name, int start, int max, int create, const char *errormsg)
{
	const char *s1;
	int i;

	if ( !name || !*name )
		return 0;

	for ( i = 1; i < max; ++i )
	{
		s1 = SV_GetConfigstringConst(start + i);

		if ( !*s1 )
			break;

		if ( !strcasecmp(s1, name) )
			return i;
	}

	if ( create )
	{
		if ( i == max )
		{
			Com_Error(ERR_DROP, va("G_FindConfigstringIndex: overflow (%d): %s", start, name));
		}

		SV_SetConfigstring(start + i, name);
		return i;
	}
	else
	{
		if ( errormsg )
		{
			Scr_Error(va("%s \"%s\" not precached", errormsg, name));
		}

		return 0;
	}
}

int G_LocalizedStringIndex(const char *string)
{
	if ( *string )
		return G_FindConfigstringIndex(string, 1310, 256, level.initializing, "localized string");
	else
		return 0;
}

int G_ShaderIndex(const char *string)
{
	char dest[64];

	strcpy(dest, string);
	I_strlwr(dest);

	return G_FindConfigstringIndex(dest, 1566, 128, level.initializing, "shader");
}

int G_TagIndex(const char *name)
{
	return G_FindConfigstringIndex(name, 110, 32, 1, 0);
}

XModel* G_CachedModelForIndex(int modelIndex)
{
	return cached_models[modelIndex];
}

unsigned int G_ModelIndex(const char *name)
{
	const char *modelName;
	signed int i;

	if ( !*name )
		return 0;

	for ( i = 1; i < MAX_MODELS; ++i )
	{
		modelName = SV_GetConfigstringConst(i + 334);

		if ( !*modelName )
			break;

		if ( !strcasecmp(modelName, name) )
			return i;
	}

	if ( !level.initializing )
	{
		Scr_Error(va("model '%s' not precached", name));
	}

	if ( i == MAX_MODELS )
		Com_Error(ERR_DROP, "G_ModelIndex: overflow");

	cached_models[i] = SV_XModelGet(name);
	SV_SetConfigstring(i + 334, name);

	return i;
}

void G_OverrideModel(int modelIndex, const char *defaultModelName)
{
	XModel *model;
	const char *modelName;

	modelName = G_ModelName(modelIndex);
	model = SV_XModelGet(defaultModelName);
	cached_models[modelIndex] = model;
	Hunk_OverrideDataForFile(FILEDATA_XMODEL, modelName + 7, model);
}

void G_SetModel(gentity_s *ent, const char *modelName)
{
	if ( *modelName )
		ent->model = G_ModelIndex(modelName);
	else
		ent->model = 0;
}

void G_SetOrigin(gentity_s *ent, const float *origin)
{
	VectorCopy(origin, ent->s.pos.trBase);
	ent->s.pos.trType = TR_STATIONARY;
	ent->s.pos.trTime = 0;
	ent->s.pos.trDuration = 0;
	VectorSet(ent->s.pos.trDelta, 0, 0, 0);
	VectorCopy(origin, ent->r.currentOrigin);
}

void G_SetAngle(gentity_s *ent, const float *angle)
{
	VectorCopy(angle, ent->s.apos.trBase);
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorSet(ent->s.pos.trDelta, 0, 0, 0);
	VectorCopy(angle, ent->r.currentAngles);
}