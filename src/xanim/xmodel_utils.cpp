#include "../qcommon/qcommon.h"

unsigned short* XModelBoneNames(XModel *model)
{
	return *model->parts->hierarchy;
}

int QDECL XModelGetBoneIndex(const XModel *model, unsigned int name)
{
	int i;

	for ( i = model->parts->numBones - 1; i >= 0 && name != (*model->parts->hierarchy)[i]; --i );

	return i;
}

int XModelGetContents(const XModel *model)
{
	return model->contents;
}

DObjAnimMat* XModelGetBasePose(const XModel *model)
{
	return &model->parts->skel.Mat;
}

int QDECL XModelNumBones(const XModel *model)
{
	return model->parts->numBones;
}