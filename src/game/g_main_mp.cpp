#include "../qcommon/qcommon.h"
#include "g_shared.h"

level_locals_t level;

gentity_t g_entities[MAX_GENTITIES];
gclient_t g_clients[MAX_CLIENTS];

dvar_t *g_cheats;
dvar_t *g_gametype;
dvar_t *g_maxclients;
dvar_t *g_synchronousClients;
dvar_t *g_log;
dvar_t *g_logSync;
dvar_t *g_password;
dvar_t *g_banIPs;
dvar_t *g_dedicated;
dvar_t *g_speed;
dvar_t *g_gravity;
dvar_t *g_knockback;
dvar_t *g_weaponAmmoPools;
dvar_t *g_maxDroppedWeapons;
dvar_t *g_inactivity;
dvar_t *g_debugDamage;
dvar_t *g_debugBullets;
dvar_t *g_motd;
dvar_t *g_playerCollisionEjectSpeed;
dvar_t *g_dropForwardSpeed;
dvar_t *g_dropUpSpeedBase;
dvar_t *g_dropUpSpeedRand;
dvar_t *g_clonePlayerMaxVelocity;
dvar_t *voice_global;
dvar_t *voice_localEcho;
dvar_t *voice_deadChat;
dvar_t *g_allowVote;
dvar_t *g_listEntity;
dvar_t *g_deadChat;
dvar_t *g_voiceChatTalkingDuration;
dvar_t *g_ScoresBanner_Allies;
dvar_t *g_ScoresBanner_Axis;
dvar_t *g_ScoresBanner_None;
dvar_t *g_ScoresBanner_Spectators;
dvar_t *g_TeamName_Allies;
dvar_t *g_TeamName_Axis;
dvar_t *g_TeamColor_Allies;
dvar_t *g_TeamColor_Axis;
dvar_t *g_smoothClients;
dvar_t *g_antilag;
dvar_t *g_oldVoting;
dvar_t *g_voteAbstainWeight;
dvar_t *g_no_script_spam;
dvar_t *g_debugLocDamage;
dvar_t *g_friendlyfireDist;
dvar_t *g_friendlyNameDist;
dvar_t *player_meleeRange;
dvar_t *player_meleeWidth;
dvar_t *player_meleeHeight;
dvar_t *g_dumpAnims;
dvar_t *g_useholdtime;
dvar_t *g_useholdspawndelay;
dvar_t *g_mantleBlockTimeBuffer;

/*
==================
G_UpdateObjectiveToClients
==================
*/
void G_UpdateObjectiveToClients()
{
	objective_t *obj;
	gclient_t *client;
	gentity_t *ent;
	int clientNum, objNum;

	for ( clientNum = 0; clientNum < level.maxclients; clientNum++ )
	{
		ent = &level.gentities[clientNum];

		if ( !ent->r.inuse )
		{
			continue;
		}

		assert(ent->client);
		client = ent->client;

		for ( objNum = 0; objNum < MAX_OBJECTIVES; objNum++ )
		{
			obj = &level.objectives[objNum];

			if ( obj->state == OBJST_EMPTY || ( obj->teamNum != TEAM_NONE && obj->teamNum != client->sess.state.team ) )
			{
				client->ps.objective[objNum].state = OBJST_EMPTY;
				continue;
			}

			client->ps.objective[objNum] = *obj;
		}
	}
}

/*
=================
G_LogPrintf
Print to the logfile with a time stamp if it is open
=================
*/
void G_LogPrintf( const char *fmt, ... )
{
	va_list argptr;
	char string[MAX_STRING_CHARS];
	char string2[MAX_STRING_CHARS];

	va_start( argptr, fmt );

	if ( !level.logFile )
	{
		va_end( argptr );
		return;
	}

	Q_vsnprintf( string2, sizeof( string2 ), fmt, argptr );

	Com_sprintf( string,
	             sizeof( string ),
	             "%3i:%i%i %s",
	             level.time / 1000 / 60,
	             level.time / 1000 % 60 / 10,
	             level.time / 1000 % 60 % 10,
	             string2 );

	va_end( argptr );

	FS_Write( string, strlen( string ), level.logFile );
}

/*
=================
G_GetClientState
=================
*/
clientState_t* G_GetClientState( int clientNum )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	return &level.clients[clientNum].sess.state;
}

