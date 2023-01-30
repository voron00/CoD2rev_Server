#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

qboolean ConsoleCommand()
{
	UNIMPLEMENTED(__FUNCTION__);
	return qfalse;
}

qboolean SV_GameCommand()
{
	if ( sv.state == SS_GAME )
	{
		return ConsoleCommand();
	}

	return qfalse;
}

void SV_GameSendServerCommand(int clientnum, int svscmd_type, const char *text)
{
	if ( clientnum == -1 )
	{
		SV_SendServerCommand(0, svscmd_type, "%s", text);
	}
	else if ( clientnum >= 0 && clientnum < sv_maxclients->current.integer )
	{
		SV_SendServerCommand(&svs.clients[clientnum], svscmd_type, "%s", text);
	}
}

void SV_ShutdownGameProgs()
{
	UNIMPLEMENTED(__FUNCTION__);
}