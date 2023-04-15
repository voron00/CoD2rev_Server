#include "../qcommon/qcommon.h"

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

/*
======================
SV_AddServerCommand
The given command will be transmitted to the client, and is guaranteed to
not have future snapshot_t executed before it is executed
======================
*/
void SV_CullIgnorableServerCommands(client_t *client)
{
	int v1;
	int i;

	v1 = client->reliableSent + 1;

	for(i = client->reliableSent + 1 ; i <= client->reliableSequence; ++i)
	{
		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].cmdType )
		{
			if ( (v1 & (MAX_RELIABLE_COMMANDS - 1)) != (i & (MAX_RELIABLE_COMMANDS - 1)) )
			{
				memcpy(&client->reliableCommands[v1 & (MAX_RELIABLE_COMMANDS - 1)], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)], sizeof(reliableCommands_t));
			}
			++v1;
		}
	}
	client->reliableSequence = v1 - 1;
}

int SV_CanReplaceServerCommand(client_t *client, const char *command)
{
	int i;

	for( i = client->reliableSent + 1; i <= client->reliableSequence; ++i)
	{

		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].cmdType == 0 )
			continue;

		if(client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[0] != command[0])
			continue;

		if ( command[0] >= 120 && command[0] <= 122 )
			continue;

		if ( !strcmp(&command[1], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[1]) )
			return i;


		switch ( command[0] )
		{
		case 100:
		case 118:
			if ( !I_IsEqualUnitWSpace( (char*)&command[2], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[2]))
			{
				continue;
			}
		case 67:
		case 68:
		case 97:
		case 98:
		case 111:
		case 112:
		case 113:
		case 114:
		case 116:
			return i;

		default:
			continue;

		}
	}
	return -1;
}

void SV_ShowClientUnAckCommands( client_t *client )
{
	int i;

	Com_Printf("-- Unacknowledged Server Commands for client %i:%s --\n", client - svs.clients, client->name);

	for ( i = client->reliableAcknowledge + 1; i <= client->reliableSequence; ++i )
	{
		Com_Printf("cmd %5d: %8d: %s\n", i, client->reliableCommands[i & (MAX_RELIABLE_COMMANDS -1)].cmdTime,
		           client->reliableCommands[i & (MAX_RELIABLE_COMMANDS -1)].command );
	}

	Com_Printf("-----------------------------------------------------\n");
}

void SV_AddServerCommand(client_t *client, int type, const char *cmd)
{
	int v4;
	int i;
	int j;
	int index;
	char string[64];

	if(client->netchan.remoteAddress.type == NA_BOT)
	{
		return;
	}
	if ( client->bot )
		return;

	if ( client->reliableSequence - client->reliableAcknowledge >= MAX_RELIABLE_COMMANDS / 2 || client->state != CS_ACTIVE)
	{
		SV_CullIgnorableServerCommands(client);

		if(!type)
			return;

	}

	v4 = SV_CanReplaceServerCommand(client, cmd);

	if ( v4 < 0 )
	{
		++client->reliableSequence;
	}
	else
	{
		for ( i = v4 + 1; i <= client->reliableSequence; ++v4 )
		{
			memcpy(&client->reliableCommands[v4 & 0x7F], &client->reliableCommands[i++ & 0x7F], sizeof(reliableCommands_t));
		}
	}

	if ( client->reliableSequence - client->reliableAcknowledge == (MAX_RELIABLE_COMMANDS + 1) )
	{
		Com_Printf("Client: %i lost reliable commands\n", client - svs.clients);
#if 0
		Com_Printf("===== pending server commands =====\n");
		for ( j = client->reliableAcknowledge + 1; j <= client->reliableSequence; ++j )
		{
			Com_Printf("cmd %5d: %8d: %s\n", j, client->reliableCommands[j & (MAX_RELIABLE_COMMANDS - 1)].cmdTime, &client->reliableCommands[j & (MAX_RELIABLE_COMMANDS - 1)].command);
		}
		Com_Printf("cmd %5d: %8d: %s\n", j, svs.time, cmd);
#endif

		NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, "disconnect");
		SV_DelayDropClient(client, "EXE_SERVERCOMMANDOVERFLOW");

		type = 1;
		Com_sprintf(string,sizeof(string),"%c \"EXE_SERVERCOMMANDOVERFLOW\"", 119);
		cmd = string;
	}

	index = client->reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
	MSG_WriteReliableCommandToBuffer(cmd, client->reliableCommands[ index ].command, sizeof( client->reliableCommands[ index ].command ));
	client->reliableCommands[ index ].cmdTime = svs.time;
	client->reliableCommands[ index ].cmdType = type;
