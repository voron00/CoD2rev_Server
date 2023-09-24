#include "../qcommon/qcommon.h"

void TransformToQuatRefFrame(const float *rot, float *trans)
{
	float r;
	float ra;
	float zz;
	float zza;
	float zw;
	float temp;

	zz = *rot * *rot;
	r = (float)(rot[1] * rot[1]) + zz;

	if ( r != 0.0 )
	{
		ra = 2.0 / r;
		zza = zz * ra;
		zw = (float)(*rot * rot[1]) * ra;
		temp = (float)((float)(1.0 - zza) * *trans) + (float)(zw * trans[1]);
		trans[1] = trans[1] - (float)((float)(zw * *trans) + (float)(zza * trans[1]));
		*trans = temp;
	}
}

float ShortLerpAsVec(short from, short to, float frac)
{
	return (float)from + (float)(to - from) * frac;
}

float FloatLerp(float from, float to, float frac)
{
	return (to - from) * frac + from;
}

void Vec2MadShort2Lerp(const float *from, float scale, const short *to1, const short *to2, float frac, float *out)
{
	out[0] = ShortLerpAsVec(to1[0], to2[0], frac) * scale + from[0];
	out[1] = ShortLerpAsVec(to1[1], to2[1], frac) * scale + from[1];
}

void Vec4MadShort4Lerp(const float *from, float scale, const short *to1, const short *to2, float frac, float *out)
{
	out[0] = ShortLerpAsVec(to1[0], to2[0], frac) * scale + from[0];
	out[1] = ShortLerpAsVec(to1[1], to2[1], frac) * scale + from[1];
	out[2] = ShortLerpAsVec(to1[2], to2[2], frac) * scale + from[2];
	out[3] = ShortLerpAsVec(to1[3], to2[3], frac) * scale + from[3];
}

void Vec3MadVec3Lerp(const float *from, float scale, const float *to1, const float *to2, float frac, float *out)
{
	out[0] = FloatLerp(to1[0], to2[0], frac) * scale + from[0];
	out[1] = FloatLerp(to1[1], to2[1], frac) * scale + from[1];
	out[2] = FloatLerp(to1[2], to2[2], frac) * scale + from[2];
}

void Short2LerpAsVec2(const short *from, const short *to, float frac, float *out)
{
	out[0] = (float)from[0] + (float)((float)(to[0] - from[0]) * frac);
	out[1] = (float)from[1] + (float)((float)(to[1] - from[1]) * frac);
}

void Short2CopyAsVec2(const short *from, float *to)
{
	to[0] = (float)from[0];
	to[1] = (float)from[1];
}

void Vec2MadShort2(const float *from, float frac, const short *to, float *out)
{
	out[0] = (float)to[0] * frac + from[0];
	out[1] = (float)to[1] * frac + from[1];
}

void Vec4MadShort4(const float *from, float frac, const short *to, float *out)
{
	out[0] = (float)to[0] * frac + from[0];
	out[1] = (float)to[1] * frac + from[1];
	out[2] = (float)to[2] * frac + from[2];
	out[3] = (float)to[3] * frac + from[3];
}

void XAnim_SetTime(float time, int frameCount, XAnimTime *animTime)
{
	animTime->time = time;
	animTime->frameCount = frameCount;
	animTime->frameFrac = (float)frameCount * time;
	animTime->frameIndex = (int)animTime->frameFrac;
}

void XAnim_CalcRotDeltaEntireInternal(XAnimDeltaPartQuat *quat, float *rotDelta)
{
	const short *rotDeltaLastFrame;

	if ( quat->size )
		rotDeltaLastFrame = quat->u.frames.frames[quat->size];
	else
		rotDeltaLastFrame = quat->u.frame0;

	Short2CopyAsVec2(rotDeltaLastFrame, rotDelta);
}

