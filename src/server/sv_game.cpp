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

qboolean SV_IsLocalClient(int clientNum)
{
	return NET_IsLocalAddress(svs.clients[clientNum].netchan.remoteAddress);
}

qboolean SV_MapExists(const char *name)
{
	return FS_ReadFile(va("maps/mp/%s.%s", SV_GetMapBaseName(name), GetBspExtension()), 0) >= 0;
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

void *SV_AllocXModelPrecache(int size)
{
	return Hunk_Alloc(size);
}

void *SV_AllocXModelPrecacheColl(int size)
{
	return Hunk_AllocInternal(size);
}

XModel* SV_XModelGet(const char *name)
{
	if ( !Com_ValidXModelName(name) )
		Com_Error(ERR_DROP, "bad model name %s", name);

	return XModelPrecache(name + 7, SV_AllocXModelPrecache, SV_AllocXModelPrecacheColl);
}

// these functions must be used instead of pointer arithmetic, because
// the game allocates gentities with private information after the server shared part
int SV_NumForGentity( gentity_t *ent )
{
	int num;

	num = ( (byte *)ent - (byte *)sv.gentities ) / sv.gentitySize;

	return num;
}

gentity_t *SV_GentityNum( int num )
{
	gentity_t *ent;

	ent = ( gentity_t * )( (byte *)sv.gentities + sv.gentitySize * ( num ) );

	return ent;
}

playerState_t *SV_GameClientNum( int num )
{
	playerState_t   *ps;

	ps = ( playerState_t * )( (byte *)sv.gameClients + sv.gameClientSize * ( num ) );

	return ps;
}

svEntity_t  *SV_SvEntityForGentity( gentity_t *gEnt )
{
	if ( !gEnt || gEnt->s.number < 0 || gEnt->s.number >= MAX_GENTITIES )
	{
		Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
	}

	return &sv.svEntities[ gEnt->s.number ];
}

gentity_t *SV_GEntityForSvEntity( svEntity_t *svEnt )
{
	int num;

	num = svEnt - sv.svEntities;
	return SV_GentityNum( num );
}

void SV_ShutdownGameProgs()
{
	UNIMPLEMENTED(__FUNCTION__);
}