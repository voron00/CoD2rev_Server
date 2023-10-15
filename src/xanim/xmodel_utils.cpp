#include "../qcommon/qcommon.h"

unsigned short* XModelBoneNames(XModel *model)
{
	return model->parts->hierarchy->names;
}

int XModelGetBoneIndex(const XModel *model, unsigned int name)
{
	int i;

	for ( i = model->parts->numBones - 1; i >= 0 && name != model->parts->hierarchy->names[i]; --i )
		;

	return i;
}

int XModelGetContents(const XModel *model)
{
	return model->contents;
}

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