void XAnim_CalcRotDeltaEntire(const XAnimDeltaPart *animDelta, float *rotDelta)
{
	if ( animDelta->quat )
	{
		XAnim_CalcRotDeltaEntireInternal(animDelta->quat, rotDelta);
	}
	else
	{
		rotDelta[0] = 0.0;
		rotDelta[1] = 32767.0;
	}
}

void XAnim_CalcPosDeltaEntireInternal(const XAnimDeltaPartTrans *trans, float *posDelta)
{
	if ( trans->size )
		VectorCopy(trans->u.frames.frames[trans->size], posDelta);
	else
		VectorCopy(trans->u.frame0, posDelta);
}

void XAnim_CalcPosDeltaEntire(const XAnimDeltaPart *animDelta, float *posDelta)
{
	if ( animDelta->trans )
		XAnim_CalcPosDeltaEntireInternal(animDelta->trans, posDelta);
	else
		VectorClear(posDelta);
}

void XAnim_GetTimeIndex_unsigned_short_(const XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	int high;
	int low;
	int frameIndex;
	int index;

	index = (int)((float)tableSize * animTime->time);
	frameIndex = animTime->frameIndex;

	if ( frameIndex >= indices[index] )
	{
		if ( frameIndex >= indices[index + 1] )
		{
			low = index + 1;
			high = tableSize;
			do
			{
				while ( 1 )
				{
					index = (high + low) / 2;
					if ( frameIndex < indices[index] )
						break;
					if ( frameIndex < indices[index + 1] )
						goto out;
					low = index + 1;
				}
				high = (high + low++) / 2;
			}
			while ( frameIndex >= indices[low] );
			index = low - 1;
		}
	}
	else
	{
		low = 0;
		high = (int)((float)tableSize * animTime->time);
		while ( 1 )
		{
			while ( 1 )
			{
				index = (high + low) / 2;
				if ( frameIndex >= indices[index] )
					break;
				high = (high + low) / 2;
			}
			if ( frameIndex < indices[index + 1] )
				break;
			low = index + 1;
			if ( frameIndex >= indices[--high] )
			{
				index = high;
				break;
			}
		}
	}
out:
	*keyFrameLerpFrac = (animTime->frameFrac - (float)indices[index])
	                    / (float)(indices[index + 1] - indices[index]);
	*keyFrameIndex = index;
}

void XAnim_GetTimeIndexCompressed_unsigned_short_(XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	if ( tableSize >= animTime->frameCount )
	{
		*keyFrameLerpFrac = animTime->frameFrac - (float)animTime->frameIndex;
		*keyFrameIndex = animTime->frameIndex;
	}
	else
	{
		XAnim_GetTimeIndex_unsigned_short_(animTime, indices, tableSize, keyFrameIndex, keyFrameLerpFrac);
	}
}

void XAnim_CalcRotDeltaDuringInternal_unsigned_short_(XAnimDeltaPartQuat *quat, const float time, int numFrames, float *rotDelta)
{
	float keyFrameIndex;
	int keyFrameLerpFrac;
	XAnimTime animTime;

	if ( quat->size )
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    &animTime,
		    quat->u.frames.indices._2,
		    quat->size,
		    &keyFrameLerpFrac,
		    &keyFrameIndex);
		Short2LerpAsVec2(
		    quat->u.frames.frames[keyFrameLerpFrac],
		    quat->u.frames.frames[keyFrameLerpFrac + 1],
		    keyFrameIndex,
		    rotDelta);
	}
	else
	{
		Short2CopyAsVec2(quat->u.frame0, rotDelta);
	}
}

void XAnim_CalcRotDeltaDuring_unsigned_short_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *rotDelta)
{
	if ( animDelta->quat )
	{
		XAnim_CalcRotDeltaDuringInternal_unsigned_short_(animDelta->quat, time, numFrames, rotDelta);
	}
	else
	{
		rotDelta[0] = 0.0;
		rotDelta[1] = 32767.0;
	}
}

