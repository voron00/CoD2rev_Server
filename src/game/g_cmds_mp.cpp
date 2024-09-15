#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "g_shared.h"

/*
===============
G_IsPlaying
===============
*/
qboolean G_IsPlaying( gentity_t *ent )
{
	assert(ent->client);
	assert(ent->client->sess.connected != CON_DISCONNECTED);

	return ent->client->sess.sessionState == SESS_STATE_PLAYING;
}

/*
==================
SanitizeString

Remove case and control characters
==================
*/
void SanitizeString( char *in, char *out )
{
	while ( *in )
	{
		if ( *in == 27 )
		{
			in += 2;		// skip color code
			continue;
		}
		if ( *in < 32 )
		{
			in++;
			continue;
		}
		*out++ = tolower( *in++ );
	}

	*out = 0;
}

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

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent )
{
	SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, vtos(ent->r.currentOrigin)));
}

/*
==================
ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int ClientNumberFromString( gentity_t *to, char *s )
{
	gclient_t	*cl;
	int			idnum;
	char		s2[MAX_STRING_CHARS];
	char		n2[MAX_STRING_CHARS];

	// numeric values are just slot numbers
	if (s[0] >= '0' && s[0] <= '9')
	{
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients )
		{
			SV_GameSendServerCommand( to-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_BADCLIENTSLOT\"", 101, idnum));
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->sess.connected != CON_CONNECTED )
		{
			SV_GameSendServerCommand( to-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CLIENTNOTACTIVE\"", 101, idnum));
			return -1;
		}
		return idnum;
	}

	// check for a name match
	SanitizeString( s, s2 );
	for ( idnum=0,cl=level.clients ; idnum < level.maxclients ; idnum++,cl++ )
	{
		if ( cl->sess.connected != CON_CONNECTED )
		{
			continue;
		}
		SanitizeString( cl->sess.state.name, n2 );
		if ( !strcmp( n2, s2 ) )
		{
			return idnum;
		}
	}

	SV_GameSendServerCommand( to-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_USERNOTONSERVER\"", 101, s));
	return -1;
}

/*
==================
CheatsOk
==================
*/
qboolean CheatsOk( gentity_t *ent )
{
	if ( !g_cheats->current.boolean )
	{
		SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CHEATSNOTENABLED\"", 101));
		return qfalse;
	}
	if ( ent->health <= 0 )
	{
		SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_MUSTBEALIVECOMMAND\"", 101));
		return qfalse;
	}
	return qtrue;
}

/*
==================
DeathmatchScoreboardMessage
==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent )
{
	char entry[1024];
	char string[1400];
	int ping;
	int clientNum;
	int numSorted;
	gclient_t *client;
	int j;
	int i;
	int stringlength;

	// send the latest information on all clients
	string[0] = 0;
	stringlength = 0;

	numSorted = level.numConnectedClients;

	if ( level.numConnectedClients > MAX_CLIENTS )
		numSorted = MAX_CLIENTS;

	for ( i = 0; i < numSorted; i++ )
	{
		clientNum = level.sortedClients[i];
		client = &level.clients[clientNum];

		if ( client->sess.connected == CON_CONNECTING )
		{
			Com_sprintf(
			    entry,
			    sizeof(entry),
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
			    sizeof(entry),
			    " %i %i %i %i %i",
			    level.sortedClients[i],
			    client->sess.score,
			    ping,
			    client->sess.deaths,
			    client->sess.statusIcon);
		}

		j = strlen(entry);
		if ( stringlength + j > 1024 )
			break;
		strcpy(string + stringlength, entry);
		stringlength += j;
	}

	SV_GameSendServerCommand( ent - g_entities, SV_CMD_RELIABLE, va("%c %i %i %i%s", 98, i,
	                          level.teamScores[TEAM_AXIS], level.teamScores[TEAM_ALLIES],
	                          string ) );
}

/*
==================
Cmd_UFO_f
==================
*/
void Cmd_UFO_f( gentity_t *ent )
{
	const char *msg;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	if ( ent->client->ufo )
	{
		msg = "GAME_UFOOFF";
	}
	else
	{
		msg = "GAME_UFOON";
	}
	ent->client->ufo = !ent->client->ufo;

	SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, msg) );
}

