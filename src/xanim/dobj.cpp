#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

unsigned int g_empty;

/*
==================
SIGN
==================
*/
float SIGN( float x )
{
	return I_fsel( x, 1.0, -1.0 );
}

/*
==================
NormalizeQuatTrans
==================
*/
void NormalizeQuatTrans( DObjAnimMat *mat )
{
	float l = Vec4LengthSq(mat->quat);

	if ( l == 0 )
	{
		mat->quat[3] = 1.0;
		mat->transWeight = 2.0;
	}
	else
	{
		mat->transWeight = 2.0 / l;
	}
}

/*
==================
ConvertQuatToMat
==================
*/
void ConvertQuatToMat( const DObjAnimMat *mat, float axis[3][3] )
{
	float xx, xy, xw, xz, yy, yw, yz, zw, zz;
	float scaledQuat[3];

	assert(!IS_NAN((mat->quat)[0]) && !IS_NAN((mat->quat)[1]) && !IS_NAN((mat->quat)[2]) && !IS_NAN((mat->quat)[3]));
	assert(!IS_NAN(mat->transWeight));

	VectorScale(mat->quat, mat->transWeight, scaledQuat);

	xx = scaledQuat[0] * mat->quat[0];
	xy = scaledQuat[0] * mat->quat[1];
	xz = scaledQuat[0] * mat->quat[2];
	xw = scaledQuat[0] * mat->quat[3];

	yy = scaledQuat[1] * mat->quat[1];
	yz = scaledQuat[1] * mat->quat[2];
	yw = scaledQuat[1] * mat->quat[3];

	zz = scaledQuat[2] * mat->quat[2];
	zw = scaledQuat[2] * mat->quat[3];

	axis[0][0] = 1.0 - (yy + zz);

	axis[0][1] = xy + zw;
	axis[0][2] = xz - yw;
	axis[1][0] = xy - zw;

	axis[1][1] = 1.0 - (xx + zz);

	axis[1][2] = yz + xw;
	axis[2][0] = xz + yw;
	axis[2][1] = yz - xw;

	axis[2][2] = 1.0 - (xx + yy);
}

/*
==================
LocalMatrixTransformVectorQuatTransEquals
==================
*/
void LocalMatrixTransformVectorQuatTransEquals( vec3_t inout, DObjAnimMat *in )
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

/*
==================
LocalQuatMultiplyEquals
==================
*/
void LocalQuatMultiplyEquals( const vec4_t in, vec4_t inout )
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

/*
==================
LocalQuatMultiplyReverseEquals
==================
*/
void LocalQuatMultiplyReverseEquals( vec4_t in, vec4_t inout )
{
	float temp[3];

	temp[0] = in[0] * inout[3] + in[3] * inout[0] + in[2] * inout[1] - in[1] * inout[2];
	temp[1] = in[1] * inout[3] - in[2] * inout[0] + in[3] * inout[1] + in[0] * inout[2];
	temp[2] = in[2] * inout[3] + in[1] * inout[0] - in[0] * inout[1] + in[3] * inout[2];

	in[3] =   in[3] * inout[3] - in[0] * inout[0] - in[1] * inout[1] - in[2] * inout[2];

	in[0] =   temp[0];
	in[1] =   temp[1];
	in[2] =   temp[2];
}

/*
===============
LocalInvMatrixTransformVectorQuatTrans
===============
*/
void LocalInvMatrixTransformVectorQuatTrans( const vec3_t in, const DObjAnimMat *mat, vec3_t out )
{
	float axis[3][3];
	float temp[3];

	VectorSubtract(in, mat->trans, temp);
	ConvertQuatToMat(mat, axis);

	out[0] = temp[0] * axis[0][0] + temp[1] * axis[0][1] + temp[2] * axis[0][2];
	out[1] = temp[0] * axis[1][0] + temp[1] * axis[1][1] + temp[2] * axis[1][2];
	out[2] = temp[0] * axis[2][0] + temp[1] * axis[2][1] + temp[2] * axis[2][2];
}

/*
==================
MatrixTransformVectorQuatTrans
==================
*/
void MatrixTransformVectorQuatTrans( const vec3_t in, const DObjAnimMat *mat, vec3_t out )
{
	float axis[3][3];

	ConvertQuatToMat(mat, axis);

	out[0] = in[0] * axis[0][0] + in[1] * axis[1][0] + in[2] * axis[2][0] + mat->trans[0];
	out[1] = in[0] * axis[0][1] + in[1] * axis[1][1] + in[2] * axis[2][1] + mat->trans[1];
	out[2] = in[0] * axis[0][2] + in[1] * axis[1][2] + in[2] * axis[2][2] + mat->trans[2];
}

/*
==================
QuatMultiply
==================
*/
void QuatMultiply( const vec4_t in1, const vec4_t in2, vec4_t out )
{
	out[0] = in1[0] * in2[3] + in1[3] * in2[0] + in1[2] * in2[1] - in1[1] * in2[2];
	out[1] = in1[1] * in2[3] - in1[2] * in2[0] + in1[3] * in2[1] + in1[0] * in2[2];
	out[2] = in1[2] * in2[3] + in1[1] * in2[0] - in1[0] * in2[1] + in1[3] * in2[2];
	out[3] = in1[3] * in2[3] - in1[0] * in2[0] - in1[1] * in2[1] - in1[2] * in2[2];
}

/*
==================
DObjUnlock
==================
*/
void DObjUnlock( DObj *obj )
{
	assert(obj->locked);
	obj->locked = false;
}

/*
==================
DObjSetModel
==================
*/
void DObjSetModel( DObj *obj, const XModel *model )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
DObjHasContents
==================
*/
int DObjHasContents( DObj *obj, int contentmask )
{
	for ( int i = 0; i < obj->numModels; i++ )
	{
		if ( contentmask & XModelGetContents( obj->models[i] ) )
		{
			return 1;
		}
	}

	return 0;
}

