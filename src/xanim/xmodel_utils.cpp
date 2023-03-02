#include "../qcommon/qcommon.h"

unsigned short* XModelBoneNames(XModel *model)
{
	return *model->modelParts->boneNames;
}

int QDECL XModelGetBoneIndex(const XModel *model, unsigned int name)
{
	int i;

	for ( i = model->modelParts->numBones - 1; i >= 0 && name != (*model->modelParts->boneNames)[i]; --i );

	return i;
}

int XModelGetContents(const XModel *model)
{
	return model->contents;
}