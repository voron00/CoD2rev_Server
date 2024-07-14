#include "../qcommon/qcommon.h"

/*
==============
SV_FlushRedirect
==============
*/
void SV_FlushRedirect(char *outputbuf)
{
	char buf[1300];
	char c;
	size_t len;

	len = strlen(outputbuf);

	while ( len > sizeof(buf) - 6 )
	{
		c = outputbuf[sizeof(buf) - 6];
		outputbuf[sizeof(buf) - 6] = 0;
		Com_sprintf(buf, sizeof(buf), "print\n%s", outputbuf);
		NET_OutOfBandPrint(NS_SERVER, svs.redirectAddress, buf);
		len -= sizeof(buf) - 6;
		outputbuf += sizeof(buf) - 6;
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
void SVC_RemoteCommand( netadr_t from, msg_t *msg )
{
	qboolean valid;
	int time;
	char remaining[1024];
	// show_bug.cgi?id=376
	// if we send an OOB print message this size, 1.31 clients die in a Com_Printf buffer overflow
	// the buffer overflow will be fixed in > 1.31 clients
	// but we want a server side fix
	// we must NEVER send an OOB message that will be > 1.31 MAXPRINTMSG (4096)
#define SV_OUTPUTBUF_LENGTH ( 256 * MAX_CLIENTS - 16 )
	char sv_outputbuf[SV_OUTPUTBUF_LENGTH];
	static int lasttime = 0;

#ifdef LIBCOD
	if (!sv_allowRcon->current.boolean)
		return;
#endif

#if LIBCOD_COMPILE_RATELIMITER == 1
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
	if ( lasttime && time < ( lasttime + 500 ) )
	{
		return;
	}
	lasttime = time;

	const char *password = Cmd_Argv( 1 );

	if ( !strlen( rcon_password->current.string ) ||
	        strcmp( password, rcon_password->current.string ) )
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

	if ( !rcon_password->current.string[0] )
	{
		Com_Printf("The server must set 'rcon_password' for clients to use 'rcon'.\n");
	}
	else if ( valid )
	{
		// ATVI Wolfenstein Misc #284
		// get the command directly, "rcon <pass> <command>" to avoid quoting issues
		// extract the command by walking
		// since the cmd formatting can fuckup (amount of spaces), using a dumb step by step parsing
		int len = 0;
		int maxlen = 1024;

		for ( int i = 2; i < SV_Cmd_Argc(); ++i )
		{
			len = Com_AddToString(SV_Cmd_Argv(i), remaining, len, maxlen, 1);
			len = Com_AddToString(" ", remaining, len, maxlen, 0);
		}

		if ( len < maxlen )
		{
			remaining[len] = 0;
			SV_Cmd_ExecuteString(remaining);
		}
	}
	else if ( password[0] )
	{
		Com_Printf("Invalid password.\n");
	}
	else
	{
		Com_Printf("You must log in with 'rcon login <password>' before using 'rcon'.\n");
	}

	Com_EndRedirect();
}

void SV_MatchEnd(void)
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
================
SV_MasterAddress
================
*/
netadr_t *SV_MasterAddress()
{
	static netadr_t sv_master_address;
	unsigned short port;

	if ( sv_master_address.type == NA_BAD )
	{
		return &sv_master_address;
	}

	Com_Printf("Resolving %s\n", MASTER_SERVER_NAME);

	if ( !NET_StringToAdr(MASTER_SERVER_NAME, &sv_master_address) )
	{
		Com_Printf("Couldn't resolve address: %s\n", MASTER_SERVER_NAME);
		return &sv_master_address;
	}

	if ( !strstr(":", MASTER_SERVER_NAME) )
	{
		sv_master_address.port = BigShort(PORT_MASTER);
	}

	port = BigShort(sv_master_address.port);

	Com_Printf(
	    "%s resolved to %i.%i.%i.%i:%i\n",
	    MASTER_SERVER_NAME,
	    sv_master_address.ip[0],
	    sv_master_address.ip[1],
	    sv_master_address.ip[2],
	    sv_master_address.ip[3],
	    port);

	return &sv_master_address;
}

/*
=================
SV_MasterGameCompleteStatus
NERVE - SMF - Sends gameCompleteStatus messages to all master servers
=================
*/
void SV_MasterGameCompleteStatus()
{
	netadr_t *adr;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !com_dedicated || com_dedicated->current.integer != 2 )
	{
		return;     // only dedicated servers send master game status
	}

	adr = SV_MasterAddress();

	if ( adr->type != NA_BAD )
	{
		Com_Printf("Sending gameCompleteStatus to %s\n", MASTER_SERVER_NAME);
		SVC_GameCompleteStatus(*adr);
	}
}

void SV_MasterHeartbeat( const char *hbname )
{
	netadr_t *adr;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !com_dedicated || com_dedicated->current.integer != 2 )
	{
		return;     // only dedicated servers send heartbeats
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextHeartbeatTime )
	{
		svs.nextHeartbeatTime = svs.time + HEARTBEAT_MSEC;

		adr = SV_MasterAddress();

		if ( adr->type != NA_BAD )
		{
			Com_Printf("Sending heartbeat to %s\n", MASTER_SERVER_NAME);
			NET_OutOfBandPrint(NS_SERVER, *adr, va("heartbeat %s\n", hbname));
		}
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextStatusResponseTime )
	{
		svs.nextStatusResponseTime = svs.time + STATUS_MSEC;

		adr = SV_MasterAddress();

		if ( adr->type != NA_BAD )
		{
			SVC_Status(*adr);
		}
	}
}

/*
=================
SV_MasterShutdown

Informs all masters that this server is going down
=================
*/
void SV_MasterShutdown( void )
{
	// send a hearbeat right now
	svs.nextHeartbeatTime = 0x80000000;

#ifdef LIBCOD
	SV_MasterHeartbeat_libcod( HEARTBEAT_DEAD );
#else
	SV_MasterHeartbeat( HEARTBEAT_DEAD );               // NERVE - SMF - changed to flatline
#endif
}