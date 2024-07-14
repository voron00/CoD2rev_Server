#include "gsc_level.hpp"

#if LIBCOD_COMPILE_LEVEL == 1

void gsc_level_getnumberofstaticmodels()
{
	stackPushInt(cm.numStaticModels);
}

void gsc_level_getstaticmodelname()
{
	int index;

	if ( ! stackGetParams("i", &index))
	{
		stackError("gsc_level_getstaticmodelname() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (index < 0 || index >= (int)cm.numStaticModels)
	{
		stackError("gsc_level_getstaticmodelname() index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushString(cm.staticModelList[index].xmodel->name);
}

void gsc_level_getstaticmodelorigin()
{
	int index;

	if ( ! stackGetParams("i", &index))
	{
		stackError("gsc_level_getstaticmodelorigin() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (index < 0 || index >= (int)cm.numStaticModels)
	{
		stackError("gsc_level_getstaticmodelorigin() index is out of range");
		stackPushUndefined();
		return;
	}

	stackPushVector(cm.staticModelList[index].origin);
}

#endif
