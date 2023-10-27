#include "../qcommon/qcommon.h"

static bool shouldQuitOnError = false;

static void RefreshQuitOnErrorCondition()
{
	if ( !Dvar_IsSystemActive() )
		return;

	shouldQuitOnError = Dvar_GetBool("QuitOnError") || Dvar_GetInt("r_vc_compile") == 2;
}

bool QuitOnError()
{
	RefreshQuitOnErrorCondition();
	return shouldQuitOnError;
}