/*
=================
G_SetClientArchiveTime
=================
*/
void G_SetClientArchiveTime( int clientNum, int iTime )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	level.clients[clientNum].sess.archiveTime = iTime;
}

/*
=================
G_GetClientArchiveTime
=================
*/
int G_GetClientArchiveTime( int clientNum )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	return level.clients[clientNum].sess.archiveTime;
}

/*
=================
G_GetClientScore
=================
*/
int G_GetClientScore( int clientNum )
{
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	return level.clients[clientNum].sess.score;
}

/*
=================
G_GetFogOpaqueDistSqrd
=================
*/
float G_GetFogOpaqueDistSqrd()
{
	return level.fFogOpaqueDistSqrd;
}

/*
=================
G_SetSavePersist
=================
*/
void G_SetSavePersist( int savepersist )
{
	level.savePersist = savepersist;
}

/*
=================
G_GetSavePersist
=================
*/
int G_GetSavePersist()
{
	return level.savePersist;
}

/*
=================
G_UpdateHudElemsToClients
=================
*/
void G_UpdateHudElemsToClients( void )
{
	gentity_t *ent;
	int clientNum;

	for ( clientNum = 0; clientNum < level.maxclients; clientNum++ )
	{
		ent = &level.gentities[clientNum];

		if ( !ent->r.inuse )
		{
			continue;
		}

		assert(ent->client);
		HudElem_UpdateClient(ent->client, ent->s.number, HUDELEM_UPDATE_ARCHIVAL_AND_CURRENT);
	}
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
SortRanks
=============
*/
static signed int SortRanks( const void *a, const void *b )
{
	gclient_t *ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// connecting clients
	if ( ca->sess.connected == CON_CONNECTING )
	{
		return 1;
	}
	if ( cb->sess.connected == CON_CONNECTING )
	{
		return -1;
	}
	// then spectators
	if ( ca->sess.state.team == TEAM_SPECTATOR && cb->sess.state.team == TEAM_SPECTATOR )
	{
		if ( ca < cb)
		{
			return -1;
		}
		if ( ca > cb )
		{
			return 1;
		}
		return 0;
	}
	if ( ca->sess.state.team == TEAM_SPECTATOR )
	{
		return 1;
	}
	if ( cb->sess.state.team == TEAM_SPECTATOR )
	{
		return -1;
	}
	// then sort by score
	if ( ca->sess.score > cb->sess.score )
	{
		return -1;
	}
	if ( ca->sess.score < cb->sess.score )
	{
		return 1;
	}
	// then sort by deaths (cod2)
	if ( ca->sess.deaths < cb->sess.deaths )
	{
		return -1;
	}
	if ( ca->sess.deaths > cb->sess.deaths )
	{
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks()
{
	int i;

	level.numConnectedClients = 0;
	level.numVotingClients = 0;

	for ( i = 0; i < level.maxclients; ++i )
	{
		if ( level.clients[i].sess.connected == CON_DISCONNECTED )
		{
			continue;
		}

		level.sortedClients[level.numConnectedClients] = i;
		level.numConnectedClients++;

		if ( level.clients[i].sess.state.team == TEAM_SPECTATOR )
		{
			continue;
		}

		if ( level.clients[i].sess.connected != CON_CONNECTED )
		{
			continue;
		}

		level.numVotingClients++;
	}

	qsort( level.sortedClients, level.numConnectedClients,
	       sizeof( level.sortedClients[0] ), SortRanks );

	level.bUpdateScoresForIntermission = qtrue;
}

/*
=============
G_AddDebugString
=============
*/
void G_AddDebugString( const vec3_t xyz, const vec4_t color, float scale, const char *pszText )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllIntermissionClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
static void SendScoreboardMessageToAllIntermissionClients( void )
{
	int clientNum;

	if ( !level.bUpdateScoresForIntermission )
	{
		return;
	}

	for ( clientNum = 0; clientNum < level.maxclients; clientNum++ )
	{
		if ( level.clients[clientNum].sess.connected != CON_CONNECTED )
		{
			continue;
		}

		if (  level.clients[clientNum].ps.pm_type != PM_INTERMISSION )
		{
			continue;
		}

		DeathmatchScoreboardMessage(&g_entities[clientNum]);
	}

	level.bUpdateScoresForIntermission = qfalse;
}

/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel( void )
{
	int i;

	Cbuf_ExecuteText(EXEC_APPEND, "map_rotate\n");

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_AXIS] = 0;
	level.teamScores[TEAM_ALLIES] = 0;

	for ( i = 0; i < g_maxclients->current.integer; i++ )
	{
		if ( level.clients[i].sess.connected != CON_CONNECTED )
		{
			continue;
		}
		level.clients[i].sess.score = 0;
	}

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for ( i = 0; i < g_maxclients->current.integer; i++ )
	{
		if ( level.clients[i].sess.connected != CON_CONNECTED )
		{
			continue;
		}
		level.clients[i].sess.connected = CON_CONNECTING;
	}

	G_LogPrintf("ExitLevel: executed\n");
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink( gentity_t *ent )
{
	void (*think)(gentity_t *);
	int thinktime;

	thinktime = ent->nextthink;
	if ( thinktime <= 0 )
	{
		return;
	}
	if ( thinktime > level.time )
	{
		return;
	}

	ent->nextthink = 0;
	think = entityHandlers[ent->handler].think;

	if ( !think )
	{
		Com_Error(ERR_DROP, "NULL ent->think");
	}

	think(ent);
}

/*
==================
CheckVote
==================
*/
void CheckVote()
{
	if ( level.voteExecuteTime && level.voteExecuteTime < level.time )
	{
		level.voteExecuteTime = 0;
		Cbuf_ExecuteText( EXEC_APPEND, va("%s\n", level.voteString ) );
	}
	if ( !level.voteTime )
	{
		return;
	}
	if ( level.time - level.voteTime >= 0 )
	{
		SV_GameSendServerCommand( -1, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEFAILED\"", 101) );
	}
	else
	{
		// ATVI Q3 1.32 Patch #9, WNF
		int passCount = level.numVotingClients / 2 + 1;
		if ( level.voteYes >= passCount )
		{
			// execute the command, then remove the vote
			SV_GameSendServerCommand( -1, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEPASSED\"", 101) );
			level.voteExecuteTime = level.time + 3000;
		}
		else if ( level.voteNo > level.numVotingClients - passCount )
		{
			// same behavior as a timeout
			SV_GameSendServerCommand( -1, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEFAILED\"", 101) );
		}
		else if ( level.voteYes > ceil( level.numVotingClients - ( level.voteYes + level.voteNo ) * g_voteAbstainWeight->current.decimal ) + level.voteNo )
		{
			// execute the command, then remove the vote
			SV_GameSendServerCommand( -1, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEPASSED\"", 101) );
			level.voteExecuteTime = level.time + 3000;
		}
		else
		{
			// still waiting for a majority
			return;
		}
	}
	level.voteTime = 0;
	SV_SetConfigstring( CS_VOTE_TIME, "" );
}

