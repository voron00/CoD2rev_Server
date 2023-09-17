#include "../qcommon/qcommon.h"

int XAnim_ReadShort(const unsigned char **pos)
{
	short dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += 2;
	return dest;
}

int XAnim_ReadUnsignedShort(const unsigned char **pos)
{
	unsigned short dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += 2;
	return dest;
}

int XAnim_ReadInt(const unsigned char **pos)
{
	int dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += 4;
	return dest;
}

float XAnim_ReadFloat(const unsigned char **pos)
{
	float dest;

	memcpy(&dest, *pos, sizeof(dest));
	*pos += 4;
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
		out[3] = (int)floor(sqrt((float)temp) + 0.5);
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
		out[1] = (int)floor(sqrt((float)temp) + 0.5);
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
	notify = (XAnimNotifyInfo *)Alloc(8 * parts->notifyCount);
	parts->notify = notify;

	for ( i = 0; i < numNoteTracks; ++i )
	{
		notify->name = SL_GetString_((const char *)*pos, 0);
		*pos += strlen((const char *)*pos) + 1;
		frametime = (unsigned short)XAnim_ReadUnsignedShort(pos);

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

XAnimParts_s* XAnimLoadFile(const char *name, void *(*Alloc)(int))
{
	bool bSimpleQuat;
	bool bFlipQuat;
	char *simpleQuatBits;
	short *temp;
	int16_t *sQ;
	int i;
	int j;
	int k;
	int m;
	int n;
	int ii;
	int jj;
	int kk;
	int mm;
	int numIndices;
	vec3_t vTrans;
	short quat[4];
	bool useSmallIndices;
	unsigned short numTransIndices;
	unsigned short numQuatIndices;
	bool deltaFlag;
	bool loopFlag;
	char flags;
	short version;
	short framerate;
	size_t len;
	char filename[64];
	const unsigned char *quatBits;
	const unsigned char *pos;
	byte *buf;
	XAnimDeltaPart *deltaPart;
	XAnimPart *part;
	unsigned short numloopframes;
	unsigned short frames;
	short boneCount;
	XAnimParts_s *parts;
	uint16_t *names;
	int fileSize;

	if ( Com_sprintf(filename, sizeof(filename), "xanim/%s", name) <= 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return 0;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize <= 0 )
	{
		Com_Printf("^1ERROR: xanim '%s' not found\n", name);
		return 0;
	}

	if ( !fileSize )
	{
		Com_Printf("^1ERROR: xanim '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return 0;
	}

	pos = buf;
	version = XAnim_ReadShort(&pos);

	if ( version != XANIM_VERSION )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xanim '%s' out of date (version %d, expecting %d)\n", name, version, 14);
		return 0;
	}

	frames = XAnim_ReadShort(&pos);
	boneCount = XAnim_ReadShort(&pos);

	if ( boneCount )
		names = (uint16_t *)Alloc(2 * boneCount);
	else
		names = 0;

	flags = *pos++;
	loopFlag = flags & 1;
	deltaFlag = (flags & 2) != 0;
	framerate = XAnim_ReadShort(&pos);
	parts = (XAnimParts_s *)Alloc(44);
	parts->boneCount = boneCount;
	parts->names = names;
	parts->framerate = (float)framerate;
	parts->bLoop = loopFlag;
	parts->bDelta = deltaFlag;

	if ( loopFlag )
		numloopframes = frames + 1;
	else
		numloopframes = frames;

	useSmallIndices = numloopframes <= 0x100u;
	parts->numframes = numloopframes - 1;

	if ( parts->numframes )
		parts->frequency = parts->framerate / (float)parts->numframes;
	else
		parts->frequency = 0.0;

	if ( deltaFlag )
	{
		deltaPart = (XAnimDeltaPart *)Alloc(8);
		parts->deltaPart = deltaPart;
		numQuatIndices = XAnim_ReadUnsignedShort(&pos);

		if ( numQuatIndices )
		{
			if ( numQuatIndices == 1 )
			{
				ConsumeQuat2(&pos, quat);
				deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(8);
				deltaPart->quat->size = 0;
				deltaPart->quat->u.frame0[0] = quat[0];
				deltaPart->quat->u.frame0[1] = quat[1];
			}
			else
			{
				if ( numQuatIndices >= numloopframes )
				{
					deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(8);
				}
				else
				{
					if ( useSmallIndices )
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(numQuatIndices + 8);
						memcpy(&deltaPart->quat->u.frame0[2], pos, numQuatIndices);
						pos += numQuatIndices;
					}
					else
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(2 * numQuatIndices + 8);
						memcpy(&deltaPart->quat->u.frame0[2], pos, 2 * numQuatIndices);
						pos += 2 * numQuatIndices;
					}
				}

				deltaPart->quat->size = numQuatIndices - 1;
				deltaPart->quat->u.frames.frames = (int16_t (*)[2])Alloc(4 * numQuatIndices);

				for ( i = 0; i < numQuatIndices; ++i )
				{
					ConsumeQuat2(&pos, quat);
					deltaPart->quat->u.frames.frames[i][0] = quat[0];
					deltaPart->quat->u.frames.frames[i][1] = quat[1];
				}

				for ( j = 1; j < numQuatIndices; ++j )
				{
					sQ = deltaPart->quat->u.frames.frames[j];

					if ( sQ[1] * *(sQ - 1) + *(sQ - 2) * *sQ < 0 )
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

		numTransIndices = XAnim_ReadUnsignedShort(&pos);

		if ( numTransIndices )
		{
			if ( numTransIndices == 1 )
			{
				vTrans[0] = XAnim_ReadFloat(&pos);
				vTrans[1] = XAnim_ReadFloat(&pos);
				vTrans[2] = XAnim_ReadFloat(&pos);
				deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(16);
				deltaPart->trans->size = 0;
				VectorCopy(vTrans, deltaPart->trans->u.frame0);
			}
			else
			{
				if ( numTransIndices >= numloopframes )
				{
					deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(8);
				}
				else
				{
					if ( useSmallIndices )
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(numTransIndices + 8);
						numIndices = numTransIndices;
						memcpy(&deltaPart->trans->u.frame0[1], pos, numTransIndices);
					}
					else
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(2 * numTransIndices + 8);
						numIndices = 2 * numTransIndices;
						memcpy(&deltaPart->trans->u.frame0[1], pos, numIndices);
					}

					pos += numIndices;
				}

				deltaPart->trans->size = numTransIndices - 1;
				deltaPart->trans->u.frames.frames = (float (*)[3])Alloc(12 * numTransIndices);

				for ( k = 0; k < numTransIndices; ++k )
				{
					deltaPart->trans->u.frames.frames[k][0] = XAnim_ReadFloat(&pos);
					deltaPart->trans->u.frames.frames[k][1] = XAnim_ReadFloat(&pos);
					deltaPart->trans->u.frames.frames[k][2] = XAnim_ReadFloat(&pos);
				}
			}
		}
		else
		{
			deltaPart->trans = 0;
		}
	}

	if ( boneCount )
	{
		len = ((boneCount - 1) >> 3) + 1;
		quatBits = pos;
		pos += len;
		simpleQuatBits = (char *)Alloc(len);
		memcpy(simpleQuatBits, pos, len);
		pos += len;
		parts->simpleQuatBits = simpleQuatBits;
		parts->parts = (XAnimPart *)Alloc(8 * boneCount);
	}
	else
	{
		quatBits = 0;
		simpleQuatBits = 0;
	}

	for ( m = 0; m < boneCount; ++m )
	{
		len = strlen((const char *)pos) + 1;
		names[m] = SL_GetStringOfLen((const char *)pos, 0, len);
		pos += len;
	}

	for ( m = 0; m < boneCount; ++m )
	{
		bFlipQuat = ((int)quatBits[m >> 3] >> (m & 7)) & 1;
		bSimpleQuat = (simpleQuatBits[m >> 3] >> (m & 7)) & 1;
		part = &parts->parts[m];
		numQuatIndices = XAnim_ReadUnsignedShort(&pos);

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

					part->quat = (XAnimPartQuat *)Alloc(8);
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

					part->quat = (XAnimPartQuat *)Alloc(12);
					part->quat->u.frame0[0] = quat[0];
					part->quat->u.frame0[1] = quat[1];
					part->quat->u.frame0[2] = quat[2];
					part->quat->u.frame0[3] = quat[3];
				}

				part->quat->size = 0;
			}
			else
			{
				if ( numQuatIndices >= numloopframes )
				{
					part->quat = (XAnimPartQuat *)Alloc(8);
				}
				else
				{
					if ( useSmallIndices )
					{
						part->quat = (XAnimPartQuat *)Alloc(numQuatIndices + 8);
						numIndices = numQuatIndices;
						memcpy(&part->quat->u.frame02[2], pos, numQuatIndices);
					}
					else
					{
						part->quat = (XAnimPartQuat *)Alloc(2 * numQuatIndices + 8);
						numIndices = 2 * numQuatIndices;
						memcpy(&part->quat->u.frame02[2], pos, numIndices);
					}

					pos += numIndices;
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

					(*part->quat->u.frames.u.frames)[0] = quat[0];
					(*part->quat->u.frames.u.frames)[1] = quat[1];

					for ( n = 1; n < numQuatIndices; ++n )
					{
						ConsumeQuat2(&pos, quat);
						(*part->quat->u.frames.u.frames)[2 * n] = quat[0];
						(*part->quat->u.frames.u.frames)[2 * n + 1] = quat[1];
					}

					for ( ii = 1; ii < numQuatIndices; ++ii )
					{
						sQ = &(*part->quat->u.frames.u.frames)[2 * ii];

						if ( sQ[1] * *(sQ - 1) + *(sQ - 2) * *sQ < 0 )
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

					(*part->quat->u.frames.u.frames)[0] = quat[0];
					(*part->quat->u.frames.u.frames)[1] = quat[1];
					(*part->quat->u.frames.u.frames)[2] = quat[2];
					(*part->quat->u.frames.u.frames)[3] = quat[3];

					for ( jj = 1; jj < numQuatIndices; ++jj )
					{
						ConsumeQuat(&pos, quat);
						part->quat->u.frames.u.frames[jj][0] = quat[0];
						part->quat->u.frames.u.frames[jj][1] = quat[1];
						part->quat->u.frames.u.frames[jj][2] = quat[2];
						part->quat->u.frames.u.frames[jj][3] = quat[3];
					}

					for ( kk = 1; kk < numQuatIndices; ++kk )
					{
						sQ = part->quat->u.frames.u.frames[kk];
						temp = sQ - 4;

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

		numTransIndices = XAnim_ReadUnsignedShort(&pos);

		if ( numTransIndices )
		{
			if ( numTransIndices == 1 )
			{
				vTrans[0] = XAnim_ReadFloat(&pos);
				vTrans[1] = XAnim_ReadFloat(&pos);
				vTrans[2] = XAnim_ReadFloat(&pos);
				part->trans = (XAnimPartTrans *)Alloc(16);
				part->trans->size = 0;
				VectorCopy(vTrans, part->trans->u.frame0);
			}
			else
			{
				if ( numTransIndices >= numloopframes )
				{
					part->trans = (XAnimPartTrans *)Alloc(8);
				}
				else
				{
					if ( useSmallIndices )
					{
						part->trans = (XAnimPartTrans *)Alloc(numTransIndices + 8);
						numIndices = numTransIndices;
						memcpy(&part->trans->u.frame0[1], pos, numTransIndices);
					}
					else
					{
						part->trans = (XAnimPartTrans *)Alloc(2 * numTransIndices + 8);
						numIndices = 2 * numTransIndices;
						memcpy(&part->trans->u.frame0[1], pos, numIndices);
					}

					pos += numIndices;
				}

				part->trans->size = numTransIndices - 1;
				part->trans->u.frames.frames = (float (*)[3])Alloc(12 * numTransIndices);

				for ( mm = 0; mm < numTransIndices; ++mm )
				{
					part->trans->u.frames.frames[mm][0] = XAnim_ReadFloat(&pos);
					part->trans->u.frames.frames[mm][1] = XAnim_ReadFloat(&pos);
					part->trans->u.frames.frames[mm][2] = XAnim_ReadFloat(&pos);
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