/*
==================
DObjNumBones
==================
*/
int DObjNumBones( const DObj *obj )
{
	return obj->numBones;
}

/*
==================
DObjGetTree
==================
*/
XAnimTree_s* DObjGetTree( const DObj *obj )
{
	assert(obj);
	return obj->tree;
}

/*
==================
DObjGetSurface
==================
*/
XSurface_s* DObjGetSurface( const DObj* obj, int modelIndex, int subMatIndex, int lod )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==================
DObjGetNumSurfaces
==================
*/
int DObjGetNumSurfaces( const DObj *obj, char *lods )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==================
DObjSetControlRotTransIndex
==================
*/
int DObjSetControlRotTransIndex( const DObj *obj, const int *partBits, int boneIndex )
{
	assert(obj);
	assert(&obj->skel->Mat);
	assert(boneIndex >= 0);
	assert(boneIndex < obj->numBones);

	int boneIndexHigh = boneIndex >> 5;
	int boneIndexLow = 1 << (boneIndex & 0x1F);

	if ( !( partBits[boneIndex >> 5] & boneIndexLow ) )
	{
		return 0;
	}

	DSkel_t *skel = obj->skel;

	if ( skel->partBits.skel[boneIndexHigh] & boneIndexLow )
	{
		return 0;
	}

	assert(!(skel->partBits.skel[boneIndexHigh] & boneIndexLow));
	skel->partBits.control[boneIndexHigh] |= boneIndexLow;
	skel->partBits.anim[boneIndexHigh] |= boneIndexLow;

	return 1;
}

/*
==================
DObjSetRotTransIndex
==================
*/
int DObjSetRotTransIndex( const DObj *obj, const int *partBits, int boneIndex )
{
	assert(obj);
	assert(&obj->skel->Mat);
	assert(boneIndex >= 0);
	assert(boneIndex < obj->numBones);

	int boneIndexHigh = boneIndex >> 5;
	int boneIndexLow = 1 << (boneIndex & 0x1F);

	if ( !( partBits[boneIndex >> 5] & boneIndexLow ) )
	{
		return 0;
	}

	DSkel_t *skel = obj->skel;

	if ( skel->partBits.skel[boneIndexHigh] & boneIndexLow )
	{
		return 0;
	}

	assert(!(skel->partBits.skel[boneIndexHigh] & boneIndexLow));
	skel->partBits.anim[boneIndexHigh] |= boneIndexLow;

	return 1;
}

/*
==================
DObjGetBoneInfo
==================
*/
void DObjGetBoneInfo( DObj *obj, XBoneInfo **boneInfo )
{
	XModel *model;
	XModel **models;
	int size, i, j;

	models = obj->models;

	for ( j = 0; j < obj->numModels; j++ )
	{
		model = models[j];
		size = model->parts->numBones;

		for ( i = 0; i < size; i++ )
		{
			*boneInfo++ = &model->boneInfo[i];
		}
	}
}

/*
==================
DObjGetMatOffset
==================
*/
int DObjGetMatOffset( const DObj *obj, int modelIndex )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==================
DObjGetRotTransArray
==================
*/
DObjAnimMat* DObjGetRotTransArray( const DObj *obj )
{
	assert(obj);

	if ( !obj->skel )
	{
		return NULL;
	}

	return &obj->skel->Mat;
}

/*
==================
DObjGetModel
==================
*/
XModel* DObjGetModel( DObj *obj, int modelIndex )
{
	assert(obj);
	return obj->models[modelIndex];
}

/*
==================
DObjGetNumModels
==================
*/
int DObjGetNumModels( const DObj *obj )
{
	assert(obj);
	return obj->numModels;
}

/*
==================
DObjCreateSkel
==================
*/
void DObjCreateSkel( DObj *obj, DSkel_t *buf, int timeStamp )
{
	obj->skel = buf;
	obj->timeStamp = timeStamp;

	for ( int i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		obj->skel->partBits.anim[i] = 0;
		obj->skel->partBits.control[i] = 0;
		obj->skel->partBits.skel[i] = 0;
	}
}

/*
==================
DObjSkelClear
==================
*/
void DObjSkelClear( DObj *obj )
{
	obj->timeStamp = 0;
	obj->skel = NULL;
}

/*
==================
DObjSkelExistsConst
==================
*/
int DObjSkelExistsConst( const DObj_s *obj, int timeStamp )
{
	if ( obj->timeStamp == timeStamp )
	{
		return obj->skel != NULL;
	}

	return 0;
}

/*
==================
DObjSkelExists
==================
*/
int DObjSkelExists( DObj *obj, int timeStamp )
{
	if ( obj->timeStamp == timeStamp )
	{
		return obj->skel != NULL;
	}

	obj->skel = NULL;
	return 0;
}

/*
==================
DObjGetAllocSkelSize
==================
*/
int DObjGetAllocSkelSize( const DObj *obj )
{
	return int( sizeof( DObjAnimMat ) * obj->numBones + sizeof( DSkelPartBits_s ) );
}

/*
==================
DObjSetTree
==================
*/
void DObjSetTree( DObj *obj, XAnimTree *tree )
{
	byte next;
	byte *index;
	int size;

	obj->tree = tree;

	if ( !tree )
	{
		obj->animToModel = NULL;
		return;
	}

	size = tree->anims->size;
	obj->animToModel = tree->infoArray + size;

	index = (byte *)(obj->animToModel + size);
	next = *index + 1;

	if ( *index == NO_BONEINDEX )
	{
		next = 1;
		memset(index + 1, 0, size);
	}

	*index = next;
}

