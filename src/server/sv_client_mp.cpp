#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

/*
=================
SV_DelayDropClient
=================
*/
void SV_DelayDropClient( client_t *drop, const char *reason )
{
	assert(drop);
	assert(reason);
	assert(drop->state != CS_FREE);

	if ( drop->state == CS_ZOMBIE )
	{
		return; // already dropped
	}

	if ( drop->dropReason == NULL )
	{
		drop->dropReason = reason;
	}
}

/*
=================
SV_FindFreeTempBanSlot
=================
*/
unsigned int SV_FindFreeTempBanSlot()
{
	unsigned int oldestSlot;
	unsigned int banSlot;

	oldestSlot = 0;

	for ( banSlot = 0; banSlot < MAX_TEMPBAN_SLOTS; banSlot++ )
	{
		if ( !svs.tempBans[banSlot].guid )
			return banSlot;

		if ( svs.tempBans[banSlot].banTime < svs.tempBans[oldestSlot].banTime )
			oldestSlot = banSlot;
	}

	return oldestSlot;
}

/*
=================
SV_BanGuidBriefly
=================
*/
void SV_BanGuidBriefly( int guid )
{
	unsigned int slot;

	slot = SV_FindFreeTempBanSlot();

	svs.tempBans[slot].guid = guid;
	svs.tempBans[slot].banTime = svs.time;
}

/*
=================
SV_IsBannedGuid
=================
*/
bool SV_IsBannedGuid( int guid )
{
	bool banned;
	char *file;
	const char *token;
	const char *text;

	if ( !guid )
		return false;

	if ( FS_ReadFile(BAN_LIST_NAME, (void **)&file) < 0 )
		return false;

	text = file;
	banned = false;

	while ( 1 )
	{
		token = Com_Parse(&text);

		if ( !token[0] )
			break;

		if ( atoi(token) == guid )
		{
			banned = true;
			break;
		}

		Com_SkipRestOfLine(&text);
	}

	FS_FreeFile(file);

	return banned;
}

/*
=================
SV_IsTempBannedGuid
=================
*/
bool SV_IsTempBannedGuid( int guid )
{
	unsigned int banSlot;

	if ( !guid )
		return false;

	for ( banSlot = 0; banSlot < MAX_TEMPBAN_SLOTS; ++banSlot )
	{
		if ( svs.tempBans[banSlot].guid == guid
		        && (float)(svs.time - svs.tempBans[banSlot].banTime) <= (float)(sv_kickBanTime->current.decimal * 1000.0) )
		{
			return true;
		}
	}

	return false;
}

/*
=================
SV_FX_GetVisibility
=================
*/
float SV_FX_GetVisibility( const vec3_t start, const vec3_t end )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 1.0;
}

/*
==================
SV_FreeClientScriptPers
Free client .pers array
==================
*/
void SV_FreeClientScriptPers()
{
	client_t *cl;
	int i;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( cl->state < CS_CONNECTED )
			continue;

		SV_FreeClientScriptId(cl);
		cl->scriptId = Scr_AllocArray();
	}
}

/*
=================
SV_BanClient
=================
*/
void SV_BanClient( client_t *cl )
{
	int file;
	char cleanName[64];

	assert(cl);

	if ( cl->netchan.remoteAddress.type == NA_LOOPBACK )
	{
		SV_SendServerCommand(NULL, SV_CMD_CAN_IGNORE, "%c \"EXE_CANNOTKICKHOSTPLAYER\"", 101);
		return;
	}

	if ( !cl->guid )
	{
		Com_Printf("Can't ban user, GUID is 0\n");
		return;
	}

	if ( SV_IsBannedGuid(cl->guid) )
	{
		Com_Printf("This GUID (%i) is already banned\n", cl->guid);
		return;
	}

	if ( FS_FOpenFileByMode(BAN_LIST_NAME, &file, FS_APPEND) < 0 )
		return;

	I_strncpyz(cleanName, cl->name, sizeof(cleanName));
	I_CleanStr(cleanName);

	FS_Printf(file, "%i %s\r\n", cl->guid, cleanName);
	FS_FCloseFile(file);

	SV_DropClient(cl, "EXE_PLAYERKICKED");
	cl->lastPacketTime = svs.time;
}

/*
=================
SV_UnbanClient
=================
*/
void SV_UnbanClient( const char *name )
{
	int nameLen;
	char *file;
	int fileSize;
	int unban;
	char cleanName[64];
	char *line;
	const char *token;
	char *text;
	int found;

	fileSize = FS_ReadFile(BAN_LIST_NAME, (void **)&file);

	if ( fileSize < 0 )
		return;

	I_strncpyz(cleanName, name, sizeof(cleanName));
	I_CleanStr(cleanName);
	nameLen = &cleanName[strlen(cleanName) + 1] - &cleanName[1];
	found = 0;
	text = file;

	while ( 1 )
	{
		line = text;
		token = Com_Parse((const char **)&text);

		if ( !token[0] )
			break;

		while ( text[0] && text[0] <= ' ' )
			++text;

		unban = !I_strnicmp(text, cleanName, nameLen) && (text[nameLen] == 13 || text[nameLen] == 10);
		Com_SkipRestOfLine((const char **)&text);

		if ( unban )
		{
			++found;
			memmove((unsigned char *)line, (unsigned char *)text, fileSize - (text - file) + 1);
			fileSize -= text - line;
			text = line;
		}
	}

	FS_WriteFile(BAN_LIST_NAME, file, fileSize);
	FS_FreeFile(file);

	if ( found )
		Com_Printf("unbanned %i user(s) named %s\n", found, cleanName);
	else
		Com_Printf("no banned user has name %s\n", cleanName);
}

/*
=================
SV_UserinfoChanged

Pull specific info from a newly changed userinfo string
into a more C friendly form.
=================
*/
void SV_UserinfoChanged( client_t *cl )
{
	const char	*val;
	int		i;

	// name for C code
	Q_strncpyz( cl->name, Info_ValueForKey (cl->userinfo, "name"), sizeof(cl->name) );

	// rate command

	// if the client is on the same subnet as the server and we aren't running an
	// internet public server, assume they don't need a rate choke
	if ( Sys_IsLANAddress( cl->netchan.remoteAddress ) && com_dedicated->current.integer != 2 )
	{
		cl->rate = 99999;	// lans should not rate limit
	}
	else
	{
		val = Info_ValueForKey (cl->userinfo, "rate");
		if (strlen(val))
		{
			i = atoi(val);
			cl->rate = i;
			if (cl->rate < 1000)
			{
				cl->rate = 1000;
			}
			else if (cl->rate > 90000)
			{
				cl->rate = 90000;
			}
		}
		else
		{
			cl->rate = 5000;
		}
	}

	// snaps command
	val = Info_ValueForKey (cl->userinfo, "snaps");
	if (strlen(val))
	{
		i = atoi(val);
		if ( i < 1 )
		{
			i = 1;
		}
		else if ( i > 30 )
		{
			i = 30;
		}
		cl->snapshotMsec = 1000/i;
	}
	else
	{
		cl->snapshotMsec = 50;
	}

	// voice command
	val = Info_ValueForKey (cl->userinfo, "cl_voice");
	cl->sendVoice = atoi(val) > 0;
	if ( cl->rate < 5000 )
		cl->sendVoice = 0;

	// wwwdl command
	val = Info_ValueForKey (cl->userinfo, "cl_wwwDownload");
	cl->wwwOk = atoi(val) > 0;
}

/*
=================
SV_GetChallenge
A "getchallenge" OOB command has been received
Returns a challenge number that can be used
in a subsequent connectResponse command.
We do this to prevent denial of service attacks that
flood the server with invalid connection IPs.  With a
challenge, they must give a valid IP address.
If we are authorizing, a challenge request will cause a packet
to be sent to the authorize server.
When an authorizeip is returned, a challenge response will be
sent to that ip.
=================
*/
void SV_GetChallenge( netadr_t from )
{
	int i;
	int oldest;
	int oldestTime;
	challenge_t *challenge;

#if LIBCOD_COMPILE_RATELIMITER == 1
	extern leakyBucket_t outboundLeakyBucket;
	// Prevent using getchallenge as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow getchallenge to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SV_GetChallenge: rate limit exceeded, dropping request\n" );
		return;
	}
