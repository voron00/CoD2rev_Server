#include "../qcommon/qcommon.h"

void XModelCalcBasePose(XModelParts_s *modelParts)
{
	vec4_t tempQuat;
	unsigned char *parentList;
	DObjAnimMat *quatTrans;
	int numBones;
	float *trans;
	short *quats;
	int numRootBones;
	float vLenSq;

	parentList = modelParts->hierarchy->parentList;
	numBones = modelParts->numBones;
	quats = modelParts->quats;
	trans = modelParts->trans;
	quatTrans = &modelParts->skel.Mat;
	numRootBones = modelParts->numRootBones;

	while ( numRootBones )
	{
		VectorClear(quatTrans->quat);
		quatTrans->quat[3] = 1.0;
		VectorClear(quatTrans->trans);
		quatTrans->transWeight = 2.0;
		--numRootBones;
		++quatTrans;
	}

	numRootBones = numBones - modelParts->numRootBones;

	while ( numRootBones )
	{
		tempQuat[0] = (float)quats[0] * 0.000030518509;
		tempQuat[1] = (float)quats[1] * 0.000030518509;
		tempQuat[2] = (float)quats[2] * 0.000030518509;
		tempQuat[3] = (float)quats[3] * 0.000030518509;

		QuatMultiply(tempQuat, quatTrans[-*parentList].quat, quatTrans->quat);
		vLenSq = Vec4LengthSq(quatTrans->quat);

		if ( vLenSq == 0.0 )
		{
			quatTrans->quat[3] = 1.0;
			quatTrans->transWeight = 2.0;
		}
		else
		{
			quatTrans->transWeight = 2.0 / vLenSq;
		}

		MatrixTransformVectorQuatTrans(trans, &quatTrans[-*parentList], quatTrans->trans);
		--numRootBones;
		quats += 4;
		trans += 3;
		++quatTrans;
		++parentList;
	}

	memset(&modelParts->skel, 255, 0x10u);
	memset(modelParts->skel.partBits.skel, 255, sizeof(modelParts->skel.partBits.skel));
}

XModelParts_s* QDECL XModelPartsLoadFile(XModel *model, const char *name, void *(*Alloc)(int))
{
	int fileSize;
	short *quats;
	short version;
	char index;
	unsigned int len;
	float *trans;
	int i;
	short numChildBones;
	short numRootBones;
	short numBones;
	byte *parentList;
	unsigned short *boneNames;
	XBoneHierarchy *boneHiearchy;
	const unsigned char *pos;
	byte *buf;
	char filename[64];
	XModelParts_s *modelParts;

	if ( Com_sprintf(filename, sizeof(filename), "xmodelparts/%s", name) <= 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return 0;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize <= 0 )
	{
		Com_Printf("^1ERROR: xmodelparts '%s' not found\n", name);
		return 0;
	}

	if ( !fileSize )
	{
		Com_Printf("^1ERROR: xmodelparts '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return 0;
	}

	pos = buf;
	version = XAnim_ReadShort(&pos);

	if ( version != XMODEL_VERSION )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xmodelparts '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);
		return 0;
	}

	numChildBones = XAnim_ReadShort(&pos);
	numRootBones = XAnim_ReadShort(&pos);
	numBones = numRootBones + numChildBones;
	boneNames = (unsigned short *)Alloc(sizeof(short) * (short)(numRootBones + numChildBones));
	model->memUsage += sizeof(short) * (short)(numRootBones + numChildBones);

	if ( (short)(numRootBones + numChildBones) >= 127 )
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xmodel '%s' has more than %d bones\n", name, 127);
		return 0;
	}

	boneHiearchy = (XBoneHierarchy *)Alloc(numChildBones + 7);
	model->memUsage += numChildBones + 7;
	boneHiearchy->names = boneNames;
	parentList = boneHiearchy->parentList;
	modelParts = (XModelParts_s *)Alloc(sizeof(DSkel_t) * numBones + (sizeof(XModelParts_s) - sizeof(DSkel_t)));
	model->memUsage += sizeof(DSkel_t) * numBones + sizeof(XModelParts_s) - sizeof(DSkel_t);
	modelParts->hierarchy = boneHiearchy;

	if ( numChildBones )
	{
		modelParts->quats = (short *)Alloc(8 * numChildBones);
		model->memUsage += 8 * numChildBones;
		modelParts->trans = (float *)Alloc(16 * numChildBones);
		model->memUsage += 16 * numChildBones;
	}
	else
	{
		modelParts->quats = 0;
		modelParts->trans = 0;
	}

	modelParts->partClassification = (byte *)Alloc(numBones);
	model->memUsage += numBones;
	modelParts->numBones = numBones;
	modelParts->numRootBones = numRootBones;
	quats = modelParts->quats;
	trans = modelParts->trans;

	while ( numRootBones < numBones )
	{
		index = *pos++;
		*parentList = numRootBones - index;
		trans[0] = XAnim_ReadFloat(&pos);
		trans[1] = XAnim_ReadFloat(&pos);
		trans[2] = XAnim_ReadFloat(&pos);
		ConsumeQuat(&pos, quats);
		++numRootBones;
		quats += 4;
		trans += 3;
		parentList++;
	}

	for ( i = 0; i < numBones; ++i )
	{
		len = I_strlen((char *)pos) + 1;
		boneNames[i] = SL_GetStringOfLen((const char *)pos, 0, len);
		pos += len;
	}

	memcpy(modelParts->partClassification, pos, numBones);
	pos += numBones;
	FS_FreeFile(buf);
	XModelCalcBasePose(modelParts);

	return modelParts;
}