//    Com_Printf(CON_CHANNEL_SERVER,"ReliableCommand: %s\n", cmd);
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

#if COMPILE_RATELIMITER == 1
// ioquake3 rate limit connectionless requests
// https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c

// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024

static leakyBucket_t buckets[ MAX_BUCKETS ];
static leakyBucket_t* bucketHashes[ MAX_HASHES ];
leakyBucket_t outboundLeakyBucket;

static long SVC_HashForAddress( netadr_t address )
{
	unsigned char *ip = address.ip;
	int	i;
	long hash = 0;

	for ( i = 0; i < 4; i++ )
	{
		hash += (long)( ip[ i ] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( MAX_HASHES - 1 );

	return hash;
}

static leakyBucket_t *SVC_BucketForAddress( netadr_t address, int burst, int period )
{
	leakyBucket_t *bucket = NULL;
	int	i;
	long hash = SVC_HashForAddress( address );
	uint64_t now = Sys_Milliseconds64();

	for ( bucket = bucketHashes[ hash ]; bucket; bucket = bucket->next )
	{
		if ( memcmp( bucket->adr, address.ip, 4 ) == 0 )
		{
			return bucket;
		}
	}

	for ( i = 0; i < MAX_BUCKETS; i++ )
	{
		int interval;

		bucket = &buckets[ i ];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
		                               interval < 0 ) )
		{
			if ( bucket->prev != NULL )
			{
				bucket->prev->next = bucket->next;
			}
			else
			{
				bucketHashes[ bucket->hash ] = bucket->next;
			}

			if ( bucket->next != NULL )
			{
				bucket->next->prev = bucket->prev;
			}

			memset( bucket, 0, sizeof( leakyBucket_t ) );
		}

		if ( bucket->type == 0 )
		{
			bucket->type = address.type;
			memcpy( bucket->adr, address.ip, 4 );

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = bucketHashes[ hash ];
			if ( bucketHashes[ hash ] != NULL )
			{
				bucketHashes[ hash ]->prev = bucket;
			}

			bucket->prev = NULL;
			bucketHashes[ hash ] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return NULL;
}

bool SVC_RateLimit( leakyBucket_t *bucket, int burst, int period )
{
	if ( bucket != NULL )
	{
		uint64_t now = Sys_Milliseconds64();
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst || interval < 0 )
		{
			bucket->burst = 0;
			bucket->lastTime = now;
		}
		else
		{
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst )
		{
			bucket->burst++;

			return false;
		}
	}

	return true;
}

bool SVC_RateLimitAddress( netadr_t from, int burst, int period )
{
	leakyBucket_t *bucket = SVC_BucketForAddress( from, burst, period );

	return SVC_RateLimit( bucket, burst, period );
}
#endif

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

#ifdef LIBCOD
extern dvar_t *sv_allowRcon;
#endif

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

#ifdef LIBCOD
	if (!sv_allowRcon->current.boolean)
		return;
#endif

#if COMPILE_RATELIMITER == 1
	static leakyBucket_t bucket;
	// Prevent using rcon as an amplifier and make dictionary attacks impractical
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Make DoS via rcon impractical
	if ( SVC_RateLimit( &bucket, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_RemoteCommand: rate limit exceeded, dropping request\n" );
		return;
	}
#endif

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

#if COMPILE_RATELIMITER == 1
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
	Info_SetValueForKey( infostring, "pure", va( "%i", sv_pure->current.boolean ) );

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

	Info_SetValueForKey( infostring, "sv_allowAnonymous", va( "%i", sv_allowAnonymous->current.boolean ) );

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
extern int gameInitialized;
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

#if COMPILE_RATELIMITER == 1
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

	strcpy( infostring, Dvar_InfoString(0x404u) );

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
================
SV_MasterHeartbeat
Send a message to the masters every few minutes to
let it know we are alive, and log information.
We will also have a heartbeat sent when a server
changes from empty to non-empty, and full to non-full,
but not on every player enter or exit.
================
*/
extern dvar_t *sv_master[];
#define	HEARTBEAT_MSEC	180000
#define	STATUS_MSEC		600000
void SV_MasterHeartbeat(const char *game)
{
	static netadr_t	adr[MAX_MASTER_SERVERS];
	char heartbeat[32];
	int	i;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !com_dedicated || com_dedicated->current.integer != 2 )
	{
		return;     // only dedicated servers send heartbeats
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextHeartbeatTime )
	{
		svs.nextHeartbeatTime = svs.time + HEARTBEAT_MSEC;

		// send to group masters
		for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
		{
			if ( !sv_master[i]->current.string[0] )
			{
				continue;
			}

			// see if we haven't already resolved the name
			// do it when needed
			if ( sv_master[i]->modified || !adr[i].type )
			{
				sv_master[i]->modified = qfalse;

				Com_Printf( "Resolving %s\n", sv_master[i]->current.string );
				if ( !NET_StringToAdr( sv_master[i]->current.string, &adr[i] ) )
				{
					// if the address failed to resolve, clear it
					// so we don't take repeated dns hits
					Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->current.string );
					Dvar_SetString(sv_master[i], "");
					sv_master[i]->modified = qfalse;
					continue;
				}
				if ( !strstr( ":", sv_master[i]->current.string ) )
				{
					adr[i].port = BigShort( PORT_MASTER );
				}
				Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->current.string,
				            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
				            BigShort( adr[i].port ) );
			}

			if (strlen(sv_master[i]->current.string))
			{
				Com_Printf( "Sending heartbeat to %s\n", sv_master[i]->current.string );
				sprintf(heartbeat, "heartbeat %s\n", game);
				NET_OutOfBandPrint( NS_SERVER, adr[i], heartbeat );
			}
		}
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextStatusResponseTime )
	{
		svs.nextStatusResponseTime = svs.time + STATUS_MSEC;

		// send to group masters
		for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
		{
			if ( !sv_master[i]->current.string[0] )
			{
				continue;
			}

			// see if we haven't already resolved the name
			// do it when needed
			if ( sv_master[i]->modified || !adr[i].type )
			{
				sv_master[i]->modified = qfalse;

				Com_Printf( "Resolving %s\n", sv_master[i]->current.string );
				if ( !NET_StringToAdr( sv_master[i]->current.string, &adr[i] ) )
				{
					// if the address failed to resolve, clear it
					// so we don't take repeated dns hits
					Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->current.string );
					Dvar_SetString(sv_master[i], "");
					sv_master[i]->modified = qfalse;
					continue;
				}
				if ( !strstr( ":", sv_master[i]->current.string ) )
				{
					adr[i].port = BigShort( PORT_MASTER );
				}
				Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->current.string,
				            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
				            BigShort( adr[i].port ) );
			}

			if (strlen(sv_master[i]->current.string))
			{
				SVC_Status(adr[i]);
			}
		}
	}
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

	strcpy( infostring, Dvar_InfoString(0x404u) );

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
SV_MasterGameCompleteStatus
NERVE - SMF - Sends gameCompleteStatus messages to all master servers
=================
*/
void SV_MasterGameCompleteStatus()
{
	static netadr_t adr[MAX_MASTER_SERVERS];
	int i;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !com_dedicated || com_dedicated->current.integer != 2 )
	{
		return;     // only dedicated servers send master game status
	}

	// send to group masters
	for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
	{
		if ( !sv_master[i]->current.string[0] )
		{
			continue;
		}

		// see if we haven't already resolved the name
		// resolving usually causes hitches on win95, so only
		// do it when needed
		if ( sv_master[i]->modified )
		{
			sv_master[i]->modified = qfalse;

			Com_Printf( "Resolving %s\n", sv_master[i]->current.string );
			if ( !NET_StringToAdr( sv_master[i]->current.string, &adr[i] ) )
			{
				// if the address failed to resolve, clear it
				// so we don't take repeated dns hits
				Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->current.string );
				Dvar_SetString(sv_master[i], "");
				sv_master[i]->modified = qfalse;
				continue;
			}
			if ( !strstr( ":", sv_master[i]->current.string ) )
			{
				adr[i].port = BigShort( PORT_MASTER );
			}
			Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->current.string,
			            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
			            BigShort( adr[i].port ) );
		}

		Com_Printf( "Sending gameCompleteStatus to %s\n", sv_master[i]->current.string );
		// this command should be changed if the server info / status format
		// ever incompatably changes
		SVC_GameCompleteStatus( adr[i] );
	}
}

