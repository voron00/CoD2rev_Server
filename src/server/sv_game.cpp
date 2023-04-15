#include "../qcommon/qcommon.h"

char g_sv_skel_memory[262144];
char *g_sv_skel_memory_start;
int g_sv_skel_warn_count;

int gameInitialized;

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

int playerstateToClientNum(playerState_t* ps)
{
	return (int)(((byte *)ps - (byte *)sv.gameClients) / sv.gameClientSize);
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

int SV_DObjUpdateServerTime(gentity_s *ent, float dtime, int bNotify)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		return DObjUpdateServerInfo(obj, dtime, bNotify);
	else
		return 0;
}

void SV_DObjDisplayAnim(gentity_s *ent)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		DObjDisplayAnim(obj);
}

void SV_DObjInitServerTime(gentity_s *ent, float dtime)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		DObjInitServerTime(obj, dtime);
}

qboolean SV_DObjExists(gentity_s *ent)
{
	return Com_GetServerDObj(ent->s.number) != 0;
}

void SV_ResetSkeletonCache()
{
	if ( !++sv.skelTimeStamp )
		++sv.skelTimeStamp;

	g_sv_skel_memory_start = (char *)((unsigned int)g_sv_skel_memory & ~15);
	sv.skelMemPos = 0;
}

void SV_ResetEntityParsePoint()
{
	sv.entityParsePoint = CM_EntityString();
}

char* SV_AllocSkelMemory(unsigned int size)
{
	char *pos;
	unsigned int aligment;

	aligment = (size + 15) & ~15;

	while ( 1 )
	{
		pos = &g_sv_skel_memory_start[sv.skelMemPos];
		sv.skelMemPos += aligment;

		if ( sv.skelMemPos <= 262128 )
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

int SV_DObjCreateSkelForBone(gentity_s *ent, int boneIndex)
{
	DSkelPart_s *skel;
	int size;
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( DObjSkelExists(obj, sv.skelTimeStamp) )
		return DObjSkelIsBoneUpToDate(obj, boneIndex);

	size = DObjGetAllocSkelSize(obj);
	skel = (DSkelPart_s *)SV_AllocSkelMemory(size);
	DObjCreateSkel(obj, skel, sv.skelTimeStamp);

	return 0;
}

int SV_DObjCreateSkelForBones(gentity_s *ent, int *partBits)
{
	DSkelPart_s *skel;
	int size;
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( DObjSkelExists(obj, sv.skelTimeStamp) )
		return DObjSkelAreBonesUpToDate(obj, partBits);

	size = DObjGetAllocSkelSize(obj);
	skel = (DSkelPart_s *)SV_AllocSkelMemory(size);
	DObjCreateSkel(obj, skel, sv.skelTimeStamp);

	return 0;
}

void SV_DObjCalcAnim(gentity_s *ent, int *partBits)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjCalcAnim(obj, partBits);
}

void SV_DObjGetHierarchyBits(gentity_s *ent, int boneIndex, int *partBits)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjGetHierarchyBits(obj, boneIndex, partBits);
}

void SV_DObjCalcSkel(gentity_s *ent, int *partBits)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);
	DObjCalcSkel(obj, partBits);
}

XAnimTree_s* SV_DObjGetTree(gentity_s *ent)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		return DObjGetTree(obj);
	else
		return 0;
}

int SV_DObjGetBoneIndex(const gentity_s *ent, unsigned int boneName)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		return DObjGetBoneIndex(obj, boneName);
	else
		return -1;
}

DObjAnimMat* SV_DObjGetMatrixArray(gentity_s *ent)
{
	DObj_s *obj;

	obj = Com_GetServerDObj(ent->s.number);
	return DObjGetRotTransArray(obj);
}

extern dvar_t *com_developer;
void SV_DObjDumpInfo(gentity_s *ent)
{
	DObj_s *obj;

	if ( com_developer->current.integer )
	{
		obj = Com_GetServerDObj(ent->s.number);

		if ( obj )
			DObjDumpInfo(obj);
		else
			Com_Printf("no model.\n");
	}
}

