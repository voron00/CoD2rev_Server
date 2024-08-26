#include "../qcommon/qcommon.h"
#include "../qcommon/sys_thread.h"

/*
================
SV_FreeClientScriptId
================
*/
void SV_FreeClientScriptId( client_t *cl )
{
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);
	assert(cl->scriptId);

	Scr_FreeValue(cl->scriptId);
	cl->scriptId = 0;
}

/*
===============
SV_ExpandNewlines

Converts newlines to "\n" so a line prints nicer
===============
*/
char *SV_ExpandNewlines( char *in )
{
	static	char	string[1024];
	int		l;

	l = 0;
	while ( *in && l < sizeof(string) - 3 )
	{
		if ( *in == '\n' )
		{
			string[l++] = '\\';
			string[l++] = 'n';
		}
		else
		{
			string[l++] = *in;
		}
		in++;
	}
	string[l] = 0;

	return string;
}

/*
===================
SV_CalcPings

Updates the cl->ping variables
===================
*/
void SV_CalcPings( void )
{
	int i, j;
	client_t    *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		cl = &svs.clients[i];

		if ( cl->state != CS_ACTIVE )
		{
			cl->ping = 999;
			continue;
		}
		if ( !cl->gentity )
		{
			cl->ping = 999;
			continue;
		}

		total = 0;
		count = 0;
		for ( j = 0 ; j < PACKET_BACKUP ; j++ )
		{
			if ( cl->frames[j].messageAcked <= 0 )
			{
				continue;
			}
			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}
		if ( !count )
		{
			cl->ping = 999;
		}
		else
		{
			cl->ping = total / count;
			if ( cl->ping > 999 )
			{
				cl->ping = 999;
			}
		}
	}
}

/*
==================
SV_CheckTimeouts

If a packet has not been received from a client for timeout->integer
seconds, drop the conneciton.  Server time is used instead of
realtime to avoid dropping the local client while debugging.

When a client is normally dropped, the client_t goes into a zombie state
for a few seconds to make sure any final reliable message gets resent
if necessary
==================
*/
void SV_CheckTimeouts( void )
{
	int i;
	client_t    *cl;
	int droppoint;
	int zombiepoint;

	droppoint = svs.time - 1000 * sv_timeout->current.integer;
	zombiepoint = svs.time - 1000 * sv_zombietime->current.integer;

	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		// message times may be wrong across a changelevel
		if ( cl->lastPacketTime > svs.time )
		{
			cl->lastPacketTime = svs.time;
		}

		if (cl->bIsTestClient)
		{
			continue;
		}

		if ( cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint )
		{
			// using the client id cause the cl->name is empty at this point
			Com_DPrintf( "Going from CS_ZOMBIE to CS_FREE for %s\n", cl->name );
			cl->state = CS_FREE;    // can now be reused
			continue;
		}

		if ( cl->state >= CS_CONNECTED && cl->lastPacketTime < droppoint )
		{
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 )
			{
				SV_DropClient( cl, "EXE_TIMEDOUT" );
				cl->state = CS_FREE;    // don't bother with zombie state
			}
		}
		else
		{
			cl->timeoutCount = 0;
		}
	}
}

/*
===================
SV_CullIgnorableServerCommands
===================
*/
void SV_CullIgnorableServerCommands( client_t *client )
{
	svscmd_info_t *svscmd;
	int from;
	int fromIndex;
	int to;

	to = client->reliableSent + 1;

	for ( from = to; from <= client->reliableSequence; ++from )
	{
		fromIndex = from & (MAX_RELIABLE_COMMANDS - 1);

		if ( client->reliableCommandInfo[fromIndex].type )
		{
			if ( (to & (MAX_RELIABLE_COMMANDS - 1)) != fromIndex )
			{
				svscmd = &client->reliableCommandInfo[to & (MAX_RELIABLE_COMMANDS - 1)];
				*svscmd = client->reliableCommandInfo[fromIndex];
			}

			++to;
		}
	}

	client->reliableSequence = to - 1;
}

/*
===================
SV_IsFirstTokenEqual
===================
*/
qboolean SV_IsFirstTokenEqual( const char *str1, const char *str2 )
{
	while ( 1 )
	{
		if ( !(*str1) || !(*str2) )
			break;

		if ( *str1 == ' ' || *str2 == ' ' )
			break;

		if ( *str1 != *str2 )
			return qfalse;

		str1++;
		str2++;
	}

	if ( *str1 && *str1 != ' ')
	{
		return qfalse;
	}

	if ( *str2 && *str2 != ' ')
	{
		return qfalse;
	}

	return qtrue;
}

