#include "../qcommon/qcommon.h"

int XAnim_ReadShort(const unsigned char **pos)
{
	short dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += sizeof(short);
	return dest;
}

int XAnim_ReadInt(const unsigned char **pos)
{
	int dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += sizeof(int);
	return dest;
}

float XAnim_ReadFloat(const unsigned char **pos)
{
	float dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += sizeof(float);
	return dest;
}

void ConsumeQuat(const unsigned char **pos, short *out)
{
	int temp;

	out[0] = XAnim_ReadShort(pos);
	out[1] = XAnim_ReadShort(pos);
	out[2] = XAnim_ReadShort(pos);

	temp = 1073676289 - (out[2] * out[2] + out[1] * out[1] + out[0] * out[0]);

	if ( temp <= 0 )
	{
		out[3] = 0;
	}
	else
	{
		out[3] = (int)floor(I_sqrt((float)temp) + 0.5);
	}
}

void ConsumeQuat2(const unsigned char **pos, short *out)
{
	int temp;

	out[0] = XAnim_ReadShort(pos);
	temp = 1073676289 - out[0] * out[0];

	if ( temp <= 0 )
	{
		out[1] = 0;
	}

	else
	{
		out[1] = (int)floor(I_sqrt((float)temp) + 0.5);
	}
}

void ReadNoteTracks(const char *name, const unsigned char **pos, XAnimParts_s *parts, void *(*Alloc)(int))
{
	XAnimNotifyInfo *notify;
	int frametime;
	int i;
	int numNoteTracks;

	numNoteTracks = *(*pos)++;
	parts->notifyCount = numNoteTracks + 1;
	notify = (XAnimNotifyInfo *)Alloc(sizeof(XAnimNotifyInfo) * parts->notifyCount);
	parts->notify = notify;

	for ( i = 0; i < numNoteTracks; ++i )
	{
		notify->name = SL_GetString_((const char *)*pos, 0);
		*pos += strlen((const char *)*pos) + 1;
		frametime = (unsigned short)XAnim_ReadShort(pos);

		if ( parts->numframes )
		{
			notify->time = (float)frametime / (float)parts->numframes;
		}
		else
		{
			notify->time = 0.0;
		}

		++notify;
	}

	notify->name = SL_GetString_("end", 0);
	notify->time = 1.0;
}