/*
==================
Cmd_Noclip_f
==================
*/
void Cmd_Noclip_f( gentity_t *ent )
{
	const char *msg;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	if ( ent->client->noclip )
	{
		msg = "GAME_NOCLIPOFF";
	}
	else
	{
		msg = "GAME_NOCLIPON";
	}
	ent->client->noclip = !ent->client->noclip;

	SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, msg) );
}

/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent )
{
	const char *msg;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "GAME_NOTARGETOFF";
	else
		msg = "GAME_NOTARGETON";

	SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, msg) );
}

/*
==================
Cmd_DemiGod_f
==================
*/
void Cmd_DemiGod_f( gentity_t *ent )
{
	const char *msg;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	ent->flags ^= FL_DEMI_GODMODE;
	if (!(ent->flags & FL_DEMI_GODMODE) )
		msg = "GAME_DEMI_GODMODE_OFF";
	else
		msg = "GAME_DEMI_GODMODE_ON";

	SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, msg) );
}

/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f( gentity_t *ent )
{
	const char *msg;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "GAME_GODMODE_OFF";
	else
		msg = "GAME_GODMODE_ON";

	SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"%s\"", 101, msg) );
}

/*
==================
Cmd_Take_f
==================
*/
void Cmd_Take_f( gentity_t *ent )
{
	char *name;
	char *amt;
	int amount;
	qboolean take_all;
	int weapIndex;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	amt = ConcatArgs(2);
	amount = atoi(amt);
	name = ConcatArgs(1);

	if ( !name )
	{
		return;
	}

	if ( !name[0] )
	{
		return;
	}

	// take all?
	take_all = I_stricmp(name, "all") == 0;

	// take health
	if ( take_all || !I_strnicmp(name, "health", 6) )
	{
		if ( amount )
		{
			ent->health -= amount;

			if ( ent->health <= 0 )
			{
				ent->health = 1;
			}
		}
		else
		{
			ent->health = 1;
		}
	}
	// take weapons
	if ( take_all || !I_stricmp(name, "weapons") )
	{
		for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
		{
			BG_TakePlayerWeapon(&ent->client->ps, weapIndex);

			ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] = 0;
			ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] = 0;
		}

		if ( ent->client->ps.weapon )
		{
			ent->client->ps.weapon = WP_NONE;
			G_SelectWeaponIndex(ent - g_entities, ent->client->ps.weapon);
		}
	}
	// take ammo
	if ( take_all || !I_strnicmp(name, "ammo", 4) )
	{
		if ( amount )
		{
			if ( ent->client->ps.weapon )
			{
				weapIndex = ent->client->ps.weapon;
				ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] -= amount;

				if ( ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] < 0 )
				{
					ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] += ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)];
					ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] = 0;

					if ( ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] < 0 )
					{
						ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] = 0;
					}
				}
			}
		}
		else
		{
			for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
			{
				ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] = 0;
				ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] = 0;
			}
		}
	}
	// take allammo
	if ( take_all || !I_strnicmp(name, "allammo", 7) )
	{
		if ( amount )
		{
			for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
			{
				ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] -= amount;

				if ( ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] < 0 )
				{
					ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] += ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)];
					ent->client->ps.ammo[BG_AmmoForWeapon(weapIndex)] = 0;

					if ( ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] < 0 )
					{
						ent->client->ps.ammoclip[BG_ClipForWeapon(weapIndex)] = 0;
					}
				}
			}
		}
	}
}

/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent )
{
	DeathmatchScoreboardMessage( ent );
}

/*
==================
G_setfog
==================
*/
void G_setfog( const char *fogstring )
{
	float fTime;
	float r;
	float g;
	float b;
	float fDensity;
	float fFar;
	float fNear;

	SV_SetConfigstring(CS_FOGVARS, fogstring);

	level.fFogOpaqueDist = FLT_MAX;
	level.fFogOpaqueDistSqrd = FLT_MAX;

	if ( sscanf(fogstring, "%f %f %f %f %f %f %f", &fNear, &fFar, &fDensity, &r, &g, &b, &fTime) != 7 )
	{
		return;
	}

	if ( fDensity < 1.0f )
	{
		return;
	}

	level.fFogOpaqueDist = fFar - fNear + fNear;
	level.fFogOpaqueDistSqrd = Square(level.fFogOpaqueDist);
}

