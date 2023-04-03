#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

/*
==================
ConcatArgs
==================
*/
char *ConcatArgs( int start )
{
	int i, c, tlen;
	static char line[MAX_STRING_CHARS];
	int len;
	char arg[MAX_STRING_CHARS];

	len = 0;
	c = SV_Cmd_Argc();
	for ( i = start ; i < c ; i++ )
	{
		SV_Cmd_ArgvBuffer( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 )
		{
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 )
		{
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

qboolean G_IsPlaying(gentity_s *ent)
{
	return ent->client->sess.sessionState == STATE_PLAYING;
}

extern dvar_t *g_cheats;
int CheatsOk(gentity_s *ent)
{
	if ( g_cheats->current.boolean )
	{
		if ( ent->health > 0 )
		{
			return 1;
		}
		else
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_MUSTBEALIVECOMMAND\"", 101));
			return 0;
		}
	}
	else
	{
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_CHEATSNOTENABLED\"", 101));
		return 0;
	}
}

void DeathmatchScoreboardMessage(gentity_s *ent)
{
	int ping;
	int clientNum;
	int numSorted;
	gclient_s *client;
	int len;
	int i;
	int stringlength;
	char string[1400];
	char entry[1024];

	string[0] = 0;
	stringlength = 0;

	numSorted = level.numConnectedClients;

	if ( level.numConnectedClients > MAX_CLIENTS )
		numSorted = MAX_CLIENTS;

	for ( i = 0; i < numSorted; ++i )
	{
		clientNum = level.sortedClients[i];
		client = &level.clients[clientNum];

		if ( client->sess.connected == CS_ZOMBIE )
		{
			Com_sprintf(
			    entry,
			    0x400u,
			    " %i %i %i %i %i",
			    level.sortedClients[i],
			    client->sess.score,
			    -1,
			    client->sess.deaths,
			    client->sess.statusIcon);
		}
		else
		{
			ping = SV_GetClientPing(clientNum);

			Com_sprintf(
			    entry,
			    0x400u,
			    " %i %i %i %i %i",
			    level.sortedClients[i],
			    client->sess.score,
			    ping,
			    client->sess.deaths,
			    client->sess.statusIcon);
		}

		len = strlen(entry);

		if ( stringlength + len > 1024 )
			break;

		strcpy(&string[stringlength], entry);
		stringlength += len;
	}

	SV_GameSendServerCommand(ent - g_entities, 1, va("%c %i %i %i%s", 98, i, level.teamScores[1], level.teamScores[2], string));
}

void Cmd_Score_f(gentity_s *ent)
{
	DeathmatchScoreboardMessage(ent);
}

void StopFollowing(gentity_s *ent)
{
	trace_t trace;
	vec3_t vMaxs;
	vec3_t vMins;
	vec3_t vEnd;
	vec3_t vUp;
	vec3_t vForward;
	vec3_t vPos;
	vec3_t vAngles;
	gclient_s *client;

	client = ent->client;
	client->sess.forceSpectatorClient = -1;
	client->spectatorClient = -1;

	if ( (client->ps.pm_flags & 0x400000) != 0 )
	{
		G_GetPlayerViewOrigin(ent, vPos);
		G_GetPlayerViewDirection(ent, vForward, 0, vUp);
		VectorCopy(client->ps.viewangles, vAngles);
		vAngles[0] = vAngles[0] + 15.0;
		VectorMA(vPos, -40.0, vForward, vEnd);
		VectorMA(vEnd, 10.0, vUp, vEnd);
		VectorSet(vMins, -8.0, -8.0, -8.0);
		VectorSet(vMaxs, 8.0, 8.0, 8.0);
		G_TraceCapsule(&trace, vPos, vMins, vMaxs, vEnd, 1023, 8454161);
		Vec3Lerp(vPos, vEnd, trace.fraction, vPos);
		client->ps.clientNum = ent - g_entities;
		client->ps.eFlags &= 0xFFFFFCFF;
		client->ps.viewlocked = 0;
		client->ps.viewlocked_entNum = 1023;
		client->ps.pm_flags &= 0xFFBFFFBF;
		client->ps.fWeaponPosFrac = 0.0;
		G_SetOrigin(ent, vPos);
		VectorCopy(vPos, client->ps.origin);
		SetClientViewAngle(ent, vAngles);
		client->ps.shellshockIndex = 0;
		client->ps.shellshockTime = 0;
		client->ps.shellshockDuration = 0;
	}
}

