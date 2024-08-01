#include "../qcommon/qcommon.h"

char g_sv_skel_memory[MAX_GENTITIES * MAX_MODELS];
char *g_sv_skel_memory_start;
int g_sv_skel_warn_count;

qboolean gameInitialized = qfalse;

/*
====================
SV_GetClientPing
====================
*/
int SV_GetClientPing(int clientNum)
{
	return svs.clients[clientNum].ping;
}

/*
===============
SV_GetGuid
===============
*/
int SV_GetGuid(int clientNum)
{
	if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
	{
		return 0;
	}

	return svs.clients[clientNum].guid;
}

/*
====================
SV_MapExists
====================
*/
qboolean SV_MapExists(const char *name)
{
	return FS_ReadFile(va("maps/mp/%s.%s", SV_GetMapBaseName(name), GetBspExtension()), 0) >= 0;
}

/*
===============
SV_GetUsercmd
===============
*/
void SV_GetUsercmd( int clientNum, usercmd_t *cmd )
{
	if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
	{
		Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
	}

	*cmd = svs.clients[clientNum].lastUsercmd;
}

/*
===============
SV_LocateGameData
===============
*/
void SV_LocateGameData(gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient)
{
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;

	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
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

int playerstateToClientNum(playerState_t* ps)
{
	return (int)(((byte *)ps - (byte *)sv.gameClients) / sv.gameClientSize);
}

svEntity_t *SV_SvEntityForGentity( gentity_t *gEnt )
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

/*
====================
SV_GetEntityToken
====================
*/
qboolean SV_GetEntityToken( char *buffer, int bufferSize )
{
	return G_GetEntityToken( buffer, bufferSize );
}

/*
====================
SV_IsLocalClient
====================
*/
qboolean SV_IsLocalClient(int clientNum)
{
	return NET_IsLocalAddress(svs.clients[clientNum].netchan.remoteAddress);
}

/*
===============
SV_SetWeaponInfoMemory
===============
*/
void SV_SetWeaponInfoMemory()
{
	Com_SetWeaponInfoMemory(1);
}

/*
===============
SV_FreeWeaponInfoMemory
===============
*/
void SV_FreeWeaponInfoMemory()
{
	Com_FreeWeaponInfoMemory(1);
}

/*
===============
SV_Hunk_FreeTempMemoryInternal
===============
*/
void SV_Hunk_FreeTempMemoryInternal(void *buf)
{
	Hunk_FreeTempMemory(buf);
}

/*
===============
SV_Hunk_AllocateTempMemoryInternal
===============
*/
void* SV_Hunk_AllocateTempMemoryInternal(int size)
{
	return Hunk_AllocateTempMemory(size);
}

/*
===============
SV_Hunk_AllocAlignInternal
===============
*/
void* SV_Hunk_AllocAlignInternal(int size, int aligment)
{
	return Hunk_AllocAlignInternal(size, aligment);
}

/*
===============
SV_Hunk_AllocInternal
===============
*/
void* SV_Hunk_AllocInternal(int size)
{
	return Hunk_Alloc(size);
}

/*
===============
SV_AllocXModelPrecache
===============
*/
void *SV_AllocXModelPrecache(int size)
{
	return Hunk_Alloc(size);
}

/*
===============
SV_AllocXModelPrecacheColl
===============
*/
void *SV_AllocXModelPrecacheColl(int size)
{
	return Hunk_Alloc(size);
}

/*
===============
SV_XModelGet
===============
*/
XModel* SV_XModelGet(const char *name)
{
	if ( !Com_ValidXModelName(name) )
		Com_Error(ERR_DROP, "bad model name %s", name);

	return XModelPrecache(name + 7, SV_AllocXModelPrecache, SV_AllocXModelPrecacheColl);
}

/*
===============
SV_inSnapshot
===============
*/
qboolean SV_inSnapshot(const float *origin, int iEntityNum)
{
	int clientcluster;
	float fogOpaqueDistSqrd;
	svEntity_s *svEnt;
	int l;
	int leafnum;
	gentity_t *ent;
	int i;
	byte *bitvector;

	ent = SV_GentityNum(iEntityNum);

	if ( !ent->r.linked )
	{
		return qfalse;
	}

	if ( ent->r.broadcastTime )
	{
		return qtrue;
	}

	if ( ent->r.svFlags & SVF_NOCLIENT )
	{
		return qfalse;
	}

	if ( ent->r.svFlags & 0x18 )
	{
		return qtrue;
	}

	svEnt = SV_SvEntityForGentity(ent);
	leafnum = CM_PointLeafnum(origin);

	if ( !svEnt->numClusters )
	{
		return qfalse;
	}

	clientcluster = CM_LeafCluster(leafnum);
	bitvector = CM_ClusterPVS(clientcluster);
	l = 0;

	for ( i = 0; i < svEnt->numClusters; ++i )
	{
		l = svEnt->clusternums[i];

		if ( (1 << (l & 7)) & (byte)bitvector[l >> 3] )
		{
			break;
		}
	}

	if ( i == svEnt->numClusters )
	{
		if ( !svEnt->lastCluster )
		{
			return qfalse;
		}

		while ( l <= svEnt->lastCluster && !((1 << (l & 7)) & (byte)bitvector[l >> 3]) )
		{
			++l;
		}

		if ( l == svEnt->lastCluster )
		{
			return qfalse;
		}
	}

	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd != FLT_MAX )
	{
		return BoxDistSqrdExceeds(ent->r.absmin, ent->r.absmax, origin, fogOpaqueDistSqrd) == 0;
	}

	return qtrue;
}

