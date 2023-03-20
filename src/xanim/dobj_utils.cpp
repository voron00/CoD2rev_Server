#include "../qcommon/qcommon.h"

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

DObjAnimMat* QDECL DObjGetRotTransArray(const DObj_s *obj)
{
	if ( obj->skel.partBits )
		return &obj->skel.partBits->Mat;
	else
		return 0;
}