void SV_MasterShutdown()
{
	svs.nextHeartbeatTime = 0x80000000;
	SV_MasterHeartbeat("flatline");
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

/*
==================
SV_CheckPaused
==================
*/
extern dvar_t *cl_paused;
extern dvar_t *sv_paused;
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
		if ( cl->state >= CS_CONNECTED && cl->netchan.remoteAddress.type != NA_BOT )
		{
			count++;
		}
	}

	if ( count > 1 )
	{
		// don't pause
		if ( sv_paused->current.integer )
		{
			Dvar_SetInt( sv_paused, 0 );
		}
		return qfalse;
	}

	if ( !sv_paused->current.integer )
	{
		Dvar_SetInt( sv_paused, 1 );
	}

	return qtrue;
}

/*
================
SV_CreateBaseline
Entity baselines are used to compress non-delta messages
to the clients -- only the fields that differ from the
baseline will be transmitted
================
*/
void SV_CreateBaseline( void )
{
	gentity_t *svent;
	int entnum;

	for ( entnum = 1; entnum < sv.num_entities ; entnum++ )
	{
		svent = SV_GentityNum( entnum );
		if ( !svent->r.linked )
		{
			continue;
		}
		svent->s.number = entnum;
		//
		// take current state as baseline
		//
		sv.svEntities[entnum].baseline.s = svent->s;

		sv.svEntities[entnum].baseline.r.svFlags = svent->r.svFlags;

		sv.svEntities[entnum].baseline.r.clientMask[0] = svent->r.clientMask[0];
		sv.svEntities[entnum].baseline.r.clientMask[1] = svent->r.clientMask[1];

		VectorCopy(svent->r.absmin, sv.svEntities[entnum].baseline.r.absmin);
		VectorCopy(svent->r.absmax, sv.svEntities[entnum].baseline.r.absmax);

		if ( svent->s.clientNum >= MAX_CLIENTS )
		{
			svent->s.clientNum = MAX_CLIENTS;
		}
	}
}

