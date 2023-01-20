#include "../qcommon/qcommon.h"

qboolean ConsoleCommand()
{
	return qfalse;
}

qboolean SV_GameCommand()
{
	if ( sv.state == SS_GAME )
	{
		return ConsoleCommand();
	}
	return 0;
}

void SV_ShutdownGameProgs()
{
	UNIMPLEMENTED(__FUNCTION__);
}