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
dvar_t *g_mantleBlockEnable;
dvar_t *g_mantleBlockTimeBuffer;

#ifdef LIBCOD
dvar_t *g_playerCollision;
dvar_t *g_playerEject;
#endif

dvar_t *g_fixedWeaponSpreads;
dvar_t *g_dropGrenadeOnDeath;

entityHandler_t entityHandlers[] =
{
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, Touch_Multi, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, hurt_use, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, hurt_touch, hurt_use, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, Use_trigger_damage, Pain_trigger_damage, Die_trigger_damage, NULL, 0, 0 },
	{ NULL, Reached_ScriptMover, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, Reached_ScriptMover, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ G_ExplodeMissile, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3, 4 },
	{ G_ExplodeMissile, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 5, 6 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, player_die, G_PlayerController, 0, 0 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, player_die, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, G_PlayerController, 0, 0 },
	{ BodyEnd, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
	{ turret_think_init, NULL, NULL, NULL, turret_use, NULL, NULL, turret_controller, 0, 0 },
	{ turret_think, NULL, NULL, NULL, turret_use, NULL, NULL, turret_controller, 0, 0 },
	{ DroppedItemClearOwner, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, 0, 0 },
	{ FinishSpawningItem, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, Touch_Item_Auto, NULL, NULL, NULL, NULL, 0, 0 },
	{ NULL, NULL, NULL, NULL, use_trigger_use, NULL, NULL, NULL, 0, 0 },
	{ G_FreeEntity, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0 },
};

clientState_t* G_GetClientState(int num)
{
	return &level.clients[num].sess.state;
}

int G_GetClientArchiveTime(int clientindex)
{
	return level.clients[clientindex].sess.archiveTime;
}

void G_SetClientArchiveTime(int clindex, int time)
{
	level.clients[clindex].sess.archiveTime = time;
}

int G_GetClientScore(int clientNum)
{
	return level.clients[clientNum].sess.score;
}

void Scr_LocalizationError(int iParm, const char *pszErrorMessage)
{
	Com_Error(ERR_LOCALIZATION, pszErrorMessage);
}

void ExitLevel()
{
	int i;
	int j;

	Cbuf_ExecuteText(EXEC_APPEND, "map_rotate\n");

	level.teamScores[1] = 0;
	level.teamScores[2] = 0;

	for ( i = 0; i < g_maxclients->current.integer; ++i )
	{
		if ( level.clients[i].sess.connected == CON_CONNECTED )
			level.clients[i].sess.score = 0;
	}

	for ( j = 0; j < g_maxclients->current.integer; ++j )
	{
		if ( level.clients[j].sess.connected == CON_CONNECTED )
			level.clients[j].sess.connected = CON_CONNECTING;
	}

	G_LogPrintf("ExitLevel: executed\n");
}

void G_RunThink(gentity_s *ent)
{
	void (*think)(struct gentity_s *);
	int thinktime;

	thinktime = ent->nextthink;

	if ( thinktime > 0 && thinktime <= level.time )
	{
		ent->nextthink = 0;
		think = entityHandlers[ent->handler].think;

		if ( !think )
			Com_Error(ERR_DROP, "NULL ent->think");

		think(ent);
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
	char string[1024];
	int min, tens, sec, l;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof( string ), "%i:%i%i ", min, tens, sec );

	l = strlen( string );

	va_start( argptr, fmt );
	Q_vsnprintf( string + l, sizeof( string ) - l, fmt, argptr );
	va_end( argptr );

	if ( !level.logFile )
	{
		return;
	}

	FS_Write( string, strlen( string ), level.logFile );
}

void G_TraceCapsule(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask)
{
	SV_Trace(results, start, mins, maxs, end, passEntityNum, contentmask, 0, 0, 0);
}

int G_TraceCapsuleComplete(const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask)
{
	return SV_TracePassed(start, mins, maxs, end, passEntityNum, 1023, contentmask, 0, 0);
}

void G_LocationalTrace(trace_t *results, const float *start, const float *end, int passentitynum, int contentmask, unsigned char *priorityMap)
{
	SV_Trace(results, start, vec3_origin, vec3_origin, end, passentitynum, contentmask, 1, priorityMap, 1);
}

