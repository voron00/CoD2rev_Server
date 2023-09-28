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
	vec3_t vTrans;
	short quat[4];
	bool useSmallIndices;
	unsigned short numTransIndices;
	unsigned short numQuatIndices;
	bool bDelta;
	bool bLoop;
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
		names = (uint16_t *)Alloc(sizeof(uint16_t) * boneCount);
	else
		names = 0;

	flags = *pos++;
	bLoop = flags & 1;
	bDelta = (flags & 2) != 0;
	framerate = XAnim_ReadShort(&pos);
	parts = (XAnimParts_s *)Alloc(sizeof(XAnimParts_s));
	parts->boneCount = boneCount;
	parts->names = names;
	parts->framerate = (float)framerate;
	parts->bLoop = bLoop;
	parts->bDelta = bDelta;

	if ( bLoop )
		numloopframes = frames + 1;
	else
		numloopframes = frames;

	useSmallIndices = numloopframes <= 0x100u;
	parts->numframes = numloopframes - 1;

	if ( parts->numframes )
		parts->frequency = parts->framerate / (float)parts->numframes;
	else
		parts->frequency = 0.0;

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
				if ( numQuatIndices >= numloopframes )
				{
					deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(sizeof(XAnimDeltaPartQuat));
				}
				else
				{
					if ( useSmallIndices )
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(numQuatIndices + sizeof(XAnimDeltaPartQuat));
						memcpy(deltaPart->quat->u.frames.indices._1, pos, numQuatIndices);
						pos += numQuatIndices;
					}
					else
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(2 * numQuatIndices + sizeof(XAnimDeltaPartQuat));
						memcpy(deltaPart->quat->u.frames.indices._1, pos, 2 * numQuatIndices);
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

		numTransIndices = XAnim_ReadShort(&pos);

		if ( numTransIndices )
		{
			if ( numTransIndices == 1 )
			{
				vTrans[0] = XAnim_ReadFloat(&pos);
				vTrans[1] = XAnim_ReadFloat(&pos);
				vTrans[2] = XAnim_ReadFloat(&pos);
				deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(sizeof(XAnimDeltaPartTrans) * 2);
				deltaPart->trans->size = 0;
				VectorCopy(vTrans, deltaPart->trans->u.frame0);
			}
			else
			{
				if ( numTransIndices >= numloopframes )
				{
					deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(sizeof(XAnimDeltaPartTrans));
				}
				else
				{
					if ( useSmallIndices )
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(numTransIndices + sizeof(XAnimDeltaPartTrans));
						memcpy(deltaPart->trans->u.frames.indices._1, pos, numTransIndices);
						pos += numTransIndices;
					}
					else
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(2 * numTransIndices + sizeof(XAnimDeltaPartTrans));
						memcpy(deltaPart->trans->u.frames.indices._1, pos, 2 * numTransIndices);
						pos += 2 * numTransIndices;
					}
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
		parts->parts = (XAnimPart *)Alloc(sizeof(XAnimPart) * boneCount);
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
				if ( numQuatIndices >= numloopframes )
				{
					part->quat = (XAnimPartQuat *)Alloc(sizeof(XAnimPartQuat));
				}
				else
				{
					if ( useSmallIndices )
					{
						part->quat = (XAnimPartQuat *)Alloc(numQuatIndices + sizeof(XAnimPartQuat));
						memcpy(part->quat->u.frames.indices._1, pos, numQuatIndices);
						pos += numQuatIndices;
					}
					else
					{
						part->quat = (XAnimPartQuat *)Alloc(2 * numQuatIndices + sizeof(XAnimPartQuat));
						memcpy(part->quat->u.frames.indices._1, pos, 2 * numQuatIndices);
						pos += 2 * numQuatIndices;
					}
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

		numTransIndices = XAnim_ReadShort(&pos);

		if ( numTransIndices )
		{
			if ( numTransIndices == 1 )
			{
				vTrans[0] = XAnim_ReadFloat(&pos);
				vTrans[1] = XAnim_ReadFloat(&pos);
				vTrans[2] = XAnim_ReadFloat(&pos);
				part->trans = (XAnimPartTrans *)Alloc(sizeof(XAnimPartTrans) * 2);
				part->trans->size = 0;
				VectorCopy(vTrans, part->trans->u.frame0);
			}
			else
			{
				if ( numTransIndices >= numloopframes )
				{
					part->trans = (XAnimPartTrans *)Alloc(sizeof(XAnimPartTrans));
				}
				else
				{
					if ( useSmallIndices )
					{
						part->trans = (XAnimPartTrans *)Alloc(numTransIndices + sizeof(XAnimPartTrans));
						memcpy(part->trans->u.frames.indices._1, pos, numTransIndices);
						pos += numTransIndices;
					}
					else
					{
						part->trans = (XAnimPartTrans *)Alloc(2 * numTransIndices + sizeof(XAnimPartTrans));
						memcpy(part->trans->u.frames.indices._1, pos, 2 * numTransIndices);
						pos += 2 * numTransIndices;
					}
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
