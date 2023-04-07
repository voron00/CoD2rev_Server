#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
gentity_t g_entities[MAX_GENTITIES];
#endif

#ifdef TESTING_LIBRARY
#define g_clients ((gclient_t*)( 0x086F1480 ))
#else
gclient_t g_clients[MAX_CLIENTS];
#endif

dvar_t *g_cheats;
dvar_t *g_gametype;
dvar_t *g_maxclients;
dvar_t *g_synchronousClients;
dvar_t *g_log;
dvar_t *g_logSync;
dvar_t *g_password;
dvar_t *g_banIPs;
dvar_t *dedicated;
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
void QDECL G_LogPrintf( const char *fmt, ... )
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

void G_LocationalTrace(trace_t *results, const float *start, const float *end, int passentitynum, int contentmask, char *priorityMap)
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

gentity_s* G_FX_VisibilityTrace(trace_t *trace, const float *start, const float *end, int passentitynum, int contentmask, char *priorityMap, float *forwardAngles)
{
#ifndef DEDICATED
	float dist;
	float visible;
	vec3_t endPos;
#endif

	G_LocationalTrace(trace, start, end, passentitynum, contentmask, priorityMap);

	if ( trace->hitId > 1021 )
		return 0;

#ifdef DEDICATED
	return &g_entities[trace->hitId];
#else
	dist = trace->fraction * 15000.0;
	VectorMA(start, dist, forwardAngles, endPos);

	visible = SV_FX_GetVisibility(start, endPos);

	if ( visible >= 0.2 )
		return &g_entities[trace->hitId];
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

	if ( client2->sess.connected == CS_ZOMBIE )
		return 1;

	if ( client1->sess.connected == CS_ZOMBIE )
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

	qsort(level.sortedClients, level.numConnectedClients, 4, SortRanks);
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

void ExitLevel()
{
	int i;
	int j;

	Cbuf_ExecuteText(2, "map_rotate\n");

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

void G_RegisterDvars()
{
	g_cheats = Dvar_RegisterBool("sv_cheats", 0, 0x1000u);
	Dvar_RegisterString("gamename", "Call of Duty 2", 0x1044u);
	Dvar_RegisterString("gamedate", "Oct 24 2005", 0x1040u);
	Dvar_RegisterString("sv_mapname", "", 0x1044u);
	g_gametype = Dvar_RegisterString("g_gametype", "dm", 0x1024u);
	g_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, 64, 0x1025u);
	g_synchronousClients = Dvar_RegisterBool("g_synchronousClients", 0, 0x1008u);
	g_log = Dvar_RegisterString("g_log", "games_mp.log", 0x1001u);
	g_logSync = Dvar_RegisterBool("g_logSync", 0, 0x1001u);
	g_password = Dvar_RegisterString("g_password", "", 0x1000u);
	g_banIPs = Dvar_RegisterString("g_banIPs", "", 0x1001u);
	dedicated = Dvar_RegisterInt("dedicated", 0, 0, 2, 0x1020u);
	if ( dedicated->current.integer )
		Dvar_RegisterInt("dedicated", 0, 0, 2, 0x1040u);
	g_speed = Dvar_RegisterInt("g_speed", 190, 0x80000000, 0x7FFFFFFF, 0x1000u);
	g_gravity = Dvar_RegisterFloat("g_gravity", 800.0, 1.0, 3.4028235e38, 0x1000u);
	g_knockback = Dvar_RegisterFloat("g_knockback", 1000.0, -3.4028235e38, 3.4028235e38, 0x1000u);
	g_weaponAmmoPools = Dvar_RegisterBool("g_weaponAmmoPools", 0, 0x1000u);
	g_maxDroppedWeapons = Dvar_RegisterInt("g_maxDroppedWeapons", 16, 1, 32, 0x1000u);
	g_inactivity = Dvar_RegisterInt("g_inactivity", 0, 0, 0x7FFFFFFF, 0x1000u);
	g_debugDamage = Dvar_RegisterBool("g_debugDamage", 0, 0x1080u);
	g_debugBullets = Dvar_RegisterInt("g_debugBullets", 0, -3, 6, 0x1080u);
	g_motd = Dvar_RegisterString("g_motd", "", 0x1000u);
	g_playerCollisionEjectSpeed = Dvar_RegisterInt("g_playerCollisionEjectSpeed", 25, 0, 32000, 0x1001u);
	g_dropForwardSpeed = Dvar_RegisterFloat("g_dropForwardSpeed", 10.0, 0.0, 1000.0, 0x1001u);
	g_dropUpSpeedBase = Dvar_RegisterFloat("g_dropUpSpeedBase", 10.0, 0.0, 1000.0, 0x1001u);
	g_dropUpSpeedRand = Dvar_RegisterFloat("g_dropUpSpeedRand", 5.0, 0.0, 1000.0, 0x1001u);
	g_clonePlayerMaxVelocity = Dvar_RegisterFloat("g_clonePlayerMaxVelocity", 80.0, 0.0, 3.4028235e38, 0x1001u);
	voice_global = Dvar_RegisterBool("voice_global", 0, 0x1001u);
	voice_localEcho = Dvar_RegisterBool("voice_localEcho", 0, 0x1001u);
	voice_deadChat = Dvar_RegisterBool("voice_deadChat", 0, 0x1001u);
	g_allowVote = Dvar_RegisterBool("g_allowVote", 1, 0x1000u);
	g_listEntity = Dvar_RegisterBool("g_listEntity", 0, 0x1000u);
	g_deadChat = Dvar_RegisterBool("g_deadChat", 0, 0x1001u);
	g_voiceChatTalkingDuration = Dvar_RegisterInt("g_voiceChatTalkingDuration", 500, 0, 10000, 0x1001u);
	g_ScoresBanner_Allies = Dvar_RegisterString("g_ScoresBanner_Allies", "mpflag_american", 0x1100u);
	g_ScoresBanner_Axis = Dvar_RegisterString("g_ScoresBanner_Axis", "mpflag_german", 0x1100u);
	g_ScoresBanner_None = Dvar_RegisterString("g_ScoresBanner_None", "mpflag_none", 0x1100u);
	g_ScoresBanner_Spectators = Dvar_RegisterString("g_ScoresBanner_Spectators", "mpflag_spectator", 0x1100u);
	g_TeamName_Allies = Dvar_RegisterString("g_TeamName_Allies", "GAME_ALLIES", 0x1100u);
	g_TeamName_Axis = Dvar_RegisterString("g_TeamName_Axis", "GAME_AXIS", 0x1100u);
	g_TeamColor_Allies = Dvar_RegisterColor("g_TeamColor_Allies", 0.5, 0.5, 1.0, 1.0, 0x1100u);
	g_TeamColor_Axis = Dvar_RegisterColor("g_TeamColor_Axis", 1.0, 0.5, 0.5, 1.0, 0x1100u);
	g_smoothClients = Dvar_RegisterBool("g_smoothClients", 1, 0x1000u);
	g_antilag = Dvar_RegisterBool("g_antilag", 1, 0x1005u);
	g_oldVoting = Dvar_RegisterBool("g_oldVoting", 1, 0x1001u);
	g_voteAbstainWeight = Dvar_RegisterFloat("g_voteAbstainWeight", 0.5, 0.0, 1.0, 0x1001u);
	g_no_script_spam = Dvar_RegisterBool("g_no_script_spam", 0, 0x1000u);
	g_debugLocDamage = Dvar_RegisterBool("g_debugLocDamage", 0, 0x1080u);
	g_friendlyfireDist = Dvar_RegisterFloat("g_friendlyfireDist", 256.0, 0.0, 15000.0, 0x1080u);
	g_friendlyNameDist = Dvar_RegisterFloat("g_friendlyNameDist", 15000.0, 0.0, 15000.0, 0x1080u);
	player_meleeRange = Dvar_RegisterFloat("player_meleeRange", 64.0, 0.0, 1000.0, 0x1080u);
	player_meleeWidth = Dvar_RegisterFloat("player_meleeWidth", 10.0, 0.0, 1000.0, 0x1080u);
	player_meleeHeight = Dvar_RegisterFloat("player_meleeHeight", 10.0, 0.0, 1000.0, 0x1080u);
	g_dumpAnims = Dvar_RegisterInt("g_dumpAnims", -1, -1, 1023, 0x1080u);
	g_useholdtime = Dvar_RegisterInt("g_useholdtime", 0, 0, 0x7FFFFFFF, 0x1000u);
	g_useholdspawndelay = Dvar_RegisterInt("g_useholdspawndelay", 1, 0, 10, 0x1081u);
	g_mantleBlockTimeBuffer = Dvar_RegisterInt("g_mantleBlockTimeBuffer", 500, 0, 60000, 0x1080u);
	BG_RegisterDvars();
}