/*
==================
Hunk_AllocXAnimServer
==================
*/
void *Hunk_AllocXAnimServer( int size )
{
	return Hunk_AllocLowInternal(size);
}

/*
==================
G_FreeEntities
==================
*/
void G_FreeEntities( void )
{
	gentity_t *e;
	int i;

	for ( e = g_entities, i = 0; i < level.num_entities; i++, e++ )
	{
		if ( !e->r.inuse )
		{
			continue;
		}

		G_FreeEntity(e);
	}

	if ( g_entities[ENTITYNUM_WORLD].r.inuse )
	{
		G_FreeEntity(&g_entities[ENTITYNUM_WORLD]);
	}

	level.num_entities = 0;
	level.firstFreeEnt = 0;
	level.lastFreeEnt = 0;
}

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( qboolean freeScripts )
{
	Com_Printf("==== ShutdownGame ====\n");

	if ( level.logFile )
	{
		G_LogPrintf("ShutdownGame:\n");
		G_LogPrintf("------------------------------------------------------------\n");
		FS_FCloseFile(level.logFile);
	}

	G_FreeEntities();

	HudElem_DestroyAll();

	if ( Scr_IsSystemActive() && !level.savePersist )
	{
		SV_FreeClientScriptPers();
	}

	Scr_ShutdownSystem(1, level.savePersist == qfalse);

	if ( freeScripts )
	{
		Mantle_ShutdownAnims();
		GScr_FreeScripts();
		Scr_FreeScripts();
		G_FreeAnimTreeInstances();
		Hunk_ClearLow(0);
	}

	for ( int i = 0; i < 1; i++ )
	{
		if ( level.openScriptIOFileBuffers[i] )
		{
			Z_Free(level.openScriptIOFileBuffers[i]);
		}

		level.openScriptIOFileBuffers[i] = NULL;

		if ( level.openScriptIOFileHandles[i] >= 0 )
		{
			FS_FCloseFile(level.openScriptIOFileHandles[i]);
		}

		level.openScriptIOFileHandles[i] = -1;
	}
}