int G_LocationalTracePassed(const float *start, const float *end, int passEntityNum, int contentmask)
{
	return SV_TracePassed(start, vec3_origin, vec3_origin, end, passEntityNum, 1023, contentmask, 1, 1);
}

void G_SightTrace(int *hitNum, const float *start, const float *end, int passEntityNum, int contentmask)
{
	SV_SightTrace(hitNum, start, vec3_origin, vec3_origin, end, passEntityNum, 1023, contentmask);
}

gentity_s* G_FX_VisibilityTrace(trace_t *trace, const float *start, const float *end, int passentitynum, int contentmask, unsigned char *priorityMap, float *forwardAngles)
{
#ifndef DEDICATED
	float dist;
	float visible;
	vec3_t endPos;
#endif

	G_LocationalTrace(trace, start, end, passentitynum, contentmask, priorityMap);

	if ( trace->entityNum > 1021 )
		return 0;

#ifdef DEDICATED
	return &g_entities[trace->entityNum];
#else
	dist = trace->fraction * 15000.0;
	VectorMA(start, dist, forwardAngles, endPos);

	visible = SV_FX_GetVisibility(start, endPos);

	if ( visible >= 0.2 )
		return &g_entities[trace->entityNum];
	else
		return 0;
#endif
}

static signed int SortRanks(const void *num1, const void *num2)
{
	gclient_t *client1;
	gclient_t *client2;

	client2 = &level.clients[*(int *)num1];
	client1 = &level.clients[*(int *)num2];

	if ( client2->sess.connected == CON_CONNECTING )
		return 1;

	if ( client1->sess.connected == CON_CONNECTING )
		return -1;

	if ( client2->sess.state.team == TEAM_SPECTATOR && client1->sess.state.team == TEAM_SPECTATOR )
	{
		if ( client2 >= client1 )
			return client2 > client1;
		else
			return -1;
	}
	else if ( client2->sess.state.team == TEAM_SPECTATOR )
	{
		return 1;
	}
	else if ( client1->sess.state.team == TEAM_SPECTATOR )
	{
		return -1;
	}
	else if ( client2->sess.score <= client1->sess.score )
	{
		if ( client2->sess.score >= client1->sess.score )
		{
			if ( client2->sess.deaths >= client1->sess.deaths )
				return client2->sess.deaths > client1->sess.deaths;
			else
				return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return -1;
	}
}

void CalculateRanks()
{
	int i;

	level.numConnectedClients = 0;
	level.numVotingClients = 0;

	for ( i = 0; i < level.maxclients; ++i )
	{
		if ( level.clients[i].sess.connected )
		{
			level.sortedClients[level.numConnectedClients++] = i;

			if ( level.clients[i].sess.state.team != TEAM_SPECTATOR && level.clients[i].sess.connected == CON_CONNECTED )
				++level.numVotingClients;
		}
	}

	qsort(level.sortedClients, level.numConnectedClients, sizeof(intptr_t), SortRanks);
	level.bUpdateScoresForIntermission = 1;
}

float G_GetFogOpaqueDistSqrd()
{
	return level.fFogOpaqueDistSqrd;
}

int G_GetSavePersist()
{
	return level.savePersist;
}

void G_SetSavePersist(int savepersist)
{
	level.savePersist = savepersist;
}

void G_RunFrameForEntity(gentity_s *ent)
{
	int flags;

	if ( ent->framenum != level.framenum )
	{
		ent->framenum = level.framenum;

		if ( !ent->client )
		{
			if ( (ent->flags & 0x800) != 0 )
				flags = ent->s.eFlags | 0x20;
			else
				flags = ent->s.eFlags & 0xFFFFFFDF;

			ent->s.eFlags = flags;
		}

		if ( ent->s.eFlags == 0x10000 && level.time > ent->s.time2 )
		{
			G_FreeEntity(ent);
			return;
		}

		if ( level.time - ent->eventTime > 300 )
		{
			if ( ent->freeAfterEvent )
			{
				G_FreeEntity(ent);
				return;
			}

			if ( ent->unlinkAfterEvent )
			{
				ent->unlinkAfterEvent = 0;
				SV_UnlinkEntity(ent);
			}
		}

		if ( !ent->freeAfterEvent )
		{
			switch ( ent->s.eType )
			{
			case ET_MISSILE:
				G_RunMissile(ent);
				return;

			case ET_ITEM:
				if ( ent->tagInfo )
				{
					G_GeneralLink(ent);
					G_RunThink(ent);
					return;
				}
				G_RunItem(ent);
				return;

			case ET_PLAYER_CORPSE:
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
			}
			else if ( ent->client )
			{
				G_RunClient(ent);
			}
			else
			{
				if ( ent->s.eType == ET_GENERAL )
				{
					if ( ent->tagInfo )
						G_GeneralLink(ent);
				}

				G_RunThink(ent);
			}
		}
	}
}

void DebugDumpAnims()
{
	if ( g_dumpAnims->current.integer >= 0 )
	{
		Com_Printf("server:\n");
		SV_DObjDisplayAnim(&level.gentities[g_dumpAnims->current.integer]);
	}
}

void CheckVote()
{
	int passCount;

	if ( level.voteExecuteTime )
	{
		if ( level.voteExecuteTime < level.time )
		{
			level.voteExecuteTime = 0;
			Cbuf_ExecuteText(EXEC_APPEND, va("%s\n", level.voteString));
		}
	}

	if ( level.voteTime )
	{
		if ( level.time - level.voteTime < 0 )
		{
			passCount = level.numVotingClients / 2 + 1;

			if ( level.voteYes >= passCount )
				goto pass;

			if ( level.voteNo <= level.numVotingClients - passCount )
				return;
		}
		else
		{
			if ( level.voteYes > (int)(ceil(
			                               (level.numVotingClients - (level.voteYes + level.voteNo))
			                               * g_voteAbstainWeight->current.decimal)
			                           + level.voteNo) )
			{
pass:
				SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEPASSED\"", 101));
				level.voteExecuteTime = level.time + 3000;
fail:
				level.voteTime = 0;
				SV_SetConfigstring(CS_VOTE_TIME, "");
				return;
			}
		}

		SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEFAILED\"", 101));
		goto fail;
	}
}