/*
==================
DObjSkelAreBonesUpToDate
==================
*/
int DObjSkelAreBonesUpToDate( const DObj *obj, int *partBits )
{
	assert(obj);
	assert(obj->skel);

	for ( int i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		if ( partBits[i] & ~obj->skel->partBits.skel[i] )
		{
			return 0;
		}
	}

	return 1;
}

/*
==================
DObjSkelIsBoneUpToDate
==================
*/
int DObjSkelIsBoneUpToDate( DObj *obj, int boneIndex )
{
	assert(obj);
	assert(obj->skel);

	return testBit( obj->skel->partBits.skel, boneIndex );
}

/*
==================
DObjIgnoreCollision
==================
*/
bool DObjIgnoreCollision( const DObj *obj, int modelIndex )
{
	assert(obj);
	return obj->ignoreCollision & ( 1 << modelIndex );
}

/*
==================
DObjAbort
==================
*/
void DObjAbort()
{
	g_empty = 0;
}

/*
==================
DObjGetLodOutDist
==================
*/
float DObjGetLodOutDist( const DObj *obj )
{
	float dist, maxDist;
	int i;

	maxDist = 0;

	for ( i = 0; i < obj->numModels; i++ )
	{
		dist = XModelGetLodOutDist(obj->models[i]);

		if ( dist > maxDist )
		{
			maxDist = dist;
		}
	}

	return maxDist;
}

/*
==================
DObjGetLodForDist
==================
*/
int DObjGetLodForDist( const DObj *obj, int modelIndex, float dist )
{
	 return XModelGetLodForDist( obj->models[modelIndex], dist );
}

/*
==================
DObjGetBoneIndex
==================
*/
int DObjGetBoneIndex( const DObj *obj, unsigned int boneName )
{
	int index, boneIndex, i, numModels;
	XModel *model;

	assert(obj);
	assert(boneName);

	numModels = obj->numModels;
	boneIndex = 0;

	for ( i = 0; i < numModels; i++ )
	{
		model = obj->models[i];
		index = XModelGetBoneIndex(model, boneName);

		if ( index >= 0 )
		{
			return boneIndex + index;
		}

		boneIndex += model->parts->numBones;
	}

	return -1;
}

/*
==================
DObjGetBounds
==================
*/
void DObjGetBounds( const DObj_s *obj, vec3_t mins, vec3_t maxs )
{
	assert(obj);

	VectorCopy(obj->mins, mins);
	VectorCopy(obj->maxs, maxs);
}

/*
==================
DObjGetBoneName
==================
*/
const char* DObjGetBoneName( const DObj *obj, int boneIndex )
{
	int index = 0, i, numBones;
	XModel *model;

	assert(obj);

	for ( i = 0; i < obj->numModels; i++ )
	{
		model = obj->models[i];
		numBones = model->parts->numBones;
		assert(index >= 0);

		if ( boneIndex - index < numBones )
		{
			return SL_ConvertToString( model->parts->hierarchy->names[ boneIndex - index ] );
		}

		index += numBones;
	}

	return 0;
}

/*
==================
DObjGetSurfaceName
==================
*/
const char* DObjGetSurfaceName( DObj *obj, int modelIndex, int subMatIndex, int lod )
{
	unsigned short name = obj->models[modelIndex]->lodInfo[lod].surfNames[subMatIndex];

	if ( !name )
	{
		return "DEFAULT";
	}

	return SL_ConvertToString(name);
}

/*
==================
DObjGetCreateParms
==================
*/
void DObjGetCreateParms( const DObj *obj, DObjModel_s *dobjModels, unsigned short *numModels, XAnimTree_s **tree, unsigned short *entnum )
{
	unsigned short *boneNames;
	int boneIndex, parentModelIndex, modelIndex;

	assert(obj);
	assert(obj->numModels > 0 && obj->numModels <= DOBJ_MAX_SUBMODELS);
	assert(dobjModels);
	assert(numModels);
	//assert(tree);

	*numModels = obj->numModels;
	*tree = obj->tree;
	*entnum = obj->tree ? obj->tree->entnum : 0;

	for ( modelIndex = 0; modelIndex < obj->numModels; modelIndex++, dobjModels++ )
	{
		dobjModels->model = obj->models[modelIndex];
		dobjModels->boneName = 0;
		dobjModels->ignoreCollision = obj->ignoreCollision & ( 1 << modelIndex );

		if ( obj->modelParents[modelIndex] == NO_BONEINDEX )
		{
			continue;
		}

		for ( parentModelIndex = modelIndex - 1; parentModelIndex >= 0; parentModelIndex-- )
		{
			if ( obj->modelParents[modelIndex] >= obj->matOffset[parentModelIndex] )
			{
				boneIndex = obj->modelParents[modelIndex] - obj->matOffset[parentModelIndex];
				boneNames = XModelBoneNames(obj->models[parentModelIndex]);

				dobjModels->boneName = SL_ConvertToString(boneNames[boneIndex]);
				break;
			}
		}
	}
}

/*
==================
DObjClone
==================
*/
void DObjClone( DObj *from, XAnimTree *tree, DObj *to )
{
	*to = *from;
	to->skel = NULL;

	if ( to->duplicateParts && to->duplicateParts != g_empty )
	{
		SL_AddRefToString(to->duplicateParts);
	}

	DObjSetTree(to, tree);
}

/*
==================
DObjClearAngles
==================
*/
void DObjClearAngles( DObjAnimMat *rotTrans )
{
	rotTrans->quat[0] = 0;
	rotTrans->quat[1] = 0;
	rotTrans->quat[2] = 0;
	rotTrans->quat[3] = 1.0;
}

