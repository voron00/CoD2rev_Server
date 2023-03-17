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

qboolean SV_inSnapshot(const float *origin, int iEntityNum)
{
	int clientcluster;
	float fogOpaqueDistSqrd;
	svEntity_s *svEnt;
	int l;
	int leafnum;
	gentity_s *ent;
	int i;
	byte *bitvector;

	ent = SV_GentityNum(iEntityNum);

	if ( !ent->r.linked )
	{
		return 0;
	}

	if ( ent->r.broadcastTime )
	{
		return 1;
	}

	if ( ent->r.svFlags & 1 )
	{
		return 0;
	}

	if ( ent->r.svFlags & 0x18 )
	{
		return 1;
	}

	svEnt = SV_SvEntityForGentity(ent);
	leafnum = CM_PointLeafnum(origin);

	if ( !svEnt->numClusters )
	{
		return 0;
	}

	clientcluster = CM_LeafCluster(leafnum);
	bitvector = CM_ClusterPVS(clientcluster);
	l = 0;

	for ( i = 0; i < svEnt->numClusters; ++i )
	{
		l = svEnt->clusternums[i];

		if ( (1 << (l & 7)) & (uint8_t)bitvector[l >> 3] )
		{
			break;
		}
	}

	if ( i == svEnt->numClusters )
	{
		if ( !svEnt->lastCluster )
		{
			return 0;
		}

		while ( l <= svEnt->lastCluster && !((1 << (l & 7)) & (uint8_t)bitvector[l >> 3]) )
		{
			++l;
		}

		if ( l == svEnt->lastCluster )
		{
			return 0;
		}
	}

	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd != 3.4028235e38 )
	{
		return BoxDistSqrdExceeds(ent->r.absmin, ent->r.absmax, origin, fogOpaqueDistSqrd) == 0;
	}

	return 1;
}

void SV_DObjUpdateServerTime(gentity_s *ent, float dtime, int bNotify)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		DObjUpdateServerInfo(obj, dtime, bNotify);
}

void SV_ShutdownGameProgs()
{
	UNIMPLEMENTED(__FUNCTION__);
}