#endif

	oldest = 0;
	oldestTime = 0x7fffffff;

	// see if we already have a challenge for this ip
	challenge = &svs.challenges[0];
	for ( i = 0 ; i < MAX_CHALLENGES ; i++, challenge++ )
	{
		if ( !challenge->connected && NET_CompareAdr( from, challenge->adr ) )
		{
			break;
		}
		if ( challenge->time < oldestTime )
		{
			oldestTime = challenge->time;
			oldest = i;
		}
	}

	if ( i == MAX_CHALLENGES )
	{
		// this is the first time this client has asked for a challenge
		challenge = &svs.challenges[oldest];

		challenge->challenge = ( ( rand() << 16 ) ^ rand() ) ^ svs.time;
		challenge->adr = from;
		challenge->firstTime = svs.time;
		challenge->firstPing = 0;
		challenge->time = svs.time;
		challenge->connected = qfalse;
		i = oldest;
	}

	// if they are on a lan address, send the challengeResponse immediately
	if ( !net_lanauthorize->current.boolean && Sys_IsLANAddress(from) )
	{
		challenge->pingTime = svs.time;
		NET_OutOfBandPrint(NS_SERVER, from, va("challengeResponse %i", challenge->challenge));
		return;
	}

	// look up the authorize server's IP
	if ( !svs.authorizeAddress.ip[0] && svs.authorizeAddress.type != NA_BAD )
	{
		Com_Printf( "Resolving %s\n", AUTHORIZE_SERVER_NAME );
		if ( !NET_StringToAdr( AUTHORIZE_SERVER_NAME, &svs.authorizeAddress ) )
		{
			Com_Printf( "Couldn't resolve address\n" );
			return;
		}
		svs.authorizeAddress.port = BigShort( PORT_AUTHORIZE );
		Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", AUTHORIZE_SERVER_NAME,
		            svs.authorizeAddress.ip[0], svs.authorizeAddress.ip[1],
		            svs.authorizeAddress.ip[2], svs.authorizeAddress.ip[3],
		            BigShort( svs.authorizeAddress.port ) );
	}

	// if they have been challenging for a long time and we
	// haven't heard anything from the authoirze server, go ahead and
	// let them in, assuming the id server is down
	if ( svs.time - challenge->firstTime > AUTHORIZE_TIMEOUT )
	{
		netadr_t adr = *SV_MasterAddress();

		if (!NET_CompareAdr(from, adr))
		{
			Com_DPrintf( "authorize server timed out\n" );

			challenge->pingTime = svs.time;
			NET_OutOfBandPrint( NS_SERVER, challenge->adr, va("challengeResponse %i", challenge->challenge) );

			return;
		}
	}

	// otherwise send their ip to the authorize server
	SV_AuthorizeRequest(from, svs.challenges[i].challenge);
}

/*
==================
SV_ClientEnterWorld
==================
*/
void SV_ClientEnterWorld( client_t *client, usercmd_t *cmd )
{
	gentity_t *ent;
	int clientNum;

	Com_DPrintf("Going from CS_PRIMED to CS_ACTIVE for %s\n", client->name);
	client->state = CS_ACTIVE;

	// set up the entity for the client
	clientNum = client - svs.clients;
	assert(clientNum >= 0 && clientNum < MAX_CLIENTS);
	ent = SV_GentityNum(clientNum);
	ent->s.number = clientNum;
	client->gentity = ent;

	client->deltaMessage = -1;
	client->nextSnapshotTime = svs.time;    // generate a snapshot immediately
	client->lastUsercmd = *cmd;

	// call the game begin function
	ClientBegin(client - svs.clients);
}

/*
==================
SV_FreeClient
==================
*/
void SV_FreeClient( client_t *cl )
{
	assert(cl->state > CS_ZOMBIE);
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	// Kill any existing download
	SV_CloseDownload(cl);

	if ( SV_Loaded() )
	{
		ClientDisconnect(cl - svs.clients);
	}

	SV_SetUserinfo(cl - svs.clients, NULL);
	SV_FreeClientScriptId(cl);
}

/*
=====================
SV_DropClient

Called when the player is totally leaving the server, either willingly
or unwillingly.  This is NOT called if the entire server is quiting
or crashing -- SV_FinalCommand() will handle that
=====================
*/
void SV_DropClient( client_t *drop, const char *reason )
{
	challenge_t *challenge;
	int i;
	char name[MAX_NAME_LENGTH];

	assert(drop->state != CS_FREE);

	if ( drop->state == CS_ZOMBIE )
	{
		return;     // already dropped
	}

	assert(drop->state >= CS_ZOMBIE);

	// VoroN: Stock CoD2 bug fix: game attempts to use already freed name
	I_strncpyz(name, drop->name, sizeof(name));

	drop->dropReason = NULL;
	SV_FreeClient(drop);
	Com_DPrintf("Going to CS_ZOMBIE for %s\n", name);
	drop->state = CS_ZOMBIE;

	if ( !drop->gentity )
	{
		// see if we already have a challenge for this ip
		challenge = svs.challenges;

		for ( i = 0; i < MAX_CHALLENGES; ++i )
		{
			if ( NET_CompareAdr(drop->netchan.remoteAddress, challenge->adr) )
			{
				challenge->connected = qfalse;
				break;
			}

			challenge++;
		}
	}

	// tell everyone why they got dropped
	if ( I_stricmp(reason, "EXE_DISCONNECTED") )
	{
		// VoroN: another stock bug fix for broken disconnect msg due to absence of localizedstrings
		if ( !I_strnicmp(reason, "EXE", 3) || !I_strnicmp(reason, "GAME", 4) || !I_strnicmp(reason, "PC", 2) )
			SV_SendServerCommand(NULL, SV_CMD_CAN_IGNORE, "%c \"\x15%s^7 %s%s\"\0", 101, name, "\x14", reason);
		else
			SV_SendServerCommand(NULL, SV_CMD_CAN_IGNORE, "%c \"\x15%s^7 %s\"\0", 101, name, reason);
	}

	assert(drop - svs.clients >= 0 && drop - svs.clients < MAX_CLIENTS);
	Com_Printf("%i:%s %s\n", drop - svs.clients, name, reason);

	// add the disconnect command
	SV_SendServerCommand(NULL, SV_CMD_RELIABLE, "%c %d", 74, drop - svs.clients);
	SV_SendServerCommand(drop, SV_CMD_RELIABLE, "%c \"%s\"", 119, reason);

	// if this was the last client on the server, send a heartbeat
	// to the master so it is known the server is empty
	// send a heartbeat now so the master will get up to date info
	// if there is already a slot for this ip, reuse it
	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			break;
		}
	}

	if ( i == sv_maxclients->current.integer )
	{
		SV_Heartbeat_f();
	}
}

