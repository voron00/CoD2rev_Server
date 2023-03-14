#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define g_xAnimInfo (((XAnimInfo*)( 0x08527A80 )))
#else
extern XAnimInfo g_xAnimInfo[];
#endif

void XAnimCalcClearQuat(DObjAnimMat *Mat)
{
	Mat->quat[0] = 0.0;
	Mat->quat[1] = 0.0;
	Mat->quat[2] = 0.0;
	Mat->quat[3] = 0.0;
}

void XAnimCalcScaleQuats(int16_t *from, float scale, float *to)
{
	to[0] = (float)from[0] * scale;
	to[1] = (float)from[1] * scale;
	to[2] = (float)from[2] * scale;
	to[3] = (float)from[3] * scale;
}

void XanimCalcClearInfo(const DObj_s *obj, DObjAnimMat *rotTransArray, XAnimCalcAnimInfo *animInfo)
{
	int i;

	for ( i = 0; i < obj->numBones; ++i )
	{
		if ( ((animInfo->ignorePartBits[(i >> 5) + 1] >> (i & 0x1F)) & 1) == 0 )
		{
			XAnimCalcClearQuat(rotTransArray);
			rotTransArray->transWeight = 0.0;
			VectorClear(rotTransArray->trans);
		}

		++rotTransArray;
	}
}

void XAnimCalc(const DObj_s *obj, unsigned int entry, float weightScale, DObjAnimMat *rotTransArray, bool bClear, bool bNormQuat, XAnimCalcAnimInfo *animInfo, int buffer)
{
	unsigned short *modelNames;
	unsigned short *duplicateParts;
	float scale;
	float scale3;
	float scale4;
	float scale2;
	float time;
	XAnimParts_s *parts;
	const char *partName;
	XAnimTree_s *tree;
	byte *part;
	XAnimEntry *animEntry;
	uint16_t childIndex;
	uint16_t childIndex2;
	uint16_t childIndex3;
	float scaleWeight;
	float len2;
	float len4;
	DObjAnimMat *animMat;
	int numAnims;
	int j;
	int k;
	int jj;
	int i;
	int ii;
	int m;
	int n;
	float weight;
	float weight2;
	float childWeight;

	tree = obj->tree;
	animEntry = &obj->tree->anims->entries[entry];
	numAnims = animEntry->numAnims;

	if ( animEntry->numAnims )
	{
		for ( i = 0; ; ++i )
		{
			if ( i >= numAnims )
			{
				if ( bClear )
					XanimCalcClearInfo(obj, rotTransArray, animInfo);

				return;
			}

			childIndex = tree->children[i + obj->tree->anims->entries[entry].u.animParent.children];

			if ( childIndex )
			{
				weight = g_xAnimInfo[childIndex].state.weight;

				if ( weight != 0.0 )
					break;
			}
		}

		for ( j = i + 1; ; ++j )
		{
			if ( j >= numAnims )
			{
				XAnimCalc(
				    obj,
				    i + obj->tree->anims->entries[entry].u.animParent.children,
				    weightScale,
				    rotTransArray,
				    bClear,
				    bNormQuat,
				    animInfo,
				    buffer);
				return;
			}

			childIndex2 = tree->children[j + obj->tree->anims->entries[entry].u.animParent.children];

			if ( childIndex2 )
			{
				weight2 = g_xAnimInfo[childIndex2].state.weight;

				if ( weight2 != 0.0 )
					break;
			}
		}

		if ( bClear )
		{
			animMat = rotTransArray;
		}
		else
		{
			animMat = &animInfo->rotTransArray[buffer];
			buffer += obj->numBones;

			if ( buffer > 512 )
			{
				Com_Printf("MAX_CALC_ANIM_BUFFER exceeded\n");
				return;
			}
		}

		XAnimCalc(obj, i + obj->tree->anims->entries[entry].u.animParent.children, weight, animMat, 1, 1, animInfo, buffer);
		XAnimCalc(obj, j + animEntry->u.animParent.children, weight2, animMat, 0, 1, animInfo, buffer);

		for ( k = j + 1; k < numAnims; ++k )
		{
			childIndex3 = tree->children[k + animEntry->u.animParent.children];

			if ( childIndex3 )
			{
				childWeight = g_xAnimInfo[childIndex3].state.weight;

				if ( childWeight != 0.0 )
					XAnimCalc(obj, k + animEntry->u.animParent.children, childWeight, animMat, 0, 1, animInfo, buffer);
			}
		}

		if ( bNormQuat )
		{
			if ( bClear )
			{
				for ( m = 0; m < obj->numBones; ++m )
				{
					if ( ((animInfo->ignorePartBits[(m >> 5) + 1] >> (m & 0x1F)) & 1) == 0 )
					{
						len2 = Vec4LengthSq(rotTransArray->quat);

						if ( len2 != 0.0 )
						{
							scale = XAnimCalc_GetLowestElement(len2) * weightScale;
							VectorScale4(rotTransArray->quat, scale, rotTransArray->quat);
						}

						if ( rotTransArray->transWeight != 0.0 )
						{
							scale3 = weightScale / rotTransArray->transWeight;
							VectorScale(rotTransArray->trans, scale3, rotTransArray->trans);
							rotTransArray->transWeight = weightScale;
						}
					}

					++rotTransArray;
				}
			}
			else
			{
				for ( n = 0; n < obj->numBones; ++n )
				{
					if ( ((animInfo->ignorePartBits[(n >> 5) + 1] >> (n & 0x1F)) & 1) == 0 )
					{
						len4 = Vec4LengthSq(animMat->quat);

						if ( len4 != 0.0 )
						{
							scale4 = XAnimCalc_GetLowestElement(len4) * weightScale;
							VectorMA(rotTransArray->quat, scale4, animMat->quat, rotTransArray->quat);
						}

						if ( animMat->transWeight != 0.0 )
						{
							scale2 = weightScale / animMat->transWeight;
							VectorMA(rotTransArray->trans, scale2, animMat->trans, rotTransArray->trans);
							rotTransArray->transWeight = rotTransArray->transWeight + weightScale;
						}
					}

					++rotTransArray;
					++animMat;
				}
			}
		}
		else
		{
			for ( ii = 0; ii < obj->numBones; ++ii )
			{
				if ( ((animInfo->ignorePartBits[(ii >> 5) + 1] >> (ii & 0x1F)) & 1) == 0 && rotTransArray->transWeight != 0.0 )
				{
					scaleWeight = 1.0 / rotTransArray->transWeight;
					VectorScale4(rotTransArray->quat, scaleWeight, rotTransArray->quat);
					VectorScale(rotTransArray->trans, scaleWeight, rotTransArray->trans);
				}

				++rotTransArray;
			}
		}
	}
	else
	{
		if ( bClear )
			XanimCalcClearInfo(obj, rotTransArray, animInfo);

		part = (byte *)&obj->duplicateParts[tree->anims->size];

		if ( obj->duplicateParts[entry] )
		{
			if ( part[entry + 1] != *part )
			{
				part[entry + 1] = *part;
				SL_RemoveRefToStringOfLen(obj->duplicateParts[entry], animEntry->u.parts->boneCount + 16);
				duplicateParts = obj->duplicateParts;
				duplicateParts[entry] = XAnimSetModel(animEntry, obj->models, obj->numModels);
			}
		}
		else
		{
			part[entry + 1] = *part;
			modelNames = obj->duplicateParts;
			modelNames[entry] = XAnimSetModel(animEntry, obj->models, obj->numModels);
		}

		partName = SL_ConvertToString(obj->duplicateParts[entry]);

		for ( jj = 0; jj <= 3; ++jj )
			animInfo->animPartBits[jj] |= *(_DWORD *)&partName[4 * jj] & ~animInfo->ignorePartBits[jj + 1];

		parts = animEntry->u.parts;
		time = g_xAnimInfo[tree->children[entry]].state.time;

		if ( time != 1.0 && parts->numframes )
		{
			if ( parts->numframes > 0xFFu )
				XAnimCalcParts(
				    parts,
				    (byte *)partName + 16,
				    time,
				    weightScale,
				    rotTransArray,
				    &animInfo->ignorePartBits[1]);
			else
				XAnimCalcParts2(
				    parts,
				    (byte *)partName + 16,
				    time,
				    weightScale,
				    rotTransArray,
				    &animInfo->ignorePartBits[1]);
		}
		else
		{
			XAnimCalcNonLoopEnd(
			    parts,
			    (byte *)partName + 16,
			    weightScale,
			    rotTransArray,
			    &animInfo->ignorePartBits[1]);
		}
	}
}