/*
=================
DebugDumpAnims
=================
*/
void DebugDumpAnims()
{
	if ( g_dumpAnims->current.integer < 0 )
	{
		return;
	}

	Com_Printf("server:\n");
	SV_DObjDisplayAnim(&level.gentities[g_dumpAnims->current.integer]);
}

/*
=================
G_SightTrace
=================
*/
void G_SightTrace( int *hitNum, const vec3_t start, const vec3_t end, int passEntityNum, int contentmask )
{
	SV_SightTrace( hitNum, start, vec3_origin, vec3_origin, end, passEntityNum, ENTITYNUM_NONE, contentmask );
}

/*
=================
G_SightTrace
=================
*/
int G_LocationalTracePassed( const vec3_t start, const vec3_t end, int passEntityNum, int contentmask )
{
	return SV_TracePassed( start, vec3_origin, vec3_origin, end, passEntityNum, ENTITYNUM_NONE, contentmask, qtrue, qtrue );
}

/*
=================
G_LocationalTrace
=================
*/
void G_LocationalTrace( trace_t *results, const vec3_t start, const vec3_t end, int passentitynum, int contentmask, unsigned char *priorityMap )
{
	SV_Trace( results, start, vec3_origin, vec3_origin, end, passentitynum, contentmask, qtrue, priorityMap, qtrue );
}

/*
=================
G_TraceCapsuleComplete
=================
*/
int G_TraceCapsuleComplete( const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask )
{
	return SV_TracePassed( start, mins, maxs, end, passEntityNum, ENTITYNUM_NONE, contentmask, qfalse, qfalse );
}

/*
=================
G_TraceCapsule
=================
*/
void G_TraceCapsule( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask )
{
	SV_Trace( results, start, mins, maxs, end, passEntityNum, contentmask, qfalse, NULL, qfalse );
}