/*
==================
SV_DirectConnect
A "connect" OOB command has been received
==================
*/
void SV_DirectConnect( netadr_t from )
{
	char userinfo[MAX_INFO_STRING];
	int i;
	client_t    *cl, *newcl;
	MAC_STATIC client_t temp;
	gentity_t *ent;
	int clientNum;
	int version;
	int qport;
	int challenge;
	const char *password;
	int startIndex;
	const char *denied;
	int count;
	int guid;

	Com_DPrintf("SV_DirectConnect()\n");
	Q_strncpyz( userinfo, Cmd_Argv( 1 ), sizeof( userinfo ) );

	// DHM - Nerve :: Update Server allows any protocol to connect
	// NOTE TTimo: but we might need to store the protocol around for potential non http/ftp clients
	version = atoi( Info_ValueForKey( userinfo, "protocol" ) );

#if PROTOCOL_VERSION == 115 and defined LIBCOD
	if ( version < 115 || version > 119)
#else
	if ( version != PROTOCOL_VERSION )
#endif
	{
		NET_OutOfBandPrint( NS_SERVER, from, va("error\nEXE_SERVER_IS_DIFFERENT_VER\x15%s\n", PRODUCT_VERSION) );
		Com_DPrintf( "    rejected connect from protocol version %i (should be %i)\n", version, PROTOCOL_VERSION );
		return;
	}

	challenge = atoi( Info_ValueForKey( userinfo, "challenge" ) );
	qport = atoi( Info_ValueForKey( userinfo, "qport" ) );

	// quick reject
	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( NET_CompareBaseAdr( from, cl->netchan.remoteAddress )
		        && ( cl->netchan.qport == qport
		             || from.port == cl->netchan.remoteAddress.port ) )
		{
			if ( ( svs.time - cl->lastConnectTime )
			        < ( sv_reconnectlimit->current.integer * 1000 ) )
			{
				Com_DPrintf( "%s:reconnect rejected : too soon\n", NET_AdrToString( from ) );
				return;
			}
			break;
		}
	}

	guid = 0;

	// see if the challenge is valid (local clients don't need to challenge)
	if ( !NET_IsLocalAddress( from ) )
	{
		int ping;

		for ( i = 0 ; i < MAX_CHALLENGES ; i++ )
		{
			if ( NET_CompareAdr( from, svs.challenges[i].adr ) )
			{
				if ( challenge == svs.challenges[i].challenge )
				{
					guid = svs.challenges[i].guid;
					break;
				}
			}
		}
		if ( i == MAX_CHALLENGES )
		{
			NET_OutOfBandPrint( NS_SERVER, from, "error\nEXE_BAD_CHALLENGE" );
			return;
		}

		if ( svs.challenges[i].firstPing == 0 )
		{
			ping = svs.time - svs.challenges[i].pingTime;
			svs.challenges[i].firstPing = ping;
		}
		else
		{
			ping = svs.challenges[i].firstPing;
		}

		Com_Printf( "Client %i connecting with %i challenge ping from %s\n", i, ping, NET_AdrToString(from) );
		svs.challenges[i].connected = qtrue;

		// never reject a LAN client based on ping
		if ( !Sys_IsLANAddress( from ) )
		{
			if ( sv_minPing->current.integer && ping < sv_minPing->current.integer )
			{
				NET_OutOfBandPrint( NS_SERVER, from, "error\nEXE_ERR_HIGH_PING_ONLY" );
				Com_DPrintf( "Client %i rejected on a too low ping\n", i );
				return;
			}

			if ( sv_maxPing->current.integer && ping > sv_maxPing->current.integer )
			{
				NET_OutOfBandPrint( NS_SERVER, from, "error\nEXE_ERR_LOW_PING_ONLY" );
				Com_DPrintf( "Client %i rejected on a too high ping: %i\n", i, ping );
				return;
			}
		}
	}

	newcl = &temp;
	memset( newcl, 0, sizeof( client_t ) );

	// if there is already a slot for this ip, reuse it
	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( cl->state == CS_FREE )
		{
			continue;
		}
		if ( NET_CompareBaseAdr( from, cl->netchan.remoteAddress )
		        && ( cl->netchan.qport == qport
		             || from.port == cl->netchan.remoteAddress.port ) )
		{
			Com_Printf( "%s:reconnect\n", NET_AdrToString( from ) );

			if ( cl->state > CS_ZOMBIE )
				SV_FreeClient(cl);

			newcl = cl;
			memset(cl, 0, sizeof(client_t));

			goto gotnewcl;
		}
	}

	// find a client slot
	// if "sv_privateClients" is set > 0, then that number
	// of client slots will be reserved for connections that
	// have "password" set to the value of "sv_privatePassword"
	// Info requests will report the maxclients as if the private
	// slots didn't exist, to prevent people from trying to connect
	// to a full server.
	// This is to allow us to reserve a couple slots here on our
	// servers so we can play without having to kick people.

	// check for privateClient password
	password = Info_ValueForKey( userinfo, "password" );
	if ( !strcmp( password, sv_privatePassword->current.string ) )
	{
		startIndex = 0;
	}
	else
	{
		// skip past the reserved slots
		startIndex = sv_privateClients->current.integer;
	}

	newcl = NULL;
	for ( i = startIndex; i < sv_maxclients->current.integer ; i++ )
	{
		cl = &svs.clients[i];
		if ( cl->state == CS_FREE )
		{
			newcl = cl;
			break;
		}
	}

	if ( !newcl )
	{
		NET_OutOfBandPrint( NS_SERVER, from, "error\nEXE_SERVERISFULL" );
		Com_DPrintf( "Rejected a connection.\n" );
		return;
	}

	// we got a newcl, so reset the reliableSequence and reliableAcknowledge
	cl->reliableAcknowledge = 0;
	cl->reliableSequence = 0;

	memset( newcl, 0, sizeof( client_t ) );

gotnewcl:
	// build a new connection
	// accept the new client
	// this is the only place a client_t is ever initialized
	*newcl = temp;
	clientNum = newcl - svs.clients;
	ent = SV_GentityNum( clientNum );
	newcl->gentity = ent;
	assert(ent->s.clientNum == ent->s.number);
	assert(!newcl->scriptId);
	newcl->scriptId = Scr_AllocArray();

	// save the challenge
	newcl->challenge = challenge;
	newcl->guid = guid;

	// save the address
	Netchan_Setup(NS_SERVER, &newcl->netchan, from, qport);
#ifdef LIBCOD
	newcl->netchan.protocol = version;
#endif
	// init the netchan queue
	newcl->voicePacketCount = 0;
	newcl->sendVoice = 1;

	// save the userinfo
	Q_strncpyz( newcl->userinfo, userinfo, sizeof( newcl->userinfo ) );

	// get the game a chance to reject this connection or modify the userinfo
	denied = ClientConnect(clientNum, newcl->scriptId);

	if ( denied )
	{
		// we can't just use VM_ArgPtr, because that is only valid inside a VM_Call
		NET_OutOfBandPrint(NS_SERVER, from, va("error\n%s", denied));
		Com_DPrintf("Game rejected a connection: %s.\n", denied);
		SV_FreeClientScriptId(newcl);
		return;
	}

	Com_Printf("Going from CS_FREE to CS_CONNECTED for %s (num %i guid %i)\n", newcl->name, clientNum, newcl->guid);

	newcl->state = CS_CONNECTED;
	newcl->nextSnapshotTime = svs.time;
	newcl->lastPacketTime = svs.time;
	newcl->lastConnectTime = svs.time;

	SV_UserinfoChanged( newcl );

	// DHM - Nerve :: Clear out firstPing now that client is connected
	svs.challenges[i].firstPing = 0;

	// send the connect packet to the client
	NET_OutOfBandPrint( NS_SERVER, from, "connectResponse" );

	// when we receive the first packet from the client, we will
	// notice that it is from a different serverid and that the
	// gamestate message was not just sent, forcing a retransmit
	newcl->gamestateMessageNum = -1;

	// if this was the first client on the server, or the last client
	// the server can hold, send a heartbeat to the master.
	count = 0;
	for ( i = 0,cl = svs.clients ; i < sv_maxclients->current.integer ; i++,cl++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			count++;
		}
	}
	if ( count == 1 || count == sv_maxclients->current.integer )
	{
		SV_Heartbeat_f();
	}
}

/*
==================
SV_FreeClients
==================
*/
void SV_FreeClients()
{
	int i;
	client_t *cl;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		if ( cl->state < CS_CONNECTED )
			continue;

		SV_FreeClient(cl);
	}

	Z_Free(svs.clients);
}

/*
====================
SV_AuthorizeIpPacket

A packet has been returned from the authorize server.
If we have a challenge adr for that ip, send the
challengeResponse to it
====================
*/
void SV_AuthorizeIpPacket( netadr_t from )
{
	int challenge;
	int i;
	const char    *s;
	const char    *r;
	char ret[1024];

	if ( !NET_CompareBaseAdr( from, svs.authorizeAddress ) )
	{
		Com_Printf( "SV_AuthorizeIpPacket: not from authorize server\n" );
		return;
	}

	challenge = atoi( SV_Cmd_Argv( 1 ) );

	for ( i = 0 ; i < MAX_CHALLENGES ; i++ )
	{
		if ( svs.challenges[i].challenge == challenge )
		{
			break;
		}
	}

	if ( i == MAX_CHALLENGES )
	{
		Com_Printf( "SV_AuthorizeIpPacket: challenge not found\n" );
		return;
	}

	// send a packet back to the original client
	svs.challenges[i].pingTime = svs.time;

	s = SV_Cmd_Argv( 2 );
	r = SV_Cmd_Argv( 3 );

#ifdef LIBCOD
	if (sv_cracked->current.boolean)
	{
		s = "accept";
		r = "KEY_IS_GOOD";
	}
#endif

	if ( !Q_stricmp( s, "demo" ) )
	{
		if ( Dvar_GetBool( "fs_restrict" ) )
		{
			// a demo client connecting to a demo server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr,
			                    va("challengeResponse %i", svs.challenges[i].challenge) );
			return;
		}
		// they are a demo client trying to connect to a real server
		NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_NOT_A_DEMO_SERVER" );
		// clear the challenge record so it won't timeout and let them through
		memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
		return;
	}

	if ( !Q_stricmp( s, "accept" ) )
	{
		svs.challenges[i].guid = atoi(SV_Cmd_Argv( 4 ));

#ifdef LIBCOD
		if ( SV_IsBannedIp(svs.challenges[i].adr) )
		{
			Com_Printf("rejected connection from permanently banned IP %s\n", NET_AdrToStringNoPort(svs.challenges[i].adr));
			// they are a demo client trying to connect to a real server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\n\x15You are permanently banned from this server" );
			// clear the challenge record so it won't timeout and let them through
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}
#endif

		if ( SV_IsBannedGuid(svs.challenges[i].guid) )
		{
			Com_Printf("rejected connection from permanently banned GUID %i\n", svs.challenges[i].guid);
			// they are a demo client trying to connect to a real server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\n\x15You are permanently banned from this server" );
			// clear the challenge record so it won't timeout and let them through
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}

		if ( SV_IsTempBannedGuid(svs.challenges[i].guid) )
		{
			Com_Printf("rejected connection from temporarily banned GUID %i\n", svs.challenges[i].guid);
			// they are a demo client trying to connect to a real server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\n\x15You are temporarily banned from this server" );
			// clear the challenge record so it won't timeout and let them through
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}

		if ( !svs.challenges[i].connected )
		{
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr,
			                    va("challengeResponse %i", svs.challenges[i].challenge) );
			return;
		}

		return;
	}

	// authorization denied
	if ( Q_stricmp( s, "deny" ) )
	{
		if ( !r || !r[0] )
		{
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_BAD_CDKEY");
		}
		else
		{
			snprintf(ret, sizeof(ret), "error\n%s", r);
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, ret);
		}

		memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
		return;
	}

	// authorization failed
	if ( !r || !r[0] )
	{
		NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_CDKEY_IN_USE");
		memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
		return;
	}
	else
	{
		if ( !Q_stricmp(r, "CLIENT_UNKNOWN_TO_AUTH") || !Q_stricmp(r, "BAD_CDKEY") )
		{
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "needcdkey");
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}

		if ( !Q_stricmp(r, "INVALID_CDKEY") || !Q_stricmp(r, "BANNED_CDKEY") )
		{
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_BAD_CDKEY");
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}
	}
}

