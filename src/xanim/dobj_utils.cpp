#include "../qcommon/qcommon.h"

void MatrixTransformVectorQuatTrans(const float *in, const DObjAnimMat *mat, float *out)
{
	float axis[3][3];

	ConvertQuatToMat(mat, axis);

	out[0] = in[0] * axis[0][0] + in[1] * axis[1][0] + in[2] * axis[2][0] + mat->trans[0];
	out[1] = in[0] * axis[0][1] + in[1] * axis[1][1] + in[2] * axis[2][1] + mat->trans[1];
	out[2] = in[0] * axis[0][2] + in[1] * axis[1][2] + in[2] * axis[2][2] + mat->trans[2];
}

void QuatMultiply(const float *in1, const float *in2, float *out)
{
	out[0] = in1[0] * in2[3] + in1[3] * in2[0] + in1[2] * in2[1] - in1[1] * in2[2];
	out[1] = in1[1] * in2[3] - in1[2] * in2[0] + in1[3] * in2[1] + in1[0] * in2[2];
	out[2] = in1[2] * in2[3] + in1[1] * in2[0] - in1[0] * in2[1] + in1[3] * in2[2];
	out[3] = in1[3] * in2[3] - in1[0] * in2[0] - in1[1] * in2[1] - in1[2] * in2[2];
}

void QuatMultiplyEquals(const float *in, float *inout)
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

void QuatMultiplyReverseEquals(float *in, float *inout)
{
	float temp[3];

	temp[0] = in[0] * inout[3] + in[3] * inout[0] + in[2] * inout[1] - in[1] * inout[2];
	temp[1] = in[1] * inout[3] - in[2] * inout[0] + in[3] * inout[1] + in[0] * inout[2];
	temp[2] = in[2] * inout[3] + in[1] * inout[0] - in[0] * inout[1] + in[3] * inout[2];

	in[3] =   in[3] * inout[3] - in[0] * inout[0] - in[1] * inout[1] - in[2] * inout[2];
	in[0] = temp[0];
	in[1] = temp[1];
	in[2] = temp[2];
}

void MatrixTransformVectorQuatTransEquals(float *inout, DObjAnimMat *in)
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

void ConvertQuatToMat(const DObjAnimMat *mat, float axis[3][3])
{
	float yy;
	float xy;
	float zz;
	float zw;
	float scaledQuat[3];
	float yw;
	float xz;
	float yz;
	float xx;
	float xw;

	scaledQuat[0] = mat->transWeight * mat->quat[0];
	scaledQuat[1] = mat->transWeight * mat->quat[1];
	scaledQuat[2] = mat->transWeight * mat->quat[2];

	xx = scaledQuat[0] * mat->quat[0];
	xy = scaledQuat[0] * mat->quat[1];
	xz = scaledQuat[0] * mat->quat[2];
	xw = scaledQuat[0] * mat->quat[3];
	yy = scaledQuat[1] * mat->quat[1];
	yz = scaledQuat[1] * mat->quat[2];
	yw = scaledQuat[1] * mat->quat[3];
	zz = scaledQuat[2] * mat->quat[2];
	zw = scaledQuat[2] * mat->quat[3];

	axis[0][0] = 1.0 - (float)(yy + zz);
	axis[0][1] = xy + zw;
	axis[0][2] = xz - yw;
	axis[1][0] = xy - zw;
	axis[1][1] = 1.0 - (float)(xx + zz);
	axis[1][2] = yz + xw;
	axis[2][0] = xz + yw;
	axis[2][1] = yz - xw;
	axis[2][2] = 1.0 - (float)(xx + yy);
}

void InvMatrixTransformVectorQuatTrans(const float *in, const DObjAnimMat *mat, float *out)
{
	float axis[3][3];
	float temp[3];

	VectorSubtract(in, mat->trans, temp);
	ConvertQuatToMat(mat, axis);

	out[0] = temp[0] * axis[0][0] + temp[1] * axis[0][1] + temp[2] * axis[0][2];
	out[1] = temp[0] * axis[1][0] + temp[1] * axis[1][1] + temp[2] * axis[1][2];
	out[2] = temp[0] * axis[2][0] + temp[1] * axis[2][1] + temp[2] * axis[2][2];
}

