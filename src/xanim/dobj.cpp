#include "../qcommon/qcommon.h"
#include "../clientscript/clientscript_public.h"

unsigned int g_empty;

void QuatMultiplyEquals(const float *in, float *inout)
{
	float temp[3];

	temp[0] =  in[0] * inout[3] + in[3] * inout[0] + in[2] * inout[1] - in[1] * inout[2];
	temp[1] =  in[1] * inout[3] - in[2] * inout[0] + in[3] * inout[1] + in[0] * inout[2];
	temp[2] =  in[2] * inout[3] + in[1] * inout[0] - in[0] * inout[1] + in[3] * inout[2];

	inout[3] = in[3] * inout[3] - in[0] * inout[0] - in[1] * inout[1] - in[2] * inout[2];
	inout[0] = temp[0];
	inout[1] = temp[1];
	inout[2] = temp[2];
}

void QuatMultiplyReverseEquals(float *in, float *inout)
{
	float temp[3];

	temp[0] = in[0] * inout[3] + in[3] * inout[0] + in[2] * inout[1] - in[1] * inout[2];
	temp[1] = in[1] * inout[3] - in[2] * inout[0] + in[3] * inout[1] + in[0] * inout[2];
	temp[2] = in[2] * inout[3] + in[1] * inout[0] - in[0] * inout[1] + in[3] * inout[2];

	in[3] =   in[3] * inout[3] - in[0] * inout[0] - in[1] * inout[1] - in[2] * inout[2];
	in[0] = temp[0];
	in[1] = temp[1];
	in[2] = temp[2];
}

void MatrixTransformVectorQuatTransEquals(float *inout, DObjAnimMat *in)
{
	float axis[3][3];
	float temp[2];

	ConvertQuatToMat(in, axis);

	temp[0] =  inout[0] * axis[0][0] + inout[1] * axis[1][0] + inout[2] * axis[2][0] + in->trans[0];
	temp[1] =  inout[0] * axis[0][1] + inout[1] * axis[1][1] + inout[2] * axis[2][1] + in->trans[1];

	inout[2] = inout[0] * axis[0][2] + inout[1] * axis[1][2] + inout[2] * axis[2][2] + in->trans[2];
	inout[0] = temp[0];
	inout[1] = temp[1];
}

void DObjCalcTransWeight(DObjAnimMat *Mat)
{
	float vLenSq;

	vLenSq = Vec4LengthSq(Mat->quat);

	if ( vLenSq == 0.0 )
	{
		Mat->quat[3] = 1.0;
		Mat->transWeight = 2.0;
	}
	else
	{
		Mat->transWeight = 2.0 / vLenSq;
	}
}

int DObjGetBoneIndexInternal(const DObj_s *obj, unsigned int name)
{
	int bone;
	XModel *model;
	int index;
	int i;
	int numModels;

	numModels = obj->numModels;
	index = 0;

	for ( i = 0; i < numModels; ++i )
	{
		model = obj->models[i];
		bone = XModelGetBoneIndex(model, name);

		if ( bone >= 0 )
			return index + bone;

		index += model->parts->numBones;
	}

	return -1;
}