/*
================
XAnimLoadFile
================
*/
XAnimParts* XAnimLoadFile( const char *name, void *(*Alloc)(int) )
{
	char filename[64];
	int fileSize;
	short version;
	const unsigned char *pos;
	byte *buf;
	int numBones, useSmallIndices, count;
	int i, j;
	unsigned short *boneNames;
	int bLoop, bDelta;
	XAnimParts *parts;
	XAnimDeltaPart *deltaPart;
	short quat[4];
	unsigned short numQuatIndices, numTransIndices, numframes;
	const unsigned char *quatBits;
	char *bits;
	short *sQ;

	if ( Com_sprintf(filename, sizeof(filename), "xanim/%s", name) <= 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return 0;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize < 0 )
	{
		Com_Printf("^1ERROR: xanim '%s' not found\n", name);
		return 0;
	}

	if ( fileSize == 0 )
	{
		Com_Printf("^1ERROR: xanim '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return 0;
	}

	assert(buf);
	pos = buf;
	version = XAnim_ReadShort(&pos);

	if ( version != XANIM_VERSION )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xanim '%s' out of date (version %d, expecting %d)\n", name, version, XANIM_VERSION);
		return 0;
	}

	numframes = XAnim_ReadShort(&pos);
	numBones = XAnim_ReadShort(&pos);
	assert(numBones <= DOBJ_MAX_PARTS);

	if ( numBones )
		boneNames = (uint16_t *)Alloc(2 * numBones);
	else
		boneNames = 0;

	byte flags = *pos++;

	bLoop = flags & 1;
	bDelta = (flags & 2) != 0;

	parts = (XAnimParts *)Alloc(sizeof(XAnimParts));
	assert(parts);

	parts->boneCount = numBones;
	parts->names = boneNames;
	parts->framerate = (float)XAnim_ReadShort(&pos);
	parts->bLoop = bLoop;
	parts->bDelta = bDelta;

	assert(numframes);

	if ( bLoop )
		numframes += 1;

	useSmallIndices = numframes <= 256;
	parts->numframes = numframes - 1;

	parts->frequency = 0;

	if ( parts->numframes )
		parts->frequency = parts->framerate / (float)parts->numframes;

	assert(parts->frequency >= 0);

	if ( bDelta )
	{
		deltaPart = (XAnimDeltaPart *)Alloc(sizeof(XAnimDeltaPart));
		parts->deltaPart = deltaPart;

		numQuatIndices = XAnim_ReadShort(&pos);

		if ( numQuatIndices )
		{
			if ( numQuatIndices == 1 )
			{
				ConsumeQuat2(&pos, quat);

				deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(sizeof(XAnimDeltaPartQuat));
				deltaPart->quat->size = 0;

				deltaPart->quat->u.frame0[0] = quat[0];
				deltaPart->quat->u.frame0[1] = quat[1];
			}
			else
			{
				if ( numQuatIndices < numframes )
				{
					assert(numQuatIndices <= numframes);

					if ( useSmallIndices )
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(numQuatIndices + sizeof(XAnimDeltaPartQuat));
						memcpy(&deltaPart->quat->u.frames.indices, pos, numQuatIndices);
						pos += numQuatIndices;
					}
					else
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(2 * numQuatIndices + sizeof(XAnimDeltaPartQuat));
						memcpy(&deltaPart->quat->u.frames.indices, pos, 2 * numQuatIndices);
						pos += 2 * numQuatIndices;
					}
				}
				else
				{
					deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(sizeof(XAnimDeltaPartQuat));
				}

				deltaPart->quat->size = numQuatIndices - 1;
				deltaPart->quat->u.frames.frames = (int16_t (*)[2])Alloc(4 * numQuatIndices);

				for ( i = 0; i < numQuatIndices; i++ )
				{
					ConsumeQuat2(&pos, quat);

					deltaPart->quat->u.frames.frames[i][0] = quat[0];
					deltaPart->quat->u.frames.frames[i][1] = quat[1];
				}

				for ( i = 1; i < numQuatIndices; i++ )
				{
					short *sQ = deltaPart->quat->u.frames.frames[i];

					if ( sQ[1] * *(sQ - 1) + *(sQ - 2) * sQ[0] < 0 )
					{
						sQ[0] = -sQ[0];
						sQ[1] = -sQ[1];
					}
				}
			}
		}
		else
		{
			deltaPart->quat = 0;
		}

		numTransIndices = XAnim_ReadShort(&pos);

		if ( numTransIndices )
		{
			if ( numTransIndices == 1 )
			{
				vec3_t t;

				t[0] = XAnim_ReadFloat(&pos);
				t[1] = XAnim_ReadFloat(&pos);
				t[2] = XAnim_ReadFloat(&pos);

				deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(sizeof(XAnimDeltaPartTrans) * 2);
				deltaPart->trans->size = 0;

				VectorCopy(t, deltaPart->trans->u.frame0);
			}
			else
			{
				if ( numTransIndices < numframes )
				{
					assert(numTransIndices <= numframes);

					if ( useSmallIndices )
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(numTransIndices + sizeof(XAnimDeltaPartTrans));
						memcpy(&deltaPart->trans->u.frames.indices, pos, numTransIndices);
						pos += numTransIndices;
					}
					else
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(2 * numTransIndices + sizeof(XAnimDeltaPartTrans));
						memcpy(&deltaPart->trans->u.frames.indices, pos, 2 * numTransIndices);
						pos += 2 * numTransIndices;
					}
				}
				else
				{
					deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(sizeof(XAnimDeltaPartTrans));
				}

				deltaPart->trans->size = numTransIndices - 1;
				deltaPart->trans->u.frames.frames = (float (*)[3])Alloc(12 * numTransIndices);

				for ( i = 0; i < numTransIndices; i++ )
				{
					deltaPart->trans->u.frames.frames[i][0] = XAnim_ReadFloat(&pos);
					deltaPart->trans->u.frames.frames[i][1] = XAnim_ReadFloat(&pos);
					deltaPart->trans->u.frames.frames[i][2] = XAnim_ReadFloat(&pos);
				}
			}
		}
		else
		{
			deltaPart->trans = 0;
		}
	}

	if ( numBones )
	{
		count = ((numBones - 1) >> 3) + 1;

		quatBits = pos;
		pos += count;

		bits = (char *)Alloc(count);
		memcpy(bits, pos, count);
		pos += count;

		parts->simpleQuatBits = bits;
		parts->parts = (XAnimPart *)Alloc(sizeof(XAnimPart) * numBones);
	}
	else
	{
		quatBits = 0;
		bits = 0;
	}

	for ( i = 0; i < numBones; i++ )
	{
		count = strlen((const char *)pos) + 1;
		*boneNames = SL_GetStringOfLen((const char *)pos, 0, count);
		pos += count;
	}

	for ( i = 0; i < numBones; i++ )
	{
		int bFlipQuat = ((int)quatBits[i >> 3] >> (i & 7)) & 1;
		int bSimpleQuat = (bits[i >> 3] >> (i & 7)) & 1;

		XAnimPart *part = &parts->parts[i];

		numQuatIndices = XAnim_ReadShort(&pos);

		if ( numQuatIndices )
		{
			if ( numQuatIndices == 1 )
			{
				if ( bSimpleQuat )
				{
					ConsumeQuat2(&pos, quat);

					if ( bFlipQuat )
					{
						quat[0] = -quat[0];
						quat[1] = -quat[1];
					}

					part->quat = (XAnimPartQuat *)Alloc(sizeof(XAnimPartQuat));

					part->quat->u.frame0[0] = quat[0];
					part->quat->u.frame0[1] = quat[1];
				}
				else
				{
					ConsumeQuat(&pos, quat);

					if ( bFlipQuat )
					{
						quat[0] = -quat[0];
						quat[1] = -quat[1];
						quat[2] = -quat[2];
						quat[3] = -quat[3];
					}

					part->quat = (XAnimPartQuat *)Alloc(sizeof(XAnimPartQuat) + sizeof(float));

					part->quat->u.frame0[0] = quat[0];
					part->quat->u.frame0[1] = quat[1];
					part->quat->u.frame0[2] = quat[2];
					part->quat->u.frame0[3] = quat[3];
				}

				part->quat->size = 0;
			}
			else
			{
				if ( numQuatIndices < numframes )
				{
					assert(numQuatIndices <= numframes);

					if ( useSmallIndices )
					{
						part->quat = (XAnimPartQuat *)Alloc(numQuatIndices + sizeof(XAnimPartQuat));
						memcpy(&part->quat->u.frames.indices, pos, numQuatIndices);
						pos += numQuatIndices;
					}
					else
					{
						part->quat = (XAnimPartQuat *)Alloc(2 * numQuatIndices + sizeof(XAnimPartQuat));
						memcpy(&part->quat->u.frames.indices, pos, 2 * numQuatIndices);
						pos += 2 * numQuatIndices;
					}
				}
				else
				{
					part->quat = (XAnimPartQuat *)Alloc(sizeof(XAnimPartQuat));
				}

				if ( bSimpleQuat )
				{
					part->quat->u.frames.u.frames = (int16_t (*)[4])Alloc(4 * numQuatIndices);
					ConsumeQuat2(&pos, quat);

					if ( bFlipQuat )
					{
						quat[0] = -quat[0];
						quat[1] = -quat[1];
					}

					part->quat->u.frames.u.frames2[0][0] = quat[0];
					part->quat->u.frames.u.frames2[0][1] = quat[1];

					for ( j = 1; j < numQuatIndices; j++ )
					{
						ConsumeQuat2(&pos, quat);

						part->quat->u.frames.u.frames2[j][0] = quat[0];
						part->quat->u.frames.u.frames2[j][1] = quat[1];
					}

					for ( j = 1; j < numQuatIndices; j++ )
					{
						sQ = part->quat->u.frames.u.frames2[j];

						if ( sQ[1] * *(sQ - 1) + *(sQ - 2) * sQ[0] < 0 )
						{
							sQ[0] = -sQ[0];
							sQ[1] = -sQ[1];
						}
					}
				}
				else
				{
					part->quat->u.frames.u.frames = (int16_t (*)[4])Alloc(8 * numQuatIndices);
					ConsumeQuat(&pos, quat);

					if ( bFlipQuat )
					{
						quat[0] = -quat[0];
						quat[1] = -quat[1];
						quat[2] = -quat[2];
						quat[3] = -quat[3];
					}

					part->quat->u.frames.u.frames[0][0] = quat[0];
					part->quat->u.frames.u.frames[0][1] = quat[1];
					part->quat->u.frames.u.frames[0][2] = quat[2];
					part->quat->u.frames.u.frames[0][3] = quat[3];

					for ( j = 1; j < numQuatIndices; j++ )
					{
						ConsumeQuat(&pos, quat);

						part->quat->u.frames.u.frames[j][0] = quat[0];
						part->quat->u.frames.u.frames[j][1] = quat[1];
						part->quat->u.frames.u.frames[j][2] = quat[2];
						part->quat->u.frames.u.frames[j][3] = quat[3];
					}

					for ( j = 1; j < numQuatIndices; j++ )
					{
						sQ = part->quat->u.frames.u.frames[j];
						short *temp = sQ - 4;

						if ( sQ[1] * temp[1] + *temp * *sQ + sQ[3] * temp[3] + temp[2] * sQ[2] < 0 )
						{
							sQ[0] = -sQ[0];
							sQ[1] = -sQ[1];
							sQ[2] = -sQ[2];
							sQ[3] = -sQ[3];
						}
					}
				}

				part->quat->size = numQuatIndices - 1;
			}
		}
		else
		{
			part->quat = 0;
		}

		numTransIndices = XAnim_ReadShort(&pos);

		if ( numTransIndices )
		{
			if ( numTransIndices == 1 )
			{
				vec3_t t;

				t[0] = XAnim_ReadFloat(&pos);
				t[1] = XAnim_ReadFloat(&pos);
				t[2] = XAnim_ReadFloat(&pos);

				part->trans = (XAnimPartTrans *)Alloc(sizeof(XAnimPartTrans) * 2);
				part->trans->size = 0;

				VectorCopy(t, part->trans->u.frame0);
			}
			else
			{
				if ( numTransIndices < numframes )
				{
					assert(numQuatIndices <= numframes);

					if ( useSmallIndices )
					{
						part->trans = (XAnimPartTrans *)Alloc(numTransIndices + sizeof(XAnimPartTrans));
						memcpy(&part->trans->u.frames.indices, pos, numTransIndices);
						pos += numTransIndices;
					}
					else
					{
						part->trans = (XAnimPartTrans *)Alloc(2 * numTransIndices + sizeof(XAnimPartTrans));
						memcpy(&part->trans->u.frames.indices, pos, 2 * numTransIndices);
						pos += 2 * numTransIndices;
					}
				}
				else
				{
					part->trans = (XAnimPartTrans *)Alloc(sizeof(XAnimPartTrans));
				}

				part->trans->size = numTransIndices - 1;
				part->trans->u.frames.frames = (float (*)[3])Alloc(12 * numTransIndices);

				for ( j = 0; j < numTransIndices; j++ )
				{
					part->trans->u.frames.frames[j][0] = XAnim_ReadFloat(&pos);
					part->trans->u.frames.frames[j][1] = XAnim_ReadFloat(&pos);
					part->trans->u.frames.frames[j][2] = XAnim_ReadFloat(&pos);
				}
			}
		}
		else
		{
			part->trans = 0;
		}
	}

	ReadNoteTracks(name, &pos, parts, Alloc);
	FS_FreeFile(buf);

	return parts;
}