/*
==================
DObjSetLocalTagInternal
==================
*/
void DObjSetLocalTagInternal( const DObj_s *obj, const vec3_t trans, const vec3_t angles, int boneIndex )
{
	vec2_t pitchQuat, yawQuat, rollQuat, tempQuat, tempQuat2;
	DObjAnimMat *rotTrans;

	rotTrans = DObjGetRotTransArray(obj);

	if ( !rotTrans )
	{
		return;
	}

	rotTrans += boneIndex;

	if ( angles )
	{
		FastSinCos(angles[YAW] * HALF_DEG_TO_RAD, &yawQuat[0], &yawQuat[1]);
		FastSinCos(angles[PITCH] * HALF_DEG_TO_RAD, &pitchQuat[0], &pitchQuat[1]);
		FastSinCos(angles[ROLL] * HALF_DEG_TO_RAD, &rollQuat[0], &rollQuat[1]);

		tempQuat[0] = -pitchQuat[0] * yawQuat[0];
		tempQuat[1] = pitchQuat[0] * yawQuat[1];

		tempQuat2[0] = pitchQuat[1] * yawQuat[0];
		tempQuat2[1] = pitchQuat[1] * yawQuat[1];

		rotTrans->quat[0] = rollQuat[0] * tempQuat2[1] + rollQuat[1] * tempQuat[0];
		rotTrans->quat[1] = rollQuat[1] * tempQuat[1] + rollQuat[0] * tempQuat2[0];
		rotTrans->quat[2] = -rollQuat[0] * tempQuat[1] + rollQuat[1] * tempQuat2[0];
		rotTrans->quat[3] = rollQuat[1] * tempQuat2[1] - rollQuat[0] * tempQuat[0];
	}
	else
	{
		DObjClearAngles(rotTrans);
	}

	rotTrans->transWeight = 0;
	VectorCopy(trans, rotTrans->trans);
}

/*
==================
DObjGeomTraceline
==================
*/
void DObjGeomTraceline( DObj *obj, const vec3_t localStart, const vec3_t localEnd, int contentmask, DObjTrace_s *results )
{
	int partIndex, i;
	unsigned short *partName;
	DObjAnimMat *boneMtxList;
	XModelParts_s *parts;
	XModel *model;
	trace_t trace;

	results->partName = 0;
	results->partGroup = 0;

	trace.fraction = results->fraction;
	assert(trace.fraction >= 0.0f && trace.fraction <= 1.0f);
	trace.surfaceFlags = 0;

	VectorClear(trace.normal);
	boneMtxList = DObjGetRotTransArray(obj);

	if ( boneMtxList )
	{
		for ( i = 0; i < obj->numModels; i++ )
		{
			model = obj->models[i];
			parts = model->parts;

			partName = model->parts->hierarchy->names;
			partIndex = XModelTraceLine(model, &trace, boneMtxList, localStart, localEnd, contentmask);

			if ( partIndex >= 0 )
			{
				results->partName = partName[partIndex];
			}

			boneMtxList += parts->numBones;
		}
	}

	results->fraction = trace.fraction;
	assert(results->fraction >= 0.0f && results->fraction <= 1.0f);
	results->surfaceflags = trace.surfaceFlags;

	VectorCopy(trace.normal, results->normal);
}