/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent )
{
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];
	int			i;

	if ( !g_cheats->current.boolean )
	{
		SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CHEATSNOTENABLED\"", 101));
		return;
	}
	if ( SV_Cmd_Argc() != 5 )
	{
		SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_USAGE\x15: setviewpos x y z yaw\"", 101) );
		return;
	}

	VectorClear( angles );
	for ( i = 0 ; i < 3 ; i++ )
	{
		SV_Cmd_ArgvBuffer( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	SV_Cmd_ArgvBuffer( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );
	origin[2] -= ent->client->ps.viewHeightCurrent;

	TeleportPlayer( ent, origin, angles );
}

/*
=================
Cmd_Fogswitch_f
=================
*/
void Cmd_Fogswitch_f( void )
{
	const char *fogstring;

	fogstring = ConcatArgs(1);
	G_setfog(fogstring);
}

/*
=================
Cmd_EntityCount_f
=================
*/
void Cmd_EntityCount_f( void )
{
	if ( !g_cheats->current.boolean )
	{
		return;
	}

	Com_Printf("entity count = %i\n", level.num_entities);
}

/*
=================
G_SayTo
=================
*/
static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message )
{
	if (!other)
	{
		return;
	}
	if (!other->r.inuse)
	{
		return;
	}
	if (!other->client)
	{
		return;
	}
	if ( other->client->sess.connected != CON_CONNECTED )
	{
		return;
	}
	if ( mode == SAY_TEAM  && !OnSameTeam(ent, other) )
	{
		return;
	}
	if ( !g_deadChat->current.boolean && !G_IsPlaying(ent) && G_IsPlaying(other) )
	{
		return;
	}

	if ( mode == SAY_TEAM )
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"\x15%s%c%c%s\"", 105, name, 94, color, message));
	else
		SV_GameSendServerCommand(other - g_entities, SV_CMD_CAN_IGNORE, va("%c \"\x15%s%c%c%s\"", 104, name, 94, color, message));
}