void DObjCreateDuplicateParts(DObj_s *obj)
{
	static int duplicatePartBits[4]; // VoroN: let's not smash the stack?
	int index;
	bool bRootMeld;
	int len;
	byte *duplicateParts;
	unsigned short *name;
	int boneIter;
	int localBoneIndex;
	int boneIndex;
	XModel *model;
	int numBones;
	int boneCount;

	duplicateParts = (byte *)&duplicatePartBits[4]; // VoroN: not sure about that, it could be duplicatePartBits[3]?
	memset(duplicatePartBits, 0, sizeof(duplicatePartBits));
	len = 0;
	numBones = obj->models[0]->parts->numBones;
	boneCount = 1;

	while ( boneCount < obj->numModels )
	{
		model = obj->models[boneCount];

		if ( obj->modelParents[boneCount] == 0xFF )
		{
			name = model->parts->hierarchy[0];
			boneIter = model->parts->numBones;
			bRootMeld = 0;
			boneIndex = -1;

			for ( localBoneIndex = 0; localBoneIndex < boneIter; ++localBoneIndex )
			{
				boneIndex = DObjGetBoneIndexInternal(obj, name[localBoneIndex]);

				if ( boneIndex != numBones + localBoneIndex )
				{
					if ( !localBoneIndex )
						bRootMeld = 1;

					index = numBones + localBoneIndex;
					duplicateParts[len] = numBones + localBoneIndex + 1;
					duplicatePartBits[index >> 5] |= 1 << (index & 0x1F);
					duplicateParts[++len] = boneIndex + 1;
					++len;
				}
			}

			if ( !bRootMeld )
			{
				Com_Printf(
				    "WARNING: Attempting to meld model, but root part '%s' of model '%s' not found in model '%s' or any of its descendants\n",
				    SL_ConvertToString(name[0]),
				    model->name,
				    obj->models[0]->name);
			}
		}

		++boneCount;
		numBones += model->parts->numBones;
	}

	if ( len )
	{
		duplicateParts[len] = 0;
		obj->duplicateParts = SL_GetStringOfLen((const char *)duplicatePartBits, 0, len + 17);
	}
	else
	{
		obj->duplicateParts = g_empty;
	}
}

void DObjCalcSkel(DObj_s *obj, int *partBits)
{
	float *quat;
	const int *savedDuplicatePartBits;
	char bFinished;
	DSkel_t *skel;
	DObjAnimMat *childMat;
	const unsigned char *duplicateParts;
	int numModels;
	int boneBit;
	int boneIndexHigh;
	int boneIndex;
	short *quats;
	int calcPartBits[4];
	int ignorePartBits[4];
	int controlPartBits[4];
	float *trans;
	DObjAnimMat *Mat;
	unsigned char parent;
	byte *parentList;
	XModelParts_s *parts;
	int i;
	int numRootBones;
	DObjAnimMat *parentMat;

	skel = obj->skel;
	bFinished = 1;

	for ( numRootBones = 0; numRootBones < 4; ++numRootBones )
	{
		ignorePartBits[numRootBones] = skel->partBits.skel[numRootBones] | ~partBits[numRootBones];

		if ( ignorePartBits[numRootBones] != -1 )
			bFinished = 0;
	}

	if ( !bFinished )
	{
		if ( !obj->duplicateParts )
			DObjCreateDuplicateParts(obj);

		savedDuplicatePartBits = (const int *)SL_ConvertToString(obj->duplicateParts);

		for ( numRootBones = 0; numRootBones < 4; ++numRootBones )
		{
			skel->partBits.skel[numRootBones] |= partBits[numRootBones];
			controlPartBits[numRootBones] = skel->partBits.control[numRootBones] & ~ignorePartBits[numRootBones];
			calcPartBits[numRootBones] = savedDuplicatePartBits[numRootBones] | controlPartBits[numRootBones] | ignorePartBits[numRootBones];
		}

		for ( numRootBones = 0; numRootBones < 4; ++numRootBones )
			controlPartBits[numRootBones] |= ~calcPartBits[numRootBones];

		numModels = obj->numModels;
		Mat = &skel->Mat;
		parentMat = &skel->Mat;
		boneIndex = 0;
		duplicateParts = (const unsigned char *)(savedDuplicatePartBits + 4);

		for ( i = 0; i < numModels; ++i )
		{
			parts = obj->models[i]->parts;
			parent = obj->modelParents[i];

			if ( parent == 0xFF )
			{
				numRootBones = parts->numRootBones;

				while ( numRootBones )
				{
					boneBit = 1 << (boneIndex & 0x1F);

					if ( (controlPartBits[boneIndex >> 5] & boneBit) != 0 )
					{
						DObjCalcTransWeight(parentMat);
					}
					else if ( boneIndex == *duplicateParts - 1 )
					{
						duplicateParts += 2;

						if ( (ignorePartBits[boneIndex >> 5] & boneBit) == 0 )
						{
							quat = Mat[*(duplicateParts - 1) - 1].quat;

							parentMat->quat[0] = quat[0];
							parentMat->quat[1] = quat[1];
							parentMat->quat[2] = quat[2];
							parentMat->quat[3] = quat[3];
							parentMat->trans[0] = quat[4];
							parentMat->trans[1] = quat[5];
							parentMat->trans[2] = quat[6];
							parentMat->transWeight = quat[7];
						}
					}

					--numRootBones;
					++parentMat;
					++boneIndex;
				}
			}
			else
			{
				childMat = &Mat[parent];
				numRootBones = parts->numRootBones;

				while ( numRootBones )
				{
					boneBit = 1 << (boneIndex & 0x1F);

					if ( (controlPartBits[boneIndex >> 5] & boneBit) != 0 )
					{
						if ( (boneBit & calcPartBits[boneIndex >> 5]) != 0 )
							QuatMultiplyEquals(childMat->quat, parentMat->quat);
						else
							QuatMultiplyReverseEquals(parentMat->quat, childMat->quat);

						DObjCalcTransWeight(parentMat);
						MatrixTransformVectorQuatTransEquals(parentMat->trans, childMat);
					}

					--numRootBones;
					++parentMat;
					++boneIndex;
				}
			}

			quats = parts->quats;
			trans = parts->trans;
			parentList = (byte *)(parts->hierarchy + 1);
			numRootBones = parts->numBones - parts->numRootBones;

			while ( numRootBones )
			{
				boneIndexHigh = boneIndex >> 5;
				boneBit = 1 << (boneIndex & 0x1F);

				if ( (controlPartBits[boneIndex >> 5] & boneBit) != 0 )
				{
					if ( (boneBit & calcPartBits[boneIndexHigh]) != 0 )
						QuatMultiplyEquals(parentMat[-*parentList].quat, parentMat->quat);
					else
						QuatMultiplyReverseEquals(parentMat->quat, parentMat[-*parentList].quat);

					DObjCalcTransWeight(parentMat);
					VectorAdd(parentMat->trans, trans, parentMat->trans);
					MatrixTransformVectorQuatTransEquals(parentMat->trans, &parentMat[-*parentList]);
				}
				else if ( boneIndex == *duplicateParts - 1 )
				{
					duplicateParts += 2;

					if ( (ignorePartBits[boneIndexHigh] & boneBit) == 0 )
					{
						quat = Mat[*(duplicateParts - 1) - 1].quat;

						parentMat->quat[0] = quat[0];
						parentMat->quat[1] = quat[1];
						parentMat->quat[2] = quat[2];
						parentMat->quat[3] = quat[3];
						parentMat->trans[0] = quat[4];
						parentMat->trans[1] = quat[5];
						parentMat->trans[2] = quat[6];
						parentMat->transWeight = quat[7];
					}
				}

				--numRootBones;
				++parentMat;
				quats += 4;
				trans += 3;
				++parentList;
				++boneIndex;
			}
		}
	}
}

