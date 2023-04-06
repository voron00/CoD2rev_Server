#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

#ifdef TESTING_LIBRARY
#define bgs (*((bgs_t**)( 0x0855A4E0 )))
#else
extern bgs_t *bgs;
#endif

void SV_FreeClientScriptId(client_s *cl)
{
	Scr_FreeValue(cl->clscriptid);
	cl->clscriptid = 0;
}

char *SV_ExpandNewlines( char *in )
{
	static	char string[1024];
	int l;

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

void SV_CalcPings( void )
{
	int i, j;
	client_t *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		cl = &svs.clients[i];

		if ( cl->state != CS_ACTIVE )
		{
			cl->ping = -1;
			continue;
		}

		if ( !cl->gentity )
		{
			cl->ping = -1;
			continue;
		}

		if ( cl->netchan.remoteAddress.type == NA_BOT )
		{
			cl->ping = 0;
			cl->lastPacketTime = svs.time;
			continue;
		}

		total = 0;
		count = 0;

		for ( j = 0 ; j < PACKET_BACKUP ; j++ )
		{
			if ( cl->frames[j].messageAcked == 0xFFFFFFFF )
				continue;

			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}

		if ( !count )
			cl->ping = 999;
		else
		{
			cl->ping = total / count;

			if ( cl->ping > 999 )
				cl->ping = 999;
		}
	}
}

extern dvar_t *sv_timeout;
extern dvar_t *sv_zombietime;
void SV_CheckTimeouts( void )
{
	int i;
	client_t *cl;
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

		if ( cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint )
		{
			// using the client id cause the cl->name is empty at this point
			Com_DPrintf( "Going from CS_ZOMBIE to CS_FREE for client %d\n", i );
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

void SV_ShowClientUnAckCommands(client_s *client)
{
	int i;

	Com_Printf("-- Unacknowledged Server Commands for client %i:%s --\n", client - svs.clients, client->name);

	for ( i = client->reliableAcknowledge + 1; i <= client->reliableSequence; ++i )
		Com_Printf("cmd %5d: %8d: %s\n", i, client->reliableCommands[i & (MAX_RELIABLE_COMMANDS -1)].cmdTime, client->reliableCommands[i & (MAX_RELIABLE_COMMANDS -1)].command);

	Com_Printf("----------");
}

int SV_CullIgnorableServerCommands(client_t *client)
{
	int sequence;
	int i;

	sequence = client->reliableSent + 1;

	for ( i = client->reliableSent; i < client->reliableSequence + 1; ++i )
	{
		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].cmdType )
		{
			if ( (sequence & (MAX_RELIABLE_COMMANDS - 1)) != (i & (MAX_RELIABLE_COMMANDS - 1)) )
			{
				Com_Memcpy(&client->reliableCommands[sequence & (MAX_RELIABLE_COMMANDS - 1)], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)], sizeof(reliableCommands_t));
			}
			++sequence;
		}
	}

	client->reliableSequence = sequence - 1;
	return sequence - 1;
}

int SV_CanReplaceServerCommand(client_t *client, const char *command)
{
	int i;

	for ( i = client->reliableSent + 1; i < client->reliableSequence + 1; ++i )
	{
		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].cmdType == 0 )
			continue;

		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[0] != command[0] )
			continue;

		if ( command[0] >= 120 && command[0] <= 122 )
			continue;

		if ( !strcmp(&command[1], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[1]) )
			return i;

		switch ( command[0] )
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
			if ( !I_IsEqualUnitWSpace( (char*)&command[2], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[2]) )
				continue;
			return i;
		default:
			continue;
		}
	}

	return -1;
}