/*
==================
DObjTraceline
==================
*/
void DObjTraceline( DObj *obj, const vec3_t start, const vec3_t end, unsigned char *priorityMap, DObjTrace_s *trace )
{
	unsigned short classificationArray[DOBJ_MAX_PARTS];
	float deltaLengthSq, hitSign, sign, dist, dist1, dist2, solidHitFrac, fraction, enterFrac, diff2, sphereFraction, d2, invL2;
	unsigned short classification;
	XBoneHierarchy *hierarchy;
	unsigned char parentIndex;
	int ignoreCollision, i, j, globalBoneIndex, traceHitT, hitT, localBoneIndex, size;
	XBoneInfo *boneInfo;
	const unsigned char *pos;
	unsigned short *names;
	unsigned int currentPriority, lowestPriority;
	DSkel_t *skel;
	bool bEndSolid, bStartSolid;
	float *bounds;
	vec3_t localStart, localEnd, offset, enfOffset, startOffset, center, delta;
	float axis[3][3];
	DObjAnimMat *boneMatrix, *hitBoneMatrix;
	XModelParts_s *parts;
	XModel *model;

	trace->surfaceflags = 0;
	trace->partName = 0;
	trace->partGroup = 0;

	VectorClear(trace->normal);
	VectorSubtract(end, start, delta);

	deltaLengthSq = VectorLengthSquared(delta);

	if ( deltaLengthSq == 0 )
	{
		return;
	}

	boneMatrix = DObjGetRotTransArray(obj);

	if ( !boneMatrix )
	{
		return;
	}

	invL2 = 1.0 / deltaLengthSq;
	lowestPriority = 2;

	skel = obj->skel;
	assert(skel);

	assert(obj->duplicateParts);
	pos = (const unsigned char *)(SL_ConvertToString(obj->duplicateParts) + DOBJ_PARTBITS_STRING_LEN);

	assert(!IS_NAN((start)[0]) && !IS_NAN((start)[1]) && !IS_NAN((start)[2]));
	assert(!IS_NAN((end)[0]) && !IS_NAN((end)[1]) && !IS_NAN((end)[2]));

	globalBoneIndex = 0;
	hitT = -1;

	traceHitT = -1;
	hitSign = 0;

	hitBoneMatrix = 0;
	solidHitFrac = trace->fraction;

	for ( j = 0; j < obj->numModels; j++ )
	{
		model = obj->models[j];
		parts = model->parts;

		hierarchy = parts->hierarchy;
		names = hierarchy->names;

		size = parts->numBones;
		ignoreCollision = obj->ignoreCollision & (1 << j);

		for ( localBoneIndex = 0; localBoneIndex < size; localBoneIndex++, boneMatrix++, globalBoneIndex++ )
		{
			classification = parts->partClassification[localBoneIndex];
			currentPriority = priorityMap[classification];

			if ( globalBoneIndex == *pos - 1 )
			{
				pos += 2;

				if ( currentPriority == 1 )
				{
					classification = classificationArray[*(pos - 1)];
					currentPriority = priorityMap[classification];
				}
			}
			else if ( currentPriority == 1 )
			{
				if ( localBoneIndex < parts->numRootBones )
				{
					parentIndex = obj->modelParents[j];

					if ( parentIndex == NO_BONEINDEX )
						classificationArray[0] = 0;
					else
						classificationArray[0] = classificationArray[parentIndex + 1];

					classification = classificationArray[0];
				}
				else
				{
					classification = classificationArray[globalBoneIndex - hierarchy->parentList[localBoneIndex - parts->numRootBones] + 1];
				}

				currentPriority = priorityMap[classification];
			}

			classificationArray[globalBoneIndex + 1] = classification;

			if ( ignoreCollision )
			{
				continue;
			}

			boneInfo = &model->boneInfo[localBoneIndex];

			if ( boneInfo->radiusSquared == 0 )
			{
				continue;
			}

			assert(skel->partBits.skel[globalBoneIndex >> 5] & (HIGH_BIT >> (globalBoneIndex & 31)));

			if ( lowestPriority > currentPriority )
			{
				continue;
			}

			assert(!IS_NAN((boneMatrix->quat)[0]) && !IS_NAN((boneMatrix->quat)[1]) && !IS_NAN((boneMatrix->quat)[2]) && !IS_NAN((boneMatrix->quat)[3]));
			assert(!IS_NAN((boneMatrix->trans)[0]) && !IS_NAN((boneMatrix->trans)[1]) && !IS_NAN((boneMatrix->trans)[2]));

			MatrixTransformVectorQuatTrans(boneInfo->offset, boneMatrix, center);
			VectorSubtract(start, center, startOffset);

			sphereFraction = -DotProduct(startOffset, delta) * invL2;

			if ( sphereFraction < 1.0 )
			{
				if ( sphereFraction > 0 )
				{
					VectorMA(startOffset, sphereFraction, delta, offset);
					d2 = VectorLengthSquared(offset);
				}
				else
				{
					d2 = VectorLengthSquared(startOffset);
				}
			}
			else
			{
				VectorSubtract(end, center, enfOffset);
				d2 = VectorLengthSquared(enfOffset);
			}

			diff2 = boneInfo->radiusSquared - d2;

			if ( diff2 <= 0 )
			{
				continue;
			}

			if ( lowestPriority == currentPriority )
			{
				if ( sphereFraction - sqrtf(diff2 * invL2) >= trace->fraction )
				{
					continue;
				}
			}

			LocalInvMatrixTransformVectorQuatTrans(start, boneMatrix, localStart);
			LocalInvMatrixTransformVectorQuatTrans(end, boneMatrix, localEnd);

			assert(!IS_NAN((localStart)[0]) && !IS_NAN((localStart)[1]) && !IS_NAN((localStart)[2]));
			assert(!IS_NAN((localEnd)[0]) && !IS_NAN((localEnd)[1]) && !IS_NAN((localEnd)[2]));

			enterFrac = 0;

			if ( lowestPriority == currentPriority )
				fraction = trace->fraction;
			else
				fraction = solidHitFrac;

			bStartSolid = 1;
			bEndSolid = 1;
			sign = -1.0;

			for ( bounds = boneInfo->bounds[0]; ; bounds += 3 )
			{
				assert(!IS_NAN((bounds)[0]) && !IS_NAN((bounds)[1]) && !IS_NAN((bounds)[2]));

				for ( i = 0; i < 3; i++ )
				{
					dist1 = (localStart[i] - bounds[i]) * sign;
					dist2 = (localEnd[i] - bounds[i]) * sign;

					if ( dist1 > 0 )
					{
						if ( dist2 > 0 )
						{
							goto next;
						}

						bStartSolid = 0;
						dist = dist1 - dist2;
						assert(dist > 0);

						if ( dist1 > enterFrac * dist )
						{
							enterFrac = dist1 / dist;

							if ( enterFrac >= fraction )
							{
								goto next;
							}

							hitSign = sign;
							hitT = i;
						}
					}
					else if ( dist2 > 0 )
					{
						bEndSolid = 0;

						dist = dist1 - dist2;
						assert(dist < 0);

						if ( dist1 > fraction * dist )
						{
							fraction = dist1 / dist;

							if ( enterFrac >= fraction )
							{
								goto next;
							}
						}
					}
				}

				if ( sign == 1.0 )
				{
					break;
				}

				sign = 1.0;
			}

			if ( bStartSolid )
			{
				if ( bEndSolid && Dot2Product(delta, start) <= 0 )
				{
					trace->fraction = 0;

					trace->partName = names[localBoneIndex];
					trace->partGroup = classification;

					if ( delta[0] == 0 && delta[1] == 0 )
					{
						trace->normal[2] = -SIGN(delta[2]);
					}
					else
					{
						Vector2Copy(start, trace->normal);
						Vec2Normalize(trace->normal);
					}

					assert(Vec3IsNormalized(trace->normal));
					return;
				}
			}
			else
			{
				if ( lowestPriority == currentPriority )
				{
					if ( enterFrac >= trace->fraction )
					{
						continue;
					}
				}
				else
				{
					lowestPriority = currentPriority;
				}

				trace->fraction = enterFrac;
				assert(trace->fraction >= 0.0f && trace->fraction <= 1.0f);

				trace->partName = names[localBoneIndex];
				trace->partGroup = classification;

				assert(hitT >= 0);
				assert(hitT < 3);

				traceHitT = hitT;
				hitBoneMatrix = boneMatrix;
			}
next:
			;
		}
	}

	if ( hitBoneMatrix )
	{
		assert(traceHitT >= 0);
		assert(traceHitT < 3);

		ConvertQuatToMat(hitBoneMatrix, axis);
		VectorScale(axis[traceHitT], hitSign, trace->normal);
	}
}