int Cmd_FollowCycle_f(gentity_s *ent, int dir)
{
	clientState_t cstate;
	playerState_s pstate;
	int startClientNum;
	int clientNum;

	if ( dir != 1 && dir != -1 )
		Com_Error(ERR_DROP, "Cmd_FollowCycle_f: bad dir %i", dir);

	if ( ent->client->sess.sessionState != STATE_SPECTATOR )
		return 0;

	if ( ent->client->sess.forceSpectatorClient >= 0 )
		return 0;

	clientNum = ent->client->spectatorClient;

	if ( clientNum < 0 )
		clientNum = 0;

	startClientNum = clientNum;

	do
	{
		clientNum += dir;
		if ( clientNum >= level.maxclients )
			clientNum = 0;

		if ( clientNum < 0 )
			clientNum = level.maxclients - 1;

		if ( SV_GetArchivedClientInfo(clientNum, &ent->client->sess.archiveTime, &pstate, &cstate)
		        && G_ClientCanSpectateTeam(ent->client, (team_t)cstate.team) )
		{
			ent->client->spectatorClient = clientNum;
			ent->client->sess.sessionState = STATE_SPECTATOR;
			return 1;
		}
	}
	while ( clientNum != startClientNum );

	return 0;
}

void Cmd_PrintEntities_f()
{
	if ( g_cheats->current.boolean )
		G_PrintEntities();
}

void Cmd_EntityCount_f()
{
	if ( g_cheats->current.boolean )
		Com_Printf("entity count = %i\n", level.num_entities);
}

void Cmd_SetViewpos_f(gentity_s *ent)
{
	int i;
	char buffer[1024];
	vec3_t angles;
	vec3_t origin;

	if ( g_cheats->current.boolean )
	{
		if ( SV_Cmd_Argc() == 5 )
		{
			VectorClear(angles);

			for ( i = 0; i < 3; ++i )
			{
				SV_Cmd_ArgvBuffer(i + 1, buffer, 1024);
				origin[i] = atof(buffer);
			}

			SV_Cmd_ArgvBuffer(4, buffer, 1024);

			angles[1] = atof(buffer);
			origin[2] = origin[2] - ent->client->ps.viewHeightCurrent;
			TeleportPlayer(ent, origin, angles);
		}
		else
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_USAGE\x15: setviewpos x y z yaw\"", 101));
		}
	}
	else
	{
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_CHEATSNOTENABLED\"", 101));
	}
}

void Cmd_GameCommand_f(gentity_s *ent)
{
	; // VoroN: This is a deprecated feature
}

extern dvar_t *g_oldVoting;
void Cmd_Vote_f(gentity_s *ent)
{
	char msg[64];

	if ( g_oldVoting->current.boolean )
	{
		if ( !level.voteTime )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_NOVOTEINPROGRESS\"", 101));
			return;
		}

		if ( (ent->client->ps.eFlags & 0x100000) != 0 )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTEALREADYCAST\"", 101));
			return;
		}

		if ( ent->client->sess.state.team == 3 )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_NOSPECTATORVOTE\"", 101));
			return;
		}

		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTECAST\"", 101));
		ent->client->ps.eFlags |= 0x100000u;
	}

	SV_Cmd_ArgvBuffer(1, msg, sizeof(msg));

	if ( msg[0] == 121 || msg[1] == 89 || msg[1] == 49 )
	{
		if ( g_oldVoting->current.boolean )
		{
			SV_SetConfigstring(0x11u, va("%i", ++level.voteYes));
		}
		else
		{
			Scr_PlayerVote(ent, "yes");
		}
	}
	else if ( g_oldVoting->current.boolean )
	{
		SV_SetConfigstring(0x12u, va("%i", ++level.voteNo));
	}
	else
	{
		Scr_PlayerVote(ent, "no");
	}
}