void SV_AddServerCommand(client_t *client, int type, const char *cmd)
{
	int sequence;
	int i;
	int j;
	int index;

	if ( client->bot )
		return;

	if ( client->reliableSequence - client->reliableAcknowledge < MAX_RELIABLE_COMMANDS / 2 && client->state == CS_ACTIVE || (SV_CullIgnorableServerCommands(client), type) )
	{
		sequence = SV_CanReplaceServerCommand(client, cmd);

		if ( sequence < 0 )
		{
			++client->reliableSequence;
		}
		else
		{
			for ( i = sequence + 1; i < client->reliableSequence + 1; ++sequence )
			{
				Com_Memcpy(&client->reliableCommands[sequence & (MAX_RELIABLE_COMMANDS - 1)], &client->reliableCommands[i++ & (MAX_RELIABLE_COMMANDS - 1)], sizeof(reliableCommands_t));
			}
		}

		if ( client->reliableSequence - client->reliableAcknowledge == (MAX_RELIABLE_COMMANDS + 1) )
		{
			Com_Printf("Client: %i lost reliable commands\n", client - svs.clients);
			Com_Printf("===== pending server commands =====\n");

			for ( j = client->reliableAcknowledge + 1; j <= client->reliableSequence; ++j )
			{
				Com_Printf("cmd %5d: %8d: %s\n", j, client->reliableCommands[j & (MAX_RELIABLE_COMMANDS - 1)].cmdTime, &client->reliableCommands[j & (MAX_RELIABLE_COMMANDS - 1)].command);
			}

			Com_Printf("cmd %5d: %8d: %s\n", j, svs.time, cmd);

			NET_OutOfBandPrint( NS_SERVER, client->netchan.remoteAddress, "disconnect" );
			SV_DelayDropClient(client, "EXE_SERVERCOMMANDOVERFLOW");

			type = 1;
			cmd = va("%c \"EXE_SERVERCOMMANDOVERFLOW\"", 119);
		}

		index = client->reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
		MSG_WriteReliableCommandToBuffer(cmd, client->reliableCommands[ index ].command, sizeof( client->reliableCommands[ index ].command ));

		client->reliableCommands[ index ].cmdTime = svs.time;
		client->reliableCommands[ index ].cmdType = type;
	}
}

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
==============
SV_FlushRedirect
==============
*/
void SV_FlushRedirect(char *outputbuf)
{
	char buf[1300];
	char c;
	int len;

	len = strlen(outputbuf);

	while ( len > 1294 )
	{
		c = outputbuf[1294];
		outputbuf[1294] = 0;
		Com_sprintf(buf, sizeof(buf), "print\n%s", outputbuf);
		NET_OutOfBandPrint(NS_SERVER, svs.redirectAddress, buf);
		len -= 1294;
		outputbuf += 1294;
		*outputbuf = c;
	}

	Com_sprintf(buf, sizeof(buf), "print\n%s", outputbuf);
	NET_OutOfBandPrint(NS_SERVER, svs.redirectAddress, buf);
}

/*
===============
SVC_RemoteCommand
An rcon packet arrived from the network.
Shift down the remaining args
Redirect all printfs
===============
*/
extern dvar_t *rcon_password;
void SVC_RemoteCommand( netadr_t from, msg_t *msg )
{
	qboolean valid;
	unsigned int time;
	char remaining[1024];
	// show_bug.cgi?id=376
	// if we send an OOB print message this size, 1.31 clients die in a Com_Printf buffer overflow
	// the buffer overflow will be fixed in > 1.31 clients
	// but we want a server side fix
	// we must NEVER send an OOB message that will be > 1.31 MAXPRINTMSG (4096)
#define SV_OUTPUTBUF_LENGTH ( 256 * MAX_CLIENTS - 16 )
	char sv_outputbuf[SV_OUTPUTBUF_LENGTH];
	static unsigned int lasttime = 0;
	char *cmd_aux;

	// TTimo - show_bug.cgi?id=534
	time = Com_Milliseconds();
	if ( time < ( lasttime + 500 ) )
	{
		return;
	}
	lasttime = time;

	if ( !strlen( rcon_password->current.string ) ||
	        strcmp( Cmd_Argv( 1 ), rcon_password->current.string ) )
	{
		valid = qfalse;
		Com_Printf( "Bad rcon from %s:\n%s\n", NET_AdrToString( from ), Cmd_Argv( 2 ) );
	}
	else
	{
		valid = qtrue;
		Com_Printf( "Rcon from %s:\n%s\n", NET_AdrToString( from ), Cmd_Argv( 2 ) );
	}

	// start redirecting all print outputs to the packet
	svs.redirectAddress = from;
	// FIXME TTimo our rcon redirection could be improved
	//   big rcon commands such as status lead to sending
	//   out of band packets on every single call to Com_Printf
	//   which leads to client overflows
	//   see show_bug.cgi?id=51
	//     (also a Q3 issue)
	Com_BeginRedirect( sv_outputbuf, SV_OUTPUTBUF_LENGTH, SV_FlushRedirect );

	if ( !strlen( rcon_password->current.string ) )
	{
		Com_Printf( "The server must set 'rcon_password' for clients to use 'rcon'.\n" );
	}
	else if ( !valid )
	{
		Com_Printf( "Invalid password.\n" );
	}
	else
	{
		remaining[0] = 0;

		// ATVI Wolfenstein Misc #284
		// get the command directly, "rcon <pass> <command>" to avoid quoting issues
		// extract the command by walking
		// since the cmd formatting can fuckup (amount of spaces), using a dumb step by step parsing
		cmd_aux = Cmd_Cmd();
		cmd_aux += 4;
		while ( cmd_aux[0] == ' ' )
			cmd_aux++;
		while ( cmd_aux[0] && cmd_aux[0] != ' ' ) // password
			cmd_aux++;
		while ( cmd_aux[0] == ' ' )
			cmd_aux++;

		Q_strcat( remaining, sizeof( remaining ), cmd_aux );

		Cmd_ExecuteString( remaining );

	}

	Com_EndRedirect();
}