/*
===============
DObjSetLocalBoneIndex
===============
*/
int DObjSetLocalBoneIndex( DObj *obj, int *partBits, int boneIndex, const vec3_t trans, const vec3_t angles )
{
	if ( !DObjSetRotTransIndex(obj, partBits, boneIndex) )
	{
		return 0;
	}

	DObjSetLocalTagInternal(obj, trans, angles, boneIndex);
	return 1;
}

/*
===============
DObjSetLocalTag
===============
*/
void DObjSetLocalTag( const DObj *obj, int *partBits, unsigned int tagName, const vec3_t trans, const vec3_t angles )
{
	int boneIndex = DObjGetBoneIndex(obj, tagName);

	if ( boneIndex < 0 )
	{
		return;
	}

	if ( DObjSetRotTransIndex(obj, partBits, boneIndex) )
	{
		DObjSetLocalTagInternal(obj, trans, angles, boneIndex);
	}
}

/*
===============
DObjSetControlTagAngles
===============
*/
void DObjSetControlTagAngles( const DObj_s *obj, int *partBits, unsigned int tagName, const vec3_t angles )
{
	int boneIndex = DObjGetBoneIndex(obj, tagName);

	if ( boneIndex < 0 )
	{
		return;
	}

	if ( DObjSetControlRotTransIndex(obj, partBits, boneIndex) )
	{
		DObjSetLocalTagInternal(obj, vec3_origin, angles, boneIndex);
	}
}

/*
===============
DObjInit
===============
*/
void DObjInit()
{
	int duplicatePartBits[DOBJ_MAX_PART_BITS + 1];

	memset( duplicatePartBits, 0, sizeof( duplicatePartBits ) );
	g_empty = SL_GetStringOfLen( (const char *)duplicatePartBits, 0, DOBJ_PARTBITS_STRING_LEN + 1 );
}

/*
================
DObjFree
================
*/
void DObjFree( DObj *obj )
{
	assert(obj);

	if ( obj->tree )
	{
		obj->animToModel = 0;

		assert(obj->tree->anims);
		obj->tree = 0;
	}

	assert(g_empty);

	if ( !obj->duplicateParts )
	{
		return;
	}

	if ( obj->duplicateParts != g_empty )
	{
		SL_RemoveRefToStringOfLen( obj->duplicateParts, I_strlen( ( SL_ConvertToString( obj->duplicateParts ) + DOBJ_PARTBITS_STRING_LEN ) ) + DOBJ_PARTBITS_STRING_LEN + 1 );
	}

	obj->duplicateParts = 0;
}

/*
================
DObjShutdown
================
*/
void DObjShutdown()
{
	if ( !g_empty )
	{
		return;
	}

	SL_RemoveRefToStringOfLen( g_empty, DOBJ_PARTBITS_STRING_LEN + 1 );
	g_empty = 0;
}

/*
===============
DObjGetSurfaces
===============
*/
int DObjGetSurfaces( const DObj *obj, DSurface_s *surfaces, int *partBits, char *lods )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
===============
DObjCreate
===============
*/
void DObjCreate( DObjModel_s *dobjModels, unsigned int numModels, XAnimTree *tree, DObj *obj, unsigned int entnum )
{
	int modelIndex, boneIndex, numBones, i, j;
	unsigned int partName;
	const char *boneName;
	XModel *model;

	assert(dobjModels);
	assert(numModels > 0);
	assert((unsigned)numModels <= DOBJ_MAX_SUBMODELS);
	assert(obj);

	obj->skel = NULL;
	obj->timeStamp = 0;
	obj->duplicateParts = 0;
	obj->ignoreCollision = 0;

	DObjSetTree(obj, tree);

	if ( tree )
	{
		tree->entnum = entnum;
	}

	modelIndex = 0;
	numBones = 0;

	for ( i = 0; i < numModels; i++, modelIndex++, dobjModels++ )
	{
		model = dobjModels->model;

		obj->models[modelIndex] = dobjModels->model;
		obj->modelParents[modelIndex] = -1;
		obj->matOffset[modelIndex] = numBones;

		if ( dobjModels->ignoreCollision )
		{
			obj->ignoreCollision |= 1 << i;
		}

		if ( !i )
		{
			goto out;
		}

		boneName = dobjModels->boneName;

		if ( !boneName )
		{
			goto out;
		}

		if ( !boneName[0] )
		{
			goto out;
		}

		partName = SL_FindString(boneName);

		if ( !partName )
		{
			Com_Printf("WARNING: Part '%s' not found in model '%s' or any of its descendants\n", boneName, obj->models[0]->name);
			goto out;
		}

		for ( j = 0; j < modelIndex; j++ )
		{
			boneIndex = XModelGetBoneIndex(obj->models[j], partName);

			if ( boneIndex >= 0 )
			{
				obj->modelParents[modelIndex] = obj->matOffset[j] + boneIndex;
				goto out;
			}
		}
out:
		if ( model )
		{
			if ( numBones + model->parts->numBones > DOBJ_MAX_PARTS - 1 )
			{
				Com_Error(ERR_DROP, "dobj for xmodel %s has more than %d bones", obj->models[0]->name, DOBJ_MAX_PARTS - 1);
			}

			numBones += model->parts->numBones;
		}
	}

	obj->numModels = modelIndex;
	obj->numBones = numBones;

	DObjComputeBounds(obj);
}