extern dvar_t *g_allowVote;
extern dvar_t *g_gametype;
void Cmd_CallVote_f(gentity_s *ent)
{
	char name[64];
	char arg3[256];
	char arg2[256];
	char arg1[264];
	dvar_t *mapname;
	int kicknum;
	const char *kickType;
	int i;
	int j;

	if ( !g_allowVote->current.boolean )
	{
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTINGNOTENABLED\"", 101));
		return;
	}

	if ( g_oldVoting->current.boolean )
	{
		if ( level.voteTime )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTEALREADYINPROGRESS\"", 101));
			return;
		}

		if ( ent->client->sess.teamVoteCount > 2 )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_MAXVOTESCALLED\"", 101));
			return;
		}

		if ( ent->client->sess.state.team == TEAM_SPECTATOR )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_NOSPECTATORCALLVOTE\"", 101));
			return;
		}
	}

	// make sure it is a valid command to vote on
	SV_Cmd_ArgvBuffer(1, arg1, sizeof(arg1));
	SV_Cmd_ArgvBuffer(2, arg2, sizeof(arg2));
	SV_Cmd_ArgvBuffer(3, arg3, sizeof(arg3));

	if ( strchr(arg1, 59) || strchr(arg2, 59) || strchr(arg3, 59) )
	{
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_INVALIDVOTESTRING\"", 101));
		return;
	}

	if ( !g_oldVoting->current.boolean )
	{
		Scr_VoteCalled(ent, arg1, arg2, arg3);
		return;
	}

	if ( I_stricmp(arg1, "map_restart")
	        && I_stricmp(arg1, "map_rotate")
	        && I_stricmp(arg1, "typemap")
	        && I_stricmp(arg1, "map")
	        && I_stricmp(arg1, "g_gametype")
	        && I_stricmp(arg1, "kick")
	        && I_stricmp(arg1, "clientkick")
	        && I_stricmp(arg1, "tempBanUser")
	        && I_stricmp(arg1, "tempBanClient") )
	{
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_INVALIDVOTESTRING\"", 101));
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_VOTECOMMANDSARE\x15 map_restart, map_rotate, map <mapname>, g_gametype <typename>, typemap <typename> <mapname>, kick <player>, clientkick <clientnum>, tempBanUser <player>, tempBanClient <clientNum>\"", 101));
		return;
	}

	if ( level.voteExecuteTime )
	{
		level.voteExecuteTime = 0;
		Cbuf_ExecuteText(2, va("%s\n", level.voteString));
	}

	if ( !I_stricmp(arg1, "typemap") )
	{
		if ( !Scr_IsValidGameType(arg2) )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_INVALIDGAMETYPE\"", 101));
			return;
		}

		if ( !I_stricmp(arg2, g_gametype->current.string) )
			arg2[0] = 0;

		SV_Cmd_ArgvBuffer(3, arg3, 256);

		if (strlen(arg3) > MAX_QPATH)
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"\x15Map name is too long\"", 101));
			return;
		}

		if ( !SV_MapExists(arg3) )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"\x15The server doesn't have that map\"", 101));
			return;
		}

		mapname = Dvar_RegisterString("mapname", "", 0x1044u);

		if ( !I_stricmp(arg3, mapname->current.string) )
			arg3[0] = 0;

		if ( !arg2[0] && !arg3[0] )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_TYPEMAP_NOCHANGE\"", 101));
			return;
		}

		if ( arg3[0] )
		{
			if ( arg2[0] )
				Com_sprintf(level.voteString, MAX_STRING_CHARS, "g_gametype %s; map %s", arg2, arg3);
			else
				Com_sprintf(level.voteString, MAX_STRING_CHARS, "map %s", arg3);

			if ( arg2[0] )
			{
				Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_GAMETYPE\x14%s\x15 - \x14GAME_VOTE_MAP\x15%s", Scr_GetGameTypeNameForScript(arg2), arg3);
			}
			else
			{
				Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_MAP\x15%s", arg3);
			}
		}
		else
		{
			Com_sprintf(level.voteString, MAX_STRING_CHARS, "g_gametype %s; map_restart", arg2);
			Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_GAMETYPE\x14%s", Scr_GetGameTypeNameForScript(arg2));
		}

		goto start_vote;
	}

	if ( !I_stricmp(arg1, "g_gametype") )
	{
		if ( !Scr_IsValidGameType(arg2) )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_INVALIDGAMETYPE\"", 101));
			return;
		}

		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s %s; map_restart", arg1, arg2);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_GAMETYPE\x14%s", Scr_GetGameTypeNameForScript(arg2));

		goto start_vote;
	}

	if ( !I_stricmp(arg1, "map_restart") )
	{
		Com_sprintf(level.voteString, MAX_STRING_CHARS, "fast_restart");
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_MAPRESTART");

		goto start_vote;
	}

	if ( !I_stricmp(arg1, "map_rotate") )
	{
		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s", arg1);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_NEXTMAP");

		goto start_vote;
	}

	if ( !I_stricmp(arg1, "map") )
	{
		if (strlen(arg2) > MAX_QPATH)
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"\x15Map name is too long\"", 101));
			return;
		}

		if ( !SV_MapExists(arg2) )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"\x15The server doesn't have that map\"", 101));
			return;
		}

		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s %s", arg1, arg2);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_MAP\x15%s", arg2);

		goto start_vote;
	}

	if ( !I_stricmp(arg1, "kick")
	        || !I_stricmp(arg1, "clientkick")
	        || !I_stricmp(arg1, "tempBanUser")
	        || !I_stricmp(arg1, "tempBanClient") )
	{
		kicknum = MAX_CLIENTS;

		if ( I_stricmp(arg1, "kick") && I_stricmp(arg1, "tempBanUser") )
		{
			kicknum = atoi(arg2);

			if ( (kicknum || !I_stricmp(arg2, "0"))
			        && kicknum < MAX_CLIENTS
			        && level.clients[kicknum].sess.connected == CS_CONNECTED )
			{
				I_strncpyz(name, level.clients[kicknum].sess.state.name, sizeof(name));
				I_CleanStr(name);
			}
			else
			{
				kicknum = MAX_CLIENTS;
			}
		}
		else
		{
			for ( j = 0; j < MAX_CLIENTS; ++j )
			{
				if ( level.clients[j].sess.connected == CS_CONNECTED )
				{
					I_strncpyz(name, level.clients[j].sess.state.name, sizeof(name));
					I_CleanStr(name);

					if ( !I_stricmp(name, arg2) )
						kicknum = j;
				}
			}
		}

		if ( kicknum == MAX_CLIENTS )
		{
			SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"GAME_CLIENTNOTONSERVER\"", 101));
			return;
		}

		if ( arg1[0] == 116 || arg1[0] == 84 )
			kickType = "tempBanClient";
		else
			kickType = "clientkick";

		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s \"%d\"", kickType, kicknum);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_KICK\x15(%i)%s", kicknum, level.clients[kicknum].sess.state.name);

		goto start_vote;
	}

