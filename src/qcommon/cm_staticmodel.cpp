#include "qcommon.h"
#include "cm_local.h"

void* CM_Hunk_AllocXModel(int size)
{
	return Hunk_Alloc(size);
}

void* CM_Hunk_AllocXModelColl(int size)
{
	return Hunk_Alloc(size);
}

XModel* CM_XModelPrecache(const char *name)
{
	return XModelPrecache(name, CM_Hunk_AllocXModel, CM_Hunk_AllocXModelColl);
}