/*
===================
SV_CanReplaceServerCommand
===================
*/
int SV_CanReplaceServerCommand( client_t *client, const char *cmd )
{
	int i;
	int index;

	for( i = client->reliableSent + 1; i <= client->reliableSequence; ++i)
	{
		index = i & (MAX_RELIABLE_COMMANDS - 1);

		if ( client->reliableCommandInfo[index].type == SV_CMD_CAN_IGNORE )
			continue;

		assert(client->reliableCommandInfo[index].type == SV_CMD_RELIABLE);

		if ( client->reliableCommandInfo[index].cmd[0] != cmd[0] )
			continue;

		if ( cmd[0] >= 'x' && cmd[0] <= 'z' )
			continue;

		if ( !strcmp(cmd + 1, &client->reliableCommandInfo[index].cmd[1]) )
			return i;

		switch ( cmd[0] )
		{
		case 'C':
		case 'D':
		case 'a':
		case 'b':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 't':
			return i;
		case 'd':
		case 'v':
			assert(cmd[1] == ' ');
			if ( !SV_IsFirstTokenEqual(cmd + 2, &client->reliableCommandInfo[index].cmd[2]) )
				continue;
			return i;
		default:
			continue;;
		}
	}

	return -1;
}

/*
======================
SV_AddServerCommand

The given command will be transmitted to the client, and is guaranteed to
not have future snapshot_t executed before it is executed
======================
*/
void SV_AddServerCommand( client_t *client, int type, const char *cmd )
{
	int from;
	int to;
	int index;
	int i;
	svscmd_info_t *svscmd;

	if ( client->bIsTestClient )
	{
		return;
	}

	if ( client->reliableSequence - client->reliableAcknowledge >= MAX_RELIABLE_COMMANDS / 2 || client->state != CS_ACTIVE)
	{
		SV_CullIgnorableServerCommands(client);

		if ( type == SV_CMD_CAN_IGNORE )
		{
			return;
		}
	}

	to = SV_CanReplaceServerCommand(client, cmd);

	if ( to >= 0 )
	{
		for ( from = to + 1; from <= client->reliableSequence; from++, to++ )
		{
			svscmd = &client->reliableCommandInfo[to & (MAX_RELIABLE_COMMANDS - 1)];
			*svscmd = client->reliableCommandInfo[from & (MAX_RELIABLE_COMMANDS - 1)];
		}
	}
	else
	{
		client->reliableSequence++;
	}

	// if we would be losing an old command that hasn't been acknowledged,
	// we must drop the connection
	// we check == instead of >= so a broadcast print added by SV_DropClient()
	// doesn't cause a recursive drop client
	if ( client->reliableSequence - client->reliableAcknowledge == MAX_RELIABLE_COMMANDS + 1 )
	{
		Com_Printf("===== pending server commands =====\n");
		for ( i = client->reliableAcknowledge + 1; i <= client->reliableSequence; ++i )
		{
			Com_Printf("cmd %5d: %8d: %s\n", i, client->reliableCommandInfo[i & ( MAX_RELIABLE_COMMANDS - 1 )].time, client->reliableCommandInfo[i & ( MAX_RELIABLE_COMMANDS - 1 )].cmd);
		}
		Com_Printf("cmd %5d: %8d: %s\n", i, svs.time, cmd);
		NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, "disconnect");
		SV_DelayDropClient(client, "EXE_SERVERCOMMANDOVERFLOW");
		type = SV_CMD_RELIABLE;
		cmd = va("%c \"EXE_SERVERCOMMANDOVERFLOW\"", 119);
	}

	index = client->reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
	MSG_WriteReliableCommandToBuffer(cmd, client->reliableCommandInfo[index].cmd, sizeof(client->reliableCommandInfo[index].cmd));

	client->reliableCommandInfo[index].time = svs.time;
	client->reliableCommandInfo[index].type = type;
}