void XAnim_CalcPosDeltaDuringInternal_unsigned_short_(XAnimDeltaPartTrans *trans, const float time, int numFrames, float *posDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;
	XAnimTime animTime;

	if ( trans->size )
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    &animTime,
		    trans->u.frames.indices._2,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3Lerp(
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
	else
	{
		VectorCopy(trans->u.frame0, posDelta);
	}
}

void XAnim_GetTimeIndex_unsigned_char_(const XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	int high;
	int low;
	int frameIndex;
	int index;

	index = (int)((float)tableSize * animTime->time);
	frameIndex = animTime->frameIndex;

	if ( frameIndex >= indices[index] )
	{
		if ( frameIndex >= indices[index + 1] )
		{
			low = index + 1;
			high = tableSize;
			do
			{
				while ( 1 )
				{
					index = (high + low) / 2;
					if ( frameIndex < indices[index] )
						break;
					if ( frameIndex < indices[index + 1] )
						goto out;
					low = index + 1;
				}
				high = (high + low++) / 2;
			}
			while ( frameIndex >= indices[low] );
			index = low - 1;
		}
	}
	else
	{
		low = 0;
		high = (int)((float)tableSize * animTime->time);
		while ( 1 )
		{
			while ( 1 )
			{
				index = (high + low) / 2;
				if ( frameIndex >= indices[index] )
					break;
				high = (high + low) / 2;
			}
			if ( frameIndex < indices[index + 1] )
				break;
			low = index + 1;
			if ( frameIndex >= indices[--high] )
			{
				index = high;
				break;
			}
		}
	}
out:
	*keyFrameLerpFrac = (animTime->frameFrac - (float)indices[index])
	                    / (float)(indices[index + 1] - indices[index]);
	*keyFrameIndex = index;
}

void XAnim_CalcPosDeltaDuring_unsigned_short_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *posDelta)
{
	if ( animDelta->trans )
		XAnim_CalcPosDeltaDuringInternal_unsigned_short_(animDelta->trans, time, numFrames, posDelta);
	else
		VectorClear(posDelta);
}

void XAnim_GetTimeIndexCompressed_unsigned_char(XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac)
{
	if ( tableSize >= animTime->frameCount )
	{
		*keyFrameLerpFrac = animTime->frameFrac - (float)animTime->frameIndex;
		*keyFrameIndex = animTime->frameIndex;
	}
	else
	{
		XAnim_GetTimeIndex_unsigned_char_(animTime, indices, tableSize, keyFrameIndex, keyFrameLerpFrac);
	}
}

void XAnim_CalcRotDeltaDuringInternal_unsigned_char_(XAnimDeltaPartQuat *quat, const float time, int numFrames, float *rotDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;
	XAnimTime animTime;

	if ( quat->size )
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    &animTime,
		    quat->u.frames.indices._1,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Short2LerpAsVec2(
		    quat->u.frames.frames[keyFrameIndex],
		    quat->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
	else
	{
		Short2CopyAsVec2(quat->u.frame0, rotDelta);
	}
}

void XAnim_CalcRotDeltaDuring_unsigned_char_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *rotDelta)
{
	if ( animDelta->quat )
	{
		XAnim_CalcRotDeltaDuringInternal_unsigned_char_(animDelta->quat, time, numFrames, rotDelta);
	}
	else
	{
		rotDelta[0] = 0.0;
		rotDelta[1] = 32767.0;
	}
}