/*
=================
SV_WWWRedirectClient
=================
*/
void SV_WWWRedirectClient( client_t *cl, msg_t *msg )
{
	int downloadSize;
	fileHandle_t handle;
	int download_flag = 0;

	downloadSize = FS_SV_FOpenFileRead(cl->downloadName, &handle);

	// that should NOT happen - even regular download would fail then anyway
	if ( !downloadSize )
	{
		Com_Printf("ERROR: Client '%s': couldn't extract file size for %s\n", cl->name, cl->downloadName);
		return;
	}

	FS_FCloseFile(handle); // don't keep open, we only care about the size

	I_strncpyz(cl->downloadURL, va("%s/%s", sv_wwwBaseURL->current.string, cl->downloadName), sizeof(cl->downloadURL));
	Com_Printf("Redirecting client '%s' to %s\n", cl->name, cl->downloadURL);

	// once cl->downloadName is set (and possibly we have our listening socket), let the client know
	cl->downloadingWWW = qtrue;
	MSG_WriteByte( msg, svc_download );
	MSG_WriteShort( msg, -1 ); // block -1 means ftp/http download
	// compatible with legacy svc_download protocol: [size] [size bytes]
	// download URL, size of the download file, download flags
	MSG_WriteString( msg, cl->downloadURL );
	MSG_WriteLong( msg, downloadSize );
	if ( sv_wwwDlDisconnected->current.boolean )
	{
		download_flag = 1;
	}
	MSG_WriteLong( msg, download_flag ); // flags
	*cl->downloadName = 0;
}

/*
==================
SV_WriteDownloadToClient
Check to see if the client wants a file, open it if needed and start pumping the client
Fill up msg with data
==================
*/
void SV_WriteDownloadToClient( client_t *cl, msg_t *msg )
{
	int curindex;
	int rate;
	int blockspersnap;
	int iwdFile;
	char errorMessage[MAX_STRING_CHARS];

	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	if ( cl->state == CS_ACTIVE )
	{
		return; // Client already in game
	}
	if ( !*cl->downloadName )
	{
		return; // Nothing being downloaded
	}
	if ( cl->clientDownloadingWWW )
	{
		return; // The client acked and is downloading with ftp/http
	}
	// CVE-2006-2082
	// validate the download against the list of iwd files
	if ( !FS_VerifyIwd( cl->downloadName ) )
	{
		// will drop the client and leave it hanging on the other side. good for him
		SV_DropClient( cl, "illegal download request" );
		return;
	}

#ifdef LIBCOD
	if (*sv_downloadMessage->current.string)
	{
		Com_sprintf(errorMessage, sizeof(errorMessage), sv_downloadMessage->current.string);
		MSG_WriteByte( msg, svc_download );
		MSG_WriteShort( msg, 0 ); // client is expecting block zero
		MSG_WriteLong( msg, -1 ); // illegal file size
		MSG_WriteString( msg, errorMessage );
		*cl->downloadName = 0;
		return;
	}
#endif

	// www download redirect protocol
	// NOTE: this is called repeatedly while a client connects. Maybe we should sort of cache the message or something
	// FIXME: we need to abstract this to an independant module for maximum configuration/usability by server admins
	// FIXME: I could rework that, it's crappy
	if (sv_wwwDownload->current.boolean && cl->wwwOk)
	{
		if (!cl->wwwFallback)
		{
			SV_WWWRedirectClient(cl, msg);
			return;
		}
	}

	if (!cl->download)
	{
		// We open the file here

		Com_Printf( "clientDownload: %d : begining \"%s\"\n", cl - svs.clients, cl->downloadName );

		iwdFile = FS_iwIwd(cl->downloadName, BASEGAME);

		if ( !sv_allowDownload->current.boolean || iwdFile ||
		        ( cl->downloadSize = FS_SV_FOpenFileRead( cl->downloadName, &cl->download ) ) <= 0 )
		{
			// cannot auto-download file
			if (iwdFile)
			{
				Com_Printf("clientDownload: %d : \"%s\" cannot download iwd files\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEIWD\x15%s", cl->downloadName);
			}
			else if ( !sv_allowDownload->current.boolean )
			{
				Com_Printf("clientDownload: %d : \"%s\" download disabled", cl - svs.clients, cl->downloadName);
				if (sv_pure->current.boolean)
				{
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED_PURE\x15%s", cl->downloadName);
				}
				else
				{
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED\x15%s", cl->downloadName);
				}
			}
			else
			{
				// NOTE TTimo this is NOT supposed to happen unless bug in our filesystem scheme?
				//   if the iwd is referenced, it must have been found somewhere in the filesystem
				Com_Printf("clientDownload: %d : \"%s\" file not found on server\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
			}
			MSG_WriteByte( msg, svc_download );
			MSG_WriteShort( msg, 0 ); // client is expecting block zero
			MSG_WriteLong( msg, -1 ); // illegal file size
			MSG_WriteString( msg, errorMessage );

			*cl->downloadName = 0;
			return;
		}

		// Init
		cl->downloadCurrentBlock = cl->downloadClientBlock = cl->downloadXmitBlock = 0;
		cl->downloadCount = 0;
		cl->downloadEOF = qfalse;
	}

	// Perform any reads that we need to
	while (cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW &&
	        cl->downloadSize != cl->downloadCount)
	{

		curindex = (cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW);

		if (!cl->downloadBlocks[curindex])
			cl->downloadBlocks[curindex] = (unsigned char *)Z_Malloc( MAX_DOWNLOAD_BLKSIZE );

		cl->downloadBlockSize[curindex] = FS_Read( cl->downloadBlocks[curindex], MAX_DOWNLOAD_BLKSIZE, cl->download );

		if (cl->downloadBlockSize[curindex] < 0)
		{
			// EOF right now
			cl->downloadCount = cl->downloadSize;
			break;
		}

		cl->downloadCount += cl->downloadBlockSize[curindex];

		// Load in next block
		cl->downloadCurrentBlock++;
	}

	// Check to see if we have eof condition and add the EOF block
	if (cl->downloadCount == cl->downloadSize &&
	        !cl->downloadEOF &&
	        cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW)
	{

		cl->downloadBlockSize[cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW] = 0;
		cl->downloadCurrentBlock++;

		cl->downloadEOF = qtrue;  // We have added the EOF block
	}

	// Loop up to window size times based on how many blocks we can fit in the
	// client snapMsec and rate

	// based on the rate, how many bytes can we fit in the snapMsec time of the client
	// normal rate / snapshotMsec calculation
	rate = cl->rate;

	if ( sv_maxRate->current.integer )
	{
		if ( sv_maxRate->current.integer < 1000 )
		{
			Dvar_SetInt( sv_maxRate, 1000 );
		}
		if ( sv_maxRate->current.integer < rate )
		{
			rate = sv_maxRate->current.integer;
		}
	}

	if (!rate)
	{
		blockspersnap = 1;
	}
	else
	{
		blockspersnap = ( (rate * cl->snapshotMsec) / 1000 + MAX_DOWNLOAD_BLKSIZE ) / MAX_DOWNLOAD_BLKSIZE;
	}

	if (blockspersnap < 0)
		blockspersnap = 1;

	if (blockspersnap > 7) // 2048 block size * 8 == 16384 is a max msg len, tldr can't send more than 7 blocks.
		blockspersnap = 7; // fixes crash when client set snaps value too low

#ifdef LIBCOD
	if (sv_fastDownload->current.boolean)
		blockspersnap = 1; // always one block for fast dl
#endif

	while (blockspersnap--)
	{
		// Write out the next section of the file, if we have already reached our window,
		// automatically start retransmitting

		if (cl->downloadClientBlock == cl->downloadCurrentBlock)
			return; // Nothing to transmit

		if (cl->downloadXmitBlock == cl->downloadCurrentBlock)
		{
			// We have transmitted the complete window, should we start resending?

			//FIXME:  This uses a hardcoded one second timeout for lost blocks
			//the timeout should be based on client rate somehow
			if (svs.time - cl->downloadSendTime > 1000)
				cl->downloadXmitBlock = cl->downloadClientBlock;
			else
				return;
		}

		// Send current block
		curindex = (cl->downloadXmitBlock % MAX_DOWNLOAD_WINDOW);

		MSG_WriteByte( msg, svc_download );
		MSG_WriteShort( msg, cl->downloadXmitBlock );

		// block zero is special, contains file size
		if ( cl->downloadXmitBlock == 0 )
			MSG_WriteLong( msg, cl->downloadSize );

		MSG_WriteShort( msg, cl->downloadBlockSize[curindex] );

		// Write the block
		if ( cl->downloadBlockSize[curindex] )
		{
			MSG_WriteData( msg, cl->downloadBlocks[curindex], cl->downloadBlockSize[curindex] );
		}

		Com_DPrintf( "clientDownload: %d : writing block %d\n", cl - svs.clients, cl->downloadXmitBlock );

		// Move on to the next block
		// It will get sent with next snap shot.  The rate will keep us in line.
		cl->downloadXmitBlock++;
		cl->downloadSendTime = svs.time;
	}
}

/*
==================
SV_CloseDownload
clear/free any download vars
==================
*/
void SV_CloseDownload( client_t *cl )
{
	int i;

	// EOF
	if (cl->download)
	{
		FS_FCloseFile( cl->download );
	}

	cl->download = 0;
	*cl->downloadName = 0;

	// Free the temporary buffer space
	for (i = 0; i < MAX_DOWNLOAD_WINDOW; i++)
	{
		if (cl->downloadBlocks[i])
		{
			Z_Free( cl->downloadBlocks[i] );
			cl->downloadBlocks[i] = NULL;
		}
	}
}

/*
==================
SV_ClientThink

Also called by bot code
==================
*/
void SV_ClientThink( client_t *cl, usercmd_t *cmd )
{
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);
	cl->lastUsercmd = *cmd;

	if ( cl->state != CS_ACTIVE )
	{
		return;     // may have been kicked during the last usercmd
	}

	G_SetLastServerTime(cl - svs.clients, cmd->serverTime);
	ClientThink(cl - svs.clients);
}