start_vote:
	SV_GameSendServerCommand(-1, 0, va("%c \"GAME_CALLEDAVOTE\x15%s\"", 101, ent->client->sess.state.name));

	level.voteTime = level.time + 30000;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0; i < level.maxclients; ++i )
		level.clients[i].ps.eFlags &= ~0x100000u;

	ent->client->ps.eFlags |= 0x100000u;

	SV_SetConfigstring(0xFu, va("%i", level.voteTime));
	SV_SetConfigstring(0x10u, level.voteDisplayString);
	SV_SetConfigstring(0x11u, va("%i", level.voteYes));
	SV_SetConfigstring(0x12u, va("%i", level.voteNo));
}

void Cmd_Where_f(gentity_s *ent)
{
	if ( g_cheats->current.boolean )
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, vtos(ent->r.currentOrigin)));
}

void Cmd_Kill_f(gentity_s *ent)
{
	gclient_s *client;

	if ( ent->client->sess.sessionState == STATE_PLAYING )
	{
		ent->flags &= 0xFFFFFFFC;
		client = ent->client;
		ent->health = 0;
		client->ps.stats[0] = 0;
		player_die(ent, ent, ent, 100000, 12, 0, 0, HITLOC_NONE, 0);
	}
}

void Cmd_UFO_f(gentity_s *ent)
{
	const char *enabled;

	if ( CheatsOk(ent) )
	{
		if ( ent->client->ufo )
			enabled = "GAME_UFOOFF";
		else
			enabled = "GAME_UFOON";

		ent->client->ufo = ent->client->ufo == 0;
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, enabled));
	}
}