void XAnim_CalcPosDeltaDuringInternal_unsigned_char_(XAnimDeltaPartTrans *trans, const float time, int numFrames, float *posDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;
	XAnimTime animTime;

	if ( trans->size )
	{
		XAnim_SetTime(time, numFrames, &animTime);
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    &animTime,
		    trans->u.frames.indices._1,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3Lerp(
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
	else
	{
		VectorCopy(trans->u.frame0, posDelta);
	}
}

void XAnim_CalcPosDeltaDuring_unsigned_char_(XAnimDeltaPart *animDelta, const float time, int numFrames, float *posDelta)
{
	if ( animDelta->trans )
		XAnim_CalcPosDeltaDuringInternal_unsigned_char_(animDelta->trans, time, numFrames, posDelta);
	else
		VectorClear(posDelta);
}

void XAnim_CalcDeltaForTime(const XAnimParts_s *anim, const float time, float *rotDelta, float *posDelta)
{
	bool isComplete;
	XAnimDeltaPart *animDelta;
	int numFrames;

	animDelta = anim->deltaPart;
	numFrames = anim->numframes;
	isComplete = 0;

	if ( time == 1.0 || !anim->numframes )
		isComplete = 1;

	if ( isComplete )
	{
		XAnim_CalcRotDeltaEntire(animDelta, rotDelta);
		XAnim_CalcPosDeltaEntire(animDelta, posDelta);
	}
	else if ( anim->numframes > 0xFFu )
	{
		XAnim_CalcRotDeltaDuring_unsigned_short_(animDelta, time, numFrames, rotDelta);
		XAnim_CalcPosDeltaDuring_unsigned_short_(animDelta, time, numFrames, posDelta);
	}
	else
	{
		XAnim_CalcRotDeltaDuring_unsigned_char_(animDelta, time, numFrames, rotDelta);
		XAnim_CalcPosDeltaDuring_unsigned_char_(animDelta, time, numFrames, posDelta);
	}
}

void XAnimCalcRelDeltaParts(const XAnimParts_s *parts, float weightScale, float time1, float time2, XAnimSimpleRotPos *rotPos, int quatIndex)
{
	XAnimDeltaPartTrans *trans;
	float rotWeightScale;
	vec3_t pos;
	vec4_t vec2;
	vec4_t vec1;
	float Q[2][2];

	XAnim_CalcDeltaForTime(parts, time1, Q[0], vec1);
	XAnim_CalcDeltaForTime(parts, time2, Q[1], vec2);

	if ( parts->bLoop )
	{
		if ( time1 > time2 )
		{
			trans = parts->deltaPart->trans;

			if ( trans )
			{
				if ( trans->size )
				{
					VectorAdd(vec2, trans->u.frames.frames[trans->size], vec2);
					VectorSubtract(vec2, trans->u.frames.frames[0], vec2); // warning
				}
			}
		}
	}

	rotWeightScale = weightScale * 9.3137942e-10;
	rotPos->rot[0] = (Q[1][0] * Q[0][1] - Q[1][1] * Q[0][0]) * rotWeightScale + rotPos->rot[0];
	rotPos->rot[1] = (Q[1][0] * Q[0][0] + Q[1][1] * Q[0][1]) * rotWeightScale + rotPos->rot[1];
	VectorSubtract(vec2, vec1, pos);
	TransformToQuatRefFrame(Q[quatIndex], pos);
	rotPos->posWeight = rotPos->posWeight + weightScale;
	VectorMA(rotPos->pos, weightScale, pos, rotPos->pos);
}

void XAnimGetRelDelta(const XAnim_s *anims, unsigned int animIndex, float *rot, float *trans, float time1, float time2)
{
	XAnimParts_s *parts;
	XAnimSimpleRotPos rotPos;

	if ( anims->entries[animIndex].numAnims || (parts = anims->entries[animIndex].u.parts, !parts->bDelta) )
	{
		rot[0] = 0.0;
		rot[1] = 1.0;
		VectorClear(trans);
	}
	else
	{
		Vector2Clear(rotPos.rot);
		rotPos.posWeight = 0.0;
		VectorClear(rotPos.pos);
		XAnimCalcRelDeltaParts(parts, 1.0, time1, time2, &rotPos, 0);

		if ( rotPos.rot[0] == 0.0 && rotPos.rot[1] == 0.0 )
		{
			rot[0] = 0.0;
			rot[1] = 1.0;
		}
		else
		{
			Vector2Copy(rotPos.rot, rot);
		}

		VectorCopy(rotPos.pos, trans);
	}
}

void XAnimCalcAbsDeltaParts(const XAnimParts_s *parts, const float weightScale, float time, XAnimSimpleRotPos *rotPos)
{
	vec4_t pos;
	float Q[2];

	XAnim_CalcDeltaForTime(parts, time, Q, pos);
	VectorMA2(rotPos->rot, weightScale * 0.000030518509, Q, rotPos->rot);
	rotPos->posWeight = rotPos->posWeight + weightScale;
	VectorMA(rotPos->pos, weightScale, pos, rotPos->pos);
}

void XAnimCalcDeltaTree(const XAnimTree_s *tree, unsigned int infoIndex, float weightScale, bool bClear, bool bNormQuat, XAnimSimpleRotPos *rotPos)
{
	float weight;
	float firstWeight;
	XAnimParts_s *parts;
	XAnimEntry *entry;
	uint16_t childInfoIndex;
	XAnimSimpleRotPos newRotPos;
	float r;
	XAnimSimpleRotPos *rotPos2;
	int numAnims;
	int j;
	int i;

	entry = &tree->anims->entries[infoIndex];
	numAnims = entry->numAnims;

	if ( numAnims )
	{
		for ( i = 0; ; ++i )
		{
			if ( i >= numAnims )
			{
				if ( bClear )
				{
					Vector2Clear(rotPos->rot);
					rotPos->posWeight = 0.0;
					VectorClear(rotPos->pos);
				}
				return;
			}

			childInfoIndex = tree->infoArray[i + entry->u.animParent.children];

			if ( childInfoIndex )
			{
				firstWeight = tree->bUseGoalWeight
				              ? g_xAnimInfo[childInfoIndex].state.goalWeight
				              : g_xAnimInfo[childInfoIndex].state.weight;

				if ( firstWeight != 0.0 )
					break;
			}
		}
		for ( j = i + 1; ; ++j )
		{
			if ( j >= numAnims )
			{
				XAnimCalcDeltaTree(tree, i + entry->u.animParent.children, weightScale, bClear, bNormQuat, rotPos);
				return;
			}

			childInfoIndex = tree->infoArray[j + entry->u.animParent.children];

			if ( childInfoIndex )
			{
				weight = tree->bUseGoalWeight
				         ? g_xAnimInfo[childInfoIndex].state.goalWeight
				         : g_xAnimInfo[childInfoIndex].state.weight;

				if ( weight != 0.0 )
					break;
			}
		}

		if ( bClear )
			rotPos2 = rotPos;
		else
			rotPos2 = &newRotPos;

		XAnimCalcDeltaTree(tree, i + entry->u.animParent.children, firstWeight, 1, 1, rotPos2);
		XAnimCalcDeltaTree(tree, j + entry->u.animParent.children, weight, 0, 1, rotPos2);
		++j;

		while ( j < numAnims )
		{
			childInfoIndex = tree->infoArray[j + entry->u.animParent.children];

			if ( childInfoIndex )
			{
				weight = tree->bUseGoalWeight
				         ? g_xAnimInfo[childInfoIndex].state.goalWeight
				         : g_xAnimInfo[childInfoIndex].state.weight;

				if ( weight != 0.0 )
					XAnimCalcDeltaTree(tree, j + entry->u.animParent.children, weight, 0, 1, rotPos2);
			}

			++j;
		}

		if ( bNormQuat )
		{
			if ( bClear )
			{
				r = Vec2LengthSq(rotPos->rot);

				if ( r != 0.0 )
				{
					Vec2Scale(rotPos->rot, I_rsqrt(r) * weightScale, rotPos->rot);
				}

				if ( rotPos->posWeight != 0.0 )
				{
					VectorScale(rotPos->pos, weightScale / rotPos->posWeight, rotPos->pos);
					rotPos->posWeight = weightScale;
				}
			}
			else
			{
				r = Vec2LengthSq(rotPos2->rot);

				if ( r != 0.0 )
				{
					VectorMA2(rotPos->rot, I_rsqrt(r) * weightScale, rotPos2->rot, rotPos->rot);
				}

				if ( rotPos2->posWeight != 0.0 )
				{
					VectorMA(rotPos->pos, weightScale / rotPos2->posWeight, rotPos2->pos, rotPos->pos);
					rotPos->posWeight = rotPos->posWeight + weightScale;
				}
			}
		}
		else if ( rotPos->posWeight != 0.0 )
		{
			VectorScale(rotPos->pos, 1.0 / rotPos->posWeight, rotPos->pos);
		}
	}
	else
	{
		if ( bClear )
		{
			Vector2Clear(rotPos->rot);
			rotPos->posWeight = 0.0;
			VectorClear(rotPos->pos);
		}

		parts = entry->u.parts;

		if ( parts->bDelta )
		{
			childInfoIndex = tree->infoArray[infoIndex];

			if ( childInfoIndex )
			{
				if ( tree->bAbs )
					XAnimCalcAbsDeltaParts(parts, weightScale, g_xAnimInfo[childInfoIndex].state.time, rotPos);
				else
					XAnimCalcRelDeltaParts(
					    parts,
					    weightScale,
					    g_xAnimInfo[childInfoIndex].state.oldTime,
					    g_xAnimInfo[childInfoIndex].state.time,
					    rotPos,
					    1);
			}
		}
	}
}

void XAnimCalcDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans, bool bUseGoalWeight)
{
	XAnimSimpleRotPos rotPos;

	tree->bAbs = 0;
	tree->bUseGoalWeight = bUseGoalWeight;

	XAnimCalcDeltaTree(tree, animIndex, 1.0, 1, 0, &rotPos);

	if ( rotPos.rot[0] == 0.0 || rotPos.rot[1] == 0.0 )
	{
		rot[0] = 0.0;
		rot[1] = 1.0;
	}
	else
	{
		Vector2Copy(rotPos.rot, rot);
	}

	VectorCopy(rotPos.pos, trans);
}

void XAnimGetAbsDelta(const XAnim_s *anims, unsigned int animIndex, float *rot, float *trans, float time)
{
	XAnimParts_s *parts;
	XAnimSimpleRotPos rotPos;

	if ( anims->entries[animIndex].numAnims || (parts = anims->entries[animIndex].u.parts, !parts->bDelta) )
	{
		rot[0] = 0.0;
		rot[1] = 1.0;
		VectorClear(trans);
	}
	else
	{
		Vector2Clear(rotPos.rot);
		rotPos.posWeight = 0.0;
		VectorClear(rotPos.pos);
		XAnimCalcAbsDeltaParts(parts, 1.0, time, &rotPos);

		if ( rotPos.rot[0] == 0.0 && rotPos.rot[1] == 0.0 )
		{
			rot[0] = 0.0;
			rot[1] = 1.0;
		}
		else
		{
			Vector2Copy(rotPos.rot, rot);
		}

		VectorCopy(rotPos.pos, trans);
	}
}

void XAnimCalcAbsDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans)
{
	XAnimSimpleRotPos rotPos;

	tree->bAbs = 1;
	tree->bUseGoalWeight = 1;

	XAnimCalcDeltaTree(tree, animIndex, 1.0, 1, 0, &rotPos);

	if ( rotPos.rot[0] == 0.0 && rotPos.rot[1] == 0.0 )
	{
		rot[0] = 0.0;
		rot[1] = 1.0;
	}
	else
	{
		Vector2Copy(rotPos.rot, rot);
	}

	VectorCopy(rotPos.pos, trans);
}