/*
=================
G_Say
=================
*/
void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText )
{
	const char *pszTeamColorString;
	const char *pszTeamString;
	// don't let text be too long for malicious reasons
	char text[MAX_SAY_TEXT];
	char name[128];
	char teamname[64];
	char cleanname[64];
	int color;
	gentity_t *other;
	int i;

	if ( mode == SAY_TEAM && ent->client->sess.state.team != TEAM_AXIS )
		mode = ent->client->sess.state.team == TEAM_ALLIES;

	I_strncpyz(cleanname, ent->client->sess.state.name, sizeof(cleanname));
	I_CleanStr(cleanname);

	if ( ent->client->sess.state.team == TEAM_AXIS )
	{
		pszTeamColorString = "^9";
	}
	else if ( ent->client->sess.state.team == TEAM_ALLIES )
	{
		pszTeamColorString = "^8";
	}
	else
	{
		pszTeamColorString = "";
	}

	if ( ent->client->sess.state.team == TEAM_SPECTATOR )
	{
		Com_sprintf(teamname, sizeof(teamname), "\x15(\x14GAME_SPECTATOR\x15)");
	}
	else if ( ent->client->sess.sessionState == SESS_STATE_DEAD )
	{
		Com_sprintf(teamname, sizeof(teamname), "\x15%s(\x14GAME_DEAD\x15)", pszTeamColorString);
	}
	else
	{
		Com_sprintf(teamname, sizeof(teamname), "\x15%s", pszTeamColorString);
	}

	if ( mode == SAY_TEAM )
	{
		if ( ent->client->sess.state.team == TEAM_AXIS )
			pszTeamString = "GAME_AXIS";
		else
			pszTeamString = "GAME_ALLIES";

		G_LogPrintf("sayteam;%d;%d;%s;%s\n", SV_GetGuid(ent->s.number), ent->s.number, cleanname, chatText);
		Com_sprintf(name, sizeof(name), "%s(\x14%s\x15)%s%s: ", teamname, pszTeamString, cleanname, "^7");
		color = '5';
	}
	else if ( mode == SAY_TELL )
	{
		Com_sprintf(name,  sizeof(name), "%s[%s]%s: ", teamname, cleanname, "^7");
		color = '3';
	}
	else
	{
		G_LogPrintf("say;%d;%d;%s;%s\n", SV_GetGuid(ent->s.number), ent->s.number, cleanname, chatText);
		Com_sprintf(name,  sizeof(name), "%s%s%s: ", teamname, cleanname, "^7");
		color = '7';
	}

	I_strncpyz(text, chatText, sizeof(text));

	if ( target )
	{
		G_SayTo(ent, target, mode, color, name, text);
		return;
	}

	// echo the text to the console
	if ( g_dedicated->current.integer )
	{
		Com_Printf("%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for ( i = 0; i < level.maxclients; i++ )
	{
		other = &g_entities[i];
		G_SayTo(ent, other, mode, color, name, text);
	}
}

static const char *gc_orders[] =
{
	"GAME_GC_HOLDYOURPOSITION",
	"GAME_GC_HOLDTHISPOSITION",
	"GAME_GC_COMEHERE",
	"GAME_GC_COVERME",
	"GAME_GC_GUARDLOCATION",
	"GAME_GC_SEARCHDESTROY",
	"GAME_GC_REPORT"
};

/*
==================
Cmd_GameCommand_f
==================
*/
void Cmd_GameCommand_f( gentity_t *ent )
{
	int		player;
	int		order;
	char	str[MAX_TOKEN_CHARS];

	SV_Cmd_ArgvBuffer( 1, str, sizeof( str ) );
	player = atoi( str );
	SV_Cmd_ArgvBuffer( 2, str, sizeof( str ) );
	order = atoi( str );

	if ( player < 0 || player >= MAX_CLIENTS )
	{
		return;
	}
	if ( order < 0 || order > sizeof(gc_orders)/sizeof(char *)-1 )
	{
		return;
	}
	G_Say( ent, &g_entities[player], SAY_TELL, gc_orders[order] );
	G_Say( ent, ent, SAY_TELL, gc_orders[order] );
}

/*
==================
Cmd_MenuResponse_f
==================
*/
void Cmd_MenuResponse_f( gentity_t *pEnt )
{
	char szResponse[MAX_TOKEN_CHARS];
	char szMenuName[MAX_TOKEN_CHARS];
	char szServerId[MAX_TOKEN_CHARS];
	int iMenuIndex;

	iMenuIndex = -1;

	if ( SV_Cmd_Argc() == 4 )
	{
		SV_Cmd_ArgvBuffer(1, szServerId, sizeof(szServerId));

		if ( atoi(szServerId) != Dvar_GetInt("sv_serverId") )
		{
			return;
		}

		SV_Cmd_ArgvBuffer(2, szMenuName, sizeof(szMenuName));
		iMenuIndex = atoi(szMenuName);

		if ( iMenuIndex < MAX_SCRIPT_MENUS )
		{
			SV_GetConfigstring(iMenuIndex + CS_SCRIPT_MENUS - 1, szMenuName, sizeof(szMenuName));
		}

		SV_Cmd_ArgvBuffer(3, szResponse, sizeof(szResponse));
	}
	else
	{
		szMenuName[0] = 0;
		strcpy(szResponse, "bad");
	}

	Scr_AddString(szResponse);
	Scr_AddString(szMenuName);

	Scr_Notify(pEnt, scr_const.menuresponse, 2);
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent )
{
	char msg[64];

	if ( g_oldVoting->current.boolean )
	{
		if ( !level.voteTime )
		{
			SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_NOVOTEINPROGRESS\"", 101) );
			return;
		}

		if ( ent->client->ps.eFlags & EF_VOTED )
		{
			SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEALREADYCAST\"", 101) );
			return;
		}

		if ( ent->client->sess.state.team == TEAM_SPECTATOR )
		{
			SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_NOSPECTATORVOTE\"", 101) );
			return;
		}

		SV_GameSendServerCommand( ent-g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTECAST\"", 101) );
		ent->client->ps.eFlags |= EF_VOTED;
	}

	SV_Cmd_ArgvBuffer( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' )
	{
		if ( g_oldVoting->current.boolean )
		{
			SV_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes++) );
		}
		else
		{
			Scr_PlayerVote(ent, "yes");
		}
	}
	else if ( g_oldVoting->current.boolean )
	{
		SV_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo++) );
	}
	else
	{
		Scr_PlayerVote(ent, "no");
	}

	// a majority will be determined in CheckVote, which will also account
	// for players entering or leaving
}

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent )
{
	char cleanName[64];
	char arg1[256];
	char arg2[256];
	char arg3[256];
	dvar_t *mapname;
	int kicknum;
	int i;

	if ( !g_allowVote->current.boolean )
	{
		SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTINGNOTENABLED\"", 101));
		return;
	}

	if ( g_oldVoting->current.boolean )
	{
		if ( level.voteTime )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTEALREADYINPROGRESS\"", 101));
			return;
		}

		if ( ent->client->sess.teamVoteCount > 2 )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_MAXVOTESCALLED\"", 101));
			return;
		}

		if ( ent->client->sess.state.team == TEAM_SPECTATOR )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_NOSPECTATORCALLVOTE\"", 101));
			return;
		}
	}

	// make sure it is a valid command to vote on
	SV_Cmd_ArgvBuffer(1, arg1, sizeof(arg1));
	SV_Cmd_ArgvBuffer(2, arg2, sizeof(arg2));
	SV_Cmd_ArgvBuffer(3, arg3, sizeof(arg3));

	if ( strchr( arg1, ';' ) || strchr( arg2, ';' ) || strchr( arg3, ';' ) )
	{
		SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_INVALIDVOTESTRING\"", 101));
		return;
	}

	if ( !g_oldVoting->current.boolean )
	{
		Scr_VoteCalled(ent, arg1, arg2, arg3);
		return;
	}

	if ( !Q_stricmp( arg1, "map_restart" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "map_rotate" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "typemap" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "map" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "g_gametype" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "kick" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "clientkick" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "tempBanUser" ) )
	{
	}
	else if ( !Q_stricmp( arg1, "tempBanClient" ) )
	{
	}
	else
	{
		SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_INVALIDVOTESTRING\"", 101));
		SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_VOTECOMMANDSARE\x15 map_restart, map_rotate, map <mapname>, g_gametype <typename>, typemap <typename> <mapname>, kick <player>, clientkick <clientnum>, tempBanUser <player>, tempBanClient <clientNum>\"", 101));
		return;
	}

	// if there is still a vote to be executed
	if ( level.voteExecuteTime )
	{
		level.voteExecuteTime = 0;
		Cbuf_ExecuteText(EXEC_APPEND, va("%s\n", level.voteString));
	}

	if ( !I_stricmp(arg1, "typemap") )
	{
		if ( !Scr_IsValidGameType(arg2) )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_INVALIDGAMETYPE\"", 101));
			return;
		}

		if ( !I_stricmp(arg2, g_gametype->current.string) )
		{
			arg2[0] = 0;
		}

		SV_Cmd_ArgvBuffer(3, arg3, sizeof(arg3));

		if ( strlen(arg3) > MAX_QPATH )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"\x15Map name is too long\"", 101));
			return;
		}

		if ( !SV_MapExists(arg3) )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"\x15The server doesn't have that map\"", 101));
			return;
		}

		mapname = Dvar_RegisterString("mapname", "", DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);

		if ( !I_stricmp(arg3, mapname->current.string) )
		{
			arg3[0] = 0;
		}

		if ( !arg2[0] && !arg3[0] )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_TYPEMAP_NOCHANGE\"", 101));
			return;
		}

		if ( arg3[0] )
		{
			if ( arg2[0] )
				Com_sprintf(level.voteString, MAX_STRING_CHARS, "g_gametype %s; map %s", arg2, arg3);
			else
				Com_sprintf(level.voteString, MAX_STRING_CHARS, "map %s", arg3);

			if ( arg2[0] )
				Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_GAMETYPE\x14%s\x15 - \x14GAME_VOTE_MAP\x15%s", Scr_GetGameTypeNameForScript(arg2), arg3);
			else
				Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_MAP\x15%s", arg3);
		}
		else
		{
			Com_sprintf(level.voteString, MAX_STRING_CHARS, "g_gametype %s; map_restart", arg2);
			Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_GAMETYPE\x14%s", Scr_GetGameTypeNameForScript(arg2));
		}
	}
	else if ( !I_stricmp(arg1, "g_gametype") )
	{
		// special case for g_gametype, check for bad values
		if ( !Scr_IsValidGameType(arg2) )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_INVALIDGAMETYPE\"", 101));
			return;
		}

		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s %s; map_restart", arg1, arg2);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_GAMETYPE\x14%s", Scr_GetGameTypeNameForScript(arg2));
	}
	else if ( !I_stricmp(arg1, "map_restart") )
	{
		Com_sprintf(level.voteString, MAX_STRING_CHARS, "fast_restart");
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_MAPRESTART");
	}
	else if ( !I_stricmp(arg1, "map_rotate") )
	{
		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s", arg1);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_NEXTMAP");
	}
	else if ( !I_stricmp(arg1, "map") )
	{
		// special case for map changes, we want to reset the nextmap setting
		// this allows a player to change maps, but not upset the map rotation
		if ( strlen(arg2) > MAX_QPATH )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"\x15Map name is too long\"", 101));
			return;
		}

		if ( !SV_MapExists(arg2) )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"\x15The server doesn't have that map\"", 101));
			return;
		}

		Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s %s", arg1, arg2);
		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_MAP\x15%s", arg2);
	}
	else if ( !I_stricmp(arg1, "kick") || !I_stricmp(arg1, "clientkick") || !I_stricmp(arg1, "tempBanUser") || !I_stricmp(arg1, "tempBanClient") )
	{
		kicknum = MAX_CLIENTS;

		if ( I_stricmp(arg1, "kick") && I_stricmp(arg1, "tempBanUser") )
		{
			kicknum = atoi(arg2);

			if ( (kicknum || !I_stricmp(arg2, "0")) && kicknum < MAX_CLIENTS && level.clients[kicknum].sess.connected == CON_CONNECTED )
			{
				I_strncpyz(cleanName, level.clients[kicknum].sess.state.name, sizeof(cleanName));
				I_CleanStr(cleanName);
			}
			else
			{
				kicknum = MAX_CLIENTS;
			}
		}
		else
		{
			for ( i = 0; i < MAX_CLIENTS; i++ )
			{
				if ( level.clients[i].sess.connected == CON_CONNECTED )
				{
					I_strncpyz(cleanName, level.clients[i].sess.state.name, sizeof(cleanName));
					I_CleanStr(cleanName);

					if ( !I_stricmp(cleanName, arg2) )
					{
						kicknum = i;
					}
				}
			}
		}

		if ( kicknum == MAX_CLIENTS )
		{
			SV_GameSendServerCommand(ent - g_entities, SV_CMD_CAN_IGNORE, va("%c \"GAME_CLIENTNOTONSERVER\"", 101));
			return;
		}

		if ( arg1[0] == 't' || arg1[0] == 'T' )
			Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s \"%d\"", "tempBanClient", kicknum);
		else
			Com_sprintf(level.voteString, MAX_STRING_CHARS, "%s \"%d\"", "clientkick", kicknum);

		Com_sprintf(level.voteDisplayString, MAX_STRING_CHARS, "GAME_VOTE_KICK\x15(%i)%s", kicknum, level.clients[kicknum].sess.state.name);
	}

	SV_GameSendServerCommand(-1, SV_CMD_CAN_IGNORE, va("%c \"GAME_CALLEDAVOTE\x15%s\"", 101, ent->client->sess.state.name));

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time + 30000;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0; i < level.maxclients; i++ )
	{
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	SV_SetConfigstring(CS_VOTE_TIME, va("%i", level.voteTime));
	SV_SetConfigstring(CS_VOTE_STRING, level.voteDisplayString);
	SV_SetConfigstring(CS_VOTE_YES, va("%i", level.voteYes));
	SV_SetConfigstring(CS_VOTE_NO, va("%i", level.voteNo));
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent )
{
	assert(ent->client);
	assert(ent->client->sess.connected != CON_DISCONNECTED);

	if ( ent->client->sess.sessionState != SESS_STATE_PLAYING )
	{
		return;
	}

	ent->flags &= ~( FL_GODMODE | FL_DEMI_GODMODE );
	ent->health = 0;
	ent->client->ps.stats[STAT_HEALTH] = 0;

	player_die(ent, ent, ent, 100000, MOD_SUICIDE, WP_NONE, NULL, HITLOC_NONE, 0);
}