int SV_EntityContact(const float *mins, const float *maxs, gentity_s *gEnt)
{
	unsigned int model;
	float dist;
	trace_t trace;
	float center[2];

	if ( (gEnt->r.svFlags & 0x60) != 0 )
	{
		if ( (gEnt->r.svFlags & 0x20) != 0 )
		{
			if ( gEnt->r.currentOrigin[2] < maxs[2] )
			{
				if ( mins[2] < (float)(gEnt->r.currentOrigin[2] + gEnt->r.maxs[2]) )
				{
					center[0] = *mins + *maxs;
					center[1] = mins[1] + maxs[1];
					center[0] = 0.5 * center[0];
					center[1] = 0.5 * center[1];
					dist = (float)(*maxs - center[0]) + gEnt->r.maxs[0];
					return (float)(dist * dist) > (float)((float)((float)(center[0] - gEnt->r.currentOrigin[0])
					                                      * (float)(center[0] - gEnt->r.currentOrigin[0]))
					                                      + (float)((float)(center[1] - gEnt->r.currentOrigin[1])
					                                              * (float)(center[1] - gEnt->r.currentOrigin[1])));
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			center[0] = *mins + *maxs;
			center[1] = mins[1] + maxs[1];
			center[0] = 0.5 * center[0];
			center[1] = 0.5 * center[1];
			dist = (float)((float)(*maxs - center[0]) + gEnt->r.maxs[0]) - 64.0;
			return (float)((float)((float)(center[0] - gEnt->r.currentOrigin[0])
			                       * (float)(center[0] - gEnt->r.currentOrigin[0]))
			               + (float)((float)(center[1] - gEnt->r.currentOrigin[1])
			                         * (float)(center[1] - gEnt->r.currentOrigin[1]))) >= (float)(dist * dist);
		}
	}
	else
	{
		model = SV_ClipHandleForEntity(gEnt);
		CM_TransformedBoxTraceExternal(
		    &trace,
		    vec3_origin,
		    vec3_origin,
		    mins,
		    maxs,
		    model,
		    -1,
		    gEnt->r.currentOrigin,
		    gEnt->r.currentAngles);
		return trace.startsolid;
	}
}

void SV_SetWeaponInfoMemory()
{
	Com_SetWeaponInfoMemory(1);
}

void SV_FreeWeaponInfoMemory()
{
	Com_FreeWeaponInfoMemory(1);
}

void SV_GetServerinfo(char *buffer, int bufferSize)
{
	if ( bufferSize <= 0 )
		Com_Error(ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize);

	I_strncpyz(buffer, Dvar_InfoString(0x404u), bufferSize);
}

void SV_LocateGameData(gentity_s *gEnts, int numGEntities, int sizeofGEntity_t, playerState_s *clients, int sizeofGameClient)
{
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;
	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
}

void SV_GameDropClient(int clientNum, const char *reason)
{
	if ( clientNum >= 0 && clientNum < sv_maxclients->current.integer )
		SV_DropClient(&svs.clients[clientNum], reason);
}

void SV_GetUsercmd(int clientNum, usercmd_s *cmd)
{
	memcpy(cmd, &svs.clients[clientNum].lastUsercmd, sizeof(usercmd_s));
}

int SV_GetGuid(int clientNum)
{
	if ( clientNum >= 0 && clientNum < sv_maxclients->current.integer )
		return svs.clients[clientNum].guid;
	else
		return 0;
}

void SV_XModelDebugBoxes()
{
	;
}

extern dvar_t *sv_gametype;
void SV_SetGametype()
{
	char gametype[64];
	char *s;

	Dvar_RegisterString("g_gametype", "dm", 0x1024u);

	if ( com_sv_running->current.boolean && G_GetSavePersist() )
		I_strncpyz(gametype, sv.gametype, sizeof(gametype));
	else
		I_strncpyz(gametype, sv_gametype->current.string, sizeof(gametype));

	for ( s = gametype; *s; ++s )
	{
		*s = tolower(*s);
	}

	if ( !Scr_IsValidGameType(gametype) )
	{
		Com_Printf("g_gametype %s is not a valid gametype, defaulting to dm\n", gametype);
		strcpy(gametype, "dm");
	}

	Dvar_SetString(sv_gametype, gametype);
}

void SV_SetBrushModel(gentity_s *ent)
{
	vec3_t maxs;
	vec3_t mins;

	CM_ModelBounds(ent->s.index, mins, maxs);
	VectorCopy(mins, ent->r.mins);
	VectorCopy(maxs, ent->r.maxs);
	ent->r.bmodel = 1;
	ent->r.contents = -1;
	SV_LinkEntity(ent);
}

void SV_InitGameVM(int restart, int registerDvars)
{
	int msec;
	int i;

#ifndef DEDICATED
	FX_InitServer();
#endif
	sv.entityParsePoint = CM_EntityString();
#ifndef DEDICATED
	Sys_LoadingKeepAlive();
#endif
	msec = Sys_MillisecondsRaw();
	G_InitGame(svs.time, msec, restart, registerDvars);
#ifndef DEDICATED
	Sys_LoadingKeepAlive();
#endif
	for ( i = 0; i < sv_maxclients->current.integer; ++i )
		svs.clients[i].gentity = 0;

	if ( com_dedicated->current.integer )
		Com_DvarDump(CON_CHANNEL_LOGFILEONLY);
}

void SV_RestartGameProgs(int savepersist)
{
	G_ShutdownGame(0);
	com_fixedConsolePosition = 0;
	SV_InitGameVM(1, savepersist);
}

void SV_InitGameProgs(int savepersist)
{
	gameInitialized = 1;
	SV_InitGameVM(0, savepersist);
}

void SV_ShutdownGameProgs()
{
	sv.state = SS_DEAD;
#ifndef DEDICATED
	Com_UnloadSoundAliases(2);
#endif
	if ( gameInitialized )
	{
		G_ShutdownGame(1);
		SV_FreeWeaponInfoMemory();
		gameInitialized = 0;
	}
}