/*
=================
G_XAnimUpdateEnt
=================
*/
void G_XAnimUpdateEnt( gentity_t *ent )
{
	while ( 1 )
	{
		if ( !ent->r.inuse )
		{
			break;
		}

		if ( ent->flags & FL_NO_AUTO_ANIM_UPDATE )
		{
			break;
		}

		if ( !G_DObjUpdateServerTime( ent, qtrue ) )
		{
			break;
		}

		Scr_RunCurrentThreads();
	}
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime )
{
	byte entIndex[MAX_GENTITIES];
	int entnum;
	trigger_info_t *trigger_info;
	gentity_t *other;
	gentity_t *ent;
	qboolean bMoreTriggered;
	int index;
	int i;

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;
	level.frameTime = levelTime - level.previousTime;

	level_bgs.time = levelTime;
	level_bgs.latestSnapshotTime = levelTime;
	level_bgs.frametime = levelTime - level.previousTime;

	assert(level.frameTime >= 0);

	for ( ent = g_entities, i = 0; i < level.num_entities; i++, ent++ )
	{
		if ( !ent->r.inuse )
		{
			continue;
		}

		SV_DObjInitServerTime(ent, level.frameTime * 0.001);
	}

	memset(entIndex, 0, sizeof(entIndex));
	index = 0;
	assert(level.currentTriggerListSize == 0);
	Com_Memcpy(level.currentTriggerList, level.pendingTriggerList, sizeof(level.pendingTriggerList[0]) * level.pendingTriggerListSize);

	level.currentTriggerListSize = level.pendingTriggerListSize;
	level.pendingTriggerListSize = 0;

	do
	{
		bMoreTriggered = qfalse;
		index++;

		for ( i = 0; i < level.currentTriggerListSize; i++ )
		{
			trigger_info = &level.currentTriggerList[i];
			entnum = trigger_info->entnum;
			ent = &g_entities[entnum];

			if ( ent->useCount == trigger_info->useCount )
			{
				assert(ent->r.inuse);
				other = &g_entities[trigger_info->otherEntnum];

				if ( other->useCount == trigger_info->otherUseCount )
				{
					assert(other->r.inuse);
					if ( entIndex[entnum] == index )
					{
						bMoreTriggered = qtrue;
						continue;
					}

					entIndex[entnum] = index;

					Scr_AddEntity(other);
					Scr_Notify(ent, scr_const.trigger, 1);
				}
			}

			--level.currentTriggerListSize;
			--i;

			Com_Memcpy(trigger_info, &level.currentTriggerList[level.currentTriggerListSize], sizeof(level.currentTriggerList[0]));
		}

		Scr_RunCurrentThreads();
	}
	while ( bMoreTriggered );
	assert(level.currentTriggerListSize == 0);

	for ( ent = g_entities, i = 0; i < level.num_entities; i++, ent++ )
	{
		G_XAnimUpdateEnt(ent);
	}

	Scr_IncTime();

	//
	// go through all allocated objects
	//
	assert(level.currentEntityThink == -1);
	for ( ent = g_entities, level.currentEntityThink = 0; level.currentEntityThink < level.num_entities; level.currentEntityThink++, ent++ )
	{
		if ( !ent->r.inuse )
		{
			continue;
		}

		if ( ent->tagInfo )
		{
			G_RunFrameForEntity(ent->tagInfo->parent);
		}

		G_RunFrameForEntity(ent);
	}

	level.currentEntityThink = -1;

	G_UpdateObjectiveToClients();
	G_UpdateHudElemsToClients();

	// perform final fixups on the players
	for ( ent = g_entities,i=0 ; i < level.maxclients ; i++, ent++ )
	{
		if ( !ent->r.inuse )
		{
			continue;
		}
		assert(i >= 0 && i < MAX_CLIENTS);
		assert(level_bgs.clientinfo[i].infoValid);
		assert(ent->client - level.clients == i);
		ClientEndFrame( ent );
	}

	// update to team status?
	CheckTeamStatus();

	// check team votes
	if ( g_oldVoting->current.boolean )
	{
		CheckVote();
	}

	SendScoreboardMessageToAllIntermissionClients();

	if ( g_listEntity->current.boolean )
	{
		for ( i = 0; i < MAX_GENTITIES; i++ )
		{
			Com_Printf("%4i: %s\n", i, SL_ConvertToString(g_entities[i].classname));
		}

		Dvar_SetBool(g_listEntity, false);
	}

	if ( level.registerWeapons )
	{
		SaveRegisteredWeapons();
	}

	if ( level.bRegisterItems )
	{
		SaveRegisteredItems();
	}

	DebugDumpAnims();
}