/*
=================
StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void StopFollowing( gentity_t *ent )
{
	trace_t trace;
	vec3_t vMaxs;
	vec3_t vMins;
	vec3_t vEnd;
	vec3_t vUp;
	vec3_t vForward;
	vec3_t vPos;
	vec3_t vAngles;
	gclient_t *client;

	client = ent->client;
	assert(client);

	client->sess.forceSpectatorClient = -1;
	client->spectatorClient = -1;

	if ( !(client->ps.pm_flags & PMF_FOLLOW) )
	{
		return;
	}

	G_GetPlayerViewOrigin(ent, vPos);
	G_GetPlayerViewDirection(ent, vForward, NULL, vUp);

	VectorCopy(client->ps.viewangles, vAngles);
	vAngles[0] += 15;

	VectorMA(vPos, -40, vForward, vEnd);
	VectorMA(vEnd, 10, vUp, vEnd);

	VectorSet(vMins, -8, -8, -8);
	VectorSet(vMaxs, 8, 8, 8);

	G_TraceCapsule(&trace, vPos, vMins, vMaxs, vEnd, ENTITYNUM_NONE, MASK_PLAYERSOLID & ~CONTENTS_BODY);
	Vec3Lerp(vPos, vEnd, trace.fraction, vPos);

	ent->client->ps.clientNum = ent - g_entities;

	client->ps.eFlags &= ~EF_TURRET_ACTIVE;

	client->ps.viewlocked = 0;
	client->ps.viewlocked_entNum = ENTITYNUM_NONE;

	client->ps.pm_flags &= ~( PMF_ADS | PMF_FOLLOW );
	client->ps.fWeaponPosFrac = 0;

	G_SetOrigin(ent, vPos);
	VectorCopy(vPos, client->ps.origin);

	SetClientViewAngle(ent, vAngles);

	client->ps.shellshockIndex = 0;
	client->ps.shellshockTime = 0;
	client->ps.shellshockDuration = 0;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
qboolean Cmd_FollowCycle_f( gentity_t *ent, int dir )
{
	int		clientnum;
	int		original;
	clientState_t cs;
	playerState_t ps;

	if ( dir != 1 && dir != -1 )
	{
		Com_Error( ERR_DROP, "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	if ( ent->client->sess.sessionState != SESS_STATE_SPECTATOR )
	{
		return qfalse;
	}

	if ( ent->client->sess.forceSpectatorClient >= 0 )
	{
		return qfalse;
	}

	clientnum = ent->client->spectatorClient;
	if ( clientnum < 0 )
		clientnum = 0;
	original = clientnum;
	do
	{
		clientnum += dir;
		if ( clientnum >= level.maxclients )
		{
			clientnum = 0;
		}
		if ( clientnum < 0 )
		{
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if ( !SV_GetArchivedClientInfo(clientnum, &ent->client->sess.archiveTime, &ps, &cs) )
		{
			continue;
		}

		// can't follow another spectator
		if ( !G_ClientCanSpectateTeam(ent->client, cs.team) )
		{
			continue;
		}

		// this is good, we can use it
		ent->client->spectatorClient = clientnum;
		ent->client->sess.sessionState = SESS_STATE_SPECTATOR;
		return qtrue;
	}
	while ( clientnum != original );

	// leave it where it was
	return qfalse;
}

/*
=================
Cmd_Give_f
=================
*/
void Cmd_Give_f( gentity_t *ent )
{
	char *name;
	char *amt;
	int amount;
	qboolean give_all;
	int weapIndex;
	int oldWeapon;
	gitem_t *it;
	gentity_t *it_ent;

	if ( !CheatsOk( ent ) )
	{
		return;
	}

	amt = ConcatArgs(2);
	amount = atoi(amt);
	name = ConcatArgs(1);

	if ( !name )
	{
		return;
	}

	if ( !name[0] )
	{
		return;
	}

	// give all?
	give_all = I_stricmp(name, "all") == 0;

	// give health
	if ( give_all || !I_strnicmp(name, "health", 6) )
	{
		if ( amount )
		{
			ent->health += amount;
		}
		else
		{
			ent->health = ent->client->ps.stats[STAT_MAX_HEALTH];
		}
	}
	// give weapons
	if ( give_all || !I_stricmp(name, "weapons") )
	{
		level.initializing = qtrue;

		oldWeapon = ent->client->ps.weaponslots[SLOT_PRIMARYB];

		BG_TakePlayerWeapon(&ent->client->ps, ent->client->ps.weaponslots[SLOT_PRIMARY]);
		BG_TakePlayerWeapon(&ent->client->ps, ent->client->ps.weaponslots[SLOT_PRIMARYB]);

		for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
		{
			if ( !BG_DoesWeaponRequireSlot(weapIndex) || weapIndex > oldWeapon && BG_IsAnyEmptyPrimaryWeaponSlot(&ent->client->ps) )
			{
				G_GivePlayerWeapon(&ent->client->ps, weapIndex);
			}
		}

		for ( weapIndex = 1; BG_IsAnyEmptyPrimaryWeaponSlot(&ent->client->ps) && weapIndex <= BG_GetNumWeapons(); weapIndex++ )
		{
			if ( BG_DoesWeaponRequireSlot(weapIndex) )
			{
				G_GivePlayerWeapon(&ent->client->ps, weapIndex);
			}
		}

		level.initializing = qfalse;
	}
	// give ammo
	if ( give_all || !I_strnicmp(name, "ammo", 4) )
	{
		if ( amount )
		{
			if ( ent->client->ps.weapon )
			{
				Add_Ammo(ent, ent->client->ps.weapon, amount, qtrue);
			}
		}
		else
		{
			for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
			{
				Add_Ammo(ent, weapIndex, 998, qtrue);
			}
		}
	}
	// give allammo
	if ( give_all || !I_strnicmp(name, "allammo", 7) )
	{
		if ( amount )
		{
			for ( weapIndex = 1; weapIndex <= BG_GetNumWeapons(); weapIndex++ )
			{
				Add_Ammo(ent, weapIndex, amount, qtrue);
			}
		}
	}
	// give item
	if ( !give_all )
	{
		it = G_FindItem(name);

		if ( it )
		{
			level.initializing = qtrue;

			it_ent = G_Spawn();
			VectorCopy(ent->r.currentOrigin, it_ent->r.currentOrigin);

			G_GetItemClassname(it, &it_ent->classname);
			G_SpawnItem(it_ent, it);

			it_ent->active = qtrue;
			Touch_Item(it_ent, ent, qtrue);
			it_ent->active = qfalse;

			if ( it_ent->r.inuse )
			{
				G_FreeEntity(it_ent);
			}

			level.initializing = qfalse;
		}
	}
}