/*
===============
DObjDumpInfo
===============
*/
void DObjDumpInfo( const DObj *obj )
{
	XModel *model;
	const unsigned char *pos;
	int boneIndex, numModels, numBones, i;

	if ( !obj )
	{
		Com_Printf("No Dobj\n");
		return;
	}

	Com_Printf("\nModels:\n");

	numModels = obj->numModels;
	boneIndex = 0;

	for ( i = 0; i < numModels; i++ )
	{
		model = obj->models[i];
		Com_Printf("%d: '%s'\n", boneIndex, model->name);
		boneIndex += model->parts->numBones;
	}

	Com_Printf("\nBones:\n");

	numBones = obj->numBones;

	for ( i = 0; i < numBones; i++ )
	{
		Com_Printf("Bone %d: '%s'\n", i, DObjGetBoneName(obj, i));
	}

	if ( !obj->duplicateParts )
	{
		Com_Printf("\nNo part duplicates.\n");
		return;
	}

	Com_Printf("\nPart duplicates:\n");

	for ( pos = (const unsigned char *)( SL_ConvertToString( obj->duplicateParts ) + DOBJ_PARTBITS_STRING_LEN ); *pos; pos += 2 )
	{
		Com_Printf("%d ('%s') -> %d ('%s')\n", *pos - 1, DObjGetBoneName(obj, *pos - 1), pos[1] - 1, DObjGetBoneName(obj, pos[1] - 1));
	}

	Com_Printf("\n");
}

/*
===============
DObjCalcSkel
===============
*/
void DObjCalcSkel( DObj *obj, int *partBits )
{
	const int *savedDuplicatePartBits;
	bool bFinished;
	DSkel_t *skel;
	DObjAnimMat *parentMat, *childMat, *mat;
	XModelParts_s *parts;
	const unsigned char *duplicateParts;
	int numModels, boneIndex, boneIndexLow, boneIndexHigh, i, j;
	short *quats;
	int calcPartBits[DOBJ_MAX_PART_BITS], ignorePartBits[DOBJ_MAX_PART_BITS], controlPartBits[DOBJ_MAX_PART_BITS];
	float *trans;
	unsigned char parentIndex, *parentList;

	assert(obj);
	skel = obj->skel;
	assert(skel);

	bFinished = true;

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		ignorePartBits[i] = skel->partBits.skel[i] | ~partBits[i];

		if ( ignorePartBits[i] != -1 )
		{
			bFinished = false;
		}
	}

	if ( bFinished )
	{
		return;
	}

	if ( !obj->duplicateParts )
	{
		DObjCreateDuplicateParts(obj);
	}

	assert(obj->duplicateParts);
	savedDuplicatePartBits = (const int *)SL_ConvertToString( obj->duplicateParts );

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		skel->partBits.skel[i] |= partBits[i];

		controlPartBits[i] = skel->partBits.control[i] & ~ignorePartBits[i];
		calcPartBits[i] = savedDuplicatePartBits[i] | controlPartBits[i] | ignorePartBits[i];

	}

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		controlPartBits[i] |= ~calcPartBits[i];
	}

	numModels = obj->numModels;
	mat = &skel->Mat;

	parentMat = &skel->Mat;
	boneIndex = 0;

	duplicateParts = (const unsigned char *)( savedDuplicatePartBits + DOBJ_MAX_PART_BITS );

	for ( j = 0; j < numModels; j++ )
	{
		parts = obj->models[j]->parts;
		parentIndex = obj->modelParents[j];

		if ( parentIndex == NO_BONEINDEX )
		{
			for ( i = parts->numRootBones; i; i--, parentMat++, boneIndex++ )
			{
				boneIndexLow = 1 << (boneIndex & 0x1F);

				if ( controlPartBits[boneIndex >> 5] & boneIndexLow )
				{
					NormalizeQuatTrans(parentMat);
				}
				else if ( boneIndex == *duplicateParts - 1 )
				{
					duplicateParts += 2;

					if ( !( ignorePartBits[boneIndex >> 5] & boneIndexLow ) )
					{
						*parentMat = mat[*(duplicateParts - 1) - 1];
					}
				}
			}
		}
		else
		{
			for ( childMat = &mat[parentIndex], i = parts->numRootBones; i; i--, parentMat++, boneIndex++ )
			{
				boneIndexLow = 1 << (boneIndex & 0x1F);

				if ( controlPartBits[boneIndex >> 5] & boneIndexLow )
				{
					if ( boneIndexLow & calcPartBits[boneIndex >> 5] )
						LocalQuatMultiplyEquals(childMat->quat, parentMat->quat);
					else
						LocalQuatMultiplyReverseEquals(parentMat->quat, childMat->quat);

					NormalizeQuatTrans(parentMat);
					LocalMatrixTransformVectorQuatTransEquals(parentMat->trans, childMat);
				}
			}
		}

		for ( quats = parts->quats, trans = parts->trans, parentList = parts->hierarchy->parentList, i = parts->numBones - parts->numRootBones; i; i--, parentMat++, quats += 4, trans += 3, parentList++, boneIndex++ )
		{
			boneIndexHigh = boneIndex >> 5;
			boneIndexLow = 1 << (boneIndex & 0x1F);

			if ( controlPartBits[boneIndex >> 5] & boneIndexLow )
			{
				if ( boneIndexLow & calcPartBits[boneIndexHigh] )
					LocalQuatMultiplyEquals(parentMat[-*parentList].quat, parentMat->quat);
				else
					LocalQuatMultiplyReverseEquals(parentMat->quat, parentMat[-*parentList].quat);

				NormalizeQuatTrans(parentMat);
				VectorAdd(parentMat->trans, trans, parentMat->trans);
				LocalMatrixTransformVectorQuatTransEquals(parentMat->trans, &parentMat[-*parentList]);
			}
			else if ( boneIndex == *duplicateParts - 1 )
			{
				duplicateParts += 2;

				if ( !( ignorePartBits[boneIndexHigh] & boneIndexLow ) )
				{
					*parentMat = mat[*(duplicateParts - 1) - 1];
				}
			}
		}
	}
}