void G_UpdateHudElemsToClients()
{
	int i;

	for ( i = 0; i < level.maxclients; ++i )
	{
		if ( level.gentities[i].r.inuse )
			HudElem_UpdateClient(level.gentities[i].client, level.gentities[i].s.number, HUDELEM_UPDATE_ARCHIVAL_AND_CURRENT);
	}
}

void G_UpdateObjectiveToClients()
{
	objective_t *objective;
	int team;
	gclient_s *client;
	int j;
	gentity_s *ent;
	int i;

	for ( i = 0; i < level.maxclients; ++i )
	{
		ent = &level.gentities[i];

		if ( ent->r.inuse )
		{
			client = ent->client;
			team = client->sess.state.team;

			for ( j = 0; j < MAX_OBJECTIVES; ++j )
			{
				if ( level.objectives[j].state && (!level.objectives[j].teamNum || level.objectives[j].teamNum == team) )
				{
					objective = &client->ps.objective[j];
					objective->state = level.objectives[j].state;
					objective->origin[0] = level.objectives[j].origin[0];
					objective->origin[1] = level.objectives[j].origin[1];
					objective->origin[2] = level.objectives[j].origin[2];
					objective->entNum = level.objectives[j].entNum;
					objective->teamNum = level.objectives[j].teamNum;
					objective->icon = level.objectives[j].icon;
				}
				else
				{
					client->ps.objective[j].state = OBJST_EMPTY;
				}
			}
		}
	}
}

void G_XAnimUpdateEnt(gentity_s *ent)
{
	while ( ent->r.inuse && (ent->flags & 0x2000) == 0 && G_DObjUpdateServerTime(ent, 1) )
		Scr_RunCurrentThreads();
}