XAnimTree_s* DObjGetTree(const DObj_s *obj)
{
	return obj->tree;
}

void DObjSetTree(DObj_s *obj, XAnimTree_s *tree)
{
	byte model;
	byte *parent;
	unsigned int animTreeSize;

	obj->tree = tree;

	if ( tree )
	{
		animTreeSize = tree->anims->size;
		obj->animToModel = &tree->infoArray[animTreeSize];
		parent = (byte *)&obj->animToModel[animTreeSize];
		model = *parent + 1;

		if ( *parent == 0xFF )
		{
			model = 1;
			memset(parent + 1, 0, animTreeSize);
		}

		*parent = model;
	}
	else
	{
		obj->animToModel = 0;
	}
}

void DObjGetHierarchyBits(DObj_s *obj, int boneIndex, int *partBits)
{
	const int *duplicatePartBits;
	int startIndex[8];
	int newBoneIndex;
	const unsigned char *pos;
	int localBoneIndex;
	byte *parentList;
	const unsigned char *duplicateParts;
	XModelParts_s *parts;
	int i;
	int numModels;

	for ( i = 0; i < 4; ++i )
		partBits[i] = 0;

	numModels = obj->numModels;

	if ( !obj->duplicateParts )
		DObjCreateDuplicateParts(obj);

	duplicatePartBits = (const int *)SL_ConvertToString(obj->duplicateParts);
	duplicateParts = (const unsigned char *)(duplicatePartBits + 4);
	pos = (const unsigned char *)(duplicatePartBits + 4);

	startIndex[0] = 0;

	for ( i = 0; ; startIndex[i] = newBoneIndex )
	{
		parts = obj->models[i]->parts;
		newBoneIndex = startIndex[i] + parts->numBones;

		if ( newBoneIndex > boneIndex )
		{
			for ( parentList = (byte *)(parts->hierarchy + 1); ; boneIndex -= parentList[newBoneIndex] )
			{
				localBoneIndex = boneIndex - startIndex[i];

				while ( 1 )
				{
					partBits[boneIndex >> 5] |= 1 << (boneIndex & 0x1F);

					if ( (((byte)(duplicatePartBits[boneIndex >> 5] >> (boneIndex & 0x1F)) ^ 1) & 1) == 0 )
					{
						for ( pos = duplicateParts; boneIndex != *pos - 1; pos += 2 )
							;
						boneIndex = pos[1] - 1;
						goto out;
					}

					newBoneIndex = localBoneIndex - parts->numRootBones;

					if ( newBoneIndex >= 0 )
						break;

					boneIndex = obj->modelParents[i];

					if ( boneIndex == 255 )
						return;
					do
out:
						localBoneIndex = boneIndex - startIndex[--i];
					while ( localBoneIndex < 0 );

					parts = obj->models[i]->parts;
					parentList = (byte *)(parts->hierarchy + 1);
				}
			}
		}

		if ( ++i == numModels )
			break;
	}
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

const char* DObjGetBoneName(const DObj_s *dobj, int index)
{
	int numBones;
	XModel *model;
	int count;
	int i;

	count = 0;

	for ( i = 0; i < dobj->numModels; ++i )
	{
		model = dobj->models[i];
		numBones = model->parts->numBones;

		if ( index - count < numBones )
			return SL_ConvertToString((*model->parts->hierarchy)[index - count]);

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
			bones += model->parts->numBones;
		}

		Com_Printf("\nBones:\n");
		numBones = obj->numBones;

		for ( j = 0; j < numBones; ++j )
		{
			Com_Printf("Bone %d: '%s'\n", j, DObjGetBoneName(obj, j));
		}

		if ( obj->duplicateParts )
		{
			Com_Printf("\nPart duplicates:\n");

			for ( partName = (byte *)(SL_ConvertToString(obj->duplicateParts) + 16); *partName; partName += 2 )
			{
				Com_Printf("%d ('%s') -> %d ('%s')\n", *partName - 1, DObjGetBoneName(obj, *partName - 1), partName[1] - 1, DObjGetBoneName(obj, partName[1] - 1));
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
	XModelParts_s *parts;
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
			parts = model->parts;
			boneName = *model->parts->hierarchy;
			boneIndex = XModelTraceLine(model, &trace, pose, localStart, localEnd, contentmask);

			if ( boneIndex >= 0 )
				results->partName = boneName[boneIndex];

			pose += parts->numBones;
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
	newobj->skel = 0;
	newobj->timeStamp = 0;
	newobj->duplicateParts = 0;
	newobj->ignoreCollision = 0;

	DObjSetTree(newobj, tree);

	if ( tree )
		tree->entnum = entnum;

	modelIndex = 0;
	numBones = 0;

	for ( i = 0; i < numModels; ++i )
	{
		model = dobjModels->model;

		newobj->models[modelIndex] = dobjModels->model;
		newobj->modelParents[modelIndex] = -1;
		newobj->matOffset[modelIndex] = numBones;

		if ( dobjModels->ignoreCollision )
			newobj->ignoreCollision |= 1 << i;

		if ( i )
		{
			parentName = dobjModels->boneName;

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
								newobj->modelParents[modelIndex] = newobj->matOffset[j] + boneIndex;
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
			if ( numBones + model->parts->numBones > 127 )
				Com_Error(ERR_DROP, "dobj for xmodel %s has more than %d bones", newobj->models[0]->name, 127);

			numBones += model->parts->numBones;
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
		obj->animToModel = 0;
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