#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"
#include "../server/server.h"

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

int G_SoundAliasIndex(const char *name)
{
	return G_FindConfigstringIndex(name, 590, 256, 1, 0);
}

int G_ShellShockIndex(const char *name)
{
	return G_FindConfigstringIndex(name, 1166, 16, 1, 0);
}

int G_EffectIndex(const char *name)
{
	return G_FindConfigstringIndex(name, 846, 64, level.initializing, "effect");
}

XModel* G_GetModel(int modelIndex)
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
	VectorClear(ent->s.pos.trDelta);
	VectorCopy(origin, ent->r.currentOrigin);
}

void G_SetAngle(gentity_s *ent, const float *angle)
{
	VectorCopy(angle, ent->s.apos.trBase);
	ent->s.apos.trType = TR_STATIONARY;
	ent->s.apos.trTime = 0;
	ent->s.apos.trDuration = 0;
	VectorClear(ent->s.apos.trDelta);
	VectorCopy(angle, ent->r.currentAngles);
}

void G_SetConstString(unsigned short *to, const char *from)
{
	Scr_SetString(to, 0);
	*to = SL_GetString(from, 0);
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

int G_DObjGetWorldTagMatrix(gentity_s *ent, unsigned int tagName, float (*tagMat)[3])
{
	float ent_axis[4][3];
	DObjAnimMat *mat;
	float axis[3][3];

	mat = G_DObjGetLocalTagMatrix(ent, tagName);

	if ( !mat )
		return 0;

	AnglesToAxis(ent->r.currentAngles, ent_axis);
	ent_axis[3][0] = ent->r.currentOrigin[0];
	ent_axis[3][1] = ent->r.currentOrigin[1];
	ent_axis[3][2] = ent->r.currentOrigin[2];
	ConvertQuatToMat(mat, axis);
	MatrixMultiply(axis, ent_axis, tagMat);
	MatrixTransformVector43(mat->trans, ent_axis, &(*tagMat)[9]);

	return 1;
}

int G_DObjUpdateServerTime(gentity_s *ent, int bNotify)
{
	float nextFrameTime;

	nextFrameTime = (float)level.frameTime * 0.001;
	return SV_DObjUpdateServerTime(ent, nextFrameTime, bNotify);
}

float G_random()
{
	return (float)(int)rand() / RAND_MAX;
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

snd_alias_list_t* Com_FindSoundAlias(const char *name)
{
	return NULL; // Not supported
}

void G_PlaySoundAlias(gentity_s *ent, byte alias)
{
	if ( alias )
		G_AddEvent(ent, EV_SOUND_ALIAS, alias);
}

void G_AnimScriptSound(int clientNum, snd_alias_list_t *aliasList)
{
	byte soundIndex;

	soundIndex = G_SoundAliasIndex(aliasList->aliasName);

	if (soundIndex)
		G_PlaySoundAlias(&g_entities[clientNum], soundIndex);
}

void G_AddPredictableEvent(gentity_s *ent, int event, int eventParm)
{
	if ( ent->client )
		BG_AddPredictableEventToPlayerstate(event, eventParm, &ent->client->ps);
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

void G_SafeDObjFree(gentity_s *ent)
{
	Com_SafeServerDObjFree(ent->s.number);
}

void G_UpdateTagInfoOfChildren(gentity_s *parent, int bHasDObj)
{
	tagInfo_s *tagInfo;

	tagInfo = parent->tagInfo;

	if ( tagInfo->name )
	{
		if ( !bHasDObj || (tagInfo->index = SV_DObjGetBoneIndex(tagInfo->parent, tagInfo->name), tagInfo->index < 0) )
			G_EntUnlink(parent);
	}
	else
	{
		tagInfo->index = -1;
	}
}

void G_UpdateTags(gentity_s *ent, int bHasDObj)
{
	gentity_s *next;
	gentity_s *parent;

	for ( parent = ent->tagChildren; parent; parent = next )
	{
		next = parent->tagInfo->next;
		G_UpdateTagInfoOfChildren(parent, bHasDObj);
	}
}

void G_DObjUpdate(gentity_s *ent)
{
	int i;
	int numModels;
	DObjModel_s dobjModels[8];
	XModel *model;
	int modelIndex;

	if ( !ent->client )
	{
		G_SafeDObjFree(ent);
		modelIndex = ent->model;

		if ( modelIndex )
		{
			model = G_GetModel(modelIndex);
			dobjModels->model = model;
			dobjModels->parentModelName = 0;
			dobjModels->ignoreCollision = 0;
			numModels = 1;

			if ( ent->s.eType == ET_GENERAL || ent->s.eType == ET_SCRIPTMOVER || ent->s.eType == ET_TURRET )
				ent->s.index = modelIndex;

			for ( i = 0; i < 7; ++i )
			{
				modelIndex = ent->attachModelNames[i];

				if ( modelIndex )
				{
					dobjModels[numModels].model = G_GetModel(modelIndex);
					dobjModels[numModels].parentModelName = SL_ConvertToString(ent->attachTagNames[i]);
					dobjModels[numModels].ignoreCollision = ((int)ent->attachIgnoreCollision >> i) & 1;
					numModels++;
				}
			}

			Com_ServerDObjCreate(dobjModels, numModels, 0, ent->s.number);
			G_UpdateTags(ent, 1);
		}
		else
		{
			G_UpdateTags(ent, 0);
		}
	}
}

int G_XModelBad(int index)
{
	XModel *model;

	model = G_GetModel(index);
	return XModelBad(model);
}

/*
=============
VectorToString
This is just a convenience function
for printing vectors
=============
*/
const char    *vtos( const vec3_t v )
{
	static int index;
	static char str[8][32];
	char    *s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = ( index + 1 ) & 7;

	Com_sprintf( s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2] );

	return s;
}

const char    *vtosf( const vec3_t v )
{
	static int index;
	static char str[8][64];
	char    *s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = ( index + 1 ) & 7;

	Com_sprintf( s, 64, "(%f %f %f)", v[0], v[1], v[2] );

	return s;
}

void G_EntDetachAll(gentity_s *ent)
{
	int i;

	for ( i = 0; i <= 6; ++i )
	{
		ent->attachModelNames[i] = 0;
		Scr_SetString(&ent->attachTagNames[i], 0);
	}

	ent->attachIgnoreCollision = 0;
	G_DObjUpdate(ent);
}

int G_EntDetach(gentity_s *ent, const char *modelName, unsigned int tagName)
{
	const char *name;
	byte collisionbits;
	int i;

	for ( i = 0; ; ++i )
	{
		if ( i > 6 )
			return 0;

		if ( ent->attachTagNames[i] == tagName )
		{
			name = G_ModelName(ent->attachModelNames[i]);

			if ( !strcasecmp(name, modelName) )
				break;
		}
	}

	ent->attachModelNames[i] = 0;
	Scr_SetString(&ent->attachTagNames[i], 0);

	while ( i <= 5 )
	{
		ent->attachModelNames[i] = ent->attachModelNames[i + 1];
		ent->attachTagNames[i] = ent->attachTagNames[i + 1];

		if ( (((int)ent->attachIgnoreCollision >> (i + 1)) & 1) != 0 )
			collisionbits = ent->attachIgnoreCollision | (1 << i);
		else
			collisionbits = ent->attachIgnoreCollision & ~(unsigned char)(1 << i);

		ent->attachIgnoreCollision = collisionbits;
		++i;
	}

	ent->attachModelNames[i] = 0;
	ent->attachTagNames[i] = 0;
	ent->attachIgnoreCollision &= ~(unsigned char)(1 << i);

	G_DObjUpdate(ent);

	return 1;
}

int G_EntAttach(gentity_s *ent, const char *modelName, unsigned int tagName, int ignoreCollision)
{
	int i;

	for ( i = 0; ; ++i )
	{
		if ( i > 6 )
			return 0;

		if ( !ent->attachModelNames[i] )
			break;
	}

	ent->attachModelNames[i] = G_ModelIndex(modelName);
	Scr_SetString(&ent->attachTagNames[i], tagName);

	if ( ignoreCollision )
		ent->attachIgnoreCollision |= 1 << i;

	G_DObjUpdate(ent);

	return 1;
}

int G_EntLinkToInternal(gentity_s *ent, gentity_s *parent, unsigned int tagName)
{
	int index;
	gentity_s *checkEnt;
	tagInfo_s *tag;

	G_EntUnlink(ent);

	if ( tagName )
	{
		if ( !SV_DObjExists(parent) )
			return 0;

		index = SV_DObjGetBoneIndex(parent, tagName);

		if ( index < 0 )
			return 0;
	}
	else
	{
		index = -1;
	}

	for ( checkEnt = parent; ; checkEnt = checkEnt->tagInfo->parent )
	{
		if ( checkEnt == ent )
			return 0;

		if ( !checkEnt->tagInfo )
			break;
	}

	tag = (tagInfo_s *)MT_Alloc(sizeof(tagInfo_s));
	tag->parent = parent;
	tag->name = 0;
	Scr_SetString(&tag->name, tagName);
	tag->next = parent->tagChildren;
	tag->index = index;
	memset(tag->axis, 0, sizeof(tag->axis));
	parent->tagChildren = ent;
	ent->tagInfo = tag;
	memset(tag->parentInvAxis, 0, sizeof(tag->parentInvAxis));

	return 1;
}

int G_EntLinkToWithOffset(gentity_s *ent, gentity_s *parent, unsigned int tagName, const float *originOffset, const float *anglesOffset)
{
	tagInfo_s *tagInfo;

	if ( !G_EntLinkToInternal(ent, parent, tagName) )
		return 0;

	tagInfo = ent->tagInfo;
	AnglesToAxis(anglesOffset, tagInfo->axis);
	VectorCopy(originOffset, tagInfo->axis[3]);

	return 1;
}

void G_CalcTagParentAxis(gentity_s *ent, float (*parentAxis)[3])
{
	float *currentOrigin;
	tagInfo_s *tagInfo;
	DObj *obj;
	float tempAxis[4][3];
	gentity_s *parent;
	DObjAnimMat *mat;
	float axis[3][3];

	tagInfo = ent->tagInfo;
	parent = tagInfo->parent;
	obj = Com_GetServerDObj(parent->s.number);

	if ( tagInfo->index >= 0 && obj )
	{
		AnglesToAxis(parent->r.currentAngles, tempAxis);
		tempAxis[3][0] = parent->r.currentOrigin[0];
		tempAxis[3][1] = parent->r.currentOrigin[1];
		tempAxis[3][2] = parent->r.currentOrigin[2];
		G_DObjCalcBone(parent, tagInfo->index);
		mat = &SV_DObjGetMatrixArray(parent)[tagInfo->index];
		ConvertQuatToMat(mat, axis);
		MatrixMultiply(axis, tempAxis, parentAxis);
		MatrixTransformVector43(mat->trans, tempAxis, &(*parentAxis)[9]);
	}
	else
	{
		AnglesToAxis(parent->r.currentAngles, parentAxis);
		currentOrigin = parent->r.currentOrigin;
		(*parentAxis)[9] = parent->r.currentOrigin[0];
		(*parentAxis)[10] = currentOrigin[1];
		(*parentAxis)[11] = currentOrigin[2];
	}
}

void G_CalcTagAxis(gentity_s *ent, int bAnglesOnly)
{
	tagInfo_s *tagInfo;
	float invParentAxis[4][3];
	float parentAxis[4][3];
	float axis[4][3];

	G_CalcTagParentAxis(ent, parentAxis);
	AnglesToAxis(ent->r.currentAngles, axis);
	tagInfo = ent->tagInfo;

	if ( bAnglesOnly )
	{
		MatrixTranspose(parentAxis, invParentAxis);
		MatrixMultiply(axis, invParentAxis, tagInfo->axis);
	}
	else
	{
		MatrixInverseOrthogonal43(parentAxis, invParentAxis);
		axis[3][0] = ent->r.currentOrigin[0];
		axis[3][1] = ent->r.currentOrigin[1];
		axis[3][2] = ent->r.currentOrigin[2];
		MatrixMultiply43(axis, invParentAxis, tagInfo->axis);
	}
}

int G_EntLinkTo(gentity_s *ent, gentity_s *parent, unsigned int tagName)
{
	if ( !G_EntLinkToInternal(ent, parent, tagName) )
		return 0;

	G_CalcTagAxis(ent, 0);
	return 1;
}

void G_SetFixedLink(gentity_s *ent, int eAngles)
{
	tagInfo_s *tagInfo;
	float parentAxis[4][3];
	float axis[4][3];

	G_CalcTagParentAxis(ent, parentAxis);
	tagInfo = ent->tagInfo;

	if ( eAngles )
	{
		if ( eAngles == 1 )
		{
			MatrixMultiply43(tagInfo->axis, parentAxis, axis);
			ent->r.currentOrigin[0] = axis[3][0];
			ent->r.currentOrigin[1] = axis[3][1];
			ent->r.currentOrigin[2] = axis[3][2];
			ent->r.currentAngles[1] = vectoyaw(axis[0]);
		}
		else if ( eAngles == 2 )
		{
			MatrixTransformVector43(tagInfo->axis[3], parentAxis, axis[3]);
			ent->r.currentOrigin[0] = axis[3][0];
			ent->r.currentOrigin[1] = axis[3][1];
			ent->r.currentOrigin[2] = axis[3][2];
		}
	}
	else
	{
		MatrixMultiply43(tagInfo->axis, parentAxis, axis);
		ent->r.currentOrigin[0] = axis[3][0];
		ent->r.currentOrigin[1] = axis[3][1];
		ent->r.currentOrigin[2] = axis[3][2];
		AxisToAngles(axis, ent->r.currentAngles);
	}
}

void G_GeneralLink(gentity_s *ent)
{
	G_SetFixedLink(ent, 0);
	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);
	ent->s.pos.trType = TR_INTERPOLATE;
	ent->s.apos.trType = TR_INTERPOLATE;
	SV_LinkEntity(ent);
}

gentity_s* G_SpawnPlayerClone()
{
	int flags;
	gentity_s *entity;

	entity = &level.gentities[level.currentPlayerClone + 64];
	level.currentPlayerClone = (level.currentPlayerClone + 1) % 8;
	flags = entity->s.eFlags & 2 ^ 2;

	if ( entity->r.inuse )
		G_FreeEntity(entity);

	G_InitGentity(entity);
	entity->s.eFlags = flags;

	return entity;
}

gentity_s* G_Find(gentity_s *from, int fieldofs, unsigned short match)
{
	unsigned short s;
	gentity_s *i;

	if ( from )
		i = from + 1;
	else
		i = g_entities;

	while ( i < &g_entities[level.num_entities] )
	{
		if ( i->r.inuse )
		{
			s = *(uint16_t *)((char *)&i->s.number + fieldofs);

			if ( s )
			{
				if ( s == match )
					return i;
			}
		}

		++i;
	}

	return 0;
}

int G_GetFreePlayerCorpseIndex()
{
	int bestIndex;
	vec3_t playerPos;
	float bestDistSq;
	float vDistSq;
	gentity_s *ent;
	int i;

	bestDistSq = -1.0;
	bestIndex = 0;
	ent = G_Find(0, 360, scr_const.player);
	VectorCopy(ent->s.pos.trBase, playerPos);

	for ( i = 0; i < 8; ++i )
	{
		if ( g_scr_data.playerCorpseInfo[i].entnum == -1 )
			return i;

		ent = &level.gentities[g_scr_data.playerCorpseInfo[i].entnum];
		vDistSq = Vec3DistanceSq(ent->r.currentOrigin, playerPos);

		if ( vDistSq > bestDistSq )
		{
			bestDistSq = vDistSq;
			bestIndex = i;
		}
	}

	ent = &level.gentities[g_scr_data.playerCorpseInfo[bestIndex].entnum];
	G_FreeEntity(ent);
	g_scr_data.playerCorpseInfo[bestIndex].entnum = -1;

	return bestIndex;
}