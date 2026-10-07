#include "../qcommon/qcommon.h"



int XModelGetNumLods(const XModel *model)
{
	return model->numLods;
}

int XModelGetFlags(const XModel *model)
{
	return model->flags;
}

const char* XModelGetName(const XModel *model)
{
	return model->name;
}

DObjAnimMat* XModelGetBasePose(const XModel *model)
{
	return &model->parts->skel.Mat;
}

int XModelNumBones(const XModel *model)
{
	return model->parts->numBones;
}