void G_RunFrame(int time)
{
	float nextFrameTime;
	byte entIndex[1024];
	int entnum;
	trigger_info_t *trigger_info;
	gentity_s *other;
	gentity_s *ent;
	int bMoreTriggered;
	int index;
	int i;

	++level.framenum;
	level.previousTime = level.time;
	level.time = time;
	level.frameTime = time - level.previousTime;
	level_bgs.time = time;
	level_bgs.latestSnapshotTime = time;
	level_bgs.frametime = time - level.previousTime;
	ent = g_entities;
	i = 0;

	while ( i < level.num_entities )
	{
		if ( ent->r.inuse )
		{
			nextFrameTime = (float)level.frameTime * 0.001;
			SV_DObjInitServerTime(ent, nextFrameTime);
		}

		++i;
		++ent;
	}

	memset(entIndex, 0, sizeof(entIndex));
	index = 0;

	Com_Memcpy(level.currentTriggerList, level.pendingTriggerList, sizeof(trigger_info_t) * level.pendingTriggerListSize);

	level.currentTriggerListSize = level.pendingTriggerListSize;
	level.pendingTriggerListSize = 0;

	do
	{
		bMoreTriggered = 0;
		++index;

		for ( i = 0; i < level.currentTriggerListSize; ++i )
		{
			trigger_info = &level.currentTriggerList[i];
			entnum = trigger_info->entnum;
			ent = &g_entities[entnum];

			if ( ent->useCount == trigger_info->useCount )
			{
				other = &g_entities[trigger_info->otherEntnum];

				if ( other->useCount == trigger_info->otherUseCount )
				{
					if ( entIndex[entnum] == index )
					{
						bMoreTriggered = 1;
						continue;
					}

					entIndex[entnum] = index;
					Scr_AddEntity(other);
					Scr_Notify(ent, scr_const.trigger, 1u);
				}
			}

			--level.currentTriggerListSize;
			--i;

			Com_Memcpy(trigger_info, &level.currentTriggerList[level.currentTriggerListSize], sizeof(trigger_info_t));
		}

		Scr_RunCurrentThreads();
	}
	while ( bMoreTriggered );

	ent = g_entities;
	i = 0;

	while ( i < level.num_entities )
	{
		G_XAnimUpdateEnt(ent);
		++i;
		++ent;
	}

	Scr_IncTime();
	ent = g_entities;
	level.currentEntityThink = 0;

	while ( level.currentEntityThink < level.num_entities )
	{
		if ( ent->r.inuse )
		{
			if ( ent->tagInfo )
				G_RunFrameForEntity(ent->tagInfo->parent);

			G_RunFrameForEntity(ent);
		}

		++level.currentEntityThink;
		++ent;
	}

	level.currentEntityThink = -1;

	G_UpdateObjectiveToClients();
	G_UpdateHudElemsToClients();

	ent = g_entities;
	i = 0;

	while ( i < level.maxclients )
	{
		if ( ent->r.inuse )
			ClientEndFrame(ent);

		++i;
		++ent;
	}

	CheckTeamStatus();

	if ( g_oldVoting->current.boolean )
		CheckVote();

	G_UpdateTeamScoresForIntermission();

	if ( g_listEntity->current.boolean )
	{
		for ( i = 0; i < 1024; ++i )
		{
			Com_Printf("%4i: %s\n", i, SL_ConvertToString(g_entities[i].classname));
		}

		Dvar_SetBool(g_listEntity, 0);
	}

	if ( level.registerWeapons )
		SaveRegisteredWeapons();

	if ( level.bRegisterItems )
		SaveRegisteredItems();

	DebugDumpAnims();
}

void G_CreateDObj(DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle)
{
	Com_ServerDObjCreate(dobjModels, numModels, tree, handle);
}