/*
===============
DObjCompleteHierarchyBits
===============
*/
void DObjCompleteHierarchyBits( const DObj *obj, int *partBits )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
===============
DObjGetHierarchyBits
===============
*/
void DObjGetHierarchyBits( DObj *obj, int boneIndex, int *partBits )
{
	const int *duplicatePartBits;
	int startIndex[DOBJ_MAX_SUBMODELS];
	const unsigned char *pos;
	byte *parentList;
	const unsigned char *duplicateParts;
	XModelParts_s *parts;
	int i, numModels, newBoneIndex, localBoneIndex;

	assert(obj);
	assert(boneIndex < (unsigned)obj->numBones);

	for ( i = 0; i < DOBJ_MAX_PART_BITS; i++ )
	{
		partBits[i] = 0;
	}

	numModels = obj->numModels;
	assert(numModels > 0);

	if ( !obj->duplicateParts )
	{
		DObjCreateDuplicateParts(obj);
	}

	assert(obj->duplicateParts);

	duplicatePartBits = (const int *)SL_ConvertToString( obj->duplicateParts );
	duplicateParts = (const unsigned char *)( duplicatePartBits + DOBJ_MAX_PART_BITS );
	pos = (const unsigned char *)( duplicatePartBits + DOBJ_MAX_PART_BITS );

	startIndex[0] = 0;

	for ( i = 0; ; startIndex[i] = newBoneIndex )
	{
		parts = obj->models[i]->parts;
		newBoneIndex = startIndex[i] + parts->numBones;

		if ( newBoneIndex > boneIndex )
		{
			for ( parentList = parts->hierarchy->parentList; ; boneIndex -= parentList[newBoneIndex] )
			{
				localBoneIndex = boneIndex - startIndex[i];

				while ( 1 )
				{
					assert(localBoneIndex >= 0);
					setBit(partBits, boneIndex);

					if ( testBit( duplicatePartBits, boneIndex ) )
					{
						for ( pos = duplicateParts; ; pos += 2 )
						{
							assert(*pos);

							if ( boneIndex == *pos - 1 )
							{
								break;
							}
						}

						boneIndex = pos[1] - 1;
					}
					else
					{
						newBoneIndex = localBoneIndex - parts->numRootBones;

						if ( newBoneIndex >= 0 )
						{
							break;
						}

						boneIndex = obj->modelParents[i];

						if ( boneIndex == NO_BONEINDEX )
						{
							return;
						}
					}

					do
					{
						i--;
						assert(i >= 0);
						localBoneIndex = boneIndex - startIndex[i];
					}
					while ( localBoneIndex < 0 );

					parts = obj->models[i]->parts;
					parentList = parts->hierarchy->parentList;
				}
			}
		}

		i++;

		if ( i == numModels )
		{
			break;
		}
	}
}

/*
===============
DObjComputeBounds
===============
*/
void DObjComputeBounds( DObj *obj )
{
	vec3_t dobjmins, dobjmaxs, modelmins, modelmaxs;
	int i;

	assert(obj);

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

/*
================
DObjCreateDuplicateParts
================
*/
void DObjCreateDuplicateParts( DObj *obj )
{
	int duplicatePartBits[DOBJ_MAX_PARTS];
	int index;
	bool bRootMeld;
	byte *duplicateParts;
	unsigned short *name;
	int boneIndex, currNumModels, localBoneIndex, boneIter, boneCount, len;
	XModel *model;

	assert(!obj->duplicateParts);
	assert(obj->numModels > 0);
	assert(obj->numBones <= DOBJ_MAX_PARTS);

	duplicateParts = (byte *)&duplicatePartBits[DOBJ_MAX_PART_BITS];
	memset(duplicatePartBits, 0, DOBJ_PARTBITS_STRING_LEN);

	len = 0;
	boneCount = obj->models[0]->parts->numBones;

	for ( currNumModels = 1; currNumModels < obj->numModels; currNumModels++, boneCount += model->parts->numBones )
	{
		model = obj->models[currNumModels];

		if ( obj->modelParents[currNumModels] != NO_BONEINDEX )
		{
			continue;
		}

		name = model->parts->hierarchy->names;
		boneIter = model->parts->numBones;

		assert(boneIter);
		assert(boneIter < DOBJ_MAX_PARTS);

		bRootMeld = false;
		boneIndex = -1;

		for ( localBoneIndex = 0; localBoneIndex < boneIter; localBoneIndex++ )
		{
			boneIndex = DObjGetBoneIndex(obj, name[localBoneIndex]);
			assert(boneIndex >= 0);

			if ( boneIndex != boneCount + localBoneIndex )
			{
				if ( !localBoneIndex )
				{
					bRootMeld = true;
				}

				assert(boneCount + localBoneIndex + 1 < 256);
				assert(boneIndex + 1 < 256);
				assert(boneIndex < boneCount + localBoneIndex);

				index = boneCount + localBoneIndex;

				duplicateParts[len] = boneCount + localBoneIndex + 1;
				duplicatePartBits[index >> 5] |= 1 << (index & 0x1F);

				assert(duplicateParts[len]);
				len++;

				duplicateParts[len] = boneIndex + 1;
				assert(duplicateParts[len]);

				len++;
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

	assert(boneCount < DOBJ_MAX_PARTS);
	assert(g_empty);

	if ( len )
	{
		duplicateParts[len] = 0;
		obj->duplicateParts = SL_GetStringOfLen((const char *)duplicatePartBits, 0, len + DOBJ_PARTBITS_STRING_LEN + 1);
	}
	else
	{
		obj->duplicateParts = g_empty;
	}
}

/*
================
DObjBad
================
*/
bool DObjBad( DObj *obj )
{
	for ( int i = obj->numModels - 1; i >= 0; i-- )
	{
		if ( XModelBad(obj->models[i]) )
		{
			return true;
		}
	}

	return false;
}