/*
===============
SV_ResetEntityParsePoint
===============
*/
void SV_ResetEntityParsePoint()
{
	sv.entityParsePoint = CM_EntityString();
}

/*
===============
SV_DObjUpdateServerTime
===============
*/
int SV_DObjUpdateServerTime(gentity_t *ent, float dtime, int bNotify)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		return DObjUpdateServerInfo(obj, dtime, bNotify);

	return 0;
}

/*
===============
SV_DObjDisplayAnim
===============
*/
void SV_DObjDisplayAnim(gentity_t *ent)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		DObjDisplayAnim(obj);
}

/*
===============
SV_DObjInitServerTime
===============
*/
void SV_DObjInitServerTime(gentity_t *ent, float dtime)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		DObjInitServerTime(obj, dtime);
}

/*
===============
SV_DObjExists
===============
*/
qboolean SV_DObjExists(gentity_t *ent)
{
	return Com_GetServerDObj(ent->s.number) != NULL;
}

/*
===============
SV_ResetSkeletonCache
===============
*/
void SV_ResetSkeletonCache()
{
	if ( !++sv.skelTimeStamp )
		++sv.skelTimeStamp;

	g_sv_skel_memory_start = (char *)PADP(g_sv_skel_memory, 16);
	sv.skelMemPos = 0;
}

/*
===============
SV_AllocSkelMemory
===============
*/
char* SV_AllocSkelMemory(unsigned int size)
{
	char *pos;

	while ( 1 )
	{
		pos = &g_sv_skel_memory_start[sv.skelMemPos];
		sv.skelMemPos += PAD(size, 16);

		if ( sv.skelMemPos <= sizeof(g_sv_skel_memory) - 16 )
			break;

		if ( g_sv_skel_warn_count != sv.skelTimeStamp )
		{
			g_sv_skel_warn_count = sv.skelTimeStamp;
			Com_Printf("^3WARNING: SV_SKEL_MEMORY_SIZE exceeded\n");
		}

		SV_ResetSkeletonCache();
	}

	return pos;
}

/*
===============
SV_DObjCreateSkelForBone
===============
*/
int SV_DObjCreateSkelForBone(gentity_t *ent, int boneIndex)
{
	DSkel_t *buf;
	DObj *obj;
	int size;

	obj = Com_GetServerDObj(ent->s.number);

	if ( DObjSkelExists(obj, sv.skelTimeStamp) )
	{
		return DObjSkelIsBoneUpToDate(obj, boneIndex);
	}

	size = DObjGetAllocSkelSize(obj);
	buf = (DSkel_t *)SV_AllocSkelMemory(size);
	DObjCreateSkel(obj, buf, sv.skelTimeStamp);

	return 0;
}

