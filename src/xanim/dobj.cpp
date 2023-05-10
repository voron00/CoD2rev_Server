#include "../qcommon/qcommon.h"
#include "../clientscript/clientscript_public.h"

unsigned int g_empty;

XAnimTree_s* DObjGetTree(const DObj_s *obj)
{
	return obj->tree;
}

int DObjHasContents(DObj_s *obj, int contentmask)
{
	int i;

	for ( i = 0; i < obj->numModels; ++i )
	{
		if ( (contentmask & XModelGetContents(obj->models[i])) != 0 )
			return 1;
	}

	return 0;
}

const char* DObjInfoGetBoneName(const DObj_s *dobj, int index)
{
	int numBones;
	XModel *model;
	int count;
	int i;

	count = 0;

	for ( i = 0; i < dobj->numModels; ++i )
	{
		model = dobj->models[i];
		numBones = model->modelParts->numBones;

		if ( index - count < numBones )
			return SL_ConvertToString((*model->modelParts->boneNames)[index - count]);

		count += numBones;
	}

	return 0;
}

void DObjDumpInfo(const DObj_s *obj)
{
	XModel *model;
	byte *partName;
	int bones;
	int numModels;
	int numBones;
	int i;
	int j;

	if ( obj )
	{
		Com_Printf("\nModels:\n");
		numModels = obj->numModels;
		bones = 0;

		for ( i = 0; i < numModels; ++i )
		{
			model = obj->models[i];
			Com_Printf("%d: '%s'\n", bones, model->name);
			bones += model->modelParts->numBones;
		}

		Com_Printf("\nBones:\n");
		numBones = obj->numBones;

		for ( j = 0; j < numBones; ++j )
		{
			Com_Printf("Bone %d: '%s'\n", j, DObjInfoGetBoneName(obj, j));
		}

		if ( obj->duplicateParts )
		{
			Com_Printf("\nPart duplicates:\n");

			for ( partName = (byte *)(SL_ConvertToString(obj->duplicateParts) + 16); *partName; partName += 2 )
			{
				Com_Printf("%d ('%s') -> %d ('%s')\n", *partName - 1, DObjInfoGetBoneName(obj, *partName - 1), partName[1] - 1, DObjInfoGetBoneName(obj, partName[1] - 1));
			}
		}
		else
		{
			Com_Printf("\nNo part duplicates.\n");
		}

		Com_Printf("\n");
	}
	else
	{
		Com_Printf("No Dobj\n");
	}
}

void DObjGeomTraceline(DObj_s *obj, float *localStart, float *localEnd, int contentmask, DObjTrace_s *results)
{
	int boneIndex;
	unsigned short *boneName;
	int i;
	DObjAnimMat *pose;
	XModelParts_s *modelParts;
	XModel *model;
	trace_t trace;

	results->partName = 0;
	results->partGroup = 0;
	trace.fraction = results->fraction;
	trace.surfaceFlags = 0;
	VectorClear(trace.normal);

	pose = DObjGetRotTransArray(obj);

	if ( pose )
	{
		for ( i = 0; i < obj->numModels; ++i )
		{
			model = obj->models[i];
			modelParts = model->modelParts;
			boneName = *model->modelParts->boneNames;
			boneIndex = XModelTraceLine(model, &trace, pose, localStart, localEnd, contentmask);

			if ( boneIndex >= 0 )
				results->partName = boneName[boneIndex];

			pose += modelParts->numBones;
		}
	}

	results->fraction = trace.fraction;
	results->surfaceflags = trace.surfaceFlags;

	VectorCopy(trace.normal, results->normal);
}

void DObjGetBounds(const DObj_s *obj, float *mins, float *maxs)
{
	VectorCopy(obj->mins, mins);
	VectorCopy(obj->maxs, maxs);
}

void DObjSetBounds(DObj *obj)
{
	vec3_t dobjmaxs;
	vec3_t dobjmins;
	vec3_t modelmaxs;
	vec3_t modelmins;
	int i;

	VectorSet(dobjmins, 0, 0, 0);
	VectorSet(dobjmaxs, 0, 0, 0);

	for ( i = 0; i < obj->numModels; ++i )
	{
		if ( obj->models[i] )
		{
			XModelGetBounds(obj->models[i], modelmins, modelmaxs);

			VectorAdd(dobjmins, modelmins, dobjmins);
			VectorAdd(dobjmaxs, modelmaxs, dobjmaxs);
		}
	}

	VectorCopy(dobjmins, obj->mins);
	VectorCopy(dobjmaxs, obj->maxs);
}

void DObjCreate(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, void *buf, unsigned int entnum)
{
	DObj *newobj;
	int modelIndex;
	int boneIndex;
	unsigned int name;
	const char *parentName;
	XModel *model;
	int numBones;
	int j;
	unsigned int i;

	newobj = (DObj *)buf;
	newobj->skel.skelPart = 0;
	newobj->skel.timeStamp = 0;
	newobj->duplicateParts = 0;
	newobj->flags = 0;

	DObjSetTree(newobj, tree);

	if ( tree )
		tree->parent = entnum;

	modelIndex = 0;
	numBones = 0;

	for ( i = 0; i < numModels; ++i )
	{
		model = dobjModels->model;

		newobj->models[modelIndex] = dobjModels->model;
		newobj->modelIndex[modelIndex] = -1;
		newobj->boneIndex[modelIndex] = numBones;

		if ( dobjModels->ignoreCollision )
			newobj->flags |= 1 << i;

		if ( i )
		{
			parentName = dobjModels->parentModelName;

			if ( parentName )
			{
				if ( *parentName )
				{
					name = SL_FindString(parentName);

					if ( name )
					{
						for ( j = 0; j < modelIndex; ++j )
						{
							boneIndex = XModelGetBoneIndex(newobj->models[j], name);

							if ( boneIndex >= 0 )
							{
								newobj->modelIndex[modelIndex] = newobj->boneIndex[j] + boneIndex;
								goto setmodel;
							}
						}
					}

					Com_Printf(
					    "WARNING: Part '%s' not found in model '%s' or any of its descendants\n",
					    parentName,
					    newobj->models[0]->name);
				}
			}
		}
setmodel:
		if ( model )
		{
			if ( numBones + model->modelParts->numBones > 127 )
				Com_Error(ERR_DROP, "dobj for xmodel %s has more than %d bones", newobj->models[0]->name, 127);

			numBones += model->modelParts->numBones;
		}

		++modelIndex;
		++dobjModels;
	}

	newobj->numModels = modelIndex;
	newobj->numBones = numBones;

	DObjSetBounds(newobj);
}

void DObjFree(DObj *obj)
{
	const char *string;
	size_t len;

	if ( obj->tree )
	{
		obj->duplicatePartsIndexes = 0;
		obj->tree = 0;
	}

	if ( obj->duplicateParts )
	{
		if ( obj->duplicateParts != g_empty )
		{
			string = SL_ConvertToString(obj->duplicateParts);
			len = I_strlen((string + 16));
			SL_RemoveRefToStringOfLen(obj->duplicateParts, len + 17);
		}

		obj->duplicateParts = 0;
	}
}

void DObjInit()
{
	char s[20];

	memset(s, 0, sizeof(s));
	g_empty = SL_GetStringOfLen(s, 0, 17);
}

void DObjShutdown()
{
	if ( g_empty )
	{
		SL_RemoveRefToStringOfLen(g_empty, 17);
		g_empty = 0;
	}
}

void DObjAbort()
{
	g_empty = 0;
}