/*
==================
SV_UserMove
The message usually contains all the movement commands
that were in the last three packets, so that the information
in dropped packets can be recovered.
On very fast clients, there may be multiple usercmd packed into
each of the backup packets.
==================
*/
void SV_UserMove( client_t *cl, msg_t *msg, qboolean delta )
{
	int i, key;
	int cmdCount;
	usercmd_t nullcmd;
	usercmd_t cmds[MAX_PACKET_USERCMDS];
	usercmd_t   *cmd, *oldcmd;
	playerState_t *ps;

	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	if ( delta )
	{
		cl->deltaMessage = cl->messageAcknowledge;
	}
	else
	{
		cl->deltaMessage = -1;
	}

	if ( cl->reliableSequence - cl->reliableAcknowledge > MAX_RELIABLE_COMMANDS - 1 )
	{
		return;
	}

	cmdCount = MSG_ReadByte( msg );

	if ( cmdCount < 1 )
	{
		Com_Printf( "cmdCount < 1\n" );
		return;
	}

	if ( cmdCount > MAX_PACKET_USERCMDS )
	{
		Com_Printf( "cmdCount > MAX_PACKET_USERCMDS\n" );
		return;
	}

	// use the checksum feed in the key
	key = sv.checksumFeed;
	// also use the message acknowledge
	key ^= cl->messageAcknowledge;
	// also use the last acknowledged server command in the key
	key ^= Com_HashKey( cl->reliableCommandInfo[ cl->reliableAcknowledge & ( MAX_RELIABLE_COMMANDS - 1 ) ].cmd, 32 );
	ps = SV_GameClientNum(cl - svs.clients);
	assert(ps);
	assert(BG_ValidateWeaponNumber( ps->weapon ));
	MSG_SetDefaultUserCmd(ps, &nullcmd);
	oldcmd = &nullcmd;

	for ( i = 0 ; i < cmdCount ; i++ )
	{
		cmd = &cmds[i];
		MSG_ReadDeltaUsercmdKey( msg, key, oldcmd, cmd );
		if ( !BG_IsWeaponValid(ps, cmd->weapon) )
			cmd->weapon = ps->weapon;
		assert(ps->offHandIndex == static_cast<byte>( ps->offHandIndex ));
		if ( !BG_IsWeaponValid(ps, cmd->offHandIndex) )
			cmd->offHandIndex = ps->offHandIndex;
		oldcmd = cmd;
	}

	// save time for ping calculation
	cl->frames[ cl->messageAcknowledge & PACKET_MASK ].messageAcked = svs.time;

	// if this is the first usercmd we have received
	// this gamestate, put the client into the world
	if ( cl->state == CS_PRIMED )
	{
		SV_ClientEnterWorld( cl, cmds );
		// the moves can be processed normaly
	}

	// a bad cp command was sent, drop the client
	if ( sv_pure->current.boolean != 0 && cl->pureAuthentic == 0 )
	{
		SV_DropClient( cl, "EXE_CANNOTVALIDATEPURECLIENT" );
		return;
	}

	if ( cl->state != CS_ACTIVE )
	{
		cl->deltaMessage = -1;
		return;
	}

	// usually, the first couple commands will be duplicates
	// of ones we have previously received, but the servertimes
	// in the commands will cause them to be immediately discarded
	for ( i =  0 ; i < cmdCount ; i++ )
	{
		// if this is a cmd from before a map_restart ignore it
		if ( cmds[i].serverTime > cmds[cmdCount-1].serverTime )
		{
			continue;
		}
		// extremely lagged or cmd from before a map_restart
		//if ( cmds[i].serverTime > svs.time + 3000 ) {
		//	continue;
		//}
		// don't execute if this is an old cmd which is already executed
		// these old cmds are included when cl_packetdup > 0
		if ( cmds[i].serverTime <= cl->lastUsercmd.serverTime )
		{
			continue;
		}
#if LIBCOD_COMPILE_PLAYER == 1
		SV_ClientThink_libcod(cl, &cmds[ i ]);
#else
		SV_ClientThink(cl, &cmds[ i ]);
#endif
	}
}

/*
==================
SV_BotUserMove
==================
*/
void SV_BotUserMove( client_t *cl )
{
	usercmd_t ucmd;

	if ( cl->gentity == NULL )
	{
		return;
	}

	memset(&ucmd, 0, sizeof(ucmd));

	ucmd.weapon = SV_GameClientNum(cl - svs.clients)->weapon;

	if ( !G_GetClientArchiveTime(cl - svs.clients) )
	{
		if ( G_random() < 0.5 )
			ucmd.buttons |= BUTTON_ATTACK;

		if ( G_random() < 0.5 )
			ucmd.buttons |= (BUTTON_USE | BUTTON_USERELOAD);

		if ( G_random() < 0.33000001 )
		{
			ucmd.forwardmove = BUTTON_FORWARD;
		}
		else
		{
			if ( G_random() < 0.5 )
				ucmd.forwardmove = BUTTON_BACK;
		}

		if ( G_random() < 0.33000001 )
		{
			ucmd.rightmove = BUTTON_FORWARD;
		}
		else
		{
			if ( G_random() < 0.5 )
				ucmd.rightmove = BUTTON_BACK;
		}

		if ( G_random() < 0.33000001 )
			ucmd.angles[0] = (int)(G_crandom() * 360.0);

		if ( G_random() < 0.33000001 )
			ucmd.angles[1] = (int)(G_crandom() * 360.0);

		if ( G_random() < 0.33000001 )
			ucmd.angles[2] = (int)(G_crandom() * 360.0);
	}

	cl->deltaMessage = cl->netchan.outgoingSequence - 1;
#if LIBCOD_COMPILE_PLAYER == 1
	SV_ClientThink_libcod(cl, &ucmd);
#else
	SV_ClientThink(cl, &ucmd);
#endif
}

