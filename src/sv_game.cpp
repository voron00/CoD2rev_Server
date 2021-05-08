#include "i_common.h"
#include "i_shared.h"
#include "server.h"

serverStatic_t svs;
server_t sv;

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