void XModelLoadCollData(const unsigned char **pos, XModel *model, void *(*AllocColl)(int), const char *name)
{
	int j;
	int i;
	float *plane;
	XModelCollSurf_s *surf;

	model->numCollSurfs = XAnim_ReadInt(pos);

	if ( model->numCollSurfs )
	{
		model->collSurfs = (XModelCollSurf_s *)AllocColl(sizeof(XModelCollSurf_s) * model->numCollSurfs);

		for ( i = 0; i < model->numCollSurfs; ++i )
		{
			surf = &model->collSurfs[i];
			surf->numCollTris = XAnim_ReadInt(pos);
			surf->collTris = (XModelCollTri_s *)AllocColl(sizeof(XModelCollTri_s) * surf->numCollTris);

			for ( j = 0; j < surf->numCollTris; ++j )
			{
				plane = surf->collTris[j].plane;

				plane[0] = XAnim_ReadFloat(pos);
				plane[1] = XAnim_ReadFloat(pos);
				plane[2] = XAnim_ReadFloat(pos);
				plane[3] = XAnim_ReadFloat(pos);
				plane[4] = XAnim_ReadFloat(pos);
				plane[5] = XAnim_ReadFloat(pos);
				plane[6] = XAnim_ReadFloat(pos);
				plane[7] = XAnim_ReadFloat(pos);
				plane[8] = XAnim_ReadFloat(pos);
				plane[9] = XAnim_ReadFloat(pos);
				plane[10] = XAnim_ReadFloat(pos);
				plane[11] = XAnim_ReadFloat(pos);
			}

			surf->mins[0] = XAnim_ReadFloat(pos) - 0.001;
			surf->mins[1] = XAnim_ReadFloat(pos) - 0.001;
			surf->mins[2] = XAnim_ReadFloat(pos) - 0.001;
			surf->maxs[0] = XAnim_ReadFloat(pos) + 0.001;
			surf->maxs[1] = XAnim_ReadFloat(pos) + 0.001;
			surf->maxs[2] = XAnim_ReadFloat(pos) + 0.001;
			surf->boneIdx = XAnim_ReadInt(pos);
			surf->contents = XAnim_ReadInt(pos) & 0xDFFFFFFB;
			surf->surfFlags = XAnim_ReadInt(pos);

			model->contents |= surf->contents;
		}
	}
}