void Cmd_Noclip_f(gentity_s *ent)
{
	const char *enabled;

	if ( CheatsOk(ent) )
	{
		if ( ent->client->noclip )
			enabled = "GAME_NOCLIPOFF";
		else
			enabled = "GAME_NOCLIPON";

		ent->client->noclip = ent->client->noclip == 0;
		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, enabled));
	}
}

void Cmd_Notarget_f(gentity_s *ent)
{
	const char *enabled;

	if ( CheatsOk(ent) )
	{
		ent->flags ^= 4u;

		if ( (ent->flags & 4) != 0 )
			enabled = va("%c \"%s\"", 101, "GAME_NOTARGETON");
		else
			enabled = va("%c \"%s\"", 101, "GAME_NOTARGETOFF");

		SV_GameSendServerCommand(ent - g_entities, 0, enabled);
	}
}

void Cmd_DemiGod_f(gentity_s *ent)
{
	const char *enabled;

	if ( CheatsOk(ent) )
	{
		ent->flags ^= 2u;

		if ( (ent->flags & 2) != 0 )
			enabled = "GAME_DEMI_GODMODE_ON";
		else
			enabled = "GAME_DEMI_GODMODE_OFF";

		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, enabled));
	}
}

void Cmd_God_f(gentity_s *ent)
{
	const char *enabled;

	if ( CheatsOk(ent) )
	{
		ent->flags ^= 1u;

		if ( (ent->flags & 1) != 0 )
			enabled = "GAME_GODMODE_ON";
		else
			enabled = "GAME_GODMODE_OFF";

		SV_GameSendServerCommand(ent - g_entities, 0, va("%c \"%s\"", 101, enabled));
	}
}

void Cmd_Take_f(gentity_s *ent)
{
	UNIMPLEMENTED(__FUNCTION__);
}

void Cmd_Give_f(gentity_s *ent)
{
	UNIMPLEMENTED(__FUNCTION__);
}

void Cmd_MenuResponse_f(gentity_s *ent)
{
	char szResponse[MAX_STRING_CHARS];
	char szMenuName[MAX_STRING_CHARS];
	char szServerId[MAX_STRING_CHARS];
	unsigned int iMenuIndex;

	iMenuIndex = -1;

	if ( SV_Cmd_Argc() == 4 )
	{
		SV_Cmd_ArgvBuffer(1, szServerId, sizeof(szServerId));

		if ( atoi(szServerId) != Dvar_GetInt("sv_serverId") )
			return;

		SV_Cmd_ArgvBuffer(2, szMenuName, sizeof(szMenuName));
		iMenuIndex = atoi(szMenuName);

		if ( iMenuIndex < 32 )
			SV_GetConfigstring(iMenuIndex + 1246, szMenuName, sizeof(szMenuName));

		SV_Cmd_ArgvBuffer(3, szResponse, sizeof(szResponse));
	}
	else
	{
		szMenuName[0] = 0;
		strcpy(szResponse, "bad");
	}

	Scr_AddString(szResponse);
	Scr_AddString(szMenuName);
	Scr_Notify(ent, scr_const.menuresponse, 2u);
}

#define SAY_ALL     0
#define SAY_TEAM    1
#define SAY_BUDDY   2