/*
===================
SV_AddTestClient
===================
*/
int botport = 0;
gentity_t *SV_AddTestClient()
{
	netadr_t adr;
	usercmd_t nullcmd;
	client_t *client;
	char userinfo[MAX_INFO_STRING];

	int num = 0;

	for ( client = svs.clients; num < sv_maxclients->current.integer && client->state; client++ )
	{
		num++;
	}

	if ( num == sv_maxclients->current.integer )
	{
		return NULL;
	}

	snprintf(
	    userinfo,
		sizeof(userinfo),
	    "connect \"\\cg_predictItems\\1\\cl_anonymous\\0\\color\\4\\head\\default\\model\\multi\\snaps\\20\\rate\\5000\\name\\"
	    "bot%d\\password\\%s\\protocol\\%d\"",
	    botport,
	    g_password->current.string,
	    PROTOCOL_VERSION);

	SV_Cmd_TokenizeString(userinfo);
	memset(&adr, 0, sizeof(adr));

	adr.type = NA_BOT;
	adr.port = botport++;

	SV_DirectConnect(adr);
	num = 0;

	for ( client = svs.clients; num < sv_maxclients->current.integer && (client->state == CS_FREE || !NET_CompareBaseAdr(adr, client->netchan.remoteAddress)); client++ )
	{
		num++;
	}

	if ( num == sv_maxclients->current.integer )
	{
		return NULL;
	}

	client->bIsTestClient = qtrue;
	SV_SendClientGameState(client);
	memset(&nullcmd, 0, sizeof(nullcmd));
	SV_ClientEnterWorld(client, &nullcmd);

	return SV_GentityNum(num);
}

/*
================
SV_SendClientGameState
Sends the first message from the server to a connected client.
This will be sent on the initial connection and upon each new map load.
It will be resent if the client acknowledges a later message but has
the wrong gamestate.
================
*/
#if LIBCOD_COMPILE_PLAYER == 1
int gamestate_size[MAX_CLIENTS] = {0};
#endif
void SV_SendClientGameState( client_t *client )
{
	int start;
	entityState_t   *base, nullstate;
	msg_t msg;
	byte msgBuffer[MAX_MSGLEN];

	assert(client - svs.clients >= 0 && client - svs.clients < MAX_CLIENTS);

	while ( client->state && client->netchan.unsentFragments )
	{
		SV_Netchan_TransmitNextFragment(&client->netchan);
	}

	Com_DPrintf( "SV_SendClientGameState() for %s\n", client->name );
	Com_DPrintf( "Going from CS_CONNECTED to CS_PRIMED for %s\n", client->name );
	client->state = CS_PRIMED;
	client->pureAuthentic = 0;

	// when we receive the first packet from the client, we will
	// notice that it is from a different serverid and that the
	// gamestate message was not just sent, forcing a retransmit
	client->gamestateMessageNum = client->netchan.outgoingSequence;

	MSG_Init( &msg, msgBuffer, sizeof( msgBuffer ) );

	// NOTE, MRE: all server->client messages now acknowledge
	// let the client know which reliable clientCommands we have received
	MSG_WriteLong( &msg, client->lastClientCommand );

	// send any server commands waiting to be sent first.
	// we have to do this cause we send the client->reliableSequence
	// with a gamestate and it sets the clc.serverCommandSequence at
	// the client side
	SV_UpdateServerCommandsToClient( client, &msg );

	// send the gamestate
	MSG_WriteByte( &msg, svc_gamestate );
	MSG_WriteLong( &msg, client->reliableSequence );

	// write the configstrings
	for ( start = 0 ; start < MAX_CONFIGSTRINGS ; start++ )
	{
		if ( sv.configstrings[start][0] )
		{
			MSG_WriteByte( &msg, svc_configstring );
			MSG_WriteShort( &msg, start );
#if PROTOCOL_VERSION == 115 and defined LIBCOD
			MSG_WriteBigString( &msg, SV_ModifyConfigstringIwdChkSum( client, start ) );
#else
			MSG_WriteBigString( &msg, sv.configstrings[start] );
#endif
		}
	}

	// write the baselines
	memset( &nullstate, 0, sizeof( nullstate ) );

	for ( start = 0 ; start < MAX_GENTITIES; start++ )
	{
		base = &sv.svEntities[start].baseline.s;
		if ( !base->number )
		{
			continue;
		}
		MSG_WriteByte( &msg, svc_baseline );
		MSG_WriteDeltaEntity( &msg, &nullstate, base, qtrue );
	}

	MSG_WriteByte( &msg, svc_EOF );
	MSG_WriteLong( &msg, client - svs.clients );

	// write the checksum feed
	MSG_WriteLong( &msg, sv.checksumFeed );
	MSG_WriteByte( &msg, svc_EOF );

	// NERVE - SMF - debug info
	Com_DPrintf( "Sending %i bytes in gamestate to client: %i\n", msg.cursize, client - svs.clients );
#if LIBCOD_COMPILE_PLAYER == 1
	gamestate_size[client - svs.clients] = msg.cursize;
#endif
	// deliver this to the client
	SV_SendMessageToClient( &msg, client );
}

/*
=================
SV_AuthorizeRequest
=================
*/
void SV_AuthorizeRequest( netadr_t from, int challenge )
{
	char gameDir[MAX_STRING_CHARS];
	dvar_t *fs_game;

	if ( svs.authorizeAddress.type == NA_BAD )
		return;

	gameDir[0] = '\0';
	fs_game = Dvar_RegisterString("fs_game", "", DVAR_SERVERINFO | DVAR_SYSTEMINFO | DVAR_INIT | DVAR_CHANGEABLE_RESET);

	if ( fs_game )
	{
		if ( fs_game->current.string[0] )
			I_strncpyz(gameDir, fs_game->current.string, sizeof(gameDir));
	}

	Com_DPrintf("sending getIpAuthorize for %s\n", NET_AdrToString(from));
	NET_OutOfBandPrint(NS_SERVER, svs.authorizeAddress, va("getIpAuthorize %i %i.%i.%i.%i %s %i", challenge, from.ip[0], from.ip[1], from.ip[2], from.ip[3], gameDir, Dvar_GetBool("sv_allowAnonymous")));
}