bool XModelLoadConfigFile(const char *name, const unsigned char **pos, XModelConfig *config)
{
	short version;
	int i;

	version = XAnim_ReadShort(pos);

	if ( version != XMODEL_VERSION )
	{
		Com_Printf("^1ERROR: xmodel '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);
		return 0;
	}

	config->flags = *(*pos)++;

	config->mins[0] = XAnim_ReadFloat(pos);
	config->mins[1] = XAnim_ReadFloat(pos);
	config->mins[2] = XAnim_ReadFloat(pos);
	config->maxs[0] = XAnim_ReadFloat(pos);
	config->maxs[1] = XAnim_ReadFloat(pos);
	config->maxs[2] = XAnim_ReadFloat(pos);

	for ( i = 0; i < 4; ++i )
	{
		config->entries[i].dist = XAnim_ReadFloat(pos);
		strcpy(config->entries[i].filename, (const char *)*pos);
		*pos += strlen((const char *)*pos) + 1;
	}

	config->collLod = XAnim_ReadInt(pos);
	return 1;
}

XModel* XModelLoadFile(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	unsigned short **surfNames;
	int fileSize;
	int usage;
	XBoneInfo *bones;
	vec3_t radius;
	float *offset;
	float *maxs;
	float *mins;
	int numBones;
	byte *buf;
	char filename[64];
	int j;
	int i;
	int nameLenTotal;
	char *dest;
	const unsigned char *pos;
	int nameLen[4];
	XModel *model;
	XModelConfig config;

	if ( Com_sprintf(filename, sizeof(filename), "xmodel/%s", name) <= 0 )
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return 0;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize <= 0 )
	{
		Com_Printf("^1ERROR: xmodel '%s' not found\n", name);
		return 0;
	}

	if ( !fileSize )
	{
		Com_Printf("^1ERROR: xmodel '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return 0;
	}

	pos = buf;

	if ( !XModelLoadConfigFile(name, &pos, &config) )
	{
		Com_Printf("^1ERROR: xmodel '%s' has no config\n", name);
		FS_FreeFile(buf);
		return 0;
	}

	nameLenTotal = 0;

	for ( i = 0; i < 4; ++i )
	{
		nameLen[i] = I_strlen(config.entries[i].filename) + 1;
		nameLenTotal += nameLen[i];
	}

	usage = nameLenTotal + sizeof(XModel);
	model = (XModel *)Alloc(usage);
	model->memUsage = usage;

	XModelLoadCollData(&pos, model, AllocColl, name);

	dest = (char *)&model[1];
	model->numLods = 0;

	for ( i = 0; i < 4; ++i )
	{
		strcpy(dest, config.entries[i].filename);
		model->lodInfo[i].filename = dest;

		if ( *dest )
		{
			++model->numLods;
			model->lodInfo[i].numsurfs = XAnim_ReadShort(&pos);
			usage = 2 * model->lodInfo[i].numsurfs;
			surfNames = &model->lodInfo[i].surfNames;
			*surfNames = (unsigned short *)Alloc(usage);
			model->memUsage += usage;

			for ( j = 0; j < model->lodInfo[i].numsurfs; ++j )
			{
				pos += strlen((const char *)pos) + 1;
				model->lodInfo[i].surfNames[j] = SL_GetString_((const char *)pos, 0);
			}
		}
		else
		{
			model->lodInfo[i].surfNames = 0;
		}

		model->lodInfo[i].dist = config.mins[257 * i - 772];
		dest += nameLen[i];
	}

	model->parts = XModelPartsPrecache(model, model->lodInfo[0].filename, Alloc);

	if ( !model->parts )
	{
		FS_FreeFile(buf);
		XModelFree(model);
		return 0;
	}

	numBones = model->parts->numBones;
	bones = (XBoneInfo *)Alloc(sizeof(XBoneInfo) * numBones);
	model->memUsage += sizeof(XBoneInfo) * numBones;

	for ( i = 0; i < numBones; ++i )
	{
		mins = bones[i].bounds[0];

		mins[0] = XAnim_ReadFloat(&pos);
		mins[1] = XAnim_ReadFloat(&pos);
		mins[2] = XAnim_ReadFloat(&pos);

		maxs = bones[i].bounds[1];

		maxs[0] = XAnim_ReadFloat(&pos);
		maxs[1] = XAnim_ReadFloat(&pos);
		maxs[2] = XAnim_ReadFloat(&pos);

		offset = bones[i].offset;

		offset[0] = (mins[0] + maxs[0]) * 0.5;
		offset[1] = (mins[1] + maxs[1]) * 0.5;
		offset[2] = (mins[2] + maxs[2]) * 0.5;

		VectorSubtract(maxs, offset, radius);

		bones[i].radiusSquared = VectorLengthSquared(radius);
	}

	model->boneInfo = bones;
	FS_FreeFile(buf);
	VectorCopy(config.mins, model->mins);
	VectorCopy(config.maxs, model->maxs);
	model->collLod = config.collLod;
	model->flags = config.flags;

	return model;
}