/*
===============
SV_DObjCreateSkelForBones
===============
*/
int SV_DObjCreateSkelForBones(gentity_t *ent, int *partBits)
{
	DSkel_t *buf;
	DObj *obj;
	int size;

	obj = Com_GetServerDObj(ent->s.number);

	if ( DObjSkelExists(obj, sv.skelTimeStamp) )
	{
		return DObjSkelAreBonesUpToDate(obj, partBits);
	}

	size = DObjGetAllocSkelSize(obj);
	buf = (DSkel_t *)SV_AllocSkelMemory(size);
	DObjCreateSkel(obj, buf, sv.skelTimeStamp);

	return 0;
}

/*
===============
SV_DObjCalcAnim
===============
*/
void SV_DObjCalcAnim(gentity_t *ent, int *partBits)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjCalcAnim(obj, partBits);
}

/*
===============
SV_DObjGetHierarchyBits
===============
*/
void SV_DObjGetHierarchyBits(gentity_t *ent, int boneIndex, int *partBits)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjGetHierarchyBits(obj, boneIndex, partBits);
}

/*
===============
SV_DObjCalcSkel
===============
*/
void SV_DObjCalcSkel(gentity_t *ent, int *partBits)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjCalcSkel(obj, partBits);
}

/*
===============
SV_DObjGetTree
===============
*/
XAnimTree_s* SV_DObjGetTree(gentity_t *ent)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		return DObjGetTree(obj);

	return NULL;
}

/*
===============
SV_DObjSetControlRotTransIndex
===============
*/
int SV_DObjSetControlRotTransIndex(const gentity_t *ent, int *partBits, int boneIndex)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjSetControlRotTransIndex(obj, partBits, boneIndex);
}

/*
===============
SV_DObjSetRotTransIndex
===============
*/
int SV_DObjSetRotTransIndex(const gentity_t *ent, int *partBits, int boneIndex)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjSetRotTransIndex(obj, partBits, boneIndex);
}

/*
===============
SV_DObjGetRotTransArray
===============
*/
DObjAnimMat* SV_DObjGetRotTransArray(gentity_t *ent)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjGetRotTransArray(obj);
}

/*
===============
SV_DObjGetBoneIndex
===============
*/
int SV_DObjGetBoneIndex(const gentity_t *ent, unsigned int boneName)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		return DObjGetBoneIndex(obj, boneName);

	return -1;
}

/*
===============
SV_DObjNumBones
===============
*/
int SV_DObjNumBones(gentity_t *ent)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjNumBones(obj);
}

/*
===============
SV_DObjGetMatrixArray
===============
*/
DObjAnimMat* SV_DObjGetMatrixArray(gentity_t *ent)
{
	DObj *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjGetRotTransArray(obj);
}

/*
===============
SV_DObjDumpInfo
===============
*/
void SV_DObjDumpInfo(gentity_t *ent)
{
	DObj *obj;

	if ( !com_developer->current.integer )
	{
		return;
	}

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		DObjDumpInfo(obj);
	else
		Com_Printf("no model.\n");
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, int svscmd_type, const char *text )
{
	if ( clientNum == -1 )
	{
		SV_SendServerCommand( NULL, svscmd_type, "%s", text );
	}
	else
	{
		if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
		{
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, svscmd_type, "%s", text );
	}
}

/*
===============
SV_EntityContact
===============
*/
qboolean SV_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *gEnt )
{
	trace_t trace;
	float dist;
	vec2_t center;
	unsigned int model;

	if ( !(gEnt->r.svFlags & 0x60) )
	{
		model = SV_ClipHandleForEntity(gEnt);
		CM_TransformedBoxTraceExternal(&trace, vec3_origin, vec3_origin, mins, maxs, model, -1, gEnt->r.currentOrigin, gEnt->r.currentAngles);

		return trace.startsolid;
	}

	if ( !(gEnt->r.svFlags & 0x20) )
	{
		assert(gEnt->r.svFlags & SVF_DISK);

		Vector2Add(mins, maxs, center);
		Vec2Scale(center, 0.5, center);

		dist = maxs[0] - center[0] + gEnt->r.maxs[0] - 64.0;

		return Vec2DistanceSq(gEnt->r.currentOrigin, center) >= Square(dist);
	}

	assert(gEnt->r.mins[2] == 0);

	if ( gEnt->r.currentOrigin[2] >= maxs[2] )
	{
		return qfalse;
	}

	if ( mins[2] >= gEnt->r.currentOrigin[2] + gEnt->r.maxs[2] )
	{
		return qfalse;
	}

	Vector2Add(mins, maxs, center);
	Vec2Scale(center, 0.5, center);

	dist = maxs[0] - center[0] + gEnt->r.maxs[0];

	return Square(dist) > Vec2DistanceSq(gEnt->r.currentOrigin, center);
}