/*
=================
SV_SendServerCommand

Sends a reliable command string to be interpreted by
the client game module: "cp", "print", "chat", etc
A NULL client will broadcast to all clients
=================
*/
void SV_SendServerCommand( client_t *cl, int type, const char *fmt, ... )
{
	va_list argptr;
	char message[MAX_MSGLEN];
	client_t    *client;
	int j;

	va_start( argptr,fmt );
	Q_vsnprintf( message, sizeof( message ), fmt, argptr );
	va_end( argptr );

	if ( cl != NULL )
	{
		SV_AddServerCommand( cl, type, message );
		return;
	}

	// hack to echo broadcast prints to console
	if ( com_dedicated->current.integer && !strncmp( message, "print", 5 ) )
	{
		Com_Printf( "broadcast: %s\n", SV_ExpandNewlines( message ) );
	}

	// send the data to all relevent clients
	for ( j = 0, client = svs.clients; j < sv_maxclients->current.integer ; j++, client++ )
	{
		if ( client->state < CS_PRIMED )
		{
			continue;
		}
		// done.
		SV_AddServerCommand( client, type, message );
	}
}

/*
================
SV_VoicePacket
================
*/
void SV_VoicePacket( netadr_t from, msg_t *msg )
{
	int qport;
	int i;
	client_t *cl;

	qport = MSG_ReadShort(msg);

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( !cl->state )
		{
			continue;
		}

		if ( !NET_CompareBaseAdr(from, cl->netchan.remoteAddress) )
		{
			continue;
		}

		if ( cl->netchan.qport != qport )
		{
			continue;
		}

		if ( cl->netchan.remoteAddress.port != from.port )
		{
			Com_Printf( "SV_VoicePacket: fixing up a translated port\n" );
			cl->netchan.remoteAddress.port = from.port;
		}

		if ( cl->state == CS_ZOMBIE )
		{
			continue;
		}

		cl->lastPacketTime = svs.time;

		if ( cl->state < CS_ACTIVE )
		{
			SV_PreGameUserVoice(cl, msg);
			return;
		}

		assert(cl->gentity);
		SV_UserVoice(cl, msg);
	}
}

/*
================
SVC_Info
Responds with a short info message that should be enough to determine
if a user is interested in a server to do a full status
================
*/
void SVC_Info( netadr_t from )
{
	int i, count;
	char infostring[MAX_INFO_STRING];
	char infosend[MAX_INFO_STRING];

#if LIBCOD_COMPILE_RATELIMITER == 1
	extern leakyBucket_t outboundLeakyBucket;
	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SVC_Info: rate limit exceeded, dropping request\n" );
		return;
	}
