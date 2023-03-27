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

void DObjGeomTraceline(DObj_s *obj, float *localStart, float *localEnd, int contentmask, DObjTrace_s *results)
{
	int boneIndex;
	unsigned short *boneName;
	int i;
	DObjAnimMat *pose;
	XModelParts_s *modelParts;
	XModel *model;
	trace_t trace;

	results->modelIndex = 0;
	results->partName = 0;
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
				results->modelIndex = boneName[boneIndex];

			pose += modelParts->numBones;
		}
	}

	results->fraction = trace.fraction;
	results->sflags = trace.surfaceFlags;

	VectorCopy(trace.normal, results->normal);
}

void QDECL DObjCreateDuplicateParts(DObj *obj)
{
	const char *name;
	int buffer[4];
	byte localbits;
	XModelParts *modelParts;
	int nextBone;
	char partIndex;
	int bitcount;
	byte *bits;
	unsigned short *nameIndex;
	unsigned short **boneNames;
	int boneIter;
	int i;
	int boneIndex;
	XModel *xmodel;
	int numBones;
	int numModels;

	bits = &localbits;
	memset(buffer, 0, sizeof(buffer));
	bitcount = 0;
	numBones = obj->models[0]->modelParts->numBones;
	numModels = 1;

	while ( numModels < obj->numModels )
	{
		xmodel = obj->models[numModels];

		if ( obj->modelIndex[numModels] == 0 )
		{
			modelParts = xmodel->modelParts;
			boneNames = modelParts->boneNames;
			nameIndex = *boneNames;
			boneIter = modelParts->numBones;
			partIndex = 0;
			boneIndex = -1;

			for ( i = 0; i < boneIter; ++i )
			{
				boneIndex = DObjGetBoneIndex(obj, nameIndex[i]);

				if ( boneIndex != numBones + i )
				{
					if ( !i )
						partIndex = 1;

					nextBone = numBones + i;
					bits[bitcount] = numBones + i + 1;
					buffer[nextBone >> 5] |= 1 << (nextBone & 0x1F);
					bits[++bitcount] = boneIndex + 1;
					++bitcount;
				}
			}

			if ( !partIndex )
			{
				name = SL_ConvertToString(*nameIndex);
				Com_Printf(
				    "WARNING: Attempting to meld model, but root part '%s' of model '%s' not found in model '%s' or any of its descendants\n",
				    name,
				    xmodel->name,
				    obj->models[0]->name);
			}
		}

		++numModels;
		numBones += xmodel->modelParts->numBones;
	}

	if ( bitcount )
	{
		bits[bitcount] = 0;
		obj->duplicateParts = SL_GetStringOfLen((const char *)buffer, 0, bitcount + 17);
	}
	else
	{
		obj->duplicateParts = g_empty;
	}
}

void DObjSetTree(DObj *obj, XAnimTree *tree)
{
	byte next;
	unsigned short *part;
	unsigned int size;

	obj->tree = tree;

	if ( tree )
	{
		size = tree->anims->size;
		obj->duplicatePartsIndexes = &tree->children[size];
		part = &obj->duplicatePartsIndexes[size];
		next = *(byte *)part + 1;

		if ( *(byte *)part == 0 )
		{
			next = 1;
			memset((char *)part + 1, 0, size);
		}

		*(byte *)part = next;
	}
	else
	{
		obj->duplicatePartsIndexes = 0;
	}
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
								goto OUT;
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
OUT:
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

void DObjGetHierarchyBits(DObj_s *obj, int boneIndex, int *partBits)
{
	const char *name;
	int bones[8];
	int current;
	const char *next2;
	int index;
	unsigned short **parts;
	const char *next;
	XModelParts_s *modelParts;
	int i;
	int numModels;

	for ( i = 0; i < 4; ++i )
		partBits[i] = 0;

	numModels = obj->numModels;

	if ( !obj->duplicateParts )
		DObjCreateDuplicateParts(obj);

	name = SL_ConvertToString(obj->duplicateParts);
	next = name + 16;
	next2 = name + 16;
	bones[0] = 0;

	for ( i = 0; ; bones[i] = current )
	{
		modelParts = obj->models[i]->modelParts;
		current = bones[i] + modelParts->numBones;

		if ( current > boneIndex )
		{
			for ( parts = modelParts->boneNames + 1; ; boneIndex -= *((unsigned char *)parts + current) )
			{
				index = boneIndex - bones[i];

				while ( 1 )
				{
					partBits[boneIndex >> 5] |= 1 << (boneIndex & 0x1F);

					if ( (((unsigned char)(*(int *)&name[4 * (boneIndex >> 5)] >> (boneIndex & 0x1F)) ^ 1) & 1) == 0 )
					{
						for ( next2 = next; boneIndex != *(unsigned char *)next2 - 1; next2 += 2 )
							;
						boneIndex = *((unsigned char *)next2 + 1) - 1;
						goto skip;
					}

					current = index - modelParts->numRootBones;

					if ( current >= 0 )
						break;

					boneIndex = obj->modelIndex[i];

					if ( boneIndex == 255 )
						return;
					do
skip:
						index = boneIndex - bones[--i];
					while ( index < 0 );

					modelParts = obj->models[i]->modelParts;
					parts = modelParts->boneNames + 1;
				}
			}
		}

		if ( ++i == numModels )
			break;
	}
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