extern dvar_t *g_deadChat;
void G_SayTo(gentity_s *ent, gentity_s *other, int mode, int color, const char *cleanname, const char *message)
{
	int type;

	if ( other
	        && other->r.inuse
	        && other->client
	        && other->client->sess.connected == CS_CONNECTED
	        && (mode != 1 || OnSameTeam(ent, other))
	        && (g_deadChat->current.boolean || G_IsPlaying(ent) || !G_IsPlaying(other)) )
	{
		if ( mode == 1 )
			type = 105;
		else
			type = 104;

		SV_GameSendServerCommand(other - g_entities, 0, va("%c \"\x15%s%c%c%s\"", type, cleanname, 94, color, message));
	}
}

extern dvar_t *dedicated;
void G_Say(gentity_s *ent, gentity_s *target, int mode, const char *chatText)
{
	const char *team_color;
	const char *pszTeamString;
	char message[128];
	char teamname[64];
	char cleanname[128];
	char name[64];
	int color;
	gentity_s *other;
	int i;

	if ( mode == 1 && ent->client->sess.state.team != TEAM_AXIS && ent->client->sess.state.team != TEAM_ALLIES )
		mode = 0;

	I_strncpyz(name, ent->client->sess.state.name, sizeof(name));
	I_CleanStr(name);

	if ( ent->client->sess.state.team == TEAM_AXIS )
	{
		team_color = "^9";
	}
	else if ( ent->client->sess.state.team == TEAM_ALLIES )
	{
		team_color = "^8";
	}
	else
	{
		team_color = "";
	}

	if ( ent->client->sess.state.team == TEAM_SPECTATOR )
	{
		Com_sprintf(teamname, sizeof(teamname), "\x15(\x14GAME_SPECTATOR\x15)");
	}
	else if ( ent->client->sess.sessionState == STATE_DEAD )
	{
		Com_sprintf(teamname, sizeof(teamname), "\x15%s(\x14GAME_DEAD\x15)", team_color);
	}
	else
	{
		Com_sprintf(teamname, sizeof(teamname), "\x15%s", team_color);
	}

	if ( mode == 1 )
	{
		if ( ent->client->sess.state.team == TEAM_AXIS )
			pszTeamString = "GAME_AXIS";
		else
			pszTeamString = "GAME_ALLIES";

		G_LogPrintf("sayteam;%d;%d;%s;%s\n", SV_GetGuid(ent->s.number), ent->s.number, name, chatText);
		Com_sprintf(cleanname, sizeof(cleanname), "%s(\x14%s\x15)%s%s: ", teamname, pszTeamString, name, "^7");
		color = 53;
	}
	else if ( mode == 2 )
	{
		Com_sprintf(cleanname,  sizeof(cleanname), "%s[%s]%s: ", teamname, name, "^7");
		color = 51;
	}
	else
	{
		G_LogPrintf("say;%d;%d;%s;%s\n", SV_GetGuid(ent->s.number), ent->s.number, name, chatText);
		Com_sprintf(cleanname,  sizeof(cleanname), "%s%s%s: ", teamname, name, "^7");
		color = 55;
	}

	I_strncpyz(message, chatText, sizeof(message));

	if ( target )
	{
		G_SayTo(ent, target, mode, color, cleanname, message);
	}
	else
	{
		if ( dedicated->current.integer )
			Com_Printf("%s%s\n", cleanname, message);

		for ( i = 0; i < level.maxclients; ++i )
		{
			other = &g_entities[i];
			G_SayTo(ent, other, mode, color, cleanname, message);
		}
	}
}