#endif

	qboolean serverModded = qfalse;

	// don't count privateclients
	int privateClientCount = 0;

	for ( i = 0; i < sv_privateClients->current.integer; ++i )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
			++privateClientCount;
	}

	int clientCount = privateClientCount;

	for ( i = sv_privateClients->current.integer; i < sv_maxclients->current.integer; ++i )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
			++clientCount;
	}

	infostring[0] = 0;

	// echo back the parameter to status. so servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv( 1 ) );
	Info_SetValueForKey( infostring, "protocol", va( "%i", PROTOCOL_VERSION ) );
	Info_SetValueForKey( infostring, "hostname", sv_hostname->current.string );
	Info_SetValueForKey( infostring, "mapname", sv_mapname->current.string );

	if ( clientCount )
	{
		Info_SetValueForKey( infostring, "clients", va( "%i", clientCount ) );
	}

	int maxclients = sv_maxclients->current.integer - (sv_privateClients->current.integer - privateClientCount);

	if ( maxclients > 0 )
	{
		Info_SetValueForKey( infostring, "sv_maxclients", va( "%i", maxclients ) );
	}

	Info_SetValueForKey( infostring, "gametype", sv_gametype->current.string );

	if ( sv_pure->current.boolean || fs_numServerIwds )
	{
		Info_SetValueForKey( infostring, "pure", "1" );
	}

	if ( sv_minPing->current.integer )
	{
		Info_SetValueForKey( infostring, "minPing", va( "%i", sv_minPing->current.integer ) );
	}

	if ( sv_maxPing->current.integer )
	{
		Info_SetValueForKey( infostring, "maxPing", va( "%i", sv_maxPing->current.integer ) );
	}

	const char *gamedir = Dvar_GetString( "fs_game" );

	if ( *gamedir )
	{
		Info_SetValueForKey( infostring, "game", gamedir );
	}

	if ( sv_allowAnonymous->current.boolean )
	{
		Info_SetValueForKey( infostring, "sv_allowAnonymous", va( "%i", sv_allowAnonymous->current.boolean ) );
	}

	if ( sv_disableClientConsole->current.boolean )
	{
		Info_SetValueForKey( infostring, "con_disabled", va("%i", sv_disableClientConsole->current.boolean) );
	}

	const char *password = Dvar_GetString("g_password");

	if ( password && *password )
	{
		Info_SetValueForKey(infostring, "pswrd", "1");
	}

	int friendlyfire = Dvar_GetInt("scr_friendlyfire");

	if ( friendlyfire )
	{
		Info_SetValueForKey(infostring, "ff", va("%i", friendlyfire));
	}

	int killcam = Dvar_GetInt("scr_killcam");

	if ( killcam )
	{
		Info_SetValueForKey(infostring, "kc", va("%i", killcam));
	}

	Info_SetValueForKey(infostring, "hw", va("%i", 1));

	if ( !sv_pure->current.boolean || gamedir && *gamedir )
	{
		serverModded = qtrue;
	}
	else
	{
		const char *referencedIwdNames = Dvar_GetString("sv_referencedIwdNames");

		if ( *referencedIwdNames )
		{
			SV_Cmd_TokenizeString(referencedIwdNames);
			count = SV_Cmd_Argc();

			for ( i = 0; i < count; ++i )
			{
				char *iwd = (char *)SV_Cmd_Argv(i);

				if ( !FS_iwIwd(iwd, BASEGAME) )
				{
					serverModded = qtrue;
					break;
				}
			}
		}
	}

	Info_SetValueForKey(infostring, "mod", va("%i", serverModded));
	Info_SetValueForKey(infostring, "voice", va("%i", sv_voice->current.boolean));

	I_strncpyz(infosend, "infoResponse\n", MAX_INFO_STRING);
	I_strncat(infosend, MAX_INFO_STRING, infostring);

	NET_OutOfBandPrint(NS_SERVER, from, infosend);
}

/*
================
SVC_Status
Responds with all the info that qplug or qspy can see about the server
and all connected players.  Used for getting detailed information after
the simple info query.
================
*/
void SVC_Status( netadr_t from )
{
	int i;
	char infostring[8192];
	char msg[8192];
	char keywords[MAX_INFO_STRING];
	char status[MAX_MSGLEN];
	client_t    *cl;
	int statusLength;
	int playerLength;
	char player[MAX_INFO_STRING];
	int count;

#if LIBCOD_COMPILE_RATELIMITER == 1
	extern leakyBucket_t outboundLeakyBucket;
	// Prevent using getstatus as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SVC_Status: rate limit exceeded, dropping request\n" );
		return;
	}
#endif

	qboolean serverModded = qfalse;
	strcpy( infostring, Dvar_InfoString(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE) );

	// echo back the parameter to status. so master servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv( 1 ) );

	// add "demo" to the sv_keywords if restricted
	if ( Dvar_GetBool( "fs_restrict" ) )
	{
		Com_sprintf(keywords, sizeof( keywords ), "demo %s", Info_ValueForKey(infostring, "sv_keywords"));
		Info_SetValueForKey(infostring, "sv_keywords", keywords);
	}

	status[0] = 0;
	statusLength = 0;

	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		cl = &svs.clients[i];
		if ( cl->state >= CS_CONNECTED )
		{
			if ( gameInitialized )
				Com_sprintf( player, sizeof( player ), "%i %i \"%s\"\n", G_GetClientScore(cl - svs.clients), cl->ping, cl->name );
			else
				Com_sprintf( player, sizeof( player ), "%i %i \"%s\"\n", 0, cl->ping, cl->name );

			playerLength = strlen( player );
			if ( statusLength + playerLength >= sizeof( status ) )
			{
				break;      // can't hold any more
			}
			strcpy( status + statusLength, player );
			statusLength += playerLength;
		}
	}

	const char *password = Dvar_GetString("g_password");

	if ( password && *password )
		Info_SetValueForKey(infostring, "pswrd", "1");
	else
		Info_SetValueForKey(infostring, "pswrd", "0");

	const char *gamedir = Dvar_GetString("fs_game");

	if ( !sv_pure->current.boolean || gamedir && *gamedir )
	{
		serverModded = qtrue;
	}
	else
	{
		const char *referencedIwdNames = Dvar_GetString("sv_referencedIwdNames");

		if ( *referencedIwdNames )
		{
			SV_Cmd_TokenizeString(referencedIwdNames);
			count = SV_Cmd_Argc();

			for ( i = 0; i < count; ++i )
			{
				char *iwd = (char *)SV_Cmd_Argv(i);

				if ( !FS_iwIwd(iwd, BASEGAME) )
				{
					serverModded = qtrue;
					break;
				}
			}
		}
	}

	Info_SetValueForKey(infostring, "mod", va("%i", serverModded));

	Com_sprintf(msg, sizeof(infostring), "statusResponse\n%s\n%s", infostring, status);
	NET_OutOfBandPrint(NS_SERVER, from, msg);
}