void DObjCalcTransWeight(DObjAnimMat *Mat)
{
	float vLenSq;

	vLenSq = Vec4LengthSq(Mat->quat);

	if ( vLenSq == 0.0 )
	{
		Mat->quat[3] = 1.0;
		Mat->transWeight = 2.0;
	}
	else
	{
		Mat->transWeight = 2.0 / vLenSq;
	}
}

int DObjSkelIsBoneUpToDate(DObj_s *obj, int boneIndex)
{
	return (obj->skel->partBits.skel[boneIndex >> 5] >> (boneIndex & 0x1F)) & 1;
}

int DObjSkelAreBonesUpToDate(const DObj_s *obj, int *partBits)
{
	int i;

	for ( i = 0; i < 4; ++i )
	{
		if ( (partBits[i] & ~obj->skel->partBits.skel[i]) != 0 )
			return 0;
	}

	return 1;
}

int DObjGetBoneIndex(const DObj_s *obj, unsigned int name)
{
	int offset;
	XModel *model;
	int index;
	int i;
	int numModels;

	numModels = obj->numModels;
	index = 0;

	for ( i = 0; i < numModels; ++i )
	{
		model = obj->models[i];
		offset = XModelGetBoneIndex(model, name);

		if ( offset >= 0 )
			return index + offset;

		index += model->parts->numBones;
	}

	return -1;
}

DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj)
{
	if ( obj->skel )
		return &obj->skel->Mat;
	else
		return 0;
}

int DObjSkelExists(DObj_s *obj, int timeStamp)
{
	if ( obj->timeStamp == timeStamp )
		return obj->skel != 0;

	obj->skel = 0;
	return 0;
}

int DObjGetAllocSkelSize(const DObj_s *obj)
{
	return sizeof(DObjAnimMat) * obj->numBones + sizeof(DSkelPartBits_s);
}

int DObjNumBones(const DObj *obj)
{
	return obj->numBones;
}

void DObjCreateSkel(DObj_s *obj, DSkel_t *skel, int time)
{
	int i;

	obj->skel = skel;
	obj->timeStamp = time;

	for ( i = 0; i < 4; ++i )
	{
		skel->partBits.anim[i] = 0;
		skel->partBits.control[i] = 0;
		skel->partBits.skel[i] = 0;
	}
}

int DObjSetControlRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex)
{
	DSkel_t *skel;
	int boneIndexHigh;
	int boneIndexLow;

	boneIndexHigh = boneIndex >> 5;
	boneIndexLow = 1 << (boneIndex & 0x1F);

	if ( (partBits[boneIndex >> 5] & boneIndexLow) == 0 )
		return 0;

	skel = obj->skel;

	if ( (skel->partBits.skel[boneIndexHigh] & boneIndexLow) != 0 )
		return 0;

	skel->partBits.control[boneIndexHigh] |= boneIndexLow;
	skel->partBits.anim[boneIndexHigh] |= boneIndexLow;

	return 1;
}

int DObjSetRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex)
{
	DSkel_t *skel;
	int boneIndexHigh;
	int boneIndexLow;

	boneIndexHigh = boneIndex >> 5;
	boneIndexLow = 1 << (boneIndex & 0x1F);

	if ( (partBits[boneIndex >> 5] & boneIndexLow) == 0 )
		return 0;

	skel = obj->skel;

	if ( (skel->partBits.skel[boneIndexHigh] & boneIndexLow) != 0 )
		return 0;

	skel->partBits.anim[boneIndexHigh] |= boneIndexLow;

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
	v2 = angles[0] * 0.0087266462;
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

void DObjSetLocalTagInternal(const DObj_s *obj, const float *trans, const float *angles, int boneIndex)
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

void DObjSetControlTagAngles(const DObj_s *obj, int *partBits, unsigned int boneIndex, float *angles)
{
	int index;

	index = DObjGetBoneIndex(obj, boneIndex);

	if ( index >= 0 )
	{
		if ( DObjSetControlRotTransIndex(obj, partBits, index) )
			DObjSetLocalTagInternal(obj, vec3_origin, angles, index);
	}
}

void DObjSetLocalTag(const DObj_s *obj, int *partBits, unsigned int boneIndex, const float *trans, const float *angles)
{
	int index;

	index = DObjGetBoneIndex(obj, boneIndex);

	if ( index >= 0 )
	{
		if ( DObjSetRotTransIndex(obj, partBits, index) )
			DObjSetLocalTagInternal(obj, trans, angles, index);
	}
}