void Cmd_Tell_f(gentity_s *ent)
{
	int target_guid;
	int player_guid;
	char target_name[64];
	char player_name[64];
	char buffer[1024];
	char *msg;
	gentity_s *target;
	int i;

	if ( SV_Cmd_Argc() > 1 )
	{
		SV_Cmd_ArgvBuffer(1, buffer, sizeof(buffer));
		i = atoi(buffer);

		if ( i >= 0 && i < level.maxclients )
		{
			target = &g_entities[i];

			if ( target )
			{
				if ( target->r.inuse )
				{
					if ( target->client )
					{
						msg = ConcatArgs(2);
						I_strncpyz(player_name, ent->client->sess.state.name, sizeof(player_name));
						I_CleanStr(player_name);
						I_strncpyz(target_name, target->client->sess.state.name, sizeof(target_name));
						I_CleanStr(target_name);
						target_guid = SV_GetGuid(target->s.number);
						player_guid = SV_GetGuid(ent->s.number);
						G_LogPrintf(
						    "tell;%d;%d;%s;%d;%d;%s;%s\n",
						    player_guid,
						    ent->s.number,
						    player_name,
						    target_guid,
						    target->s.number,
						    target_name,
						    msg);
						G_Say(ent, target, 2, msg);
						G_Say(ent, ent, 2, msg);
					}
				}
			}
		}
	}
}

void Cmd_Say_f(gentity_s *ent, int mode, int arg0)
{
	const char *msg;

	if ( SV_Cmd_Argc() > 1 || arg0 )
	{
		if ( arg0 )
			msg = ConcatArgs(0);
		else
			msg = ConcatArgs(1);

		G_Say(ent, 0, mode, msg);
	}
}

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum )
{
	gentity_t *ent;
	char cmd[MAX_TOKEN_CHARS];

	ent = &g_entities[clientNum];

	if ( !ent->client )
	{
		return;     // not fully in game yet
	}

	SV_Cmd_ArgvBuffer( 0, cmd, sizeof( cmd ) );

	if ( Q_stricmp( cmd, "say" ) == 0 )
	{
		Cmd_Say_f( ent, SAY_ALL, qfalse );
		return;
	}

	if ( Q_stricmp( cmd, "say_team" ) == 0 )
	{
		Cmd_Say_f( ent, SAY_TEAM, qfalse );
		return;
	}

	if ( Q_stricmp( cmd, "tell" ) == 0 )
	{
		Cmd_Tell_f( ent );
		return;
	}

	if ( Q_stricmp( cmd, "score" ) == 0 )
	{
		Cmd_Score_f( ent );
		return;
	}

	if ( ent->client->ps.pm_type == PM_INTERMISSION )
		return;

	if ( Q_stricmp( cmd, "mr" ) == 0 )
	{
		Cmd_MenuResponse_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "give" ) == 0 )
	{
		Cmd_Give_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "take" ) == 0 )
	{
		Cmd_Take_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "god" ) == 0 )
	{
		Cmd_God_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "demigod" ) == 0 )
	{
		Cmd_DemiGod_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "notarget" ) == 0 )
	{
		Cmd_Notarget_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "noclip" ) == 0 )
	{
		Cmd_Noclip_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "ufo" ) == 0 )
	{
		Cmd_UFO_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "kill" ) == 0 )
	{
		Cmd_Kill_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "follownext" ) == 0 )
	{
		Cmd_FollowCycle_f(ent, 1);
		return;
	}

	if ( Q_stricmp( cmd, "followprev" ) == 0 )
	{
		Cmd_FollowCycle_f(ent, -1);
		return;
	}

	if ( Q_stricmp( cmd, "where" ) == 0 )
	{
		Cmd_Where_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "callvote" ) == 0 )
	{
		Cmd_CallVote_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "vote" ) == 0 )
	{
		Cmd_Vote_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "gc" ) == 0 )
	{
		Cmd_GameCommand_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "setviewpos" ) == 0 )
	{
		Cmd_SetViewpos_f(ent);
		return;
	}

	if ( Q_stricmp( cmd, "entitycount" ) == 0 )
	{
		Cmd_EntityCount_f();
		return;
	}

	if ( Q_stricmp( cmd, "printentities" ) == 0 )
	{
		Cmd_PrintEntities_f();
		return;
	}

	SV_GameSendServerCommand(clientNum, 0, va("%c \"GAME_UNKNOWNCLIENTCOMMAND\x15%s\"", 101, cmd));
}