/*
=================
SVC_GameCompleteStatus
NERVE - SMF - Send serverinfo cvars, etc to master servers when
game complete. Useful for tracking global player stats.
=================
*/
void SVC_GameCompleteStatus( netadr_t from )
{
	char player[1024];
	char status[MAX_MSGLEN];
	int i;
	client_t    *cl;
	int statusLength;
	int playerLength;
	char infostring[MAX_INFO_STRING];

	strcpy( infostring, Dvar_InfoString(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE) );

	// echo back the parameter to status. so master servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv( 1 ) );

	// add "demo" to the sv_keywords if restricted
	if ( Dvar_GetBool( "fs_restrict" ) )
	{
		char keywords[MAX_INFO_STRING];

		Com_sprintf( keywords, sizeof( keywords ), "demo %s",
		             Info_ValueForKey( infostring, "sv_keywords" ) );
		Info_SetValueForKey( infostring, "sv_keywords", keywords );
	}

	status[0] = 0;
	statusLength = 0;

	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		cl = &svs.clients[i];
		if ( cl->state >= CS_CONNECTED )
		{
			Com_sprintf( player, sizeof( player ), "%i %i \"%s\"\n",
			             G_GetClientScore(cl - svs.clients), cl->ping, cl->name );
			playerLength = strlen( player );
			if ( statusLength + playerLength >= sizeof( status ) )
			{
				break;      // can't hold any more
			}
			strcpy( status + statusLength, player );
			statusLength += playerLength;
		}
	}

	NET_OutOfBandPrint( NS_SERVER, from, va("gameCompleteStatus\n%s\n%s", infostring, status) );
}

/*
=================
SV_ConnectionlessPacket
A connectionless packet has four leading 0xff
characters to distinguish it from a game channel.
Clients that are in the game can still send
connectionless packets.
=================
*/
void SV_ConnectionlessPacket( netadr_t from, msg_t *msg )
{
	char    *s;
	const char    *c;
	char strBuf[MAX_STRING_CHARS];

	MSG_BeginReading(msg);
	MSG_ReadLong(msg);
	SV_Netchan_AddOOBProfilePacket(msg->cursize);
	s = MSG_ReadStringLine( msg, strBuf, sizeof(strBuf) );
	Cmd_TokenizeString( s );
	c = Cmd_Argv( 0 );

	if ( sv_packet_info->current.boolean )
	{
		Com_Printf("SV packet %s : %s\n", NET_AdrToString(from), c);
	}

	if ( !Q_stricmp( c, "v" ) )
	{
		SV_VoicePacket( from, msg );
	}
	else if ( !Q_stricmp( c,"getstatus" ) )
	{
		SVC_Status( from  );
	}
	else if ( !Q_stricmp( c,"getinfo" ) )
	{
		SVC_Info( from );
	}
	else if ( !Q_stricmp( c,"getchallenge" ) )
	{
		SV_GetChallenge( from );
	}
	else if ( !Q_stricmp( c,"connect" ) )
	{
		SV_DirectConnect( from );
	}
	else if ( !Q_stricmp( c,"ipAuthorize" ) )
	{
		SV_AuthorizeIpPacket( from );
	}
	else if ( !Q_stricmp( c, "rcon" ) )
	{
		SVC_RemoteCommand( from, msg );
	}
	else if ( !Q_stricmp( c,"disconnect" ) )
	{
		// if a client starts up a local server, we may see some spurious
		// server disconnect messages when their new server sees our final
		// sequenced messages to the old client
	}
}