/*
============
G_InitGame
============
*/
void G_InitGame( int levelTime, int randomSeed, int restart, int savepersist )
{
	int i;
	char cs[MAX_INFO_STRING];

	Com_Printf("------- Game Initialization -------\n");
	Com_Printf("gamename: %s\n", GAMEVERSION);
	Com_Printf("gamedate: %s\n", __DATE__);

	Swap_Init();

	memset(&level, 0, sizeof(level));

	level.initializing = qtrue;
	level.time = levelTime;
	level.startTime = levelTime;
	level.currentEntityThink = -1;

	srand( randomSeed );
	Rand_Init( randomSeed );

	G_SetupWeaponDef();

	if ( !restart || !savepersist )
	{
		G_RegisterDvars();
	}

	G_ProcessIPBans();

	level_bgs.GetXModel = SV_XModelGet;
	level_bgs.CreateDObj = G_CreateDObj;
	level_bgs.SafeDObjFree = Com_SafeServerDObjFree;
	level_bgs.AllocXAnim = Hunk_AllocXAnimServer;
	level_bgs.anim_user = 1;

	if ( g_log->current.string[0] )
	{
		if ( g_logSync->current.boolean )
		{
			FS_FOpenFileByMode(g_log->current.string, &level.logFile, FS_APPEND_SYNC);
		}
		else
		{
			FS_FOpenFileByMode(g_log->current.string, &level.logFile, FS_APPEND);
		}

		if ( level.logFile )
		{
			SV_GetServerinfo(cs, sizeof(cs));
			G_LogPrintf("------------------------------------------------------------\n");
			G_LogPrintf("InitGame: %s\n", cs);
		}
		else
		{
			Com_Printf("WARNING: Couldn't open logfile: %s\n", g_log->current.string);
		}
	}
	else
	{
		Com_Printf("Not logging to disk.\n");
	}

	for ( i = 0; i < 1; i++ )
	{
		level.openScriptIOFileHandles[i] = -1;
		level.openScriptIOFileBuffers[i] = NULL;

		Com_Memset(level.currentScriptIOLineMark, 0, sizeof(level.currentScriptIOLineMark[0]));
	}

	Mantle_CreateAnims(Hunk_AllocXAnimServer);

	if ( !restart )
	{
		// RF, init the anim scripting
		Com_Memset(&level_bgs.animData.animScriptData, 0, sizeof(level_bgs.animData.animScriptData));
		level_bgs.animData.animScriptData.soundAlias = Com_FindSoundAlias;
		level_bgs.animData.animScriptData.playSoundAlias = G_AnimScriptSound;
		GScr_LoadScripts();
		BG_LoadAnim();
		G_LoadAnimTreeInstances();
	}

	GScr_LoadConsts();

	// DHM - Nerve :: GT_WOLF checks the current value of
	SV_GetConfigstring(CS_MULTI_MAPWINNER, cs, sizeof(cs));
	Info_SetValueForKey(cs, "winner", "0");
	SV_SetConfigstring(CS_MULTI_MAPWINNER, cs);

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof( g_entities[0] ) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients->current.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof( g_clients[0] ) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS + MAX_CLIENT_CORPSES;

	level.firstFreeEnt = 0;
	level.lastFreeEnt = 0;

	// let the server system know where the entites are
	SV_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
	                   &level.clients[0].ps, sizeof( level.clients[0] ) );

	G_ParseHitLocDmgTable();

	G_InitTurrets();

	G_SpawnEntitiesFromString();

	G_setfog("0");

	G_InitObjectives();

	Scr_FreeEntityList();

	Com_Printf("-----------------------------------\n");

	Scr_InitSystem();

	Scr_SetLoading(true);

	Scr_AllocGameVariable();

	G_LoadStructs();

	Scr_LoadGameType();

	Scr_LoadLevel();

	Scr_StartupGameType();

	for ( i = 0; i < MAX_CLIENT_CORPSES; i++ )
	{
		g_scr_data.playerCorpseInfo[i].entnum = -1;
	}

	level.initializing = qfalse;

	SaveRegisteredWeapons();
	SaveRegisteredItems();
}

/*
============
G_FreeAnimTreeInstances
============
*/
void G_FreeAnimTreeInstances( void )
{
	int i;

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		if ( level_bgs.clientinfo[i].pXAnimTree == NULL )
		{
			continue;
		}

		XAnimFreeTree(level_bgs.clientinfo[i].pXAnimTree, NULL);
		level_bgs.clientinfo[i].pXAnimTree = NULL;
	}

	for ( i = 0; i < MAX_CLIENT_CORPSES; i++ )
	{
		if ( g_scr_data.playerCorpseInfo[i].tree == NULL )
		{
			continue;
		}

		XAnimFreeTree(g_scr_data.playerCorpseInfo[i].tree, NULL);
		g_scr_data.playerCorpseInfo[i].tree = NULL;
	}
}

/*
============
G_LoadAnimTreeInstances
============
*/
void G_LoadAnimTreeInstances( void )
{
	int i;
	XAnim *anims;

	anims = level_bgs.animData.generic_human.tree.anims;
	assert(anims);

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		level_bgs.clientinfo[i].pXAnimTree = XAnimCreateTree(anims, Hunk_AllocXAnimServer);
	}

	for ( i = 0; i < MAX_CLIENT_CORPSES; i++ )
	{
		g_scr_data.playerCorpseInfo[i].tree = XAnimCreateTree(anims, Hunk_AllocXAnimServer);
	}
}

/*
============
G_CreateDObj
============
*/
void G_CreateDObj( DObjModel_s *dobjModels, unsigned short numModels, XAnimTree *tree, int handle )
{
	Com_ServerDObjCreate(dobjModels, numModels, tree, handle);
}