/*
=================
SV_VerifyIwds_f

If we are pure, disconnect the client if they do no meet the following conditions:

1. the first two checksums match our view of cgame and ui DLLs
   Wolf specific: the checksum is the checksum of the iwd we found the DLL in
2. there are no any additional checksums that we do not have

This routine would be a bit simpler with a goto but i abstained

=================
*/
static void SV_VerifyIwds_f( client_t *cl )
{
	int nChkSum1, nChkSum2, nClientPaks, nServerPaks, i, j, nCurArg;
	int nClientChkSum[1024];
	int nServerChkSum[1024];
	const char *pPaks, *pArg;
	qboolean bGood = qtrue;

#if PROTOCOL_VERSION == 115 and defined LIBCOD
	if ( cl->netchan.protocol == 119 )
	{
		cl->pureAuthentic = 1;
		return;
	}
#endif

	bGood = qtrue;
	nChkSum1 = nChkSum2 = 0;
	nClientPaks = Cmd_Argc();

	// start at arg 2 ( skip serverId cl_paks )
	nCurArg = 1;

	// we basically use this while loop to avoid using 'goto' :)
	while ( bGood )
	{
		// must be at least 2: "cl_paks cgame ui @ firstref ... numChecksums"
		// numChecksums is encoded
		if ( nClientPaks < 2 )
		{
			bGood = qfalse;
			break;
		}
		// should be sitting at the delimeter now
		pArg = Cmd_Argv( nCurArg++ );
		if ( *pArg != '@' )
		{
			bGood = qfalse;
			break;
		}
		// store checksums since tokenization is not re-entrant
		for ( i = 0; nCurArg < nClientPaks; i++ )
		{
			nClientChkSum[i] = atoi( Cmd_Argv( nCurArg++ ) );
		}

		// store number to compare against (minus one cause the last is the number of checksums)
		nClientPaks = i - 1;

		// make sure none of the client check sums are the same
		// so the client can't send 5 the same checksums
		for ( i = 0; i < nClientPaks; i++ )
		{
			for ( j = 0; j < nClientPaks; j++ )
			{
				if ( i == j )
				{
					continue;
				}
				if ( nClientChkSum[i] == nClientChkSum[j] )
				{
					bGood = qfalse;
					break;
				}
			}
			if ( bGood == qfalse )
			{
				break;
			}
		}
		if ( bGood == qfalse )
		{
			break;
		}

		// get the pure checksums of the pk3 files loaded by the server
		pPaks = FS_LoadedIwdPureChecksums();
		Cmd_TokenizeString( pPaks );
		nServerPaks = Cmd_Argc();
		if ( nServerPaks > 1024 )
		{
			nServerPaks = 1024;
		}

		for ( i = 0; i < nServerPaks; i++ )
		{
			nServerChkSum[i] = atoi( Cmd_Argv( i ) );
		}

		// check if the client has provided any pure checksums of pk3 files not loaded by the server
		for ( i = 0; i < nClientPaks; i++ )
		{
			for ( j = 0; j < nServerPaks; j++ )
			{
				if ( nClientChkSum[i] == nServerChkSum[j] )
				{
					break;
				}
			}
			if ( j >= nServerPaks )
			{
				bGood = qfalse;
				break;
			}
		}
		if ( bGood == qfalse )
		{
			break;
		}

		// check if the number of checksums was correct
		nChkSum1 = sv.checksumFeed;
		for ( i = 0; i < nClientPaks; i++ )
		{
			nChkSum1 ^= nClientChkSum[i];
		}
		nChkSum1 ^= nClientPaks;
		if ( nChkSum1 != nClientChkSum[nClientPaks] )
		{
			bGood = qfalse;
			break;
		}

		// break out
		break;
	}

	if ( bGood )
	{
		cl->pureAuthentic = 1;
	}
	else
	{
		cl->pureAuthentic = 2;
	}
}

/*
=================
SV_ResetPureClient_f
=================
*/
static void SV_ResetPureClient_f( client_t *cl )
{
	cl->pureAuthentic = 0;
}

/*
==================
SV_MutePlayer_f
==================
*/
static void SV_MutePlayer_f( client_t *cl )
{
	int muteClient = atoi(SV_Cmd_Argv(1));

	if ( muteClient < 0 || muteClient >= MAX_CLIENTS )
	{
		Com_Printf("Invalid mute client %i\n", muteClient);
		return;
	}

	cl->muteList[muteClient] = true;
}

/*
==================
SV_UnmutePlayer_f
==================
*/
static void SV_UnmutePlayer_f( client_t *cl )
{
	int muteClient = atoi(SV_Cmd_Argv(1));

	if ( muteClient < 0 || muteClient >= MAX_CLIENTS )
	{
		Com_Printf("Invalid unmute client %i\n", muteClient);
		return;
	}

	cl->muteList[muteClient] = false;
}

/*
==================
SV_UpdateUserinfo_f
==================
*/
static void SV_UpdateUserinfo_f( client_t *cl )
{
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);
	I_strncpyz(cl->userinfo, SV_Cmd_Argv(1), sizeof( cl->userinfo ));
	SV_UserinfoChanged(cl);
	// call prog code to allow overrides
#ifdef LIBCOD
	hook_ClientUserinfoChanged(cl - svs.clients);
#else
	ClientUserinfoChanged(cl - svs.clients);
#endif
}

/*
=================
SV_Disconnect_f

The client is going to disconnect, so remove the connection immediately  FIXME: move to game?
=================
*/
static void SV_Disconnect_f( client_t *cl )
{
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);
	SV_DropClient(cl, "EXE_DISCONNECTED");
}

/*
==================
SV_BeginDownload_f
==================
*/
static void SV_BeginDownload_f( client_t *cl )
{
	// Kill any existing download
	SV_CloseDownload(cl);

	// cl->downloadName is non-zero now, SV_WriteDownloadToClient will see this and open
	// the file itself
	I_strncpyz(cl->downloadName, SV_Cmd_Argv(1), sizeof( cl->downloadName ));
}

/*
==================
SV_NextDownload_f

The argument will be the last acknowledged block from the client, it should be
the same as cl->downloadClientBlock
==================
*/
static void SV_NextDownload_f( client_t *cl )
{
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);
	int block = atoi( Cmd_Argv( 1 ) );

	if ( block == cl->downloadClientBlock )
	{
		Com_DPrintf( "clientDownload: %d : client acknowledge of block %d\n", cl - svs.clients, block );

		// Find out if we are done.  A zero-length block indicates EOF
		if ( cl->downloadBlockSize[cl->downloadClientBlock % MAX_DOWNLOAD_WINDOW] == 0 )
		{
			Com_Printf( "clientDownload: %d : file \"%s\" completed\n", cl - svs.clients, cl->downloadName );
			SV_CloseDownload( cl );
			return;
		}

		cl->downloadSendTime = svs.time;
		cl->downloadClientBlock++;
		return;
	}
	// We aren't getting an acknowledge for the correct block, drop the client
	// FIXME: this is bad... the client will never parse the disconnect message
	//			because the cgame isn't loaded yet
	SV_DropClient( cl, "broken download" );
}

/*
==================
SV_StopDownload_f

Abort a download if in progress
==================
*/
static void SV_StopDownload_f( client_t *cl )
{
	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	if ( *cl->downloadName )
	{
		Com_DPrintf( "clientDownload: %d : file \"%s\" aborted\n", cl - svs.clients, cl->downloadName );
	}

	SV_CloseDownload( cl );
}

/*
==================
SV_DoneDownload_f

Downloads are finished
==================
*/
static void SV_DoneDownload_f( client_t *cl )
{
	Com_DPrintf( "clientDownload: %s Done\n", cl->name );
	// resend the game state to update any clients that entered during the download
	SV_SendClientGameState( cl );
}

/*
==================
SV_RetransmitDownload_f

The argument will be the last acknowledged block from the client, it should be
the same as cl->downloadClientBlock
==================
*/
static void SV_RetransmitDownload_f( client_t *cl )
{
	if ( atoi(SV_Cmd_Argv(1)) == cl->downloadClientBlock )
		cl->downloadXmitBlock = cl->downloadClientBlock;
}

/*
==================
SV_WWWDownload_f
==================
*/
static void SV_WWWDownload_f( client_t *cl )
{
	const char *subcmd;

	subcmd = SV_Cmd_Argv(1);

	// only accept wwwdl commands for clients which we first flagged as wwwdl ourselves
	if ( !cl->downloadingWWW )
	{
		Com_Printf("SV_WWWDownload: unexpected wwwdl '%s' for client '%s'\n", subcmd, cl->name);
		SV_DropClient(cl, "PC_PATCH_1_1_UNEXPECTEDDOWLOADMESSAGE");
		return;
	}

	if ( !Q_stricmp( subcmd, "ack" ) )
	{
		if ( cl->clientDownloadingWWW )
			Com_Printf("WARNING: dupe wwwdl ack from client '%s'\n", cl->name);

		cl->clientDownloadingWWW = 1;
		return;
	}

	if ( !Q_stricmp( subcmd, "bbl8r" ) )
	{
		SV_DropClient(cl, "PC_PATCH_1_1_DOWNLOADDISCONNECTED");
		return;
	}

	// below for messages that only happen during/after download
	if ( !cl->clientDownloadingWWW )
	{
		Com_Printf("SV_WWWDownload: unexpected wwwdl '%s' for client '%s'\n", subcmd, cl->name);
		SV_DropClient(cl, "PC_PATCH_1_1_UNEXPECTEDDOWLOADMESSAGE");
		return;
	}

	if ( !Q_stricmp( subcmd, "done" ) )
	{
		cl->download = 0;
		cl->downloadName[0] = 0;
		cl->clientDownloadingWWW = 0;
		return;
	}

	if ( !Q_stricmp( subcmd, "fail" ) )
	{
		cl->download = 0;
		cl->downloadName[0] = 0;
		cl->clientDownloadingWWW = 0;
		cl->wwwFallback = 1;
		Com_Printf( "Client '%s' reported that the http download of '%s' failed, falling back to a server download\n", cl->name, cl->downloadName);
		SV_SendClientGameState(cl);
		return;
	}

	if ( !Q_stricmp( subcmd, "chkfail" ) )
	{
		Com_Printf( "WARNING: client '%s' reports that the redirect download for '%s' had wrong checksum.\n", cl->name, cl->downloadName);
		Com_Printf("         you should check your download redirect configuration.\n");
		cl->download = 0;
		cl->downloadName[0] = 0;
		cl->clientDownloadingWWW = 0;
		cl->wwwFallback = 1;
		SV_SendClientGameState(cl);
		return;
	}

	Com_Printf("SV_WWWDownload: unknown wwwdl subcommand '%s' for client '%s'\n", subcmd, cl->name);
	SV_DropClient(cl, "PC_PATCH_1_1_UNEXPECTEDDOWLOADMESSAGE");
}