/*
=================
SV_PacketEvent
=================
*/
void SV_PacketEvent( netadr_t from, msg_t *msg )
{
	int i;
	client_t    *cl;
	int qport;

	// check for connectionless packet (0xffffffff) first
	if ( msg->cursize >= 4 && *(int *)msg->data == -1 )
	{
		SV_ConnectionlessPacket( from, msg );
		return;
	}

	SV_ResetSkeletonCache();

	// read the qport out of the message so we can fix up
	// stupid address translating routers
	MSG_BeginReading( msg );
	MSG_ReadLong( msg );                // sequence number
	qport = (unsigned short)MSG_ReadShort(msg);

	// find which client the message is from
	for ( i = 0, cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( cl->state == CS_FREE )
		{
			continue;
		}

		if ( !NET_CompareBaseAdr( from, cl->netchan.remoteAddress ) )
		{
			continue;
		}

		// it is possible to have multiple clients from a single IP
		// address, so they are differentiated by the qport variable
		if ( cl->netchan.qport != qport )
		{
			continue;
		}

		// the IP port can't be used to differentiate them, because
		// some address translating routers periodically change UDP
		// port assignments
		if ( cl->netchan.remoteAddress.port != from.port )
		{
			Com_Printf( "SV_PacketEvent: fixing up a translated port\n" );
			cl->netchan.remoteAddress.port = from.port;
		}

		// make sure it is a valid, in sequence packet
		if ( Netchan_Process(&cl->netchan, msg) )
		{
			cl->serverId = MSG_ReadByte(msg);
			cl->messageAcknowledge = MSG_ReadLong(msg);

			if ( cl->messageAcknowledge < 0 )
			{
				Com_Printf("Invalid reliableAcknowledge message from %s - reliableAcknowledge is %i\n", cl->name, cl->reliableAcknowledge);
				return;
			}

			cl->reliableAcknowledge = MSG_ReadLong(msg);

			// New: Fix for invalid reliableAcknowledge exploit, see https://github.com/callofduty4x/CoD4x_Server/pull/407
			if ( ( cl->reliableSequence - cl->reliableAcknowledge ) > ( MAX_RELIABLE_COMMANDS - 1 ) || cl->reliableAcknowledge < 0 || ( cl->reliableSequence - cl->reliableAcknowledge ) < 0 )
			{
				Com_Printf("Out of range reliableAcknowledge message from %s - cl->reliableSequence is %i, reliableAcknowledge is %i\n",
				           cl->name, cl->reliableSequence, cl->reliableAcknowledge);
				cl->reliableAcknowledge = cl->reliableSequence;
				return;
			}

			SV_Netchan_Decode(cl, &msg->data[msg->readcount], msg->cursize - msg->readcount);

			// zombie clients still need to do the Netchan_Process
			// to make sure they don't need to retransmit the final
			// reliable message, but they don't do any other processing
			if ( cl->state != CS_ZOMBIE )
			{
				cl->lastPacketTime = svs.time;
				SV_ExecuteClientMessage(cl, msg);
			}
		}
		return;
	}

	// if we received a sequenced packet from an address we don't recognize,
	// send an out of band disconnect packet to it
	NET_OutOfBandPrint( NS_SERVER, from, "disconnect" );
}

/*
==================
SV_CheckPaused
==================
*/
qboolean SV_CheckPaused( void )
{
	int count;
	client_t    *cl;
	int i;

	if ( !cl_paused->current.integer )
	{
		return qfalse;
	}

	// only pause if there is just a single client connected
	count = 0;
	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( cl->state >= CS_CONNECTED )
		{
			count++;
		}
	}

	if ( count > 1 )
	{
		Dvar_SetInt( sv_paused, 0 );
		return qfalse;
	}

	Dvar_SetInt( sv_paused, 1 );

	return qtrue;
}

/*
==================
SV_UpdateBots
==================
*/
void SV_UpdateBots()
{
	client_t *cl;
	int i;

	SV_ResetSkeletonCache();

	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( !cl->state )
			continue;

		if ( cl->netchan.remoteAddress.type != NA_BOT )
			continue;

#if LIBCOD_COMPILE_BOTS == 1
		SV_BotUserMove_libcod(cl);
#else
		SV_BotUserMove(cl);
#endif
	}
}

