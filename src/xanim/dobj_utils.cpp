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

int DObjGetBoneIndex(const DObj_s *obj, unsigned int name)
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