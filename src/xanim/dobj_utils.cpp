#include "../qcommon/qcommon.h"

int DObjSkelIsBoneUpToDate(DObj_s *obj, int boneIndex)
{
	return (obj->skel.skelPart->partBits.skel[boneIndex >> 5] >> (boneIndex & 0x1F)) & 1;
}

int DObjSkelAreBonesUpToDate(const DObj_s *obj, int *partBits)
{
	int i;

	for ( i = 0; i < 4; ++i )
	{
		if ( (partBits[i] & ~obj->skel.skelPart->partBits.skel[i]) != 0 )
			return 0;
	}

	return 1;
}

int QDECL DObjGetBoneIndex(const DObj_s *obj, unsigned int name)
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

		index += model->modelParts->numBones;
	}

	return -1;
}

DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj)
{
	if ( obj->skel.skelPart )
		return &obj->skel.skelPart->Mat;
	else
		return 0;
}

int DObjSkelExists(DObj_s *obj, int timeStamp)
{
	if ( obj->skel.timeStamp == timeStamp )
		return obj->skel.skelPart != 0;

	obj->skel.skelPart = 0;
	return 0;
}

int DObjGetAllocSkelSize(const DObj_s *obj)
{
	return sizeof(DObjAnimMat) * obj->numBones + sizeof(DSkelPartBits_s);
}

void DObjCreateSkel(DObj_s *obj, DSkelPart_s *skelPart, int time)
{
	int i;

	obj->skel.skelPart = skelPart;
	obj->skel.timeStamp = time;

	for ( i = 0; i < 4; ++i )
	{
		skelPart->partBits.anim[i] = 0;
		skelPart->partBits.control[i] = 0;
		skelPart->partBits.skel[i] = 0;
	}
}

int DObjSetRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex)
{
	DSkelPart_s *skelPart;
	int boneIndexHigh;
	int boneIndexLow;

	boneIndexLow = boneIndex >> 5;
	boneIndexHigh = 1 << (boneIndex & 0x1F);

	if ( (partBits[boneIndex >> 5] & boneIndexHigh) == 0 )
		return 0;

	skelPart = obj->skel.skelPart;

	if ( (skelPart->partBits.skel[boneIndexLow] & boneIndexHigh) != 0 )
		return 0;

	skelPart->partBits.control[boneIndexLow] |= boneIndexHigh;
	skelPart->partBits.anim[boneIndexLow] |= boneIndexHigh;

	return 1;
}

void DObjSetAngles(DObjAnimMat *rotTrans, const float *angles)
{
	float v1;
	float v2;
	float v3;
	float yawQuat;
	float yawQuat_4;
	float rollQuat;
	float rollQuat_4;
	float pitchQuat;
	float pitchQuat_4;
	float tempQuat;

	v3 = angles[1] * 0.0087266462;
	yawQuat_4 = cos(v3);
	yawQuat = sin(v3);
	v2 = *angles * 0.0087266462;
	pitchQuat_4 = cos(v2);
	pitchQuat = sin(v2);
	v1 = angles[2] * 0.0087266462;
	rollQuat_4 = cos(v1);
	rollQuat = sin(v1);
	tempQuat = -pitchQuat * yawQuat;
	rotTrans->quat[0] = (float)(rollQuat * (float)(pitchQuat_4 * yawQuat_4)) + (float)(rollQuat_4 * tempQuat);
	rotTrans->quat[1] = (float)(rollQuat_4 * (float)(pitchQuat * yawQuat_4))
	                    + (float)(rollQuat * (float)(pitchQuat_4 * yawQuat));
	rotTrans->quat[2] = (float)(-rollQuat * (float)(pitchQuat * yawQuat_4))
	                    + (float)(rollQuat_4 * (float)(pitchQuat_4 * yawQuat));
	rotTrans->quat[3] = (float)(rollQuat_4 * (float)(pitchQuat_4 * yawQuat_4)) - (float)(rollQuat * tempQuat);
}

void ConvertQuatToMat(const DObjAnimMat *mat, float (*axis)[3])
{
	float transWeight;
	float yy;
	float xy;
	float zz;
	float zw;
	float scaledQuat;
	float scaledQuat_4;
	float scaledQuat_8;
	float yw;
	float xz;
	float yz;
	float xx;
	float xw;

	transWeight = mat->transWeight;
	scaledQuat = transWeight * mat->quat[0];
	scaledQuat_4 = transWeight * mat->quat[1];
	scaledQuat_8 = transWeight * mat->quat[2];

	xx = scaledQuat * mat->quat[0];
	xy = scaledQuat * mat->quat[1];
	xz = scaledQuat * mat->quat[2];
	xw = scaledQuat * mat->quat[3];
	yy = scaledQuat_4 * mat->quat[1];
	yz = scaledQuat_4 * mat->quat[2];
	yw = scaledQuat_4 * mat->quat[3];
	zz = scaledQuat_8 * mat->quat[2];
	zw = scaledQuat_8 * mat->quat[3];

	(*axis)[0] = 1.0 - (float)(yy + zz);
	(*axis)[1] = xy + zw;
	(*axis)[2] = xz - yw;
	(*axis)[3] = xy - zw;
	(*axis)[4] = 1.0 - (float)(xx + zz);
	(*axis)[5] = yz + xw;
	(*axis)[6] = xz + yw;
	(*axis)[7] = yz - xw;
	(*axis)[8] = 1.0 - (float)(xx + yy);
}

void DObjClearAngles(DObjAnimMat *rotTrans)
{
	rotTrans->quat[0] = 0.0;
	rotTrans->quat[1] = 0.0;
	rotTrans->quat[2] = 0.0;
	rotTrans->quat[3] = 1.0;
}

void DObjSetTrans(DObjAnimMat *rotTrans, const float *trans)
{
	rotTrans->transWeight = 0.0;
	rotTrans->trans[0] = trans[0];
	rotTrans->trans[1] = trans[1];
	rotTrans->trans[2] = trans[2];
}

void DObjSetLocalTagInternal(const DObj *obj, const float *trans, const float *angles, int boneIndex)
{
	DObjAnimMat *rotTrans;
	DObjAnimMat *childRotTrans;

	rotTrans = DObjGetRotTransArray(obj);

	if ( rotTrans )
	{
		childRotTrans = &rotTrans[boneIndex];

		if ( angles )
			DObjSetAngles(childRotTrans, angles);
		else
			DObjClearAngles(childRotTrans);

		DObjSetTrans(childRotTrans, trans);
	}
}

int DObjSetControlTagAngles(const DObj_s *obj, int *partBits, unsigned int boneIndex, const float *angles)
{
	int index;

	index = DObjGetBoneIndex(obj, boneIndex);

	if ( index < 0 )
		return 0;

	if ( !DObjSetRotTransIndex(obj, partBits, index) )
		return 0;

	DObjSetLocalTagInternal(obj, vec3_origin, angles, index);
	return 1;
}