/*
==================
SV_RunFrame
==================
*/
void SV_RunFrame()
{
	SV_ResetSkeletonCache();
	G_RunFrame(svs.time);
}

/*
==================
SV_Frame

Player movement occurs as a result of packet events, which
happen before SV_Frame is called
==================
*/
void SV_Frame( int msec )
{
	char mapname[MAX_QPATH];
	int frameMsec;

	if ( !com_sv_running->current.boolean )
	{
		return;
	}

	// allow pause if only the local client is connected
	if ( SV_CheckPaused() )
	{
		return;
	}

	assert(Sys_IsMainThread());
	assert(msec >= 0);

	frameMsec = 1000 / sv_fps->current.integer;
	assert(sv.timeResidual < frameMsec);
	sv.timeResidual += msec;

	if ( sv.timeResidual < frameMsec )
	{
		return;
	}

	// if time is about to hit the 32nd bit, kick all clients
	// and clear sv.time, rather
	// than checking for negative time wraparound everywhere.
	// 2giga-milliseconds = 23 days, so it won't be too often
	if ( svs.time > 0x70000000 )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown("EXE_SERVERRESTARTTIMEWRAP");
		// TTimo
		// show_bug.cgi?id=388
		// there won't be a map_restart if you have shut down the server
		// since it doesn't restart a non-running server
		// instead, re-run the current map
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	// this can happen considerably earlier when lots of clients play and the map doesn't change
	if ( svs.nextSnapshotEntities >= 0x7FFFFFFE - svs.numSnapshotEntities )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15numSnapshotEntities wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextCachedSnapshotEntities >= 0x7FFFFFFE - CACHED_SNAPSHOT_ENTITY_SIZE )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextCachedSnapshotEntities wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextCachedSnapshotClients >= 0x7FFFFFFE - CACHED_SNAPSHOT_CLIENT_SIZE )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextCachedSnapshotClients wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextArchivedSnapshotFrames >= 0x7FFFFFFE - NUM_ARCHIVED_FRAMES )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextArchivedSnapshotFrames wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextArchivedSnapshotBuffer >= 0x7FFFFFFE - ARCHIVED_SNAPSHOT_BUFFER_SIZE )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextArchivedSnapshotBuffer wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextCachedSnapshotFrames >= 0x7FFFFFFE - NUM_CACHED_FRAMES )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15svs.nextCachedSnapshotFrames wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextSnapshotClients >= 0x7FFFFFFE - svs.numSnapshotClients )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15numSnapshotClients wrapping" );
		// Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	// update infostrings if anything has been changed
	if ( (dvar_modifiedFlags & (DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE)) )
	{
		SV_SetConfigstring(CS_SERVERINFO, Dvar_InfoString(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE));
		dvar_modifiedFlags &= ~(DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE);
	}

	if ( (dvar_modifiedFlags & DVAR_SYSTEMINFO) )
	{
		SV_SetConfigstring(CS_SYSTEMINFO, Dvar_InfoString_Big(DVAR_SYSTEMINFO));
		dvar_modifiedFlags &= ~DVAR_SYSTEMINFO;
	}

	// NERVE - SMF
	if ( (dvar_modifiedFlags & DVAR_CODINFO) )
	{
		SV_SetConfig(142, 96, 256);
		dvar_modifiedFlags &= ~DVAR_CODINFO;
	}

	SV_UpdateBots();

	// update ping based on the all received frames
#ifdef LIBCOD
	SV_CalcPings_libcod();
#else
	SV_CalcPings();
#endif

	// run the game simulation in chunks
	while ( 1 )
	{
		sv.timeResidual -= frameMsec;
		svs.time += frameMsec;

		// let everything in the world think and move
		SV_RunFrame();

		Scr_SetLoading(false);

		if ( sv.timeResidual < frameMsec )
			break;

		SV_ArchiveSnapshot();
	}

	// check timeouts
#ifdef LIBCOD
	SV_CheckTimeouts_libcod();
#else
	SV_CheckTimeouts();
#endif

	// send messages back to the clients
	SV_SendClientMessages();

	SV_ArchiveSnapshot();

	// send a heartbeat to the master if needed
#ifdef LIBCOD
	SV_MasterHeartbeat_libcod( HEARTBEAT_GAME );
#else
	SV_MasterHeartbeat( HEARTBEAT_GAME );
#endif
}