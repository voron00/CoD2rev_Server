#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"
#include "../server/server.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define entityHandlers ((entityHandler_t*)( 0x08167880 ))
#else
extern entityHandler_t entityHandlers[] = {};
#endif

#ifdef TESTING_LIBRARY
#define g_scr_data (*(scr_data_t*)( 0x0879C780 ))
#else
extern scr_data_t g_scr_data;
#endif

XModel* cached_models[MAX_MODELS];

const char* G_ModelName(int modelIndex)
{
	return SV_GetConfigstringConst(modelIndex + 334);
}

void G_AddEvent(gentity_s *ent, int event, unsigned int eventParm)
{
	if ( ent->client )
	{
		ent->client->ps.events[ent->client->ps.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->client->ps.eventParms[ent->client->ps.eventSequence++ & ( MAX_EVENTS - 1 )] = eventParm;
	}
	else
	{
		ent->s.events[ent->s.eventSequence & ( MAX_EVENTS - 1 )] = event;
		ent->s.eventParms[ent->s.eventSequence++ & ( MAX_EVENTS - 1 )] = eventParm;
	}

	ent->eventTime = level.time;
	ent->r.eventTime = level.time;
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

void G_DObjCalcPose(gentity_s *ent)
{
	int partBits[4];
	void (*controller)(struct gentity_s *, int *);

	memset(partBits, 255, sizeof(partBits));

	if ( !SV_DObjCreateSkelForBones(ent, partBits) )
	{
		SV_DObjCalcAnim(ent, partBits);

		controller = entityHandlers[ent->handler].controller;

		if ( controller )
			controller(ent, partBits);

		SV_DObjCalcSkel(ent, partBits);
	}
}

void G_DObjCalcBone(gentity_s *ent, int boneIndex)
{
	int partBits[4];
	void (*controller)(struct gentity_s *, int *);

	if ( !SV_DObjCreateSkelForBone(ent, boneIndex) )
	{
		SV_DObjGetHierarchyBits(ent, boneIndex, partBits);
		SV_DObjCalcAnim(ent, partBits);

		controller = entityHandlers[ent->handler].controller;

		if ( controller )
			controller(ent, partBits);

		SV_DObjCalcSkel(ent, partBits);
	}
}

float G_random()
{
	return (float)rand() / 2147483600.0;
}

float G_crandom()
{
	return G_random() * 2.0 - 1.0;
}

void G_EntUnlink(gentity_s *ent)
{
	vec3_t viewAngles;
	gentity_s *prev;
	gentity_s *next;
	gentity_s *parent;
	tagInfo_s *tag;

	tag = ent->tagInfo;

	if ( tag )
	{
		G_SetOrigin(ent, ent->r.currentOrigin);
		G_SetAngle(ent, ent->r.currentAngles);

		if ( ent->client )
		{
			VectorCopy(ent->client->ps.viewangles, viewAngles);
			viewAngles[2] = 0.0;
			SetClientViewAngle(ent, viewAngles);
		}

		parent = tag->parent;
		prev = 0;

		for ( next = parent->tagChildren; next != ent; next = next->tagInfo->next )
			prev = next;

		if ( prev )
			prev->tagInfo->next = tag->next;
		else
			parent->tagChildren = tag->next;

		ent->tagInfo = 0;
		Scr_SetString(&tag->name, 0);
		MT_Free(tag, sizeof(tagInfo_s));
	}
}

void G_FreeEntityRefs(gentity_s *ent)
{
	gclient_s *client;
	int number;
	int i;
	int j;
	int k;
	gentity_s *other;

	number = ent->s.number;

	for ( i = 0; i < level.num_entities; ++i )
	{
		other = &g_entities[i];

		if ( other->r.inuse )
		{
			if ( other->parent == ent )
				other->parent = 0;

			if ( other->r.ownerNum == number )
			{
				other->r.ownerNum = 1023;
				if ( other->s.eType == ET_TURRET )
					other->active = 0;
			}

			if ( other->s.groundEntityNum == number )
				other->s.groundEntityNum = 1023;
		}
	}

	for ( j = 0; j < 64; ++j )
	{
		if ( g_entities[j].r.inuse )
		{
			client = g_entities[j].client;
			if ( client->pLookatEnt == ent )
				client->pLookatEnt = 0;

			if ( client->useHoldEntity == number )
				client->useHoldEntity = 1023;

			if ( client->ps.cursorHintEntIndex == number )
				client->ps.cursorHintEntIndex = 1023;
		}
	}

	for ( k = 0; k < 32; ++k )
	{
		if ( level.droppedWeaponCue[k] == ent )
			level.droppedWeaponCue[k] = 0;
	}
}

void G_FreeEntity(gentity_s *ent)
{
	XAnimTree_s *tree;
	int useCount;

	G_EntUnlink(ent);

	while ( ent->tagChildren )
		G_EntUnlink(ent->tagChildren);

	SV_UnlinkEntity(ent);

	tree = SV_DObjGetTree(ent);
	if ( tree )
		XAnimClearTree(tree);

	Com_SafeServerDObjFree(ent->s.number);
	G_FreeEntityRefs(ent);

	if ( ent->pTurretInfo )
		G_FreeTurret(ent);

	if ( ent->s.eType == ET_PLAYER_CORPSE )
		G_CorpseFree(ent);

	Scr_FreeEntity(ent);
	useCount = ent->useCount;
	memset(ent, 0, sizeof(gentity_s));
	ent->eventTime = level.time;

	if ( ent - level.gentities > 71 )
	{
		if ( level.lastFreeEnt )
			level.lastFreeEnt->nextFree = ent;
		else
			level.firstFreeEnt = ent;

		level.lastFreeEnt = ent;
		ent->nextFree = 0;
	}

	ent->useCount = useCount + 1;
}

void G_FreeEntityDelay(gentity_s *ent)
{
	unsigned short callback;

	callback = Scr_ExecEntThread(ent, g_scr_data.deletestruct, 0);
	Scr_FreeThread(callback);
}

void G_InitGentity(gentity_s *ent)
{
	ent->nextFree = 0;
	ent->r.inuse = 1;
	Scr_SetString(&ent->classname, scr_const.noclass);
	ent->s.number = ent - g_entities;
	ent->r.ownerNum = 1023;
	ent->eventTime = 0;
	ent->freeAfterEvent = 0;
}

void G_PrintEntities()
{
	const char *classname;
	int i;

	for ( i = 0; i < level.num_entities; ++i )
	{
		if ( g_entities[i].classname )
			classname = SL_ConvertToString(g_entities[i].classname);
		else
			classname = "";

		Com_Printf(
		    "%4i: '%s', origin: %f %f %f\n",
		    i,
		    classname,
		    g_entities[i].r.currentOrigin[0],
		    g_entities[i].r.currentOrigin[1],
		    g_entities[i].r.currentOrigin[2]);
	}
}

bool G_MaySpawnEntity(gentity_s *ent)
{
	if ( !ent )
		return 0;

	return level.time - ent->eventTime >= 500 || level.num_entities >= 1022;
}

gentity_s* G_Spawn(void)
{
	gentity_s *ent;

	ent = level.firstFreeEnt;

	if ( G_MaySpawnEntity(level.firstFreeEnt) )
	{
		level.firstFreeEnt = level.firstFreeEnt->nextFree;

		if ( !level.firstFreeEnt )
			level.lastFreeEnt = 0;

		ent->nextFree = 0;
	}
	else
	{
		if ( level.num_entities == 1022 )
		{
			G_PrintEntities();
			Com_Error(ERR_DROP, "G_Spawn: no free entities");
		}

		ent = &level.gentities[level.num_entities++];
		SV_LocateGameData(level.gentities, level.num_entities, sizeof(gentity_s), &level.clients->ps, sizeof(gclient_s));
	}

	G_InitGentity(ent);
	return ent;
}

gentity_s* G_TempEntity(vec3_t origin, int event)
{
	vec3_t snapped;
	gentity_s *ent;

	ent = G_Spawn();
	ent->s.eType = event + ET_EVENTS;

	Scr_SetString(&ent->classname, scr_const.tempEntity);

	ent->eventTime = level.time;
	ent->r.eventTime = level.time;
	ent->freeAfterEvent = 1;

	VectorCopy(origin, snapped);
	SnapVector(snapped);      // save network bandwidth
	G_SetOrigin(ent, snapped);

	SV_LinkEntity(ent);

	return ent;
}

DObjAnimMat* G_DObjGetLocalTagMatrix(gentity_s *ent, unsigned int tagName)
{
	int boneIndex;

	boneIndex = SV_DObjGetBoneIndex(ent, tagName);

	if ( boneIndex < 0 )
		return 0;

	G_DObjCalcBone(ent, boneIndex);
	return &SV_DObjGetMatrixArray(ent)[boneIndex];
}

int G_DObjGetWorldTagPos(gentity_s *ent, unsigned int tagName, float *pos)
{
	float ent_axis[4][3];
	DObjAnimMat *mat;

	mat = G_DObjGetLocalTagMatrix(ent, tagName);

	if ( !mat )
		return 0;

	AnglesToAxis(ent->r.currentAngles, ent_axis);
	VectorCopy(ent->r.currentOrigin, ent_axis[3]);
	MatrixTransformVector43(mat->trans, ent_axis, pos);

	return 1;
}