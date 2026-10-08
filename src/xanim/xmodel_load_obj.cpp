#include "../qcommon/qcommon.h"

/*
================
XModelGetStaticBounds
================
*/
int XModelGetStaticBounds( const XModel *model, vec3_t axis[3], vec3_t mins, vec3_t maxs )
{
	int i, j, k;
	vec3_t rotated, corner;
	XModelCollSurf *csurf;

	if ( !model->numCollSurfs )
	{
		assert(!model->contents);
		return 0;
	}

	VectorSet(mins, FLT_MAX, FLT_MAX, FLT_MAX);
	VectorSet(maxs, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	for ( i = 0; i < model->numCollSurfs; i++ )
	{
		csurf = &model->collSurfs[i];

		for ( j = 0; j < 8; j++ )
		{
			if ( j & 1 )
				corner[0] = csurf->mins[0];
			else
				corner[0] = csurf->maxs[0];

			if ( j & 2 )
				corner[1] = csurf->mins[1];
			else
				corner[1] = csurf->maxs[1];

			if ( j & 4 )
				corner[2] = csurf->mins[2];
			else
				corner[2] = csurf->maxs[2];

			MatrixTransformVector(corner, axis, rotated);

			for ( k = 0; k < 3; k++ )
			{
				if ( mins[k] > rotated[k] )
					mins[k] = rotated[k];

				if ( rotated[k] > maxs[k] )
					maxs[k] = rotated[k];
			}
		}
	}

	return 1;
}

/*
================
XModelCalcBasePose
================
*/
void XModelCalcBasePose( XModelParts *modelParts )
{
	vec4_t tempQuat;
	unsigned char *parentList;
	DObjAnimMat *quatTrans;
	int numBones;
	float *trans;
	short *quats;
	int i;

	parentList = modelParts->hierarchy->parentList;
	numBones = modelParts->numBones;

	quats = modelParts->quats;
	trans = modelParts->trans;

	for ( quatTrans = &modelParts->skel.Mat, i = modelParts->numRootBones; i; i--, quatTrans++ )
	{
		VectorClear(quatTrans->quat);
		quatTrans->quat[3] = 1.0f;

		VectorClear(quatTrans->trans);
		quatTrans->transWeight = 2.0f;
	}

	for ( i = numBones - modelParts->numRootBones; i; i--, quats += 4, trans += 3, quatTrans++, parentList++ )
	{
		tempQuat[0] = quats[0] * RANDSCALE;
		tempQuat[1] = quats[1] * RANDSCALE;
		tempQuat[2] = quats[2] * RANDSCALE;
		tempQuat[3] = quats[3] * RANDSCALE;

		QuatMultiply(tempQuat, quatTrans[-*parentList].quat, quatTrans->quat);
		NormalizeQuatTrans(quatTrans);
		MatrixTransformVectorQuatTrans(trans, &quatTrans[-*parentList], quatTrans->trans);
	}

	memset(modelParts->skel.partBits.anim, NO_BONEINDEX, sizeof(modelParts->skel.partBits.anim));
	memset(modelParts->skel.partBits.skel, NO_BONEINDEX, sizeof(modelParts->skel.partBits.skel));
}

/*
================
XModelPartsLoadFile
================
*/
XModelParts_s *XModelPartsLoadFile( XModel *model, const char *name, Alloc_t Alloc )
{
	short *quats;
	float *trans;
	int i, fileSize, numBones, size;
	short numRootBones, numChildBones;
	byte *parentList;
	unsigned short *boneNames;
	XBoneHierarchy *boneHiearchy;
	const unsigned char *pos;
	byte *buf;
	char filename[MAX_QPATH];
	XModelParts_s *modelParts;

	if ( Com_sprintf(filename, sizeof(filename), "xmodelparts/%s", name) <= 0 )
	{
		Com_Printf("ERROR: filename '%s' too long\n", filename);
		return NULL;
	}

	fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize < 0 )
	{
		assert(!buf);
		Com_Printf("ERROR: xmodelparts '%s' not found\n", name);

		return NULL;
	}

	if ( fileSize == 0 )
	{
		Com_Printf("ERROR: xmodelparts '%s' has 0 length\n", name);
		FS_FreeFile(buf);

		return NULL;
	}

	assert(buf);
	pos = buf;

	short version = ConsumeShort(&pos);

	if ( version != XMODEL_VERSION )
	{
		FS_FreeFile(buf);
		Com_Printf("ERROR: xmodelparts '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);

		return NULL;
	}

	numChildBones = ConsumeShort(&pos);
	numRootBones = ConsumeShort(&pos);

	numBones = numRootBones + numChildBones;
	size = numBones * sizeof(short);

	boneNames = (unsigned short *)Alloc( size );
	model->memUsage += size;

	if ( numBones > DOBJ_MAX_PARTS - 1 )
	{
		FS_FreeFile(buf);
		Com_Printf("ERROR: xmodel '%s' has more than %d bones\n", name, DOBJ_MAX_PARTS - 1);

		return NULL;
	}

	size = numChildBones + sizeof(XBoneHierarchy) - 1;
	boneHiearchy = (XBoneHierarchy *)Alloc( size );
	model->memUsage += size;

	boneHiearchy->names = boneNames;
	parentList = boneHiearchy->parentList;

	size = sizeof(DObjAnimMat) * numBones + sizeof(XModelParts_s) - sizeof(DObjAnimMat);
	modelParts = (XModelParts_s *)Alloc( size );
	model->memUsage += size;

	modelParts->hierarchy = boneHiearchy;

	if ( numChildBones )
	{
		size = numChildBones * 8;
		modelParts->quats = (short *)Alloc( size );
		model->memUsage += size;

		size = numChildBones * 16;
		modelParts->trans = (float *)Alloc(size);
		model->memUsage += size;
	}
	else
	{
		modelParts->quats = 0;
		modelParts->trans = 0;
	}

	size = numBones;
	modelParts->partClassification = (byte *)Alloc( size );
	model->memUsage += numBones;

	modelParts->numBones = numBones;
	assert(modelParts->numBones == numBones);

	modelParts->numRootBones = numRootBones;
	assert(modelParts->numRootBones == numRootBones);

	quats = modelParts->quats;
	trans = modelParts->trans;

	for ( i = numRootBones; i < numBones; i++, quats += 4, trans += 3, parentList++ )
	{
		int index = ConsumeByte(&pos);
		assert(index < i);

		*parentList = i - index;
		assert(i - index == *parentList);

		trans[0] = ConsumeFloat(&pos);
		trans[1] = ConsumeFloat(&pos);
		trans[2] = ConsumeFloat(&pos);

		ConsumeQuat(&pos, quats);
	}

	for ( i = 0; i < numBones; i++ )
	{
		int len = I_strlen((char *)pos) + 1;
		boneNames[i] = SL_GetStringOfLen((const char *)pos, 0, len);
		pos += len;
	}

	memcpy(modelParts->partClassification, pos, numBones);
	pos += numBones;

	FS_FreeFile(buf);

	XModelCalcBasePose(modelParts);

	return modelParts;
}

/*
================
XModelLoadCollData
================
*/
void XModelLoadCollData( const unsigned char **pos, XModel *model, Alloc_t AllocColl, const char *name )
{
	int i, j;
	XModelCollTri_s *tri;
	XModelCollSurf_s *surf;

	assert(!model->contents);
	model->numCollSurfs = ConsumeInt(pos);

	if ( !model->numCollSurfs )
	{
		return;
	}

	model->collSurfs = (XModelCollSurf_s *)AllocColl(sizeof(XModelCollSurf_s) * model->numCollSurfs);

	for ( i = 0; i < model->numCollSurfs; i++ )
	{
		surf = &model->collSurfs[i];

		int numCollTris = surf->numCollTris = ConsumeInt(pos);
		assert(numCollTris);

		surf->numCollTris = numCollTris;
		surf->collTris = (XModelCollTri_s *)AllocColl(sizeof(XModelCollTri_s) * numCollTris);

		for ( j = 0; j < numCollTris; j++ )
		{
			tri = &surf->collTris[j];

			tri->plane[0] = ConsumeFloat(pos);
			tri->plane[1] = ConsumeFloat(pos);
			tri->plane[2] = ConsumeFloat(pos);
			tri->plane[3] = ConsumeFloat(pos);

			assert(I_fabs( Vec3Length( tri->plane ) - 1.0f ) < 0.01f);

			tri->svec[0] = ConsumeFloat(pos);
			tri->svec[1] = ConsumeFloat(pos);
			tri->svec[2] = ConsumeFloat(pos);
			tri->svec[3] = ConsumeFloat(pos);

			tri->tvec[0] = ConsumeFloat(pos);
			tri->tvec[1] = ConsumeFloat(pos);
			tri->tvec[2] = ConsumeFloat(pos);
			tri->tvec[3] = ConsumeFloat(pos);
		}

		surf->mins[0] = ConsumeFloat(pos) - 0.001;
		surf->mins[1] = ConsumeFloat(pos) - 0.001;
		surf->mins[2] = ConsumeFloat(pos) - 0.001;

		surf->maxs[0] = ConsumeFloat(pos) + 0.001;
		surf->maxs[1] = ConsumeFloat(pos) + 0.001;
		surf->maxs[2] = ConsumeFloat(pos) + 0.001;

		surf->boneIdx = ConsumeInt(pos);

		surf->contents = ConsumeInt(pos) & ~( CONTENTS_TRANSLUCENT | CONTENTS_NONCOLLIDING );
		assert(!surf->contents || (surf->boneIdx >= 0));

		surf->surfFlags = ConsumeInt(pos);
		model->contents |= surf->contents;
	}
}

/*
================
XModelLoadConfigFile
================
*/
bool XModelLoadConfigFile( const char *name, const unsigned char **pos, XModelConfig *config )
{
	int i;

	short version = ConsumeShort(pos);

	if ( version != XMODEL_VERSION )
	{
		Com_Printf("ERROR: xmodel '%s' out of date (version %d, expecting %d).\n", name, version, XMODEL_VERSION);
		return false;
	}

	config->flags = ConsumeByte(pos);

	config->mins[0] = ConsumeFloat(pos);
	config->mins[1] = ConsumeFloat(pos);
	config->mins[2] = ConsumeFloat(pos);

	config->maxs[0] = ConsumeFloat(pos);
	config->maxs[1] = ConsumeFloat(pos);
	config->maxs[2] = ConsumeFloat(pos);

	for ( i = 0; i < MAX_LODS; i++ )
	{
		config->entries[i].dist = ConsumeFloat(pos);

		strcpy(config->entries[i].filename, (const char *)*pos);
		*pos += strlen((const char *)*pos) + 1;
	}

	config->collLod = ConsumeInt(pos);

	return true;
}

/*
================
XModelLoadFile
================
*/
XModel *XModelLoadFile( const char *name, Alloc_t Alloc, Alloc_t AllocColl )
{
	int i, j;
	XBoneInfo *boneInfo;
	byte *buf;
	char filename[MAX_QPATH];
	char *dest;
	const unsigned char *pos;
	int lodStringLens[MAX_LODS];
	XModel *model;
	XModelConfig config;

	if ( Com_sprintf(filename, sizeof(filename), "xmodel/%s", name) <= 0 )
	{
		Com_Printf("ERROR: filename '%s' too long\n", filename);
		return NULL;
	}

	int fileSize = FS_ReadFile(filename, (void **)&buf);

	if ( fileSize < 0 )
	{
		assert(!buf);
		Com_Printf("ERROR: xmodel '%s' not found\n", name);

		return NULL;
	}

	if ( fileSize == 0 )
	{
		Com_Printf("ERROR: xmodel '%s' has 0 length\n", name);
		FS_FreeFile(buf);

		return NULL;
	}

	pos = buf;

	if ( !XModelLoadConfigFile(name, &pos, &config) )
	{
		Com_Printf("ERROR: xmodel '%s' has no config\n", name);
		FS_FreeFile(buf);

		return NULL;
	}

	int totalLen = 0;

	for ( i = 0; i < MAX_LODS; i++ )
	{
		lodStringLens[i] = I_strlen(config.entries[i].filename) + 1;
		totalLen += lodStringLens[i];
	}

	model = (XModel *)Alloc(totalLen + sizeof(XModel));
	model->memUsage = totalLen + sizeof(XModel);

	XModelLoadCollData( &pos, model, AllocColl, name );

	dest = (char *)&model[1];
	model->numLods = 0;

	for ( i = 0; i < MAX_LODS; i++ )
	{
		strcpy(dest, config.entries[i].filename);
		model->lodInfo[i].filename = dest;

		if ( *dest )
		{
			model->numLods++;
			model->lodInfo[i].numsurfs = ConsumeShort(&pos);

			int allocSize = sizeof(short) * model->lodInfo[i].numsurfs;

			model->lodInfo[i].surfNames = (unsigned short *)Alloc(allocSize);
			model->memUsage += allocSize;

			for ( j = 0; j < model->lodInfo[i].numsurfs; j++ )
			{
				const char *s = (const char *)pos;
				pos += strlen((const char *)pos) + 1;
				model->lodInfo[i].surfNames[j] = SL_GetString_(s, 0);
			}
		}
		else
		{
			model->lodInfo[i].surfNames = 0;
		}

		model->lodInfo[i].dist = config.entries[i].dist;
		dest += lodStringLens[i];
	}

	assert(model->numLods);
	model->parts = XModelPartsPrecache( model, model->lodInfo[0].filename, Alloc );

	if ( !model->parts )
	{
		FS_FreeFile(buf);
		XModelFree(model);

		return NULL;
	}

	int numBones = model->parts->numBones;

	boneInfo = (XBoneInfo *)Alloc(sizeof(XBoneInfo) * numBones);
	model->memUsage += sizeof(XBoneInfo) * numBones;

	for ( i = 0; i < numBones; i++ )
	{
		boneInfo[i].bounds[0][0] = ConsumeFloat(&pos);
		boneInfo[i].bounds[0][1] = ConsumeFloat(&pos);
		boneInfo[i].bounds[0][2] = ConsumeFloat(&pos);

		boneInfo[i].bounds[1][0] = ConsumeFloat(&pos);
		boneInfo[i].bounds[1][1] = ConsumeFloat(&pos);
		boneInfo[i].bounds[1][2] = ConsumeFloat(&pos);

		boneInfo[i].offset[0] = (boneInfo[i].bounds[0][0] + boneInfo[i].bounds[1][0]) * 0.5;
		boneInfo[i].offset[1] = (boneInfo[i].bounds[0][1] + boneInfo[i].bounds[1][1]) * 0.5;
		boneInfo[i].offset[2] = (boneInfo[i].bounds[0][2] + boneInfo[i].bounds[1][2]) * 0.5;

		vec3_t r;
		VectorSubtract(boneInfo[i].bounds[1], boneInfo[i].offset, r);
		boneInfo[i].radiusSquared = VectorLengthSquared(r);
	}

	model->boneInfo = boneInfo;

	FS_FreeFile(buf);

	VectorCopy(config.mins, model->mins);
	VectorCopy(config.maxs, model->maxs);

	model->collLod = config.collLod;
	assert(model->collLod < model->numLods);

	model->flags = config.flags;

	return model;
}
