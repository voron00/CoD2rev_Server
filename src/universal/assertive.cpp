#include "../qcommon/qcommon.h"

bool shouldQuitOnError = false;

void RefreshQuitOnErrorCondition()
{
	bool quit;

	if ( Dvar_IsSystemActive() )
	{
		quit = false;

		if ( Dvar_GetBool("QuitOnError") || Dvar_GetInt("r_vc_compile") == 2 )
			quit = true;

		shouldQuitOnError = quit;
	}
}

bool QuitOnError()
{
	RefreshQuitOnErrorCondition();
	return shouldQuitOnError;
}