void SV_BotFrame()
{
	client_s *clients;
	int i;

	SV_ResetSkeletonCache();

	i = 0;
	clients = svs.clients;

	while ( i < sv_maxclients->current.integer )
	{
		if ( clients->state )
		{
			if ( clients->netchan.remoteAddress.type == NA_BOT )
				SV_BotUserMove(clients);
		}

		++i;
		++clients;
	}
}

void SV_RunFrame()
{
	SV_ResetSkeletonCache();
	G_RunFrame(svs.time);
}

extern dvar_t *sv_fps;
void SV_Frame(int msec)
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

	frameMsec = 1000 / sv_fps->current.integer;
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
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextSnapshotClients >= 0x7FFFFFFE - svs.numSnapshotClients )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15numSnapshotClients wrapping" );
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextCachedSnapshotEntities >= 0x7FFFBFFD )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextCachedSnapshotEntities wrapping" );
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextCachedSnapshotClients >= 0x7FFFEFFD )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextCachedSnapshotClients wrapping" );
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextArchivedSnapshotFrames >= 0x7FFFFB4D )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextArchivedSnapshotFrames wrapping" );
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextArchivedSnapshotBuffer >= 0x7DFFFFFD )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15nextArchivedSnapshotBuffer wrapping" );
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( svs.nextCachedSnapshotFrames >= 0x7FFFFDFD )
	{
		Q_strncpyz( mapname, sv_mapname->current.string, MAX_QPATH );
		Com_Shutdown( "EXE_SERVERRESTARTMISC\x15svs.nextCachedSnapshotFrames wrapping" );
		Com_Restart( );
		// TTimo see above
		Cbuf_AddText( va( "map %s\n", mapname ) );
		return;
	}

	if ( (dvar_modifiedFlags & 0x404) != 0 )
	{
		SV_SetConfigstring(0, Dvar_InfoString(0x404u));
		dvar_modifiedFlags &= 0xFFFFFBFB;
	}

	if ( (dvar_modifiedFlags & 8) != 0 )
	{
		SV_SetConfigstring(1u, Dvar_InfoString_Big(8u));
		dvar_modifiedFlags &= ~8u;
	}

	if ( (dvar_modifiedFlags & 0x100) != 0 )
	{
		SV_SetConfig(142, 96, 0x100u);
		dvar_modifiedFlags &= ~0x100u;
	}

	SV_BotFrame();
	SV_CalcPings();

	while ( 1 )
	{
		sv.timeResidual -= frameMsec;
		svs.time += frameMsec;
		SV_RunFrame();
		Scr_SetLoading(0);

		if ( sv.timeResidual < frameMsec )
			break;

		SV_ArchiveSnapshot();
	}

	SV_CheckTimeouts();
	SV_SendClientMessages();
	SV_ArchiveSnapshot();
	SV_MasterHeartbeat("COD-2");
}