/*
============
G_RunFrameForEntity
============
*/
void G_RunFrameForEntity( gentity_t *ent )
{
	assert(ent->r.inuse);

	if ( ent->processedFrame == level.framenum )
	{
		return;
	}

	ent->processedFrame = level.framenum;

	// check EF_NODRAW status for non-clients
	if ( !ent->client )
	{
		if ( ent->flags & FL_NODRAW )
		{
			ent->s.eFlags |= EF_NODRAW;
		}
		else
		{
			ent->s.eFlags &= ~EF_NODRAW;
		}
	}

	//assert(ent->r.maxs[0] >= ent->r.mins[0]);
	//assert(ent->r.maxs[1] >= ent->r.mins[2]);
	//assert(ent->r.maxs[2] >= ent->r.mins[2]);

	// clear events that are too old
	if ( ent->s.eFlags == EF_UNKNOWN )
	{
		if ( level.time > ent->s.time2 )
		{
			G_FreeEntity(ent);
			return;
		}
	}
	if ( level.time - ent->eventTime > 300 )
	{
		if ( ent->freeAfterEvent )
		{
			// tempEntities or dropped items completely go away after their event
			G_FreeEntity( ent );
			return;
		}
		if ( ent->unlinkAfterEvent )
		{
			// items that will respawn will hide themselves after their pickup event
			ent->unlinkAfterEvent = qfalse;
			SV_UnlinkEntity( ent );
		}
	}

	// temporary entities don't think
	if ( ent->freeAfterEvent )
	{
		return;
	}

	if ( ent->s.eType == ET_MISSILE )
	{
		G_RunMissile(ent);
		return;
	}

	if ( ent->s.eType == ET_ITEM )
	{
		if ( ent->tagInfo )
		{
			G_GeneralLink(ent);
			G_RunThink(ent);
			return;
		}

		G_RunItem(ent);
		return;
	}

	if ( ent->s.eType == ET_PLAYER_CORPSE )
	{
		G_RunCorpse(ent);
		return;
	}

	if ( ent->physicsObject )
	{
		G_RunItem(ent);
		return;
	}

	if ( ent->s.eType == ET_SCRIPTMOVER )
	{
		G_RunMover(ent);
		return;
	}

	if ( ent->client )
	{
		G_RunClient(ent);
		return;
	}

	if ( ent->s.eType == ET_GENERAL )
	{
		if ( ent->tagInfo )
		{
			G_GeneralLink(ent);
		}
	}

	G_RunThink( ent );
}