typedef struct
{
	const char *name;
	void ( *func )( client_t *cl );
} ucmd_t;

static ucmd_t ucmds[] =
{
	{"userinfo",     SV_UpdateUserinfo_f,     },
	{"disconnect",   SV_Disconnect_f,         },
	{"cp",           SV_VerifyIwds_f,         },
#ifdef LIBCOD
	{"vdr",          SV_ResetPureClient_libcod,    },
#else
	{"vdr",          SV_ResetPureClient_f,    },
#endif
	{"download",     SV_BeginDownload_f,      },
	{"nextdl",       SV_NextDownload_f,       },
	{"stopdl",       SV_StopDownload_f,       },
	{"donedl",       SV_DoneDownload_f,       },
	{"retransdl",    SV_RetransmitDownload_f, },
	{"muteplayer",   SV_MutePlayer_f,         },
	{"unmuteplayer", SV_UnmutePlayer_f,       },
	{ "wwwdl",       SV_WWWDownload_f,        },
	{NULL, NULL}
};

/*
==================
SV_ExecuteClientCommand
Also called by bot code
==================
*/
void SV_ExecuteClientCommand( client_t *cl, const char *s, qboolean clientOK )
{
	ucmd_t  *u;

	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	Cmd_TokenizeString( s );

	// see if it is a server level command
	for ( u = ucmds ; u->name ; u++ )
	{
		if ( !strcmp( Cmd_Argv( 0 ), u->name ) )
		{
			u->func( cl );
			break;
		}
	}

	if ( clientOK )
	{
		// pass unknown strings to the game
		if ( !u->name && SV_Loaded() )
		{
#ifdef LIBCOD
			hook_ClientCommand( cl - svs.clients );
#else
			ClientCommand( cl - svs.clients );
#endif
		}
	}
}

/*
===============
SV_ClientCommand
===============
*/
static qboolean SV_ClientCommand( client_t *cl, msg_t *msg )
{
	int seq;
	const char  *s;
	qboolean clientOk = qtrue;
	qboolean floodprotect = qtrue;
	char strBuf[MAX_STRING_CHARS];

	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	seq = MSG_ReadLong( msg );
	s = MSG_ReadString( msg, strBuf, sizeof(strBuf) );

	// see if we have already executed it
	if ( cl->lastClientCommand >= seq )
	{
		return qtrue;
	}

	if ( sv_showCommands->current.boolean )
		Com_DPrintf( "clientCommand: %s : %i : %s\n", cl->name, seq, s );

	// drop the connection if we have somehow lost commands
	if ( seq > cl->lastClientCommand + 1 )
	{
		Com_Printf( "Client %s lost %i clientCommands\n", cl->name, seq - cl->lastClientCommand + 1 );
		SV_DropClient( cl, "EXE_LOSTRELIABLECOMMANDS" );
		return qfalse;
	}

	// Gordon: AHA! Need to steal this for some other stuff BOOKMARK
	// NERVE - SMF - some server game-only commands we cannot have flood protect
	if ( !Q_strncmp( "team ", s, 5 ) || !Q_strncmp( "score ", s, 6 ) || !Q_strncmp( "mr ", s, 3 ) )
	{
//		Com_DPrintf( "Skipping flood protection for: %s\n", s );
		floodprotect = qfalse;
	}

	// malicious users may try using too many string commands
	// to lag other players.  If we decide that we want to stall
	// the command, we will stop processing the rest of the packet,
	// including the usercmd.  This causes flooders to lag themselves
	// but not other people
	// We don't do this when the client hasn't been active yet since its
	// normal to spam a lot of commands when downloading
	if (cl->state >= CS_ACTIVE &&      // (SA) this was commented out in Wolf.  Did we do that?
	        sv_floodProtect->current.boolean &&
	        svs.time < cl->nextReliableTime &&
	        floodprotect )
	{
		// ignore any other text messages from this client but let them keep playing
		// TTimo - moved the ignored verbose to the actual processing in SV_ExecuteClientCommand, only printing if the core doesn't intercept
		clientOk = qfalse;
		Com_DPrintf("client text ignored for %s: %s\n", cl->name, SV_Cmd_Argv(0));
	}

	// don't allow another command for 800 msec
	if ( floodprotect )
	{
		cl->nextReliableTime = svs.time + 800;
	}

	SV_ExecuteClientCommand( cl, s, clientOk );

	cl->lastClientCommand = seq;
	Com_sprintf( cl->lastClientCommandString, sizeof( cl->lastClientCommandString ), "%s", s );

	return qtrue;       // continue procesing
}

/*
===================
SV_ExecuteClientMessage
Parse a client packet
===================
*/
void SV_ExecuteClientMessage( client_t *cl, msg_t *msg )
{
	byte msgBuf[MAX_MSGLEN];
	msg_t decompressMsg;
	int c;

	assert(cl - svs.clients >= 0 && cl - svs.clients < MAX_CLIENTS);

	MSG_Init(&decompressMsg, msgBuf, sizeof(msgBuf));
	decompressMsg.cursize = MSG_ReadBitsCompress(&msg->data[msg->readcount], msgBuf, msg->cursize - msg->readcount);

	// if this is a usercmd from a previous gamestate,
	// ignore it or retransmit the current gamestate
	//
	// if the client was downloading, let it stay at whatever serverId and
	// gamestate it was at.  This allows it to keep downloading even when
	// the gamestate changes.  After the download is finished, we'll
	// notice and send it a new game state
	//
	// show_bug.cgi?id=536
	// don't drop as long as previous command was a nextdl, after a dl is done, downloadName is set back to ""
	// but we still need to read the next message to move to next download or send gamestate
	// I don't like this hack though, it must have been working fine at some point, suspecting the fix is somewhere else
	if ( cl->serverId != sv_serverId_value && !*cl->downloadName && !cl->downloadingWWW && !cl->clientDownloadingWWW )
	{
		if ( (cl->serverId & 0xF0) == (sv_serverId_value & 0xF0) )   // TTimo - use a comparison here to catch multiple map_restart
		{
			// they just haven't caught the map_restart yet
			// Com_DPrintf( "%s : ignoring pre map_restart / outdated client message\n", cl->name );
			if ( cl->state == CS_PRIMED )
				SV_ClientEnterWorld(cl, &cl->lastUsercmd);
			return;
		}

		// if we can tell that the client has dropped the last
		// gamestate we sent them, resend it
		if ( cl->messageAcknowledge > cl->gamestateMessageNum )
		{
			Com_DPrintf( "%s : dropped gamestate, resending\n", cl->name );
			SV_SendClientGameState( cl );

			if ( net_lanauthorize->current.boolean || !Sys_IsLANAddress(cl->netchan.remoteAddress) )
				SV_AuthorizeRequest(cl->netchan.remoteAddress, cl->challenge);
		}

		return;
	}

	// read optional clientCommand strings
	do
	{
		c = MSG_ReadBits(&decompressMsg, 3);
		if ( c == clc_EOF )
		{
			break;
		}
		if ( c != clc_clientCommand )
		{
			break;
		}
		if ( !SV_ClientCommand( cl, &decompressMsg ) )
		{
			return; // we couldn't execute it because of the flood protection
		}
		if ( cl->state == CS_ZOMBIE )
		{
			return; // disconnect command
		}
	}
	while ( 1 );

	if ( sv_pure->current.boolean && cl->pureAuthentic == 2 )
	{
		cl->nextSnapshotTime = -1;
		SV_DropClient(cl, "EXE_UNPURECLIENTDETECTED");
		cl->state = CS_ACTIVE;
		SV_SendClientSnapshot(cl);
		cl->state = CS_ZOMBIE;
	}

	// read the usercmd_t
	if ( c == clc_move )
	{
		SV_UserMove( cl, &decompressMsg, qtrue );
	}
	else if ( c == clc_moveNoDelta )
	{
		SV_UserMove( cl, &decompressMsg, qfalse );
	}
	else if ( c != clc_EOF )
	{
		Com_Printf( "WARNING: bad command byte %i for client %i\n", c, cl - svs.clients );
	}
}