/*
=================
Cmd_PrintEntities_f
=================
*/
void Cmd_PrintEntities_f( void )
{
	if ( !g_cheats->current.boolean )
	{
		return;
	}

	G_PrintEntities();
}

/*
=================
Cmd_Tell_f
=================
*/
void Cmd_Tell_f( gentity_s *ent )
{
	int target_guid;
	int guid;
	char target_cleanname[64];
	char cleanname[64];
	char arg[MAX_TOKEN_CHARS];
	char *p;
	gentity_t *target;
	int targetNum;

	if ( SV_Cmd_Argc() < 2 )
	{
		return;
	}

	SV_Cmd_ArgvBuffer(1, arg, sizeof(arg));

	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients )
	{
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->r.inuse || !target->client )
	{
		return;
	}

	p = ConcatArgs(2);

	I_strncpyz(cleanname, ent->client->sess.state.name, sizeof(cleanname));
	I_CleanStr(cleanname);

	I_strncpyz(target_cleanname, target->client->sess.state.name, sizeof(target_cleanname));
	I_CleanStr(target_cleanname);

	target_guid = SV_GetGuid(target->s.number);
	guid = SV_GetGuid(ent->s.number);

	G_LogPrintf("tell;%d;%d;%s;%d;%d;%s;%s\n", guid, ent->s.number, cleanname, target_guid, target->s.number, target_cleanname, p);

	G_Say(ent, target, SAY_TELL, p);
	G_Say(ent, ent, SAY_TELL, p);
}

/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 )
{
	char *p;

	if ( SV_Cmd_Argc() < 2 && !arg0 )
	{
		return;
	}

	if (arg0)
	{
		p = ConcatArgs( 0 );
	}
	else
	{
		p = ConcatArgs( 1 );
	}

	G_Say( ent, NULL, mode, p );
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

	// ignore all other commands when at intermission
	if ( ent->client->ps.pm_type == PM_INTERMISSION )
	{
		return;
	}

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

	SV_GameSendServerCommand(clientNum, SV_CMD_CAN_IGNORE, va("%c \"GAME_UNKNOWNCLIENTCOMMAND\x15%s\"", 101, cmd));
}
