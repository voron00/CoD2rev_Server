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

void Short4ScaleAsVec4(short *from, float scale, float *to)
{
	to[0] = (float)from[0] * scale;
	to[1] = (float)from[1] * scale;
	to[2] = (float)from[2] * scale;
	to[3] = (float)from[3] * scale;
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

void XAnim_CalcSimpleRotDuring_unsigned_short_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( quat->size )
	{
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    animTime,
		    quat->u.frames.indices._2,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec2MadShort2Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames2[keyFrameIndex],
		    quat->u.frames.u.frames2[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
	else
	{
		Vec2MadShort2(rotDelta, scale, quat->u.frame0, rotDelta);
	}
}

void XAnim_CalcRotDuring_unsigned_short_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( quat->size )
	{
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    animTime,
		    quat->u.frames.indices._2,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec4MadShort4Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames[keyFrameIndex],
		    quat->u.frames.u.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
	else
	{
		Vec4MadShort4(rotDelta, scale, quat->u.frame0, rotDelta);
	}
}

void XAnim_CalcPosDuring_unsigned_short_(XAnimTime *animTime, const XAnimPartTrans *trans, float scale, float *posDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( trans->size )
	{
		XAnim_GetTimeIndexCompressed_unsigned_short_(
		    animTime,
		    trans->u.frames.indices._2,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3MadVec3Lerp(
		    posDelta,
		    scale,
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
	else
	{
		VectorMA(posDelta, scale, trans->u.frame0, posDelta);
	}
}

void XAnimCalcParts_unsigned_short_(const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits)
{
	XAnimTime animTime;
	char *simpleQuatBits;
	int numframes;
	int boneCount;
	int modelIndex;
	float scale;
	XAnimPart *childPart;
	int i;
	DObjAnimMat *totalRotTrans;

	numframes = parts->numframes;
	XAnim_SetTime(time, numframes, &animTime);
	scale = weightScale * 0.000030518509;
	simpleQuatBits = parts->simpleQuatBits;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
	{
		modelIndex = animToModel[i];

		if ( ((ignorePartBits[modelIndex >> 5] >> (modelIndex & 0x1F)) & 1) == 0 )
		{
			totalRotTrans = &rotTransArray[modelIndex];
			childPart = &parts->parts[i];

			if ( ((simpleQuatBits[i >> 3] >> (i & 7)) & 1) != 0 )
			{
				if ( childPart->quat )
					XAnim_CalcSimpleRotDuring_unsigned_short_(&animTime, childPart->quat, scale, &totalRotTrans->quat[2]);
				else
					totalRotTrans->quat[3] = totalRotTrans->quat[3] + weightScale;
			}
			else
			{
				XAnim_CalcRotDuring_unsigned_short_(&animTime, childPart->quat, scale, totalRotTrans->quat);
			}
			if ( childPart->trans )
				XAnim_CalcPosDuring_unsigned_short_(&animTime, childPart->trans, weightScale, totalRotTrans->trans);
			totalRotTrans->transWeight = totalRotTrans->transWeight + weightScale;
		}
	}
}

void XAnim_CalcSimpleRotDuring_unsigned_char_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( quat->size )
	{
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    animTime,
		    quat->u.frames.indices._1,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec2MadShort2Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames2[keyFrameIndex],
		    quat->u.frames.u.frames2[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
	else
	{
		Vec2MadShort2(rotDelta, scale, quat->u.frame0, rotDelta);
	}
}

void XAnim_CalcRotDuring_unsigned_char_(XAnimTime *animTime, const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( quat->size )
	{
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    animTime,
		    quat->u.frames.indices._1,
		    quat->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec4MadShort4Lerp(
		    rotDelta,
		    scale,
		    quat->u.frames.u.frames[keyFrameIndex],
		    quat->u.frames.u.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    rotDelta);
	}
	else
	{
		Vec4MadShort4(rotDelta, scale, quat->u.frame0, rotDelta);
	}
}

void XAnim_CalcPosDuring_unsigned_char_(XAnimTime *animTime, const XAnimPartTrans *trans, float scale, float *posDelta)
{
	float keyFrameLerpFrac;
	int keyFrameIndex;

	if ( trans->size )
	{
		XAnim_GetTimeIndexCompressed_unsigned_char(
		    animTime,
		    trans->u.frames.indices._1,
		    trans->size,
		    &keyFrameIndex,
		    &keyFrameLerpFrac);
		Vec3MadVec3Lerp(
		    posDelta,
		    scale,
		    trans->u.frames.frames[keyFrameIndex],
		    trans->u.frames.frames[keyFrameIndex + 1],
		    keyFrameLerpFrac,
		    posDelta);
	}
	else
	{
		VectorMA(posDelta, scale, trans->u.frame0, posDelta);
	}
}

void XAnimCalcParts_unsigned_char_(const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits)
{
	XAnimTime animTime;
	char *simpleQuatBits;
	int numframes;
	int boneCount;
	int animPartIndex;
	float scale;
	XAnimPart *childPart;
	int i;
	DObjAnimMat *totalRotTrans;

	numframes = parts->numframes;
	XAnim_SetTime(time, numframes, &animTime);
	scale = weightScale * 0.000030518509;
	simpleQuatBits = parts->simpleQuatBits;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
	{
		animPartIndex = animToModel[i];

		if ( ((ignorePartBits[animPartIndex >> 5] >> (animPartIndex & 0x1F)) & 1) == 0 )
		{
			totalRotTrans = &rotTransArray[animPartIndex];
			childPart = &parts->parts[i];

			if ( ((simpleQuatBits[i >> 3] >> (i & 7)) & 1) != 0 )
			{
				if ( childPart->quat )
					XAnim_CalcSimpleRotDuring_unsigned_char_(&animTime, childPart->quat, scale, &totalRotTrans->quat[2]);
				else
					totalRotTrans->quat[3] = totalRotTrans->quat[3] + weightScale;
			}
			else
			{
				XAnim_CalcRotDuring_unsigned_char_(&animTime, childPart->quat, scale, totalRotTrans->quat);
			}

			if ( childPart->trans )
				XAnim_CalcPosDuring_unsigned_char_(&animTime, childPart->trans, weightScale, totalRotTrans->trans);

			totalRotTrans->transWeight = totalRotTrans->transWeight + weightScale;
		}
	}
}

void XAnim_CalcSimpleRotEnd(const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	if ( quat->size )
		Vec2MadShort2(rotDelta, scale, quat->u.frames.u.frames2[quat->size], rotDelta);
	else
		Vec2MadShort2(rotDelta, scale, quat->u.frame0, rotDelta);
}

void XAnim_CalcRotEnd(const XAnimPartQuat *quat, float scale, float *rotDelta)
{
	if ( quat->size )
		Vec4MadShort4(rotDelta, scale, quat->u.frames.u.frames[quat->size], rotDelta);
	else
		Vec4MadShort4(rotDelta, scale, quat->u.frame0, rotDelta);
}

void XAnim_CalcPosEnd(const XAnimPartTrans *trans, float scale, float *posDelta)
{
	if ( trans->size )
		VectorMA(posDelta, scale, trans->u.frames.frames[trans->size], posDelta);
	else
		VectorMA(posDelta, scale, trans->u.frame0, posDelta);
}

void XAnimCalcNonLoopEnd(const XAnimParts_s *parts, const unsigned char *animToModel, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits)
{
	char *simpleQuatBits;
	int boneCount;
	int animPartIndex;
	float scale;
	XAnimPart *childPart;
	int i;
	DObjAnimMat *totalRotTrans;

	scale = weightScale * 0.000030518509;
	simpleQuatBits = parts->simpleQuatBits;
	boneCount = parts->boneCount;

	for ( i = 0; i < boneCount; ++i )
	{
		animPartIndex = animToModel[i];

		if ( ((ignorePartBits[animPartIndex >> 5] >> (animPartIndex & 0x1F)) & 1) == 0 )
		{
			totalRotTrans = &rotTransArray[animPartIndex];
			childPart = &parts->parts[i];

			if ( ((simpleQuatBits[i >> 3] >> (i & 7)) & 1) != 0 )
			{
				if ( childPart->quat )
					XAnim_CalcSimpleRotEnd(childPart->quat, scale, &totalRotTrans->quat[2]);
				else
					totalRotTrans->quat[3] = totalRotTrans->quat[3] + weightScale;
			}
			else
			{
				XAnim_CalcRotEnd(childPart->quat, scale, totalRotTrans->quat);
			}

			if ( childPart->trans )
				XAnim_CalcPosEnd(childPart->trans, weightScale, totalRotTrans->trans);

			totalRotTrans->transWeight = totalRotTrans->transWeight + weightScale;
		}
	}
}

void XAnimClearQuats(DObjAnimMat *Mat)
{
	Mat->quat[0] = 0.0;
	Mat->quat[1] = 0.0;
	Mat->quat[2] = 0.0;
	Mat->quat[3] = 0.0;
}

void XAnimClearData(const DObj_s *obj, DObjAnimMat *rotTransArray, XAnimCalcAnimInfo *info)
{
	int i;

	for ( i = 0; i < obj->numBones; ++i )
	{
		if ( ((info->ignorePartBits[i >> 5] >> (i & 0x1F)) & 1) == 0 )
		{
			XAnimClearQuats(rotTransArray);
			rotTransArray->transWeight = 0.0;
			VectorClear(rotTransArray->trans);
		}

		++rotTransArray;
	}
}

void XAnimCalc(const DObj_s *obj, unsigned int entry, float weightScale, DObjAnimMat *rotTransArray, bool bClear, bool bNormQuat, XAnimCalcAnimInfo *animInfo, int bufferIndex)
{
	unsigned short *animToModel;
	float animTime;
	XAnimParts_s *animParts;
	const int *partBits;
	XAnimTree_s *tree;
	byte *parentModel;
	XAnimEntry *animEntry;
	uint16_t firstInfoIndex;
	uint16_t secondInfoIndex;
	uint16_t thirdInfoIndex;
	float vLenSq;
	DObjAnimMat *calcBuffer;
	int numAnims;
	int j;
	int k;
	int o;
	int i;
	int l;
	int m;
	int n;
	float firstWeight;
	float secondWeight;
	float thirdWeight;

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
					XAnimClearData(obj, rotTransArray, animInfo);
				return;
			}

			firstInfoIndex = tree->infoArray[i + obj->tree->anims->entries[entry].u.animParent.children];

			if ( firstInfoIndex )
			{
				firstWeight = g_xAnimInfo[firstInfoIndex].state.weight;

				if ( firstWeight != 0.0 )
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
				    bufferIndex);
				return;
			}

			secondInfoIndex = tree->infoArray[j + obj->tree->anims->entries[entry].u.animParent.children];

			if ( secondInfoIndex )
			{
				secondWeight = g_xAnimInfo[secondInfoIndex].state.weight;

				if ( secondWeight != 0.0 )
					break;
			}
		}

		if ( bClear )
		{
			calcBuffer = rotTransArray;
		}
		else
		{
			calcBuffer = &animInfo->rotTransArray[bufferIndex];
			bufferIndex += obj->numBones;

			if ( bufferIndex > 512 )
			{
				Com_Printf("MAX_CALC_ANIM_BUFFER exceeded\n");
				return;
			}
		}

		XAnimCalc(
		    obj,
		    i + obj->tree->anims->entries[entry].u.animParent.children,
		    firstWeight,
		    calcBuffer,
		    1,
		    1,
		    animInfo,
		    bufferIndex);

		XAnimCalc(obj, j + animEntry->u.animParent.children, secondWeight, calcBuffer, 0, 1, animInfo, bufferIndex);

		for ( k = j + 1; k < numAnims; ++k )
		{
			thirdInfoIndex = tree->infoArray[k + animEntry->u.animParent.children];

			if ( thirdInfoIndex )
			{
				thirdWeight = g_xAnimInfo[thirdInfoIndex].state.weight;

				if ( thirdWeight != 0.0 )
					XAnimCalc(obj, k + animEntry->u.animParent.children, thirdWeight, calcBuffer, 0, 1, animInfo, bufferIndex);
			}
		}

		if ( bNormQuat )
		{
			if ( bClear )
			{
				for ( m = 0; m < obj->numBones; ++m )
				{
					if ( ((animInfo->ignorePartBits[m >> 5] >> (m & 0x1F)) & 1) == 0 )
					{
						vLenSq = Vec4LengthSq(rotTransArray->quat);

						if ( vLenSq != 0.0 )
						{
							VectorScale4(rotTransArray->quat, I_rsqrt(vLenSq) * weightScale, rotTransArray->quat);
						}

						if ( rotTransArray->transWeight != 0.0 )
						{
							VectorScale(rotTransArray->trans, weightScale / rotTransArray->transWeight, rotTransArray->trans);
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
					if ( ((animInfo->ignorePartBits[n >> 5] >> (n & 0x1F)) & 1) == 0 )
					{
						vLenSq = Vec4LengthSq(calcBuffer->quat);

						if ( vLenSq != 0.0 )
						{
							VectorMA4(rotTransArray->quat, I_rsqrt(vLenSq) * weightScale, calcBuffer->quat, rotTransArray->quat);
						}

						if ( calcBuffer->transWeight != 0.0 )
						{
							VectorMA(rotTransArray->trans, weightScale / calcBuffer->transWeight, calcBuffer->trans, rotTransArray->trans);
							rotTransArray->transWeight = rotTransArray->transWeight + weightScale;
						}
					}

					++rotTransArray;
					++calcBuffer;
				}
			}
		}
		else
		{
			for ( l = 0; l < obj->numBones; ++l )
			{
				if ( ((animInfo->ignorePartBits[l >> 5] >> (l & 0x1F)) & 1) == 0 && rotTransArray->transWeight != 0.0 )
				{
					VectorScale4(rotTransArray->quat, 1.0 / rotTransArray->transWeight, rotTransArray->quat);
					VectorScale(rotTransArray->trans, 1.0 / rotTransArray->transWeight, rotTransArray->trans);
				}

				++rotTransArray;
			}
		}
	}
	else
	{
		if ( bClear )
			XAnimClearData(obj, rotTransArray, animInfo);

		parentModel = (byte *)&obj->animToModel[tree->anims->size];

		if ( obj->animToModel[entry] )
		{
			if ( parentModel[entry + 1] != *parentModel )
			{
				parentModel[entry + 1] = *parentModel;
				SL_RemoveRefToStringOfLen(obj->animToModel[entry], animEntry->u.parts->boneCount + 16);
				animToModel = obj->animToModel;
				animToModel[entry] = XAnimSetModel(animEntry, obj->models, obj->numModels);
			}
		}
		else
		{
			parentModel[entry + 1] = *parentModel;
			animToModel = obj->animToModel;
			animToModel[entry] = XAnimSetModel(animEntry, obj->models, obj->numModels);
		}

		partBits = (const int *)SL_ConvertToString(obj->animToModel[entry]);

		for ( o = 0; o < 4; ++o )
			animInfo->animPartBits[o] |= partBits[o] & ~animInfo->ignorePartBits[o];

		animParts = animEntry->u.parts;
		animTime = g_xAnimInfo[tree->infoArray[entry]].state.time;

		if ( animTime != 1.0 && animParts->numframes )
		{
			if ( animParts->numframes > 0xFFu )
				XAnimCalcParts_unsigned_short_(
				    animParts,
				    (const unsigned char *)partBits + 16,
				    animTime,
				    weightScale,
				    rotTransArray,
				    animInfo->ignorePartBits);
			else
				XAnimCalcParts_unsigned_char_(
				    animParts,
				    (const unsigned char *)partBits + 16,
				    animTime,
				    weightScale,
				    rotTransArray,
				    animInfo->ignorePartBits);
		}
		else
		{
			XAnimCalcNonLoopEnd(
			    animParts,
			    (const unsigned char *)partBits + 16,
			    weightScale,
			    rotTransArray,
			    animInfo->ignorePartBits);
		}
	}
}

void DObjCalcAnim(const DObj_s *obj, int *partBits)
{
	XAnimCalcAnimInfo animInfo;
	bool bNoPartBits;
	int boneIndex;
	short *quats;
	XModelParts_s *parts;
	int i;
	DObjAnimMat *Mat;
	DSkel_t *skel;
	int boneCount;

	skel = obj->skel;
	bNoPartBits = 1;

	for ( boneCount = 0; boneCount < 4; ++boneCount )
	{
		animInfo.animPartBits[boneCount] = skel->partBits.anim[boneCount] | ~partBits[boneCount];

		if ( animInfo.animPartBits[boneCount] != -1 )
			bNoPartBits = 0;
	}

	if ( !bNoPartBits )
	{
		for ( boneCount = 0; boneCount < 4; ++boneCount )
		{
			skel->partBits.anim[boneCount] |= partBits[boneCount];
			animInfo.ignorePartBits[boneCount] = animInfo.animPartBits[boneCount];
		}

		Mat = &skel->Mat;

		if ( obj->tree )
		{
			animInfo.ignorePartBits[3] |= 0x80000000;
			XAnimCalc(obj, 0, 1.0, Mat, 1, 0, &animInfo, 0);
		}

		boneIndex = 0;

		for ( i = 0; i < obj->numModels; ++i )
		{
			parts = obj->models[i]->parts;
			boneCount = parts->numRootBones;

			while ( boneCount )
			{
				if ( ((animInfo.animPartBits[boneIndex >> 5] >> (boneIndex & 0x1F)) & 1) == 0 )
				{
					Mat->quat[0] = 0.0;
					Mat->quat[1] = 0.0;
					Mat->quat[2] = 0.0;
					Mat->quat[3] = 1.0;
					VectorClear(Mat->trans);
				}

				--boneCount;
				++Mat;
				++boneIndex;
			}

			quats = parts->quats;
			boneCount = parts->numBones - parts->numRootBones;

			while ( boneCount )
			{
				if ( ((animInfo.animPartBits[boneIndex >> 5] >> (boneIndex & 0x1F)) & 1) == 0 )
				{
					Short4ScaleAsVec4(quats, 0.000030518509, Mat->quat);
					VectorClear(Mat->trans);
				}

				--boneCount;
				++Mat;
				++boneIndex;
				quats += 4;
			}
		}
	}
}