void G_InitGame(int levelTime, int randomSeed, int restart, int registerDvars)
{
	char infostring[1024];
	char configstring[1024];
	int i;
	int j;

	Com_Printf("------- Game Initialization -------\n");
	Com_Printf("gamename: %s\n", GAMEVERSION);
	Com_Printf("gamedate: %s\n", __DATE__);

	Swap_Init();
	memset(&level, 0, sizeof(level));

	level.initializing = 1;
	level.time = levelTime;
	level.startTime = levelTime;
	level.currentEntityThink = -1;

	srand(randomSeed);
	Rand_Init(randomSeed);

	G_SetupWeaponDef();

	if ( !restart || !registerDvars )
		G_RegisterDvars();

	G_ProcessIPBans();

	level_bgs.GetXModel = SV_XModelGet;
	level_bgs.CreateDObj = G_CreateDObj;
	level_bgs.SafeDObjFree = Com_SafeServerDObjFree;
	level_bgs.AllocXAnim = Hunk_AllocXAnimServer;
	level_bgs.anim_user = 1;

	if ( *g_log->current.string )
	{
		if ( g_logSync->current.boolean )
			FS_FOpenFileByMode(g_log->current.string, &level.logFile, FS_APPEND_SYNC);
		else
			FS_FOpenFileByMode(g_log->current.string, &level.logFile, FS_APPEND);

		if ( level.logFile )
		{
			SV_GetServerinfo(infostring, 1024);
			G_LogPrintf("------------------------------------------------------------\n");
			G_LogPrintf("InitGame: %s\n", infostring);
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

	for ( i = 0; i < 1; ++i )
	{
		level.openScriptIOFileHandles[i] = -1;
		level.openScriptIOFileBuffers[i] = 0;

		memset(level.currentScriptIOLineMark, 0, sizeof(com_parse_mark_t));
	}

	Mantle_CreateAnims(Hunk_AllocXAnimServer);

	if ( !restart )
	{
		memset(&level_bgs.animData.animScriptData, 0, sizeof(animScriptData_t));
		level_bgs.animData.animScriptData.soundAlias = Com_FindSoundAlias;
		level_bgs.animData.animScriptData.playSoundAlias = G_AnimScriptSound;
		GScr_LoadScripts();
		BG_LoadAnim();
		BG_PostLoadAnim();
	}

	GScr_LoadConsts();

	SV_GetConfigstring(CS_MULTI_MAPWINNER, configstring, 1024);
	Info_SetValueForKey(configstring, "winner", "0");
	SV_SetConfigstring(CS_MULTI_MAPWINNER, configstring);

	memset(g_entities, 0, sizeof(g_entities));
	level.gentities = g_entities;

	level.maxclients = g_maxclients->current.integer;
	memset(g_clients, 0, sizeof(g_clients));
	level.clients = g_clients;

	for ( j = 0; j < level.maxclients; ++j )
		g_entities[j].client = &level.clients[j];

	level.num_entities = 72;
	level.firstFreeEnt = 0;
	level.lastFreeEnt = 0;

	SV_LocateGameData(level.gentities, level.num_entities, sizeof(gentity_s), &level.clients->ps, sizeof(gclient_s));

	G_ParseHitLocDmgTable();
	G_InitTurrets();
	G_SpawnEntitiesFromString();
	G_setfog("0");
	G_InitObjectives();
	Scr_FreeEntityList();

	Com_Printf("-----------------------------------\n");

	Scr_InitSystem();
	Scr_SetLoading(1);
	Scr_AllocGameVariable();
	G_LoadStructs();
	Scr_LoadGameType();
	Scr_LoadLevel();
	Scr_StartupGameType();

	for ( j = 0; j < 8; ++j )
		g_scr_data.playerCorpseInfo[j].entnum = -1;

	level.initializing = 0;

	SaveRegisteredWeapons();
	SaveRegisteredItems();
}

void G_FreeEntities()
{
	gentity_s *ent;
	int i;

	ent = g_entities;

	for ( i = 0; i < level.num_entities; ++i )
	{
		if ( ent->r.inuse )
			G_FreeEntity(ent);

		++ent;
	}

	if ( g_entities[1022].r.inuse )
		G_FreeEntity(&g_entities[1022]);

	level.num_entities = 0;
	level.firstFreeEnt = 0;
	level.lastFreeEnt = 0;
}

void G_FreeAnimTreeInstances()
{
	int i;
	int j;

	for ( i = 0; i < 64; ++i )
	{
		if ( level_bgs.clientinfo[i].pXAnimTree )
		{
			XAnimFreeTree(level_bgs.clientinfo[i].pXAnimTree, 0);
			level_bgs.clientinfo[i].pXAnimTree = 0;
		}
	}

	for ( j = 0; j < 8; ++j )
	{
		if ( g_scr_data.playerCorpseInfo[j].tree )
		{
			XAnimFreeTree(g_scr_data.playerCorpseInfo[j].tree, 0);
			g_scr_data.playerCorpseInfo[j].tree = 0;
		}
	}
}

void G_ShutdownGame(int freeScripts)
{
	int i;

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
		SV_FreeClientScriptPers();

	Scr_ShutdownSystem(1u, level.savePersist == 0);

	if ( freeScripts )
	{
		Mantle_ShutdownAnims();
		GScr_FreeScripts();
		Scr_FreeScripts();
		G_FreeAnimTreeInstances();
		Hunk_ClearLow(0);
	}

	for ( i = 0; i <= 0; ++i )
	{
		if ( level.openScriptIOFileBuffers[i] )
			Z_FreeInternal(level.openScriptIOFileBuffers[i]);

		level.openScriptIOFileBuffers[i] = 0;

		if ( level.openScriptIOFileHandles[i] >= 0 )
			FS_FCloseFile(level.openScriptIOFileHandles[i]);

		level.openScriptIOFileHandles[i] = -1;
	}
}

void G_RegisterDvars()
{
	g_cheats = Dvar_RegisterBool("sv_cheats", 0, DVAR_CHANGEABLE_RESET);
	Dvar_RegisterString("gamename", GAMEVERSION, DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterString("gamedate", __DATE__, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterString("sv_mapname", "", DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
	g_gametype = Dvar_RegisterString("g_gametype", "dm", DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	g_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, 64, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_LATCH | DVAR_CHANGEABLE_RESET);
	g_synchronousClients = Dvar_RegisterBool("g_synchronousClients", 0, DVAR_SYSTEMINFO | DVAR_CHANGEABLE_RESET);
	g_log = Dvar_RegisterString("g_log", "games_mp.log", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_logSync = Dvar_RegisterBool("g_logSync", 0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_password = Dvar_RegisterString("g_password", "", DVAR_CHANGEABLE_RESET);
	g_banIPs = Dvar_RegisterString("g_banIPs", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_speed = Dvar_RegisterInt("g_speed", 190, 0x80000000, 0x7FFFFFFF, DVAR_CHANGEABLE_RESET);
	g_gravity = Dvar_RegisterFloat("g_gravity", 800.0, 1.0, 3.4028235e38, DVAR_CHANGEABLE_RESET);
	g_knockback = Dvar_RegisterFloat("g_knockback", 1000.0, -3.4028235e38, 3.4028235e38, DVAR_CHANGEABLE_RESET);
	g_weaponAmmoPools = Dvar_RegisterBool("g_weaponAmmoPools", 0, DVAR_CHANGEABLE_RESET);
	g_maxDroppedWeapons = Dvar_RegisterInt("g_maxDroppedWeapons", 16, 1, 32, DVAR_CHANGEABLE_RESET);
	g_inactivity = Dvar_RegisterInt("g_inactivity", 0, 0, 0x7FFFFFFF, DVAR_CHANGEABLE_RESET);
	g_debugDamage = Dvar_RegisterBool("g_debugDamage", 0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_debugBullets = Dvar_RegisterInt("g_debugBullets", 0, -3, 6, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_motd = Dvar_RegisterString("g_motd", "", DVAR_CHANGEABLE_RESET);
	g_playerCollisionEjectSpeed = Dvar_RegisterInt("g_playerCollisionEjectSpeed", 25, 0, 32000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dropForwardSpeed = Dvar_RegisterFloat("g_dropForwardSpeed", 10.0, 0.0, 1000.0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dropUpSpeedBase = Dvar_RegisterFloat("g_dropUpSpeedBase", 10.0, 0.0, 1000.0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_dropUpSpeedRand = Dvar_RegisterFloat("g_dropUpSpeedRand", 5.0, 0.0, 1000.0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_clonePlayerMaxVelocity = Dvar_RegisterFloat("g_clonePlayerMaxVelocity", 80.0, 0.0, 3.4028235e38, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	voice_global = Dvar_RegisterBool("voice_global", 0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	voice_localEcho = Dvar_RegisterBool("voice_localEcho", 0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	voice_deadChat = Dvar_RegisterBool("voice_deadChat", 0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_allowVote = Dvar_RegisterBool("g_allowVote", 1, DVAR_CHANGEABLE_RESET);
	g_listEntity = Dvar_RegisterBool("g_listEntity", 0, DVAR_CHANGEABLE_RESET);
	g_deadChat = Dvar_RegisterBool("g_deadChat", 0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_voiceChatTalkingDuration = Dvar_RegisterInt("g_voiceChatTalkingDuration", 500, 0, 10000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_Allies = Dvar_RegisterString("g_ScoresBanner_Allies", "mpflag_american", DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_Axis = Dvar_RegisterString("g_ScoresBanner_Axis", "mpflag_german", DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_None = Dvar_RegisterString("g_ScoresBanner_None", "mpflag_none", DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_ScoresBanner_Spectators = Dvar_RegisterString("g_ScoresBanner_Spectators", "mpflag_spectator", DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_TeamName_Allies = Dvar_RegisterString("g_TeamName_Allies", "GAME_ALLIES", DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_TeamName_Axis = Dvar_RegisterString("g_TeamName_Axis", "GAME_AXIS", DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_TeamColor_Allies = Dvar_RegisterColor("g_TeamColor_Allies", 0.5, 0.5, 1.0, 1.0, DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_TeamColor_Axis = Dvar_RegisterColor("g_TeamColor_Axis", 1.0, 0.5, 0.5, 1.0, DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	g_smoothClients = Dvar_RegisterBool("g_smoothClients", 1, DVAR_CHANGEABLE_RESET);
	g_antilag = Dvar_RegisterBool("g_antilag", 1, DVAR_ARCHIVE | DVAR_SERVERINFO | DVAR_CHANGEABLE_RESET);
	g_oldVoting = Dvar_RegisterBool("g_oldVoting", 1, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_voteAbstainWeight = Dvar_RegisterFloat("g_voteAbstainWeight", 0.5, 0.0, 1.0, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	g_no_script_spam = Dvar_RegisterBool("g_no_script_spam", 0, DVAR_CHANGEABLE_RESET);
	g_debugLocDamage = Dvar_RegisterBool("g_debugLocDamage", 0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_friendlyfireDist = Dvar_RegisterFloat("g_friendlyfireDist", 256.0, 0.0, 15000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_friendlyNameDist = Dvar_RegisterFloat("g_friendlyNameDist", 15000.0, 0.0, 15000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_meleeRange = Dvar_RegisterFloat("player_meleeRange", 64.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_meleeWidth = Dvar_RegisterFloat("player_meleeWidth", 10.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	player_meleeHeight = Dvar_RegisterFloat("player_meleeHeight", 10.0, 0.0, 1000.0, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_dumpAnims = Dvar_RegisterInt("g_dumpAnims", -1, -1, 1023, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_useholdtime = Dvar_RegisterInt("g_useholdtime", 0, 0, 0x7FFFFFFF, DVAR_CHANGEABLE_RESET);
	g_useholdspawndelay = Dvar_RegisterInt("g_useholdspawndelay", 1, 0, 10, DVAR_ARCHIVE | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	g_mantleBlockEnable = Dvar_RegisterBool("g_mantleBlockEnable", 1, DVAR_CHANGEABLE_RESET);
	g_mantleBlockTimeBuffer = Dvar_RegisterInt("g_mantleBlockTimeBuffer", 500, 0, 60000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	BG_RegisterDvars();
#ifdef LIBCOD
	g_playerCollision = Dvar_RegisterBool("g_playerCollision", 1, DVAR_CHANGEABLE_RESET);
	g_playerEject = Dvar_RegisterBool("g_playerEject", 1, DVAR_CHANGEABLE_RESET);
#endif
	g_fixedWeaponSpreads = Dvar_RegisterBool("g_fixedWeaponSpreads", 0, DVAR_CHANGEABLE_RESET);
	g_dropGrenadeOnDeath = Dvar_RegisterBool("g_dropGrenadeOnDeath", 1, DVAR_CHANGEABLE_RESET);
}