/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
qboolean SV_GameCommand( void )
{
	if ( !SV_Loaded() )
	{
		return qfalse;
	}

	return ConsoleCommand();
}

/*
===============
SV_GetServerinfo
===============
*/
void SV_GetServerinfo(char *buffer, int bufferSize)
{
	if ( bufferSize < 1 )
	{
		Com_Error(ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize);
	}

	I_strncpyz(buffer, Dvar_InfoString(DVAR_SERVERINFO | DVAR_SCRIPTINFO), bufferSize);
}

/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char *reason )
{
	if ( clientNum < 0 || clientNum >= sv_maxclients->current.integer )
	{
		return;
	}

	SV_DropClient( svs.clients + clientNum, reason );
}

/*
===============
SV_XModelDebugBoxes
===============
*/
void SV_XModelDebugBoxes()
{
	;
}

/*
===============
SV_SetGametype
===============
*/
void SV_SetGametype()
{
	char gametype[MAX_QPATH];

	Dvar_RegisterString("g_gametype", "dm", DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	if ( com_sv_running->current.boolean && G_GetSavePersist() )
		I_strncpyz(gametype, sv.gametype, sizeof(gametype));
	else
		I_strncpyz(gametype, sv_gametype->current.string, sizeof(gametype));

	Q_strlwr(gametype);

	if ( !Scr_IsValidGameType(gametype) )
	{
		Com_Printf("g_gametype %s is not a valid gametype, defaulting to dm\n", gametype);
		strcpy(gametype, "dm");
	}

	Dvar_SetString(sv_gametype, gametype);
}

/*
=================
SV_SetBrushModel

sets mins and maxs for inline bmodels
=================
*/
void SV_SetBrushModel(gentity_t *ent)
{
	vec3_t maxs;
	vec3_t mins;

	CM_ModelBounds(ent->s.index, mins, maxs);
	VectorCopy(mins, ent->r.mins);
	VectorCopy(maxs, ent->r.maxs);
	ent->r.bmodel = qtrue;

	ent->r.contents = -1;     // we don't know exactly what is in the brushes

	SV_LinkEntity(ent);       // FIXME: remove
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
void SV_InitGameVM(qboolean restart, qboolean registerDvars)
{
	int i;

#ifndef DEDICATED
	FX_InitServer();
#endif

	// start the entity parsing at the beginning
	sv.entityParsePoint = CM_EntityString();

#ifndef DEDICATED
	Sys_LoadingKeepAlive();
#endif

	// use the current msec count for a random seed
	// init for this gamestate
	G_InitGame(svs.time, Sys_MillisecondsRaw(), restart, registerDvars);

#ifndef DEDICATED
	Sys_LoadingKeepAlive();
#endif

	// clear all gentity pointers that might still be set from
	// a previous level
	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		svs.clients[i].gentity = NULL;
	}

	if ( com_dedicated->current.integer )
	{
		Com_DvarDump(CON_CHANNEL_LOGFILEONLY);
	}
}

/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs(qboolean savepersist)
{
	G_ShutdownGame(qfalse);
	com_fixedConsolePosition = 0;
	SV_InitGameVM(qtrue, savepersist);
}

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs()
{
	sv.state = SS_DEAD;

#ifndef DEDICATED
	Com_UnloadSoundAliases(2);
#endif

	if ( !gameInitialized )
	{
		return;
	}

	G_ShutdownGame(qtrue);
	SV_FreeWeaponInfoMemory();
	gameInitialized = qfalse;
}

/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs(qboolean savepersist)
{
	gameInitialized = qtrue;
	SV_InitGameVM(qfalse, savepersist);
}