void DObjCalcAnim(const DObj_s *obj, int *partBits)
{
	XAnimCalcAnimInfo animInfo;
	bool noPartBits;
	int bitcount;
	int16_t *quats;
	XModelParts_s *modelParts;
	int j;
	DObjAnimMat *rotTransArray;
	DSkelPartBits_s *skelPartBits;
	int i;

	skelPartBits = obj->skel.partBits;
	noPartBits = 1;

	for ( i = 0; i <= 3; ++i )
	{
		animInfo.animPartBits[i] = skelPartBits->anim[i] | ~partBits[i];

		if ( animInfo.animPartBits[i] != -1 )
			noPartBits = 0;
	}

	if ( !noPartBits )
	{
		for ( i = 0; i <= 3; ++i )
		{
			skelPartBits->anim[i] |= partBits[i];
			animInfo.ignorePartBits[i] = animInfo.animPartBits[i];
		}

		rotTransArray = &skelPartBits->Mat;

		if ( obj->tree )
		{
			animInfo.ignorePartBits[3] |= 0x80000000;
			XAnimCalc(obj, 0, 1.0, rotTransArray, 1, 0, &animInfo, 0);
		}

		bitcount = 0;

		for ( j = 0; j < obj->numModels; ++j )
		{
			modelParts = obj->models[j]->modelParts;
			i = modelParts->numRootBones;

			while ( i )
			{
				if ( ((animInfo.animPartBits[bitcount >> 5] >> (bitcount & 0x1F)) & 1) == 0 )
				{
					rotTransArray->quat[0] = 0.0;
					rotTransArray->quat[1] = 0.0;
					rotTransArray->quat[2] = 0.0;
					rotTransArray->quat[3] = 1.0;
					VectorClear(rotTransArray->trans);
				}

				--i;
				++rotTransArray;
				++bitcount;
			}

			quats = modelParts->quats;
			i = modelParts->numBones - modelParts->numRootBones;

			while ( i )
			{
				if ( ((animInfo.animPartBits[bitcount >> 5] >> (bitcount & 0x1F)) & 1) == 0 )
				{
					XAnimCalcScaleQuats(quats, 0.000030518509, rotTransArray->quat);
					VectorClear(rotTransArray->trans);
				}

				--i;
				++rotTransArray;
				++bitcount;
				quats += 4;
			}
		}
	}
}