/*
=================
G_RegisterDvars
=================
*/
void G_RegisterDvars()
{
	// don't override the cheat state set by the system
	g_cheats = Dvar_RegisterBool("sv_cheats", false, DVAR_CHANGEABLE_RESET);

	// noset vars
	Dvar_RegisterString("gamename", GAMEVERSION, DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterString("gamedate", __DATE__, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterString("sv_mapname", "", DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);

	// latched vars
	g_gametype = Dvar_RegisterString("g_gametype", "dm", DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	g_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, MAX_CLIENTS, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	// JPW NERVE multiplayer stuffs
	g_synchronousClients = Dvar_RegisterBool("g_synchronousClients", false, DVAR_SYSTEMINFO | DVAR_CHANGEABLE_RESET);
	g_log = Dvar_RegisterString("g_log", "games_mp.log", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_logSync = Dvar_RegisterBool("g_logSync", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_password = Dvar_RegisterString("g_password", "", DVAR_CHANGEABLE_RESET);
	g_banIPs = Dvar_RegisterString("g_banIPs", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dedicated = Dvar_RegisterInt("dedicated", 0, 0, 2, DVAR_LATCH | DVAR_CHANGEABLE_RESET);

	if ( g_dedicated->current.integer )
	{
		Dvar_RegisterInt("dedicated", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	}

	g_speed = Dvar_RegisterInt("g_speed", 190, INT_MIN, INT_MAX, DVAR_CHANGEABLE_RESET);
	g_gravity = Dvar_RegisterFloat("g_gravity", DEFAULT_GRAVITY, 1, FLT_MAX, DVAR_CHANGEABLE_RESET);
	g_knockback = Dvar_RegisterFloat("g_knockback", 1000, -FLT_MAX, FLT_MAX, DVAR_CHANGEABLE_RESET);

	g_weaponAmmoPools = Dvar_RegisterBool("g_weaponAmmoPools", false, DVAR_CHANGEABLE_RESET);
	g_maxDroppedWeapons = Dvar_RegisterInt("g_maxDroppedWeapons", 16, 1, 32, DVAR_CHANGEABLE_RESET);

	g_inactivity = Dvar_RegisterInt("g_inactivity", 0, 0, INT_MAX, DVAR_CHANGEABLE_RESET);

	g_debugDamage = Dvar_RegisterBool("g_debugDamage", false, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_debugBullets = Dvar_RegisterInt("g_debugBullets", 0, -3, 6, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	g_motd = Dvar_RegisterString("g_motd", "", DVAR_CHANGEABLE_RESET);

	g_playerCollisionEjectSpeed = Dvar_RegisterInt("g_playerCollisionEjectSpeed", 25, 0, 32000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dropForwardSpeed = Dvar_RegisterFloat("g_dropForwardSpeed", 10, 0, 1000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dropUpSpeedBase = Dvar_RegisterFloat("g_dropUpSpeedBase", 10, 0, 1000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dropUpSpeedRand = Dvar_RegisterFloat("g_dropUpSpeedRand", 5, 0, 1000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_clonePlayerMaxVelocity = Dvar_RegisterFloat("g_clonePlayerMaxVelocity", 80, 0, FLT_MAX, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);

	voice_global = Dvar_RegisterBool("voice_global", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	voice_localEcho = Dvar_RegisterBool("voice_localEcho", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	voice_deadChat = Dvar_RegisterBool("voice_deadChat", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);

	g_allowVote = Dvar_RegisterBool("g_allowVote", true, DVAR_CHANGEABLE_RESET);
	g_listEntity = Dvar_RegisterBool("g_listEntity", false, DVAR_CHANGEABLE_RESET);

	g_deadChat = Dvar_RegisterBool("g_deadChat", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_voiceChatTalkingDuration = Dvar_RegisterInt("g_voiceChatTalkingDuration", 500, 0, 10000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);

	g_ScoresBanner_Allies = Dvar_RegisterString("g_ScoresBanner_Allies", "mpflag_american", DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_Axis = Dvar_RegisterString("g_ScoresBanner_Axis", "mpflag_german", DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_None = Dvar_RegisterString("g_ScoresBanner_None", "mpflag_none", DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_Spectators = Dvar_RegisterString("g_ScoresBanner_Spectators", "mpflag_spectator", DVAR_CODINFO | DVAR_CHANGEABLE_RESET);

	g_TeamName_Allies = Dvar_RegisterString("g_TeamName_Allies", "GAME_ALLIES", DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	g_TeamName_Axis = Dvar_RegisterString("g_TeamName_Axis", "GAME_AXIS", DVAR_CODINFO | DVAR_CHANGEABLE_RESET);

	g_TeamColor_Allies = Dvar_RegisterColor("g_TeamColor_Allies", 0.5, 0.5, 1.0, 1.0, DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	g_TeamColor_Axis = Dvar_RegisterColor("g_TeamColor_Axis", 1.0, 0.5, 0.5, 1.0, DVAR_CODINFO | DVAR_CHANGEABLE_RESET);

	g_smoothClients = Dvar_RegisterBool("g_smoothClients", true, DVAR_CHANGEABLE_RESET);
	g_antilag = Dvar_RegisterBool("g_antilag", true, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);

	g_oldVoting = Dvar_RegisterBool("g_oldVoting", true, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_voteAbstainWeight = Dvar_RegisterFloat("g_voteAbstainWeight", 0.5, 0, 1.0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);

	g_no_script_spam = Dvar_RegisterBool("g_no_script_spam", false, DVAR_CHANGEABLE_RESET);
	g_debugLocDamage = Dvar_RegisterBool("g_debugLocDamage", false, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	g_friendlyfireDist = Dvar_RegisterFloat("g_friendlyfireDist", 256, 0, 15000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_friendlyNameDist = Dvar_RegisterFloat("g_friendlyNameDist", 15000, 0, 15000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	player_meleeRange = Dvar_RegisterFloat("player_meleeRange", 64, 0, 1000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_meleeWidth = Dvar_RegisterFloat("player_meleeWidth", 10, 0, 1000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_meleeHeight = Dvar_RegisterFloat("player_meleeHeight", 10, 0, 1000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	g_dumpAnims = Dvar_RegisterInt("g_dumpAnims", -1, -1, ENTITYNUM_NONE, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	g_useholdtime = Dvar_RegisterInt("g_useholdtime", 0, 0, INT_MAX, DVAR_CHANGEABLE_RESET);
	g_useholdspawndelay = Dvar_RegisterInt("g_useholdspawndelay", 1, 0, 10, DVAR_ARCHIVE | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	g_mantleBlockTimeBuffer = Dvar_RegisterInt("g_mantleBlockTimeBuffer", 500, 0, 60000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);

	BG_RegisterDvars();
}