void SV_VoicePacket(netadr_t from, msg_t *msg)
{
	int qport;
	int i;
	client_t *client;

	qport = MSG_ReadShort(msg);

	for ( i = 0, client = svs.clients; i < sv_maxclients->current.integer ; i++, client++ )
	{
		if ( client->state && NET_CompareBaseAdr(from, client->netchan.remoteAddress) && client->netchan.qport == qport )
		{
			if ( client->netchan.remoteAddress.port != from.port )
			{
				Com_Printf("SV_ReadPackets: fixing up a translated port\n");
				client->netchan.remoteAddress.port = from.port;
			}

			if ( client->state != CS_ZOMBIE )
			{
				client->lastPacketTime = svs.time;

				if ( client->state > CS_PRIMED )
					SV_UserVoice(client, msg);
				else
					SV_PreGameUserVoice(client, msg);
			}
		}
	}
}

/*
================
SVC_Info
Responds with a short info message that should be enough to determine
if a user is interested in a server to do a full status
================
*/
extern dvar_t *sv_privateClients;
extern dvar_t *sv_hostname;
extern dvar_t *sv_mapname;
extern dvar_t *sv_pure;
extern dvar_t *sv_minPing;
extern dvar_t *sv_maxPing;
extern dvar_t *sv_allowAnonymous;
extern dvar_t *sv_disableClientConsole;
extern dvar_t *sv_voice;
void SVC_Info( netadr_t from )
{
	int i, count;
	char infostring[MAX_INFO_STRING];
	char infosend[MAX_INFO_STRING];
	const char *gamedir;
	const char *password;
	int friendlyfire;
	int killcam;
	int serverModded;
	const char *referencedIwdNames;
	char *iwd;

	// don't count privateclients
	count = 0;

	for ( i = sv_privateClients->current.integer ; i < sv_maxclients->current.integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			count++;
		}
	}

	infostring[0] = 0;

	// echo back the parameter to status. so servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv( 1 ) );

	Info_SetValueForKey( infostring, "protocol", va( "%i", PROTOCOL_VERSION ) );
	Info_SetValueForKey( infostring, "hostname", sv_hostname->current.string );
	Info_SetValueForKey( infostring, "mapname", sv_mapname->current.string );
	Info_SetValueForKey( infostring, "clients", va( "%i", count ) );
	Info_SetValueForKey( infostring, "sv_maxclients", va( "%i", sv_maxclients->current.integer - sv_privateClients->current.integer ) );
	Info_SetValueForKey( infostring, "gametype", Dvar_GetString( "g_gametype" ) );
	Info_SetValueForKey( infostring, "pure", va( "%i", sv_pure->current.integer ) );

	if ( sv_minPing->current.integer )
	{
		Info_SetValueForKey( infostring, "minPing", va( "%i", sv_minPing->current.integer ) );
	}

	if ( sv_maxPing->current.integer )
	{
		Info_SetValueForKey( infostring, "maxPing", va( "%i", sv_maxPing->current.integer ) );
	}

	gamedir = Dvar_GetString( "fs_game" );

	if ( *gamedir )
	{
		Info_SetValueForKey( infostring, "game", gamedir );
	}

	Info_SetValueForKey( infostring, "sv_allowAnonymous", va( "%i", sv_allowAnonymous->current.integer ) );

	if ( sv_disableClientConsole->current.boolean )
	{
		Info_SetValueForKey( infostring, "con_disabled", va("%i", sv_disableClientConsole->current.boolean) );
	}

	password = Dvar_GetString("g_password");

	if ( password && *password )
		Info_SetValueForKey(infostring, "pswrd", "1");

	friendlyfire = Dvar_GetInt("scr_friendlyfire");

	if ( friendlyfire )
		Info_SetValueForKey(infostring, "ff", va("%i", friendlyfire));

	killcam = Dvar_GetInt("scr_killcam");

	if ( killcam )
		Info_SetValueForKey(infostring, "kc", va("%i", killcam));

	Info_SetValueForKey(infostring, "hw", va("%i", 1));
	serverModded = 0;

	if ( !sv_pure->current.boolean || gamedir && *gamedir )
	{
		serverModded = 1;
	}
	else
	{
		referencedIwdNames = Dvar_GetString("sv_referencedIwdNames");

		if ( *referencedIwdNames )
		{
			SV_Cmd_TokenizeString(referencedIwdNames);
			count = SV_Cmd_Argc();

			for ( i = 0; i < count; ++i )
			{
				iwd = (char *)SV_Cmd_Argv(i);

				if ( !FS_iwIwd(iwd, "main") )
				{
					serverModded = 1;
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
	const char *password;
	int serverModded;
	const char *gamedir;
	char *iwd;
	const char *referencedIwdNames;
	int count;

	strcpy( infostring, Dvar_InfoString(0x404u) );

	// echo back the parameter to status. so master servers can use it as a challenge
	// to prevent timed spoofed reply packets that add ghost servers
	Info_SetValueForKey( infostring, "challenge", Cmd_Argv( 1 ) );

	// add "demo" to the sv_keywords if restricted
	if ( Dvar_GetBool("fs_restrict") )
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
			Com_sprintf( player, sizeof( player ), "%i %i \"%s\"\n", G_GetClientScore(cl - svs.clients), cl->ping, cl->name );
			playerLength = strlen( player );
			if ( statusLength + playerLength >= sizeof( status ) )
			{
				break;      // can't hold any more
			}
			strcpy( status + statusLength, player );
			statusLength += playerLength;
		}
	}

	password = Dvar_GetString("g_password");

	if ( password && *password )
		Info_SetValueForKey(infostring, "pswrd", "1");
	else
		Info_SetValueForKey(infostring, "pswrd", "0");

	gamedir = Dvar_GetString("fs_game");
	serverModded = 0;

	if ( !sv_pure->current.boolean || gamedir && *gamedir )
	{
		serverModded = 1;
	}
	else
	{
		referencedIwdNames = Dvar_GetString("sv_referencedIwdNames");

		if ( *referencedIwdNames )
		{
			SV_Cmd_TokenizeString(referencedIwdNames);
			count = SV_Cmd_Argc();

			for ( i = 0; i < count; ++i )
			{
				iwd = (char *)SV_Cmd_Argv(i);

				if ( !FS_iwIwd(iwd, "main") )
				{
					serverModded = 1;
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
SV_ConnectionlessPacket
A connectionless packet has four leading 0xff
characters to distinguish it from a game channel.
Clients that are in the game can still send
connectionless packets.
=================
*/
extern dvar_t *sv_packet_info;
void SV_ConnectionlessPacket( netadr_t from, msg_t *msg )
{
	char    *s;
	const char    *c;

	MSG_BeginReading(msg);
	MSG_ReadLong(msg);
	SV_Netchan_AddOOBProfilePacket(msg->cursize);
	s = MSG_ReadStringLine( msg );
	Cmd_TokenizeString( s );
	c = Cmd_Argv( 0 );

	if ( sv_packet_info->current.boolean )
	{
		Com_Printf("SV packet %s : %s\n", NET_AdrToString(from), c);
	}

	if ( !Q_stricmp( c,"getstatus" ) )
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
	else if ( !Q_stricmp( c, "v" ) )
	{
		SV_VoicePacket( from, msg );
	}
	else if ( !Q_stricmp( c,"disconnect" ) )
	{
		// if a client starts up a local server, we may see some spurious
		// server disconnect messages when their new server sees our final
		// sequenced messages to the old client
	}
	else
	{
		Com_DPrintf( "bad connectionless packet from %s:\n%s\n"
		             , NET_AdrToString( from ), s );
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

			if ( cl->messageAcknowledge >= 0 )
			{
				cl->reliableAcknowledge = MSG_ReadLong(msg);

				if ( cl->reliableSequence - cl->reliableAcknowledge < 128 )
				{
					SV_Netchan_Decode(cl, &msg->data[msg->readcount], msg->cursize - msg->readcount);

					if ( cl->state != CS_ZOMBIE )
					{
						cl->lastPacketTime = svs.time;
						SV_ExecuteClientMessage(cl, msg);
						bgs = 0;
					}
				}
				else
				{
					cl->reliableAcknowledge = cl->reliableSequence;
				}
			}
		}
		return;
	}

	// if we received a sequenced packet from an address we don't recognize,
	// send an out of band disconnect packet to it
	NET_OutOfBandPrint( NS_SERVER, from, "disconnect" );
}

void SV_Frame(int msec)
{
	//UNIMPLEMENTED(__FUNCTION__);
}

void SV_Shutdown( const char* finalmsg )
{
	UNIMPLEMENTED(__FUNCTION__);
}